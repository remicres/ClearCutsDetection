/*=========================================================================

  Copyright (c) Remi Cresson (IRSTEA). All rights reserved.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef ConnectedLabelsImageFilter_H_
#define ConnectedLabelsImageFilter_H_

#include "otbImage.h"
#include "itkImageToImageFilter.h"
#include "itkNumericTraits.h"
#include "itkSimpleDataObjectDecorator.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"

// No data
#include "otbNoDataHelper.h"

namespace otb
{

/**
 * \class ConnectedLabelsImageFilter
 * \brief Filter an input image label
 *
 * Output: Filtered label image
 *
 * \ingroup ClearCutsDetection
 */
template <class TImage>
class ITK_EXPORT ConnectedLabelsImageFilter :
public itk::ImageToImageFilter<TImage, TImage>
{

public:

  /** Standard class typedefs. */
  typedef ConnectedLabelsImageFilter   Self;
  typedef itk::ImageToImageFilter<TImage, TImage>	Superclass;
  typedef itk::SmartPointer<Self>                 Pointer;
  typedef itk::SmartPointer<const Self>           ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ConnectedLabelsImageFilter, itk::ImageToImageFilter);

  /** Iterators typedefs */
  typedef TImage ImageType;

  typedef typename ImageType::RegionType  ImageRegionType;
  typedef typename ImageType::Pointer     ImagePointer;
  typedef typename ImageType::PixelType   ImagePixelType;
  typedef typename ImageType::IndexType   ImageIndexType;
  typedef typename itk::ImageRegionConstIterator<TImage>   InputImageIteratorType;
  typedef typename itk::ImageRegionIterator<TImage>        OutputImageIteratorType;
  typedef std::vector<ImageIndexType> ImageIndexListType;
  typedef std::vector<bool> BooleanListType;

  itkSetMacro(NoDataPixel, ImagePixelType);
  itkGetMacro(NoDataPixel, ImagePixelType);

  itkSetMacro(MinNumberOfComponents, unsigned int);
  itkGetMacro(MinNumberOfComponents, unsigned int);

protected:
  ConnectedLabelsImageFilter();
  virtual ~ConnectedLabelsImageFilter() {};

  virtual void GenerateInputRequestedRegion();

  virtual bool UpdateConnectedIndexList(BooleanListType& markedList,
      ImageIndexListType& indexList);

  virtual void ThreadedGenerateData(const ImageRegionType& outputRegionForThread,
      itk::ThreadIdType threadId);


private:
  ConnectedLabelsImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  unsigned int    m_MinNumberOfComponents;
  ImagePixelType  m_NoDataPixel;

};


} // end namespace gtb

#include <otbConnectedLabelsImageFilter.hxx>


#endif /* ConnectedLabelsImageFilter_H_ */
