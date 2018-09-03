# Process all the source files into the library

# We want the git version of this build to be available in the
# library. The library can then be queried to identify its built version.
gitversion_cc = vcs_tag(output : 'gitversion.cc',
                        input : 'gitversion.cc.in',
                        replace_string : '@VERSION@',
                        command : ['git', '-C', meson.source_root(), 'describe', '--tags', '--always', '--dirty'],
                        fallback : 'unknown')

libgul_src = [
    'cat.cc',
    'join_split.cc'
]

inc += include_directories('.')

libgul = both_libraries('gul', libgul_src + [ gitversion_cc ],
                        soversion : LIBGUL_API_VERSION,
                        cpp_args : '-DLIBGUL_API_VERSION=@0@'.format(LIBGUL_API_VERSION),
                        include_directories : inc,
                        link_args : [ '-fvisibility=hidden', '-fwhole-program' ],
                        install : true)

if deb_install
    pkg = import('pkgconfig')
    pkg.generate(libgul,
                 name : 'libgul',
                 description : 'General Utility Library',
                 filebase : 'libgul',
                 install_dir : '/usr/lib/pkgconfig',
                 version: '@0@'.format(LIBGUL_API_VERSION))
endif

