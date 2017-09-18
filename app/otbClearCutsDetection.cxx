/*=========================================================================

  Copyright (c) Remi Cresson (IRSTEA). All rights reserved.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "otbDeltaNDVIFunctor.h"
#include "itkFixedArray.h"
#include "itkObjectFactory.h"

// Elevation handler
#include "otbWrapperElevationParametersHandler.h"
#include "otbWrapperApplicationFactory.h"
#include "otbWrapperCompositeApplication.h"

// Application engine
#include "otbStandardFilterWatcher.h"
#include "itkFixedArray.h"

// Filters
#include "itkMaskImageFilter.h"
#include "otbStreamingStatisticsImageFilter.h"
#include "otbStreamingResampleImageFilter.h"
#include "otbMultiChannelExtractROI.h"
#include "otbDeltaNDVILabelerFilter.h"
#include "itkAndImageFilter.h"

// Helper
#include "otbRegionComparator.h"

// Mask handler
#include "otbMosaicFromDirectoryHandler.h"

// Connected components
#include "otbConnectedLabelsImageFilter.h"

// Vectorization
#include "otbCacheLessLabelImageToVectorData.h"

namespace otb
{

namespace Wrapper
{

class ClearCutsDetection : public CompositeApplication
{
public:
  /** Standard class typedefs. */
  typedef ClearCutsDetection            Self;
  typedef CompositeApplication          Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard macro */
  itkNewMacro(Self);
  itkTypeMacro(ClearCutsDetection, Application);

  /** Filters */
  typedef UInt8ImageType                                                                    MaskImageType;
  typedef otb::StreamingResampleImageFilter<FloatVectorImageType, FloatVectorImageType>     ResampleImageFilterType;
  typedef otb::StreamingResampleImageFilter<MaskImageType,MaskImageType>                    ResampleMaskImageFilterType;
  typedef itk::NearestNeighborInterpolateImageFunction<FloatVectorImageType>                NNInterpolatorType;
  typedef itk::NearestNeighborInterpolateImageFunction<MaskImageType>                       NNMaskImageInterpolatorType;
  typedef otb::Functor::DeltaNDVIFromChannels<FloatVectorImageType::PixelType,
      FloatImageType::PixelType>                                                            DeltaNDVIFunctorType;
  typedef itk::BinaryFunctorImageFilter<FloatVectorImageType, FloatVectorImageType,
      FloatImageType, DeltaNDVIFunctorType>                                                 DeltaNDVIFilterType;
  typedef itk::MaskImageFilter<FloatImageType, MaskImageType, FloatImageType>               MaskImageFilterType;
  typedef otb::DeltaNDVILabelerFilter<FloatImageType, MaskImageType>                        NDVILabelImageFilterType;
  typedef otb::StreamingStatisticsImageFilter<FloatImageType>                               StatsFilterType;
  typedef otb::MultiChannelExtractROI<FloatVectorImageType::InternalPixelType,
      FloatVectorImageType::InternalPixelType>                                              ExtractROIFilterType;
  typedef otb::MosaicFromDirectoryHandler<MaskImageType,FloatImageType>                     MaskHandlerType;
  typedef itk::AndImageFilter<MaskImageType, MaskImageType>                                 AndFilterType;
  typedef otb::ConnectedLabelsImageFilter<MaskImageType>                                    ConnectedLabelsFilterType;
  typedef otb::CacheLessLabelImageToVectorData<MaskImageType::PixelType>                    VectorizationFilterType;

  void DoUpdateParameters()
  {
    // Nothing to do here : all parameters are independent
  }

  void DoInit()
  {

    SetName("ClearCutsDetection");
    SetDescription("This application performs cuts detection from two input images and "
        "optional sets of masks (input images masks + forest mask)");

    // Documentation
    SetDocName("ClearCutsDetection");
    SetDocLimitations("None");
    SetDocAuthors("RemiCresson");
    SetDocLongDescription(" This filter implements the clear cut detection method, based "
        "on the work of Kenji Ose and Michel Deshayes at IRSTEA. Difference between NDVI "
        "at dates 0 and 1 is computed, then thresholded based on statistics moments. The "
        "output is exported in vector data format.");

    AddDocTag(Tags::ChangeDetection);

    ClearApplications();

    // Add applications
    AddApplication("ExtractGeom", "roib" , "Extract ROI from image b");
    AddApplication("ExtractGeom", "roia" , "Extract ROI from image a");

    // Shared parameters
    ShareParameter("inbmask", "roib.vec", "Input vector data for T0 Image mask (Before)", "Input vector data for T0 Image mask (Before)");
    MandatoryOff("inbmask");
    ShareParameter("inamask", "roia.vec", "Input vector data for T1 Image mask (After)" , "Input vector data for T1 Image mask (After)" );
    MandatoryOff("inamask");

    // Input images
    AddParameter(ParameterType_InputImage, "inb",  "Input T0 Image (Before)");
    AddParameter(ParameterType_InputImage, "ina",  "Input T1 Image (After)");

    // Vegetation mask
    AddParameter(ParameterType_Directory, "masksdir", "Vegetation masks directory");
    MandatoryOff("masksdir");

    // Input images band indices
    AddParameter(ParameterType_Int, "nirb", "near infrared band index for input T0 image" );
    SetParameterDescription("nirb","index of near infrared band of image b");
    SetMinimumParameterIntValue("nirb", 1);
    SetDefaultParameterInt     ("nirb", 4);

    AddParameter(ParameterType_Int, "redb", "red band index for input T0 image" );
    SetParameterDescription("redb","index of red band of image b");
    SetMinimumParameterIntValue("redb", 1);
    SetDefaultParameterInt     ("redb", 1);

    AddParameter(ParameterType_Int, "nira", "near infrared band index for input T1 image" );
    SetParameterDescription("nira","index of near infrared band of image a");
    SetMinimumParameterIntValue("nira", 1);
    SetDefaultParameterInt     ("nira", 4);

    AddParameter(ParameterType_Int, "reda", "red band index for input T1 image" );
    SetParameterDescription("reda","index of red band of image a");
    SetMinimumParameterIntValue("reda", 1);
    SetDefaultParameterInt     ("reda", 1);

    // Spatial filtering (connected components)
    AddParameter(ParameterType_Int, "filt", "Minimum number of pixels detected" );
    SetMinimumParameterIntValue("filt", 1  );
    SetMaximumParameterIntValue("filt", 100);
    SetDefaultParameterInt     ("filt", 10 );

    // Output vector
    AddParameter(ParameterType_OutputVectorData, "outvec", "Output vector layer");

    AddRAMParameter();
  }

  void PrepareFilters(FloatVectorImageType * &imageToResample,
      FloatVectorImageType * &imageToExtract,
      FloatVectorImageType::RegionType imageToExtractRegion)
  {
    // Initialize resample filter
    NNInterpolatorType::Pointer interpolator = NNInterpolatorType::New();
    m_ResampleFilter = ResampleImageFilterType::New();
    m_ResampleFilter->SetInput(imageToResample);
    m_ResampleFilter->SetInterpolator(interpolator);

    // Initialize roi extract filter
    m_ExtractROIFilter = ExtractROIFilterType::New();
    m_ExtractROIFilter->SetInput(imageToExtract);
    m_ExtractROIFilter->SetExtractionRegion(imageToExtractRegion);
    m_ExtractROIFilter->UpdateOutputInformation();

    // Set the resample filter with extracted image origin, spacing, and size
    m_ResampleFilter->SetOutputOrigin(m_ExtractROIFilter->GetOutput()->GetOrigin());
    m_ResampleFilter->SetOutputSpacing(m_ExtractROIFilter->GetOutput()->GetSpacing());
    m_ResampleFilter->SetOutputSize(m_ExtractROIFilter->GetOutput()->GetLargestPossibleRegion().GetSize());
    m_ResampleFilter->UpdateOutputInformation();
  }

  void DoExecute()
  {

    // Get input images pointers
    FloatVectorImageType* t0 = GetParameterImage("inb");
    FloatVectorImageType* t1 = GetParameterImage("ina");

    // Use input image mask for image b (t0)
    if (HasValue("inbmask"))
      {
        GetInternalApplication("roib")->SetParameterInputImage("in", GetParameterImage("inb"));
        ExecuteInternal("roib");
        t0 = static_cast<FloatVectorImageType*>(GetInternalApplication("roib")->GetParameterOutputImage("out"));
        t0->UpdateOutputInformation();
      }

    // Use input image mask for image a (t1)
    if (HasValue("inamask"))
      {
        GetInternalApplication("roia")->SetParameterInputImage("in", GetParameterImage("ina"));
        ExecuteInternal("roia");
        t1 = static_cast<FloatVectorImageType*>(GetInternalApplication("roia")->GetParameterOutputImage("out"));
        t1->UpdateOutputInformation();
      }

    // Compute rasters intersection region, check overlap
    otb::RegionComparator<FloatVectorImageType, FloatVectorImageType> comparator;
    comparator.SetImage1(t0);
    comparator.SetImage2(t1);
    if (!comparator.DoesOverlap())
      {
        otbAppLogFATAL("Inputs do not overlap!");
      }

    // Detect which input image (t0 or t1) have the smallest pixel
    FloatVectorImageType::Pointer inputExtractedT0;
    FloatVectorImageType::Pointer inputExtractedT1;
    FloatVectorImageType::SpacingType spacingT0 = t0->GetSpacing();
    FloatVectorImageType::SpacingType spacingT1 = t1->GetSpacing();
    double pixelAreaTO = vnl_math_abs(spacingT0[0]*spacingT0[1]);
    double pixelAreaT1 = vnl_math_abs(spacingT1[0]*spacingT1[1]);
    if (pixelAreaTO > pixelAreaT1)
      {
        // Resample t0 over t1 and extract ROI (overlap) of t1
        otbAppLogINFO("inb-->resampled");
        otbAppLogINFO("ina-->extracted");
        PrepareFilters(t0, t1, comparator.GetOverlapInImage2Indices());
        inputExtractedT0 = m_ResampleFilter->GetOutput();
        inputExtractedT1 = m_ExtractROIFilter->GetOutput();
      }
    else
      {
        // Resample t1 over t0 and extract ROI (overlap) of t0
        otbAppLogINFO("inb-->extracted");
        otbAppLogINFO("ina-->resampled");
        PrepareFilters(t1, t0, comparator.GetOverlapInImage1Indices());
        inputExtractedT0 = m_ExtractROIFilter->GetOutput();
        inputExtractedT1 = m_ResampleFilter->GetOutput();
      }

    // Compute Delta NDVI
    m_DeltaNDVIFilter = DeltaNDVIFilterType::New();
    m_DeltaNDVIFilter->SetInput1(inputExtractedT0);
    m_DeltaNDVIFilter->SetInput2(inputExtractedT1);
    m_DeltaNDVIFilter->GetFunctor().SetNIRChannelT0(GetParameterInt("nirb"));
    m_DeltaNDVIFilter->GetFunctor().SetRedChannelT0(GetParameterInt("redb"));
    m_DeltaNDVIFilter->GetFunctor().SetNIRChannelT1(GetParameterInt("nira"));
    m_DeltaNDVIFilter->GetFunctor().SetRedChannelT1(GetParameterInt("reda"));
    m_DeltaNDVIFilter->UpdateOutputInformation();

    // Stats filter
    m_StatsFilter = StatsFilterType::New();
    m_StatsFilter->SetIgnoreUserDefinedValue(true);
    m_StatsFilter->SetUserIgnoredValue(m_DeltaNDVIFilter->GetFunctor().GetNoDataValue());

    FloatImageType * deltaNDVIImage = m_DeltaNDVIFilter->GetOutput();

    // Mask directory
    if (HasValue("masksdir"))
      {
        otbAppLogINFO("Using vegetation masks");

        // Mask Delta NDVI image
        m_MaskImageFilter = MaskImageFilterType::New();
        m_MaskImageFilter->SetInput(m_DeltaNDVIFilter->GetOutput());
        m_MaskImageFilter->SetOutsideValue(3.0);

        // Instanciate a mosaic from directory handler
        m_MaskHandler = MaskHandlerType::New();
        m_MaskHandler->SetDirectory(GetParameterAsString("masksdir"));
        m_MaskHandler->SetReferenceImage(m_DeltaNDVIFilter->GetOutput());
        m_MaskHandler->SetUseReferenceImage(true);
        m_MaskHandler->UpdateOutputInformation();
        m_MaskImageFilter->SetMaskImage(m_MaskHandler->GetOutput());

        deltaNDVIImage = m_MaskImageFilter->GetOutput();
      }

    // Wire stats filter
    m_StatsFilter->SetInput(deltaNDVIImage);

    // Compute stats
    AddProcess(m_StatsFilter->GetStreamer(),"Computing dNDVI statistics");
    m_StatsFilter->Update();

    // Label image
    m_NDVILabelFilter = NDVILabelImageFilterType::New();
    m_NDVILabelFilter->SetInput(deltaNDVIImage);
    m_NDVILabelFilter->SetInputMeanObject(m_StatsFilter->GetMeanOutput());
    m_NDVILabelFilter->SetInputSigmaObject(m_StatsFilter->GetSigmaOutput());
    m_NDVILabelFilter->SetNumberOfClasses(2); // 2 classes
    m_NDVILabelFilter->SetFirstClassValue(0); // Label 0: no (... t enough) change, Label 1: clear cut
    m_NDVILabelFilter->SetFirstClassStart(3); // Label 0 is [mu-3*std, +inf[, Label 1 is ]-inf, mu-3*std[,
    m_NDVILabelFilter->SetInputNoDataValue(m_DeltaNDVIFilter->GetFunctor().GetNoDataValue());
    m_NDVILabelFilter->SetOutputNoDataValue(0);

    // Clean label image
    m_CleanFilter = ConnectedLabelsFilterType::New();
    m_CleanFilter->SetInput(m_NDVILabelFilter->GetOutput());
    m_CleanFilter->SetNoDataPixel(m_NDVILabelFilter->GetOutputNoDataValue());
    m_CleanFilter->SetMinNumberOfComponents(GetParameterInt("filt"));
    m_CleanFilter->UpdateOutputInformation();

    // Vectorize higher class
    m_VectorizeFilter = VectorizationFilterType::New();
    m_VectorizeFilter->SetInput(m_CleanFilter->GetOutput());
    m_VectorizeFilter->SetAutomaticAdaptativeStreaming(GetParameterInt("ram"));
    AddProcess(m_VectorizeFilter, "Computing layer");
    SetParameterOutputVectorData("outvec", m_VectorizeFilter->GetOutput());
  }

  ResampleImageFilterType::Pointer      m_ResampleFilter;
  ExtractROIFilterType::Pointer         m_ExtractROIFilter;
  DeltaNDVIFilterType::Pointer          m_DeltaNDVIFilter;
  MaskImageFilterType::Pointer          m_MaskImageFilter;
  NDVILabelImageFilterType::Pointer     m_NDVILabelFilter;
  StatsFilterType::Pointer              m_StatsFilter;
  MaskHandlerType::Pointer              m_MaskHandler;
  AndFilterType::Pointer                m_AndFilter;
  ConnectedLabelsFilterType::Pointer    m_CleanFilter;
  VectorizationFilterType::Pointer      m_VectorizeFilter;
};
}
}

OTB_APPLICATION_EXPORT( otb::Wrapper::ClearCutsDetection )
