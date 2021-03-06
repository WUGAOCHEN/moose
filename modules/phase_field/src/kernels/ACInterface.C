/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#include "ACInterface.h"

template<>
InputParameters validParams<ACInterface>()
{
  InputParameters params = validParams<Kernel>();
  params.addClassDescription("Gradient energy Allen-Cahn Kernel");
  params.addParam<MaterialPropertyName>("mob_name", "L", "The mobility used with the kernel");
  params.addParam<MaterialPropertyName>("kappa_name", "kappa_op", "The kappa used with the kernel");
  params.addCoupledVar("args", "Vector of nonlinear variable arguments this object depends on");

  params.addParam<bool>("variable_L", true, "The mobility is a function of any non-linear variable");
  params.addParam<bool>("variable_kappa", false, "Kappa is a function of any non-linear variable (must use ACInterfaceKappa Kernel along with this option)");

  return params;
}

ACInterface::ACInterface(const InputParameters & parameters) :
    DerivativeMaterialInterface<JvarMapInterface<Kernel> >(parameters),
    _L(getMaterialProperty<Real>("mob_name")),
    _kappa(getMaterialProperty<Real>("kappa_name")),
    _variable_L(getParam<bool>("variable_L")),
    _variable_kappa(getParam<bool>("variable_kappa")),
    _dLdop(getMaterialPropertyDerivative<Real>("mob_name", _var.name())),
    _d2Ldop2(getMaterialPropertyDerivative<Real>("mob_name", _var.name(), _var.name())),
    _dkappadop(getMaterialPropertyDerivative<Real>("kappa_name", _var.name())),
    _d2kappadop2(getMaterialPropertyDerivative<Real>("kappa_name", _var.name(), _var.name())),
    _nvar(_coupled_moose_vars.size()),
    _dLdarg(_nvar),
    _d2Ldargdop(_nvar),
    _d2Ldarg2(_nvar),
    _dkappadarg(_nvar),
    _d2kappadargdop(_nvar),
    _d2kappadarg2(_nvar),
    _gradarg(_nvar)
{
  // Get mobility and kappa derivatives and coupled variable gradients
  for (unsigned int i = 0; i < _nvar; ++i)
  {
    MooseVariable *ivar = _coupled_moose_vars[i];

    _dLdarg[i] = &getMaterialPropertyDerivative<Real>("mob_name", ivar->name());
    _dkappadarg[i] = &getMaterialPropertyDerivative<Real>("kappa_name", ivar->name());

    _d2Ldargdop[i] = &getMaterialPropertyDerivative<Real>("mob_name", ivar->name(), _var.name());
    _d2kappadargdop[i] = &getMaterialPropertyDerivative<Real>("kappa_name", ivar->name(), _var.name());

    _gradarg[i] = &(ivar->gradSln());

    _d2Ldarg2[i].resize(_nvar);
    _d2kappadarg2[i].resize(_nvar);
    for (unsigned int j = 0; j < _nvar; ++j)
    {
      MooseVariable *jvar = _coupled_moose_vars[j];

      _d2Ldarg2[i][j] = &getMaterialPropertyDerivative<Real>("mob_name", ivar->name(), jvar->name());
      _d2kappadarg2[i][j] = &getMaterialPropertyDerivative<Real>("kappa_name", ivar->name(), jvar->name());
    }
  }
}

void
ACInterface::initialSetup()
{
  validateCoupling<Real>("mob_name");
  validateCoupling<Real>("kappa_name");
}

RealGradient
ACInterface::gradL()
{
  RealGradient g = _grad_u[_qp] * _dLdop[_qp];
  for (unsigned int i = 0; i < _nvar; ++i)
    g += (*_gradarg[i])[_qp] * (*_dLdarg[i])[_qp];
  return g;
}

RealGradient
ACInterface::gradKappa()
{
  RealGradient g = _grad_u[_qp] * _dkappadop[_qp];
  for (unsigned int i = 0; i < _nvar; ++i)
    g += (*_gradarg[i])[_qp] * (*_dkappadarg[i])[_qp];
  return g;
}

RealGradient
ACInterface::nablaLKappaPsi()
{
  // sum is the product rule gradient \f$ \nabla (L\kappa\psi) \f$
  RealGradient sum = _kappa[_qp] * _L[_qp] * _grad_test[_i][_qp];

  if (_variable_L)
    sum += _kappa[_qp] * gradL() * _test[_i][_qp];

  if (_variable_kappa)
    sum += gradKappa() * _L[_qp] * _test[_i][_qp];

  return sum;
}

Real
ACInterface::computeQpResidual()
{
  return _grad_u[_qp] * nablaLKappaPsi();
}

Real
ACInterface::computeQpJacobian()
{
  RealGradient dsum = (_dkappadop[_qp] * _L[_qp] + _kappa[_qp] * _dLdop[_qp]) * _phi[_j][_qp] * _grad_test[_i][_qp];

  // compute the gradient of the mobility
  if (_variable_L)
  {
    RealGradient dgradL =   _grad_phi[_j][_qp] * _dLdop[_qp]
                          + _grad_u[_qp] * _phi[_j][_qp] * _d2Ldop2[_qp];
    for (unsigned int i = 0; i < _nvar; ++i)
      dgradL += (*_gradarg[i])[_qp] * _phi[_j][_qp] * (*_d2Ldargdop[i])[_qp];
    dsum += (_kappa[_qp] * dgradL + _dkappadop[_qp] * gradL()) * _test[_i][_qp];
  }

  // compute the gradient of the mobility
  if (_variable_kappa)
  {
    RealGradient dgradKappa =   _grad_phi[_j][_qp] * _dkappadop[_qp]
                              + _grad_u[_qp] * _phi[_j][_qp] * _d2kappadop2[_qp];
    for (unsigned int i = 0; i < _nvar; ++i)
      dgradKappa += (*_gradarg[i])[_qp] * _phi[_j][_qp] * (*_d2kappadargdop[i])[_qp];
    dsum += (dgradKappa * _L[_qp] + gradKappa() * _dLdop[_qp]) * _test[_i][_qp];
  }

  return _grad_phi[_j][_qp] * nablaLKappaPsi() + _grad_u[_qp] * dsum;
}

Real
ACInterface::computeQpOffDiagJacobian(unsigned int jvar)
{
  // get the coupled variable jvar is referring to
  unsigned int cvar;
  if (!mapJvarToCvar(jvar, cvar))
    return 0.0;

  // Set off-diagonal jaocbian terms from mobility dependence
  return _kappa[_qp] * (*_dLdarg[cvar])[_qp] * _phi[_j][_qp] * _grad_u[_qp] * _grad_test[_i][_qp];
}
