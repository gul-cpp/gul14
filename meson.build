# Build specification for the General Utilities Library

project('libgul', 'cpp',
    default_options : [ 'cpp_std=c++14' ],
    version : '0.1')

LIBGUL_API_VERSION = 1

# Find out if this is a normal (not local) install (not into prefix/obj/)
deb_install = get_option('bindir').startswith('obj/') == false
message('Install for debian packaging: @0@'.format(deb_install))

subdir('include')
subdir('source')

subdir('tests')

# Add debian/* files
if deb_install
#   subdir('debian.in')  to be added
endif
