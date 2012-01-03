#This is a test of the CLSHPlasticMaterial (creep linear strain hardening) model using the small strain formulation.  Note that this isn't a thermal or irradiation creep model.
#The material CLSHPlasticMaterial is a visco-plastic material i.e. a
#time-dependent linear strain hardening plasticity model.
#A similar problem was run in Abaqus with exactly the same result, although the element
#used in the Abaqus simulation was a CAX4 element.  Neverthless, due to the boundary conditions
#and load, the Bison and Abaqus result

[Mesh]
  file = 1x1x1cube.e

#  displacements = 'disp_x disp_y disp_z'
[]

[Variables]
  active = 'disp_x disp_y disp_z'

  [./disp_x]
    order = FIRST
    family = LAGRANGE
  [../]

  [./disp_y]
    order = FIRST
    family = LAGRANGE
  [../]

  [./disp_z]
    order = FIRST
    family = LAGRANGE
  [../]
[]


[AuxVariables]

#active = ' '

  [./stress_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]

  [./plastic_strain_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]

  [./plastic_strain_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]

  [./plastic_strain_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]


[Functions]
  active = 'top_pull'

  [./top_pull]
    type = ParsedFunction
    value = t/100
  [../]
[]

[SolidMechanics]
  [./solid]
    disp_x = disp_x
    disp_y = disp_y
    disp_z = disp_z
  [../]
[]

[AuxKernels]

#active = ' '

  [./stress_yy]
    type = MaterialTensorAux
    tensor = stress
    variable = stress_yy
    index = 1
  [../]

  [./plastic_strain_xx]
    type = MaterialTensorAux
    tensor = plastic_strain
    variable = plastic_strain_xx
    index = 0
  [../]

  [./plastic_strain_yy]
    type = MaterialTensorAux
    tensor = plastic_strain
    variable = plastic_strain_yy
    index = 1
  [../]

  [./plastic_strain_zz]
    type = MaterialTensorAux
    tensor = plastic_strain
    variable = plastic_strain_zz
    index = 2
  [../]

 []


[BCs]
  active = 'y_pull_function  y_bot x_bot z_bot'

  [./y_pull_function]
    type = FunctionDirichletBC
    variable = disp_y
    boundary = 5
    function = top_pull
  [../]

  [./x_bot]
    type = DirichletBC
    variable = disp_x
    boundary = 4
    value = 0.0
  [../]

  [./y_bot]
    type = DirichletBC
    variable = disp_y
    boundary = 3
    value = 0.0
  [../]

  [./z_bot]
    type = DirichletBC
    variable = disp_z
    boundary = 2
    value = 0.0
  [../]

[]

[Materials]
  active = 'constant'


  [./constant]
#    type = LinearStrainHardening
    type = CLSHPlasticMaterial
    formulation = LINEAr
    block = 1
    youngs_modulus = 1000.
    poissons_ratio = .3
    yield_stress = 10.
    hardening_constant = 100.
    c_alpha = 0.2418e-6
    c_beta = 0.1135
    tolerance = 1.e-5
    disp_x = disp_x
    disp_y = disp_y
    disp_z = disp_z
    print_debug_info = true
  [../]
[]

[Executioner]
  type = Transient
  petsc_options = '-snes_mf_operator -ksp_monitor -snes_ksp_ew'
  petsc_options_iname = '-snes_type -snes_ls -ksp_gmres_restart'
  petsc_options_value = 'ls         basic    101'

  l_max_its = 100
  nl_max_its = 100
  nl_rel_tol = 1e-12
  nl_abs_tol = 1e-10
  l_tol = 1e-9

  start_time = 0.0
#  end_time = 0.3
  num_steps = 30
  dt = 1.
[]



[Output]
  file_base = out
  interval = 1
  output_initial = true
  exodus = true
  perf_log = true
[]


