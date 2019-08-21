# Specification for all inclusion tests
#
# We just try to compile and link all the tests.
# The resulting 'library' is not used at all.
#
# This should make sure that the header files are indeed
# self contained.

incl_tests = [
    'incl_case_ascii.cc',
    'incl_cat.cc',
    'incl_escape.cc',
    'incl_finalizer.cc',
    'incl_hexdump.cc',
    'incl_join_split.cc',
    'incl_num_util.cc',
    'incl_replace.cc',
    'incl_SlidingBuffer.cc',
    'incl_statistics.cc',
    'incl_substring_checks.cc',
    'incl_time_util.cc',
    'incl_tokenize.cc',
    'incl_to_number.cc',
    'incl_Trigger.cc',
    'incl_trim.cc',
    'incl_type_name.cc',
]

static_library('include_test', incl_tests,
    cpp_args : add_cpp_args,
    include_directories : inc,
    install : false,
    link_with : libgul )

# vi:ts=4:sw=4:sts=4:et
