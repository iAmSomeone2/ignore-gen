project('ignore-gen', 'cpp',
  version : '0.0.1',
  meson_version: '>=0.56.0',
  default_options : [
    'warning_level=3',
    'cpp_std=c++17'])

summary({'bindir': get_option('bindir'),
        'libdir': get_option('libdir'),
        'datadir': get_option('datadir'),
        }, section: 'Directories')

summary({'buildtype': get_option('buildtype'),
        }, section: 'Configuration')

# Configure project
install_prefix = get_option('prefix')
datadir = get_option('datadir')
is_package_build = get_option('package_build')
cdata = configuration_data()
if is_package_build
  cdata.set_quoted('DB_FILE', join_paths(install_prefix, datadir, meson.project_name(), 'ignore.db'))
else
  cdata.set_quoted('DB_FILE', join_paths(meson.project_build_root(), 'data', 'ignore.db'))
endif

cdata.set_quoted('PROGRAM_NAME', 'ignore-gen')
cdata.set_quoted('PROGRAM_VERSION', meson.project_version())
config_header = files('projconfig.h.in')
configure_file(configuration: cdata,
  input: config_header,
  output: 'projconfig.h')
config_include = include_directories('.')

# Check dependencies
sqlite_dep = dependency('sqlite3',
  version: '>=3.36',
  required: true)
fmt_dep = dependency('fmt',
  version: '>=8.1',
  required: true)

copy = find_program('pkg/copy.py')

# Handle build targets
data_files = files('data/ignore.db')
install_data(data_files, install_mode: 'rw-r--r--')
if is_package_build
  message('Copying data files to build directory')
  run_command(
    copy,
    join_paths('data', 'ignore.db'),
    join_paths('data', 'ignore.db'),
    check: true
  )
endif

gitignorio_srcs = files(
  'src/main.cpp',
  'src/IgnoreEntry.cpp',
  'src/DbManager.cpp')
exe = executable('ignore-gen', gitignorio_srcs,
  dependencies: [
    sqlite_dep,
    fmt_dep
  ],
  include_directories: [
    config_include
  ],
  install : true)
