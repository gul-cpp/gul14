# Process all the source files of the library

libgul_src = [
    'cat.cc',
    'join_split.cc',
    'string_util.cc',
    'tokenize.cc',
]

inc += include_directories('.')

# We want the git version of this build to be available in the
# library. The library can then be queried to identify its built version.
# cvs_tag() is not used, because we want to be able to display it
git_exe = find_program('git', required : false, native : true)
if git_exe.found()
    git_vers = run_command(git_exe, '-C', meson.source_root(), 'describe', '--tags', '--always', '--dirty')
    if git_vers.returncode() != 0
        git_version = 'unknown'
    else
        git_version = git_vers.stdout().strip()
    endif
else
    git_version = git_vers.stdout().strip()
    git_version = 'unknown'
endif
git_conf = configuration_data()
git_conf.set('GIT_VERSION', git_version)
git_conf.set('LIBGUL_API_VERSION', LIBGUL_API_VERSION)
gitversion_cc = configure_file(input : 'gitversion.cc.in', output : 'gitversion.cc',
               configuration : git_conf)

libgul = shared_library('gul', libgul_src + [ gitversion_cc ],
                        soversion : LIBGUL_API_VERSION,
                        cpp_args : '-DLIBGUL_API_VERSION=@0@'.format(LIBGUL_API_VERSION),
                        include_directories : inc,
                        link_args : [ '-fvisibility=hidden', '-fwhole-program' ],
                        install : true)

libgul_s = static_library('gul', libgul_src + [ gitversion_cc ],
                          cpp_args : '-DLIBGUL_API_VERSION=@0@'.format(LIBGUL_API_VERSION),
                          include_directories : inc,
                          link_args : [ '-fvisibility=hidden', '-fwhole-program' ],
                          install : true)

if deb_install
    pkg = import('pkgconfig')
    pkg.generate(libraries : libgul,
                 name : 'libgul',
                 description : 'General Utility Library',
                 version: '@0@'.format(LIBGUL_API_VERSION))
endif
