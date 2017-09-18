/*=========================================================================

  Copyright (c) Remi Cresson (IRSTEA). All rights reserved.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __ConnectedLabelsImageFilter_hxx
#define __ConnectedLabelsImageFilter_hxx

#include <otbConnectedLabelsImageFilter.h>
#include "itkProgressReporter.h"

namespace otb
{
/**
 *
 */
template <class TImage>
ConnectedLabelsImageFilter<TImage>
::ConnectedLabelsImageFilter()
 {
  m_MinNumberOfComponents = 5;
  m_NoDataPixel = 0;

 }

template <class TImage>
void
ConnectedLabelsImageFilter<TImage>
::GenerateInputRequestedRegion()
 {

  // Output requested region
  const ImageRegionType outRegion = this->GetOutput()->GetRequestedRegion();

  // Grab input image
  ImageType * inputImage = static_cast<ImageType * >(
      Superclass::ProcessObject::GetInput(0) );
  ImageRegionType inRegion (outRegion);
  inRegion.PadByRadius(m_MinNumberOfComponents);
  inRegion.Crop(inputImage->GetLargestPossibleRegion());
  inputImage->SetRequestedRegion(inRegion);
 }

/*
 * Update the index list, and return false if all connected components were
 * walked through, or criterion reached
 */
template <class TImage>
bool
ConnectedLabelsImageFilter<TImage>
::UpdateConnectedIndexList(BooleanListType& markedList, ImageIndexListType& indexList){

  // Grab input image
  const ImageType * inputImage = const_cast<ImageType *>(this->GetInput());

  // Loop over pixels in the list
  for (unsigned int i = 0 ; i < indexList.size() ; i++)
    {
    // Check that this index is not already marked
    if (!markedList[i])
      {
      // Get current index
      ImageIndexType currentIndex = indexList[i];

      // Build neighborhood index list of this pixel
      ImageIndexListType relativeNeighborhood;
      ImageIndexType north = {currentIndex[0], currentIndex[1]+1};
      relativeNeighborhood.push_back(north);
      ImageIndexType south = {currentIndex[0],currentIndex[1]-1};
      relativeNeighborhood.push_back(south);
      ImageIndexType east  = {currentIndex[0]+1,currentIndex[1]};
      relativeNeighborhood.push_back(east);
      ImageIndexType west = {currentIndex[0]-1,currentIndex[1]};
      relativeNeighborhood.push_back(west);

      // Check the validity of these neighbors
      for (unsigned int k = 0 ; k < 4 ; k++)
        {
        ImageIndexType neighbor = relativeNeighborhood[k];

        // Does the neighbor lean inside the image?
        if (inputImage->GetLargestPossibleRegion().IsInside(neighbor))
          {

          // Value of the neighbor?
          if (inputImage->GetPixel(neighbor) ==
              inputImage->GetPixel(indexList[0]))
            {
            // Last check: is the neighbor already in the list?
            bool already_in = false;
            for (unsigned int j = 0; j < indexList.size(); j++)
              {
              if (indexList.at(j)[0] == neighbor[0] && indexList.at(j)[1] == neighbor[1])
                {
                already_in = true;
                break;
                }
              }

            if (!already_in)
              {
              // The neighbor has the same value, we add it and we iterate
              indexList.push_back(neighbor);
              markedList.push_back(false);
              if (indexList.size()>m_MinNumberOfComponents)
                return true;
              else
                return UpdateConnectedIndexList(markedList, indexList);
              }
            }

          }
        }

      // this index is marked, as be have walked through its entire neighborhood
      markedList[i] = true;

      } // pixel not marked
    } // next
  return false;
}

/**
 *
 */
template <class TImage>
void
ConnectedLabelsImageFilter<TImage>
::ThreadedGenerateData(const ImageRegionType& outputRegionForThread, itk::ThreadIdType threadId)
 {

  // Debug info
  itkDebugMacro(<<"Actually executing thread " << threadId << " in region " << outputRegionForThread);

  // Support progress methods/callbacks
  itk::ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels() );

  // Iterate through the thread region
  OutputImageIteratorType outputIt(this->GetOutput(), outputRegionForThread);
  InputImageIteratorType inputIt(this->GetInput(), outputRegionForThread);

  for ( outputIt.GoToBegin(), inputIt.GoToBegin(); !outputIt.IsAtEnd(); ++outputIt, ++inputIt )
    {
    // Output pixel
    ImagePixelType outPix (m_NoDataPixel);

    // Create a list with the central pixel inside it
    ImageIndexType index = inputIt.GetIndex();

    if (inputIt.Get() != m_NoDataPixel)
      {
      ImageIndexListType connectedPixels;
      BooleanListType markedList;
      connectedPixels.push_back(index);
      markedList.push_back(false);

      if (UpdateConnectedIndexList(markedList, connectedPixels ))
        {
        // output pixel
        outPix = inputIt.Get();
        }
      }

    outputIt.Set(outPix);
    } // Next pixel
 }
}
#endif



