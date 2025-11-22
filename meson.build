project('pydflow', 'c', 'cpp')

py = import('python').find_installation()
pybind = dependency('pybind11')

inc = include_directories('.')

# C library
libexample = library(
  'pydflow_c',
  ['pydflow.c'],
  include_directories: inc,
  install: false
)

# Python extension module
py.extension_module(
  'pydflow',
  ['bindings.cpp'],
  include_directories: inc,
  dependencies: [pybind],
  link_with: libexample,
  install: true
)