# Process all the source files of the library

libgul_src = [
    'cat.cc',
    'join_split.cc',
    'string_util.cc',
    'time_util.cc',
    'tokenize.cc',
    'Trigger.cc',
    'trim.cc',
]

inc += include_directories('.')

# We want the git and API versions of this build to be available in the
# library. The library can then be queried to identify its built version.
#
# Inside git we identify versions with two types of git tags:
# - Version tags
# - External/proprietary tags (for example Doocs version tags)
#
# The version tags have to follow this form (following https://semver.org):
# "vM.m.p" where M, m, and p are decimal numbers of Major, minor, and patch
# version numbers respectively. For example "v3.12.8".
git_exe = find_program('git', required : false, native : true)
if git_exe.found()
    git_vers = run_command(git_exe, '-C', meson.source_root(), 'describe', '--tags', '--always', '--dirty', '--match', 'v*')
    if git_vers.returncode() != 0
        git_version = 'unknown'
    else
        git_version = git_vers.stdout().strip()
    endif
    git_exte = run_command(git_exe, '-C', meson.source_root(), 'describe', '--tags', '--always', '--abbrev=0', '--match', '[^v]*')
    if git_exte.returncode() != 0
        git_external = 'unknown'
    else
        git_external = git_exte.stdout().strip()
    endif
else
    git_version = 'unknown'
    git_external = 'unknown'
endif

git_conf = configuration_data()
git_conf.set('GIT_VERSION', git_version)
git_conf.set('LIBGUL_API_VERSION', libgul_api_version)
version_cc = configure_file(input : 'version.cc.in', output : 'version.cc',
               configuration : git_conf)

# There seems to be a bug in ancient meson versions that prevent
# correct assignment and evaluation of cpp_args, so we do it here by hand :-(
add_cpp_args = [ '-DLIBGUL_API_VERSION=@0@'.format(libgul_api_version),
                 '-Wshadow' ]

libgul = shared_library('gul', libgul_src + [ version_cc ],
                        soversion : libgul_api_version,
                        cpp_args : add_cpp_args,
                        include_directories : inc,
                        link_args : [ '-fvisibility=hidden', '-fwhole-program' ],
                        install : true)

libgul_s = static_library('gul', libgul_src + [ version_cc ],
                          cpp_args : add_cpp_args,
                          include_directories : inc,
                          link_args : [ '-fvisibility=hidden', '-fwhole-program' ],
                          install : true)

pkg = import('pkgconfig')
pkg.generate(libraries : libgul,
             name : 'libgul',
             description : 'General Utility Library',
             version : '@0@'.format(libgul_api_version))
