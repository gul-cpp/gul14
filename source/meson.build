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
    'join_split.cc',
    'string_util.cc',
    'tokenize.cc'
]

inc += include_directories('.')

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
