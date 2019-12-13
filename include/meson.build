# Select which files are to be installed together with the library

libgul_main_header = files([
    'gul.h'
])

install_headers(libgul_main_header)

# The standalone_headers are tested for self-containment in the tests section
standalone_headers = [
    'gul14/bit_manip.h',
    'gul14/case_ascii.h',
    'gul14/cat.h',
    'gul14/escape.h',
    'gul14/finalizer.h',
    'gul14/hexdump.h',
    'gul14/join_split.h',
    'gul14/num_util.h',
    'gul14/replace.h',
    'gul14/SlidingBuffer.h',
    'gul14/span.h',
    'gul14/statistics.h',
    'gul14/string_util.h',
    'gul14/substring_checks.h',
    'gul14/time_util.h',
    'gul14/to_number.h',
    'gul14/tokenize.h',
    'gul14/Trigger.h',
    'gul14/trim.h',
    'gul14/type_name.h',
]

libgul_sub_headers = files(standalone_headers)
libgul_sub_headers += files([
    'gul14/gul.h',
    'gul14/catch.h',
    'gul/catch.h',
    'gul14/internal.h',
    'gul14/optional.h',
    'gul14/string_view.h',
    'gul14/version.h',
])

install_headers(
    libgul_sub_headers,
    subdir : 'gul14'
)

# For building the library we use all headers:
inc = [ include_directories([
    '.',
    'gul14'
])]

# vi:ts=4:sw=4:sts=4:et
