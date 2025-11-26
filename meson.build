project('pydflow', 'c', 'cpp')

py = import('python').find_installation()
deps = []
deps += dependency('pybind11')
deps += dependency('doca-common')
deps += dependency('doca-dpdk-bridge')
deps += dependency('doca-flow')
deps += dependency('doca-argp')
deps += dependency('libdpdk')

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
  dependencies: [deps],
  link_with: libexample,
  install: true
)