# Process all the source files of the library

libgul_src = [
    'cat.cc',
    'escape.cc',
    'join_split.cc',
    'replace.cc',
    'string_util.cc',
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

    # Get full description based on GUL version ('v0.2.0-22-g3d8867c-dirty')
    git_vers = run_command(git_exe, '-C', meson.source_root(),
                           'describe', '--tags', '--always', '--dirty', '--match', 'v*')
    if git_vers.returncode() != 0
        git_version = 'unknown'
    else
        git_version = git_vers.stdout().strip()
    endif

    # Get description based on external tag version ('DOOCSVERSION_18_11_6')
    git_exte = run_command(git_exe, '-C', meson.source_root(),
                           'describe', '--tags', '--always', '--abbrev=0', '--match', '[^v]*')
    if git_exte.returncode() != 0
        git_external = 'unknown'
    else
        git_external = git_exte.stdout().strip()
        git_external_parts = git_external.split('_')
        git_external_num = git_external_parts[1] + '.' + git_external_parts[2] + '.' + git_external_parts[3]
    endif

    # Get the number of commits since the relevant version tag
    if get_option('deb-vers-patch')
        if not get_option('deb-vers-ext')
            # Need to find the description based on GUL version ('v0.2.0')
            git_vers_b = run_command(git_exe, '-C', meson.source_root(),
                                     'describe', '--tags', '--always', '--abbrev=0', '--match', 'v*')
            if git_vers_b.returncode() != 0
                git_version_b = 'unknown'
            else
                git_version_b = git_vers_b.stdout().strip()
            endif
            # find number of commits based on 'v0.2.0'
            git_pat = run_command(git_exe, '-C', meson.source_root(),
                                  'rev-list', '--count', git_version_b + '..')
        else
            # find number of commits based on 'DOOCSVERSION_18_11_6')
            git_pat = run_command(git_exe, '-C', meson.source_root(),
                                  'rev-list', '--count', git_external + '..')
        endif
        if git_pat.returncode() != 0
            git_patch = ''
        else
            # Ignore patchlevel if it is zero
            git_patch = git_pat.stdout().strip()
            if git_patch != '0'
                git_patch = '-p' + git_patch
            else
                git_patch = ''
            endif
        endif
    endif

else
    # Have no information whatsoever
    git_version = 'unknown'
    git_external = 'unknown'
    git_patch = ''
endif

git_conf = configuration_data()
git_conf.set('GIT_VERSION', git_version)
git_conf.set('LIBGUL_API_VERSION', libgul_api_version)
version_cc = configure_file(input : 'version.cc.in', output : 'version.cc',
                            configuration : git_conf)

if not get_option('deb-vers-ext')
    so_version = libgul_api_version
else
    so_version = git_external_num
endif

if get_option('deb-vers-patch')
    so_version = so_version + git_patch
endif

# There seems to be a bug in meson versions prior 0.46.0 that prevent
# correct assignment and evaluation of cpp_args, so we do it here by hand :-(
add_cpp_args = [ '-DLIBGUL_API_VERSION=@0@'.format(libgul_api_version),
                 '-Wshadow' ]

libgul = shared_library('gul', libgul_src + [ version_cc ],
                        soversion : so_version,
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
