# Build specification for the General Utilities Library

# Increase the major project version in project() on API breaking changes.
# Increase the minor project version in project() on API extensions

project('gul14', 'cpp',
    default_options : [
        'cpp_std=c++14',
        'warning_level=3',
    ],
    version : '2.3',
    meson_version : '>=0.48')

# Enforce that the version number is according to specs
version_parts = meson.project_version().split('.')
libgul_api_version = '@0@.@1@'.format(version_parts[0], version_parts[1])

subdir('include')
subdir('src')

subdir('tools')
subdir('data')

subdir('tests')

message('Install prefix: ' + get_option('prefix'))
subdir('debian')

message('LIBGUL_API_VERSION       @0@'.format(libgul_api_version))
message('GIT FULL API VERSION    @0@'.format(git_full_api_version))
message('GIT VERSION TAG         @0@'.format(git_version))
if deb_install
    message('Debian package names    ' + deb_name + ', ' + deb_dev_name)
    message('Debian package version  ' + debian_package_version)
endif
if git_version == 'unknown'
    message('Shared library version patchlevel can\'t be determined from git')
endif
message('Shared library version  ' + so_version)

# Check if the git version (git tag starting with 'v') interpreted as
# semantic version number matches the specified API version from above
if not (git_full_api_version == 'unknown')
    git_vers_parts = git_full_api_version.split('.')
    git_major_version_parts = git_vers_parts[0].split('v')
    git_api_version = '@0@.@1@'.format(git_major_version_parts[1], git_vers_parts[1].split('-')[0])
    if not (git_api_version == libgul_api_version)
        message('API version parsed from GIT version: @0@'.format(git_api_version))
        error('LIBGUL_API_VERSION and API version deduced from GIT VERSION do not match')
    endif
endif

# vi:ts=4:sw=4:sts=4:et:syn=conf
