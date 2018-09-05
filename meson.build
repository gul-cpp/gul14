# Build specification for the General Utilities Library

project('libgul', 'cpp',
    default_options : [ 'cpp_std=c++14' ],
    version : '0.1',
    meson_version : '>=0.29')

# The API version is the major version of the project.
# Increase the major project version in project() above on API breaking changes.
LIBGUL_API_VERSION = meson.project_version().split('.')[0]

# Find out if this is a normal (not local) install (not into prefix/obj/)
deb_install = get_option('bindir').startswith('obj/') == false
message('Install for debian packaging: @0@'.format(deb_install))

subdir('include')
subdir('source')

subdir('resources')

subdir('tests')

# Add debian/* files
if deb_install
#   subdir('debian.in')  to be added
endif
