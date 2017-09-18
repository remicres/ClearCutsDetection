/*=========================================================================

  Copyright (c) Remi Cresson (IRSTEA). All rights reserved.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef DeltaNDVILabelerFilter_H_
#define DeltaNDVILabelerFilter_H_

#include "otbImage.h"
#include "itkImageToImageFilter.h"
#include "itkNumericTraits.h"
#include "itkSimpleDataObjectDecorator.h"

// No data
#include "otbNoDataHelper.h"

namespace otb
{

/**
 * \class DeltaNDVILabelerFilter
 * \brief Create a NDVI difference label image
 *
 * Input image:
 * -NDVI difference image
 *
 * Parameters:
 * -NDVI difference image mean (µ)
 * -NDVI difference image std (s)
 * -Number of classes (m_NumberOfClasses)
 * -Lower value of the first range (m_FirstClassStart). The first lower range is [µ-m_FirstClassStart*s, +inf]
 * -Label of the first range (m_FirstClassValue)
 *
 * The filter quantize the input dNDVI value in m_NumberOfClasses classes.
 * Ranges are computed like this:
 * -range (m_NumberOfClasses) is : ]-inf, µ-(m_NumberOfClasses+m_FirstClassStart)*s]
 * -range (m_NumberOfClasses-1) is : ]µ-(m_NumberOfClasses+m_FirstClassStart)*s, µ-(M_NumberOfClasses+m_FirstClassStart-1)*s]
 * -...
 * -range 1 is : ]µ-(1+m_FirstClassStart)*s, µ-m_FirstClassStart*s]
 * -range 0 is : ]µ-m_FirstClassStart*s, +inf[
 *
 * The range 0 starts with label m_FirstClassValue.
 *
 * Output: Labeled image
 *
 * \ingroup ClearCutsDetection
 */
template <class TNDVIImage, class TLabelImage>
class ITK_EXPORT DeltaNDVILabelerFilter : public itk::ImageToImageFilter<TNDVIImage, TLabelImage>
{

public:

	/** Standard class typedefs. */
	typedef DeltaNDVILabelerFilter			Self;
	typedef itk::ImageToImageFilter<TNDVIImage, TLabelImage>	Superclass;
	typedef itk::SmartPointer<Self>	Pointer;
	typedef itk::SmartPointer<const Self>	ConstPointer;

	/** Method for creation through the object factory. */
	itkNewMacro(Self);

	/** Run-time type information (and related methods). */
	itkTypeMacro(DeltaNDVILabelerFilter, itk::ImageToImageFilter);

	/** NDVI image typedefs. */
	typedef TNDVIImage					NDVIImageType;
	typedef typename NDVIImageType::PixelType	NDVIImagePixelType;

	/** Label image typedefs.  */
	typedef TLabelImage					LabelImageType;
	typedef typename LabelImageType::PixelType	LabelImagePixelType;

	/** Iterators typedefs */
	typedef typename LabelImageType::RegionType OutputImageRegionType;
	typedef itk::ImageRegionConstIterator<NDVIImageType> NDVIImageIteratorType;
	typedef itk::ImageRegionIterator<LabelImageType> LabelImageIteratorType;

	/** Decorator typedef */
	typedef typename itk::NumericTraits<NDVIImagePixelType>::RealType RealType;
	typedef itk::SimpleDataObjectDecorator<RealType>  RealObjectType;

	/** No Data Getters/Setters */
	itkGetMacro(InputNoDataValue, NDVIImagePixelType);
	itkSetMacro(InputNoDataValue, NDVIImagePixelType);
	itkGetMacro(OutputNoDataValue, LabelImagePixelType);
	itkSetMacro(OutputNoDataValue, LabelImagePixelType);

	/** Threshold Getters/Setters */
	itkGetMacro(NumberOfClasses, unsigned int);
	itkSetMacro(NumberOfClasses, unsigned int);
	itkGetMacro(FirstClassStart, int);
	itkSetMacro(FirstClassStart, int);
	itkGetMacro(FirstClassValue, LabelImagePixelType);
	itkSetMacro(FirstClassValue, LabelImagePixelType);

	/** Set the mean */
	void SetInputMeanObject(RealObjectType* inputMeanObject)
	{
		m_InputMeanObject = inputMeanObject;
	}

	/** Set the sigma */
	void SetInputSigmaObject(RealObjectType* inputSigmaObject)
	{
		m_InputSigmaObject = inputSigmaObject;
	}

protected:
	DeltaNDVILabelerFilter();
	virtual ~DeltaNDVILabelerFilter() {};

	virtual void GenerateOutputInformation(void);

	virtual void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, itk::ThreadIdType threadId);


private:
	DeltaNDVILabelerFilter(const Self&); //purposely not implemented
	void operator=(const Self&); //purposely not implemented

	// No data value
	NDVIImagePixelType	m_InputNoDataValue;
	LabelImagePixelType m_OutputNoDataValue;

	// Classes parameters
	unsigned int m_NumberOfClasses;
	int m_FirstClassStart;
	LabelImagePixelType m_FirstClassValue;

	vnl_vector<NDVIImagePixelType> inputThresholds;
	vnl_vector<LabelImagePixelType> outputValues;

	// Mean and sigma values
	RealObjectType* m_InputMeanObject;
	RealObjectType* m_InputSigmaObject;

};


} // end namespace gtb

#include "otbDeltaNDVILabelerFilter.hxx"


#endif /* DeltaNDVILabelerFilter_H_ */
