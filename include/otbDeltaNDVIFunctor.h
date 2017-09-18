/*=========================================================================

  Copyright (c) Remi Cresson (IRSTEA). All rights reserved.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbDeltaNDVIFunctor_h
#define __otbDeltaNDVIFunctor_h

#include "itkNumericTraits.h"

namespace otb
{

namespace Functor
{

/** \class DeltaNDVIFromChannels
 *  \brief This functor computes the difference between NDVI from two pixels
 *
 *  \ingroup ClearCutsDetection
 *
 */
template< class TPixel, class TOutputValue>
class DeltaNDVIFromChannels
{
public:
  DeltaNDVIFromChannels() {noData=3.0; nirChannelT0=0; nirChannelT1=0; redChannelT0=0; redChannelT1=0;}
  ~DeltaNDVIFromChannels() {}
  bool operator!=(const DeltaNDVIFromChannels &) const{
    return false;
  }

  bool operator==(const DeltaNDVIFromChannels & other) const{
    return !( *this != other );
  }

  inline TOutputValue operator()(const TPixel & T0,
      const TPixel & T1) const
  {
    double nir_t0 = static_cast< double >( T0[nirChannelT0]);
    double nir_t1 = static_cast< double >( T1[nirChannelT1]);
    double red_t0 = static_cast< double >( T0[redChannelT0]);
    double red_t1 = static_cast< double >( T1[redChannelT1]);
    if (vcl_abs(nir_t0 + red_t0) > 0.0001 && vcl_abs(nir_t1 + red_t1) > 0.0001)
      {
        return static_cast< TOutputValue >(
            (nir_t1 - red_t1) / (nir_t1 + red_t1)
            -(nir_t0 - red_t0) / (nir_t0 + red_t0) );
      }
    else
      {
        return static_cast< TOutputValue >( noData );
      }
  }

  void SetNoDataValue(double value) {noData = value; }
  void SetNIRChannelT0(unsigned int number) {nirChannelT0 = number - 1;}
  void SetRedChannelT0(unsigned int number) {redChannelT0 = number - 1;}
  unsigned int GetNIRChannelT0() {return nirChannelT0 ;}
  unsigned int GetRedChannelT0() {return redChannelT0 ;}

  double GetNoDataValue() {return noData;}
  void SetNIRChannelT1(unsigned int number) {nirChannelT1 = number - 1;}
  void SetRedChannelT1(unsigned int number) {redChannelT1 = number - 1;}
  unsigned int GetNIRChannelT1() {return nirChannelT1 ;}
  unsigned int GetRedChannelT1() {return redChannelT1 ;}


private:
  unsigned int nirChannelT0;
  unsigned int redChannelT0;
  unsigned int nirChannelT1;
  unsigned int redChannelT1;
  double noData;

};

} // namespace Functor
} // namespace otb

#endif

