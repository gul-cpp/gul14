# Process all the source files of the library

libgul_src = files([
    'case_ascii.cc',
    'cat.cc',
    'escape.cc',
    'join_split.cc',
    'replace.cc',
    'string_util.cc',
    'tokenize.cc',
    'Trigger.cc',
    'trim.cc',
])

inc += include_directories('.')

git_exe = find_program('git', required : false, native : true)
if git_exe.found()

    # Get full description based on git tag API version ('v0.2-22-g3d8867c-dirty')
    git_f_api = run_command(
        git_exe, '-C', meson.current_source_dir(),
            'describe', '--tags', '--always', '--dirty', '--match', 'v*'
    )
    if git_f_api.returncode() != 0
        git_full_api_version = 'unknown'
    else
        git_full_api_version = git_f_api.stdout().strip()
    endif

    # Get description based on git version tag ('v0.2' or 'DOOCSVERSION_18_11_6')
    git_vers = run_command(
        git_exe, '-C', meson.current_source_dir(),
            'describe', '--tags', '--always', '--abbrev=0', '--match',
            get_option('deb-vers-tag') + '*'
    )
    if git_vers.returncode() != 0
        git_version = 'unknown'
    else
        git_version = git_vers.stdout().strip()
        git_version_pure = git_version.split(get_option('deb-vers-tag'))[1]
        git_version_parts = git_version_pure.split('_')
        git_version_parts = '-'.join(git_version_parts).split('-')
        git_version_num = '.'.join(git_version_parts)
        git_version_parts = git_version_num.split('.')
    endif

    # Get the number of commits since the relevant version tag
    git_pat = run_command(
        git_exe, '-C', meson.current_source_dir(),
            'rev-list', '--count', git_version + '..'
    )
    if git_pat.returncode() == 0
        # Ignore patchlevel if it is zero
        git_patch = git_pat.stdout().strip()
        if git_patch != '0'
            if git_version_num.split('.').length() >= 3
                # If we already have 3 numbers add patchlevel with letter p
                git_patch = '.p' + git_patch
            else
                git_patch = '.' + git_patch
            endif
        endif
    endif

else
    # Have no information whatsoever
    git_full_api_version = 'unknown'
    git_version = 'unknown'
endif

git_conf = configuration_data()
git_conf.set('GIT_FULL_API_VERSION', git_full_api_version)
git_conf.set('LIBGUL_API_VERSION', libgul_api_version)
version_cc = configure_file(
    input : 'version.cc.in',
    output : 'version.cc',
    configuration : git_conf,
)

so_version = (git_version != 'unknown') ? git_version_num : libgul_api_version

add_cpp_args = [
    '-DLIBGUL_API_VERSION=@0@'.format(libgul_api_version),
    '-DGUL_COMPILING_SHARED_LIB',
]
if meson.get_compiler('cpp').has_argument('-Wshadow') # msvc doesnt have
    add_cpp_args += [ '-Wshadow' ]
endif

libgul = both_libraries(
    meson.project_name(), libgul_src + [ version_cc ],
    soversion : so_version,
    cpp_args : add_cpp_args,
    include_directories : inc,
    gnu_symbol_visibility : 'hidden',
    install : true
)
libgul_dep = declare_dependency(
    include_directories : inc,
    link_with : libgul.get_shared_lib(),
)
libgul_static_dep = declare_dependency(
    include_directories : inc,
    link_with : libgul.get_static_lib(),
)

pkg = import('pkgconfig')
pkg.generate(
    libgul,
    name : 'lib' + meson.project_name(), # This is in fact wrong (but usually correct)
    description : 'General Utility Library',
    version : '@0@'.format(libgul_api_version)
)

# vi:ts=4:sw=4:sts=4:et:syn=conf
