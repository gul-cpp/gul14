# Build specification for the General Utilities Library

project('libgul', 'cpp',
    default_options : [ 'cpp_std=c++14',
                        'warning_level=3',
                        'datadir=share' ],
    version : '0.1',
    meson_version : '>=0.37')

# The API version is the major version of the project.
# Increase the major project version in project() above on API breaking changes.
version_parts = meson.project_version().split('.')
libgul_api_version = '@0@.@1@'.format(version_parts[0], version_parts[1])

# Find out if this is a normal (not local) install (i.e. not into /usr/local)
deb_install = get_option('prefix') != '/usr/local'

subdir('include')
subdir('src')

subdir('tools')
subdir('data')

subdir('tests')

# Add debian/* files
message('Install prefix: ' + get_option('prefix'))
if deb_install
    message('Install for debian packaging: @0@'.format(deb_install))
    subdir('debian')
else
    message('Install for debian packaging: @0@ (packets installing files to /usr/local forbidden)'.format(deb_install))
    message('                              Enable by specifying correct prefix e.g. --prefix=/usr')
endif

message('LIBGUL_API_VERSION @0@'.format(libgul_api_version))
message('GIT VERSION        @0@'.format(git_version))
message('GIT EXTERNAL TAG   @0@'.format(git_external))

# Check if the git version (git tag starting with 'v') interpreted as
# semantic version number matches the specified API version from above
if not (git_version == 'unknown')
    git_vers_parts = git_version.split('.')
    git_major_version_parts = git_vers_parts[0].split('v')
    git_api_version = '@0@.@1@'.format(git_major_version_parts[1], git_vers_parts[1])
    if not (git_api_version == libgul_api_version)
        message('API version parsed from GIT version: @0@'.format(git_api_version))
        error('LIBGUL_API_VERSION and API version deduced from GIT VERSION do not match')
    endif
endif
