project('pydflow', 'c', 'cpp')

cc = meson.get_compiler('cpp')  # Compiler-Variable definieren

# Comment this line to restore warnings of experimental DOCA features
add_project_arguments('-D DOCA_ALLOW_EXPERIMENTAL_API', language: ['c', 'cpp'])

py = import('python').find_installation()
deps = []
deps += dependency('pybind11')
deps += dependency('doca-common')
deps += dependency('doca-dpdk-bridge')
deps += dependency('doca-flow')
deps += dependency('doca-argp')
deps += dependency('libdpdk')

inc = []
inc += include_directories('.')
# Common DOCA library logic
inc += include_directories('/opt/mellanox/doca/samples/doca_flow')
# Common DOCA logic (samples)
inc += include_directories('/opt/mellanox/doca/samples')
# Common DOCA logic (applications)
inc += include_directories('/opt/mellanox/doca/applications/common/')

srcs = [
  'pydflow.cpp',
  'pipe.cpp',
	# Common code for the DOCA library samples
	'/opt/mellanox/doca/samples/doca_flow/flow_common.c',
	# Flow switch common code for the DOCA library samples
	'/opt/mellanox/doca/samples/doca_flow/flow_switch_common.c',
	# Common code for all DOCA samples
	'/opt/mellanox/doca/samples/common.c',
	# Common code for all DOCA applications
	'/opt/mellanox/doca/applications/common/dpdk_utils.c',
]

# C library
libexample = library(
  'pydflow_c',
  [srcs],
  dependencies: [deps],
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