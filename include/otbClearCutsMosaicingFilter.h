/*=========================================================================

  Copyright (c) Remi Cresson (IRSTEA). All rights reserved.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __ClearCutsMosaicingFilter_H
#define __ClearCutsMosaicingFilter_H

#include "otbStreamingMosaicFilterBase.h"

namespace otb
{
/** \class ClearCutsMosaicingFilter
 * \brief Computes the mosaic of an input clear cuts label images set.
 *
 * Support streaming
 *
 * The pixels must support the operator ==, +, /, etc.
 * The "no data value", output spacing, interpolator can be chosen.
 * The behavior of the filter is to put layers in the same order
 * as they are in input
 *
 * \ingroup ClearCutsDetection
 */
template <class TInputImage, class TOutputImage, class TInternalValueType, class TFunctorType>
class ITK_EXPORT ClearCutsMosaicingFilter :
public otb::StreamingMosaicFilterBase<TInputImage, TOutputImage, TInternalValueType>
{
public:

  /** Standard Self typedef */
  typedef ClearCutsMosaicingFilter                                                   Self;
  typedef otb::StreamingMosaicFilterBase<TInputImage, TOutputImage, TInternalValueType> Superclass;
  typedef itk::SmartPointer<Self>                                                       Pointer;
  typedef itk::SmartPointer<const Self>                                                 ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(ClearCutsMosaicingFilter, StreamingMosaicFilterBase);

  /** Input image typedefs.  */
  typedef typename Superclass::InputImageType          InputImageType;
  typedef typename Superclass::InputImagePixelType     InputImagePixelType;
  typedef typename Superclass::IteratorType            IteratorType;
  typedef typename Superclass::InterpolatorPointerType InterpolatorPointerType;
  typedef typename Superclass::InputImageRegionType    InputImageRegionType;

  /** Output image typedefs.  */
  typedef typename Superclass::OutputImageType              OutputImageType;
  typedef typename Superclass::OutputImagePointType         OutputImagePointType;
  typedef typename Superclass::OutputImagePixelType         OutputImagePixelType;
  typedef typename Superclass::OutputImageInternalPixelType OutputImageInternalPixelType;
  typedef typename Superclass::OutputImageRegionType        OutputImageRegionType;

  /** Internal computing typedef support. */
  typedef typename Superclass::InternalValueType InternalValueType;
  typedef typename Superclass::InternalPixelType InternalPixelType;

protected:
  ClearCutsMosaicingFilter() {}

  virtual ~ClearCutsMosaicingFilter() {
  }

  /** Overrided methods */
  virtual void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, itk::ThreadIdType threadId );

private:
  ClearCutsMosaicingFilter(const Self&); //purposely not implemented
  void operator=(const Self&);              //purposely not implemented

  TFunctorType m_Functor;

}; // end of class

} // end namespace otb

#ifndef OTB_MANUAL_INSTANTIATION
#include "otbClearCutsMosaicingFilter.hxx"
#endif

#endif
