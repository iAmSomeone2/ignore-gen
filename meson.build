project('ignore-gen', 'cpp',
  version : '0.0.1',
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
cdata = configuration_data()
if get_option('buildtype') == 'release'
  cdata.set_quoted('DB_FILE', '{datadir}/{projectname}/ignore.db')
else
  cdata.set_quoted('DB_FILE', 'data/ignore.db')
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

# Handle build targets
data_files = files('data/ignore.db')
install_data(data_files, install_mode: 'rw-r--r--')

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
