# Select which files are to be installed together with the library

libgul_main_header = files([
    'gul.h',
])

install_headers(libgul_main_header)

libgul_sub_headers = files([
    'gul/cat.h',
    'gul/catch.h',
    'gul/escape.h',
    'gul/finalizer.h',
    'gul/hexdump.h',
    'gul/join_split.h',
    'gul/num_util.h',
    'gul/replace.h',
    'gul/SlidingBuffer.h',
    'gul/statistics.h',
    'gul/string_util.h',
    'gul/string_view.h',
    'gul/substring_checks.h',
    'gul/time_util.h',
    'gul/tokenize.h',
    'gul/Trigger.h',
    'gul/trim.h',
    'gul/type_name.h',
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
