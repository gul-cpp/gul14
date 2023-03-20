# The standalone_headers are tested for self-containment in the tests section
standalone_headers = [
    'bit_manip.h',
    'case_ascii.h',
    'cat.h',
    'date.h',
    'escape.h',
    'expected.h',
    'finalizer.h',
    'gcd_lcm.h',
    'hexdump.h',
    'join_split.h',
    'num_util.h',
    'replace.h',
    'SlidingBuffer.h',
    'SmallVector.h',
    'span.h',
    'statistics.h',
    'string_util.h',
    'substring_checks.h',
    'time_util.h',
    'to_number.h',
    'tokenize.h',
    'traits.h',
    'Trigger.h',
    'trim.h',
    'type_name.h',
]

libgul_sub_headers = files(standalone_headers)
libgul_sub_headers += files([
    'gul.h',
    'catch.h',
    'internal.h',
    'optional.h',
    'string_view.h',
    'version.h',
])

install_headers(
    libgul_sub_headers,
    subdir : meson.project_name()
)

# vi:ts=4:sw=4:sts=4:et:syn=conf
