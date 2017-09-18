/*=========================================================================

  Copyright (c) Remi Cresson (IRSTEA). All rights reserved.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __ClearCutsMosaicingFilter_txx
#define __ClearCutsMosaicingFilter_txx

#include "otbClearCutsMosaicingFilter.h"

namespace otb {

/**
 * Processing
 */
template <class TInputImage, class TOutputImage, class TInternalValueType, class TFunctorType>
void
ClearCutsMosaicingFilter<TInputImage, TOutputImage, TInternalValueType, TFunctorType>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, itk::ThreadIdType threadId)
 {

  // Debug info
  itkDebugMacro(<<"Actually executing thread " << threadId << " in region " << outputRegionForThread);

  // Support progress methods/callbacks
  itk::ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels() );

  // Get output pointer
  OutputImageType * mosaicImage = this->GetOutput();

  // Get number of used inputs
  const unsigned int nbOfUsedInputImages = Superclass::GetNumberOfUsedInputImages();

  // Iterate through the thread region
  IteratorType outputIt(mosaicImage, outputRegionForThread);

  // Prepare interpolated pixel
  InternalPixelType interpolatedMathPixel;
  interpolatedMathPixel.SetSize(1);

  // Instanciate interpolators which are DEDICATED TO THE THREAD ! (so need to
  // copy the m_interpolator)
  std::vector<InterpolatorPointerType> interps;
  std::vector<InputImageType *> currentImage;
  Superclass::PrepareImageAccessors(currentImage, interps);

  // Container for geo coordinates
  OutputImagePointType geoPoint;

  for ( outputIt.GoToBegin(); !outputIt.IsAtEnd(); ++outputIt )
    {

    // Init. pixel stack
    std::vector<OutputImagePixelType> pixelStack;

    // Prepare output pixel
    OutputImagePixelType outputPixel(Superclass::GetNoDataOutputPixel() );

    // Current pixel --> Geographical point
    mosaicImage->TransformIndexToPhysicalPoint (outputIt.GetIndex(), geoPoint) ;

    // Loop on used input images
    for (unsigned int i = 0 ; i < nbOfUsedInputImages ; i++)
      {

      // Check if the point is inside the transformed thread region
      // (i.e. the region in the current input image which match the thread
      // region)
      if (interps[i]->IsInsideBuffer(geoPoint) )
        {

        // Compute the interpolated pixel value
        InputImagePixelType interpolatedPixel = interps[i]->Evaluate(geoPoint);

        // Check that interpolated pixel is not empty
        if (Superclass::IsPixelNotEmpty(interpolatedPixel) )
          {
          // Update the output pixel
          outputPixel[0] = static_cast<OutputImageInternalPixelType>(interpolatedPixel[0]);

          // Add the pixel to the stack
          pixelStack.push_back(outputPixel);

          } // Interpolated pixel is not empty
        }   // point inside buffer
      }     // next image


    // Update output pixel value
    outputIt.Set(m_Functor(pixelStack));

    // Update progress
    progress.CompletedPixel();

    } // next output pixel

 }

} // end namespace gtb

#endif
