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

#ifndef ELEMENTINTEGRALINDICATOR_H
#define ELEMENTINTEGRALINDICATOR_H

#include "ElementIndicator.h"

class ElementIntegralIndicator;

template<>
InputParameters validParams<ElementIntegralIndicator>();

class ElementIntegralIndicator :
  public ElementIndicator
{
public:
  ElementIntegralIndicator(const InputParameters & parameters);
  ElementIntegralIndicator(const std::string & deprecated_name, InputParameters parameters); // DEPRECATED CONSTRUCTOR
  virtual ~ElementIntegralIndicator(){};

  virtual void computeIndicator();

protected:
  virtual Real computeQpIntegral();
};

#endif /* ELEMENTINTEGRALINDICATOR_H */
