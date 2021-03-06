/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#ifndef PIECEWISEBILINEAR_H
#define PIECEWISEBILINEAR_H

#include "Function.h"

class PiecewiseBilinear;
class ColumnMajorMatrix;
class BilinearInterpolation;

template<>
InputParameters validParams<PiecewiseBilinear>();

/**
 * PiecewiseBilinear reads from a file the information necessary to build the vectors x and y and
 * the ColumnMajorMatrix z, and then sends those (along with a sample point) to BilinearInterpolation.
 * See BilinearInterpolation in moose/src/utils for a description of how that works...it is a 2D linear
 * interpolation algorithm.  The format of the data file must be the following:
 *
 * 1,2
 * 1,1,2
 * 2,3,4
 *
 * The first row is the x vector data.
 * After the first row, the first column is the y vector data.
 * The rest of the data is used to build the ColumnMajorMatrix z.
 *
 * x = [1 2]
 * y = [1 2]
 *
 * z = [1 2]
 *     [3 4]
 *
 *     z has to be x.size() by y.size()
 *
 * PiecewisBilinear also sends samples to BilinearInterpolation.  These samples are the z-coordinate of the current
 * integration point, and the current value of time.  The name of the file that contains this data has to be included
 * in the function block of the inpute file like this...data_file = example.csv.
 */
class PiecewiseBilinear : public Function
{
public:
  PiecewiseBilinear(const InputParameters & parameters);
  virtual ~PiecewiseBilinear();

  /**
   * This function will return a value based on the first input argument only.
   */
  virtual Real value(Real t, const Point & pt);



private:
  MooseSharedPointer<BilinearInterpolation> _bilinear_interp;
  const std::string _data_file_name;
  const int _axis;
  const int _yaxis;
  const int _xaxis;
  const bool _axisValid;
  const bool _yaxisValid;
  const bool _xaxisValid;
  const Real _scale_factor;
  const bool _radial;


  void parse( std::vector<Real> & x,
              std::vector<Real> & y,
              ColumnMajorMatrix & z);
};

#endif //PIECEWISEBILINEAR_H
