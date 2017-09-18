/*=========================================================================

  Copyright (c) Remi Cresson (IRSTEA). All rights reserved.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkFixedArray.h"
#include "itkObjectFactory.h"

// Elevation handler
#include "otbWrapperElevationParametersHandler.h"
#include "otbWrapperApplicationFactory.h"

// Application engine
#include "otbStandardFilterWatcher.h"
#include "itkFixedArray.h"

// Mosaic filters
#include "otbClearCutsMosaicingFilter.h"

enum Modes
{
  max,mean
};

/**
 * \class Max Clear Cut Functor
 * \brief Compute the maximum of the dNDVI label value
 */
template< class T>
class MaxCC
{
public:
  MaxCC() {}

  ~MaxCC() {}

  bool operator!=( const MaxCC & ) const {
    return false;
  }

  bool operator==( const MaxCC & other ) const {
    return !(*this != other);
  }

  inline T operator()( const std::vector<T> & A ) const
  {
    T output;
    output.SetSize(1);
    output[0] = itk::NumericTraits<typename T::ValueType>::Zero;
    for (unsigned int i = 0 ; i < A.size() ; i++)
      {
      if (output[0] < A[i][0])
        output[0] = A[i][0];
      }

    return output;
  }
};

/**
 * \class Mean Clear Cut Functor
 * \brief Compute the maximum of the dNDVI label value
 */
template< class T>
class MeanCC
{
public:
  MeanCC() {}

  ~MeanCC() {}

  bool operator!=( const MeanCC & ) const {
    return false;
  }

  bool operator==( const MeanCC & other ) const {
    return !(*this != other);
  }

  inline T operator()( const std::vector<T> & A ) const
  {
    double sum = 0;
    T output;
    output.SetSize(1);
    for (unsigned int i = 0 ; i < A.size() ; i++)
      {
      sum += A[i][0];
      }
    if (A.size()>0)
      sum /= ((double) A.size());
    output[0] = static_cast<typename T::ValueType>(sum);

    return output;
  }
};

namespace otb
{

namespace Wrapper
{

class ClearCutsAggregation : public Application
{
public:
  /** Standard class typedefs. */
  typedef ClearCutsAggregation          Self;
  typedef Application                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard macro */
  itkNewMacro(Self);
  itkTypeMacro(ClearCutsAggregation, Application);

  /** Typedefs */
  typedef MaxCC<FloatVectorImageType::PixelType> MaxCCType;
  typedef MeanCC<FloatVectorImageType::PixelType> MeanCCType;
  typedef otb::ClearCutsMosaicingFilter<FloatVectorImageType,
      FloatVectorImageType, double, MaxCCType> MaxClearCutsMosaicingFilterType;
  typedef otb::ClearCutsMosaicingFilter<FloatVectorImageType,
      FloatVectorImageType, double, MeanCCType> MeanClearCutsMosaicingFilterType;

private:

  // Macro used to convert number to string
  #define SSTR( x ) dynamic_cast< std::ostringstream & >( \
    ( std::ostringstream() << std::dec << x ) ).str()

  /*
   * Create a mosaic filter, which is connected to the inputs array
   */
  template<class TMosaicFilterType>
  typename TMosaicFilterType::Pointer
  CreateConnectedMosaicFilterToInputs()
  {
    // Get the input image list
    FloatVectorImageListType::Pointer inputArray = this->GetParameterImageList("il");

    typename TMosaicFilterType::Pointer mosaicFilter = TMosaicFilterType::New();
    if (inputArray->Size() ==0)
      {
      otbAppLogFATAL("Filter array have wrong number of elements");
      }
    else
      {
      for (unsigned int i = 0 ; i < inputArray->Size() ; i++)
        {
        mosaicFilter->PushBackInput(inputArray->GetNthElement(i) );
        }
      }
    return mosaicFilter;
  }


  void DoInit()
  {

    SetName("ClearCutsAggregation");
    SetDescription("Perform ClearCutsAggregation");

    // Documentation
    SetDocName("ClearCutsAggregation");
    SetDocLongDescription("This application performs ClearCutsAggregation of images");
    SetDocLimitations("None");
    SetDocAuthors("Remi Cresson");
    SetDocSeeAlso(" ");

    AddDocTag(Tags::Manip);
    AddDocTag(Tags::Raster);

    // Input image
    AddParameter(ParameterType_InputImageList,  "il",   "Input Clear Cuts Label Images");
    SetParameterDescription("il", "Input Clear Cuts Label Images to mosaic");

    // Output image
    AddParameter(ParameterType_OutputImage,  "out",   "Output image");
    SetParameterDescription("out"," Output image.");
    SetDefaultOutputPixelType("out", ImagePixelType_uint8);

    // Mode de calcul
    AddParameter(ParameterType_Choice,"method","Aggregation method");
    SetParameterDescription("method","Set the aggregation method for composition");
    AddChoice("method.max","Maximum");
    AddChoice("method.mean","Mean");


    AddRAMParameter();

  }

  void DoUpdateParameters()
  {
    // Nothing to do here : all parameters are independent
  }

  void DoExecute()
  {

    if (GetParameterInt("method") == max)
      {
      m_MaxMosaicFilter = CreateConnectedMosaicFilterToInputs<MaxClearCutsMosaicingFilterType>();
      SetParameterOutputImage("out", m_MaxMosaicFilter->GetOutput() );
      }
    else if (GetParameterInt("method") == mean)
      {
      m_MeanMosaicFilter = CreateConnectedMosaicFilterToInputs<MeanClearCutsMosaicingFilterType>();
      SetParameterOutputImage("out", m_MeanMosaicFilter->GetOutput() );
      }
    else
      otbAppLogFATAL("Unknow aggregation method");


  }   // DOExecute()

  void AfterExecuteAndWriteOutputs()
  {
    // Nothing to do
  }

  MaxClearCutsMosaicingFilterType::Pointer m_MaxMosaicFilter;
  MeanClearCutsMosaicingFilterType::Pointer m_MeanMosaicFilter;

};
}
}

OTB_APPLICATION_EXPORT( otb::Wrapper::ClearCutsAggregation )
