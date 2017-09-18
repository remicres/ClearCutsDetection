/*=========================================================================

  Copyright (c) Remi Cresson (IRSTEA). All rights reserved.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __DeltaNDVILabelerFilter_hxx
#define __DeltaNDVILabelerFilter_hxx

#include "otbDeltaNDVILabelerFilter.h"

#include "itkProgressReporter.h"

namespace otb
{
/**
 *
 */
template <class TNDVIImage, class TLabelImage>
DeltaNDVILabelerFilter<TNDVIImage, TLabelImage>
::DeltaNDVILabelerFilter()
 {
  // NoDataValues
  m_OutputNoDataValue = itk::NumericTraits<LabelImagePixelType>::Zero;
  m_InputNoDataValue = 3.0; // deltaNDVI no data value

  // Default classes parameters
  m_NumberOfClasses = 4; // number of classes
  m_FirstClassStart = 1; // first sigma multiplicator
  m_FirstClassValue = 1; // label value of the first range
  // Class 0: +inf  > dNDVI > µ-1*s // label = 1 // No changes
  // Class 1: µ-1*s > dNDVI > µ-2*s // label = 2 // Low probability of detection
  // Class 2: µ-2*s > dNDVI > µ-3*s // label = 3 // Medium-high probability of detection
  // Class 3: µ-3*s > dNDVI > -inf  // label = 4 // Very-high probability of detection

 }

template <class TNDVIImage, class TLabelImage>
void
DeltaNDVILabelerFilter<TNDVIImage, TLabelImage>
::GenerateOutputInformation()
 {

  Superclass::GenerateOutputInformation();

  // Write no data flags
  std::vector<bool> noDataValueAvailable; noDataValueAvailable.push_back(true);
  std::vector<double> noDataValues1; noDataValues1.push_back(0.0);
  otb::WriteNoDataFlags(noDataValueAvailable, noDataValues1, this->GetOutput()->GetMetaDataDictionary());

  inputThresholds.set_size(m_NumberOfClasses);
  outputValues.set_size(m_NumberOfClasses);

  RealType sigma = m_InputSigmaObject->Get();
  RealType mean = m_InputMeanObject->Get();
  for (unsigned int i = 0 ; i < m_NumberOfClasses ; i++)
    {
    NDVIImagePixelType thresh = mean - ((NDVIImagePixelType) (i+m_FirstClassStart)) * sigma;
    inputThresholds[i] = thresh;
    outputValues[i] = m_FirstClassValue + i;
    }
 }

/**
 *
 */
template <class TNDVIImage, class TLabelImage>
void
DeltaNDVILabelerFilter<TNDVIImage, TLabelImage>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, itk::ThreadIdType threadId)
 {

  // Debug info
  itkDebugMacro(<<"Actually executing thread " << threadId << " in region " << outputRegionForThread);

  // Support progress methods/callbacks
  itk::ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels() );

  // Iterate through the thread region
  LabelImageIteratorType outputIt(this->GetOutput(), outputRegionForThread);
  NDVIImageIteratorType inputIt(this->GetInput(), outputRegionForThread);

  for ( outputIt.GoToBegin(), inputIt.GoToBegin(); !outputIt.IsAtEnd(); ++outputIt, ++inputIt )
    {
    NDVIImagePixelType delta = inputIt.Get();
    if (delta != m_InputNoDataValue)
      {
      // inputThresholds[0]---> +inf
      if (inputThresholds[0] < delta)
        {
        outputIt.Set( outputValues[0] );
        }
      // -inf ----> inputThresholds[n]
      else if (delta <= inputThresholds[m_NumberOfClasses-1])
        {
        outputIt.Set( outputValues[m_NumberOfClasses-1] );
        }
      // inputThresholds[0] ----> inputThresholds[n]
      else if (m_NumberOfClasses>1)
        {
        for (unsigned int i = 0 ; i < m_NumberOfClasses-1 ; i++)
          {
          if (inputThresholds[i+1] < delta && delta <= inputThresholds[i] )
            {
            outputIt.Set( outputValues[i+1] );
            }
          }
        }
      else
        {
        outputIt.Set( m_OutputNoDataValue );
        }
      }
    else
      {
      outputIt.Set( m_OutputNoDataValue );
      }
    progress.CompletedPixel();
    } // Next pixel
 }
}
#endif



