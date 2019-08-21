# Select which files are to be installed together with the library

libgul_main_header = files([
    'gul.h',
])

install_headers(libgul_main_header)

# The standalone_headers are tested for self-containment in the tests section
standalone_headers = [
    'gul/case_ascii.h',
    'gul/cat.h',
    'gul/escape.h',
    'gul/finalizer.h',
    'gul/hexdump.h',
    'gul/join_split.h',
    'gul/num_util.h',
    'gul/replace.h',
    'gul/SlidingBuffer.h',
    'gul/statistics.h',
    'gul/substring_checks.h',
    'gul/time_util.h',
    'gul/to_number.h',
    'gul/tokenize.h',
    'gul/Trigger.h',
    'gul/trim.h',
    'gul/type_name.h',
]

libgul_sub_headers = files(standalone_headers)
libgul_sub_headers += files([
    'gul/catch.h',
    'gul/internal.h',
    'gul/optional.h',
    'gul/string_util.h',
    'gul/string_view.h',
    'gul/version.h',
])

install_headers(
    libgul_sub_headers,
    subdir : 'gul'
)

# For building the library we use all headers:
inc = [ include_directories([
    '.',
    'gul'
])]

# vi:ts=4:sw=4:sts=4:et
