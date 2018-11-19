# Select which files are to be installed together with the library

install_headers(
    'gul.h'
)

install_headers(
    'gul/cat.h',
    'gul/hexdump.h',
    'gul/join_split.h',
    'gul/string_util.h',
    'gul/string_view.h',
    'gul/time_util.h',
    'gul/tokenize.h',
    'gul/Trigger.h',
    'gul/trim.h',
    'gul/version.h',
    subdir : 'gul'
)

# For building the library we use all headers:
inc = [ include_directories([
    '.',
    'gul'
])]
