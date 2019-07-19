# Specification for all tests that are provided with libgul
# Run the tests with `ninja test` in the build dir

if not get_option('tests')
    message('This build is defined to have no tests.')
    message('Use \'meson configure -Dtests=true\' to enable building tests (mesonconf for ancient meson versions).')
    # subdir_done() to avoid long 'else', meson 0.46+
else

test_inc = inc + [ include_directories('.') ]
catch_main = 'test_main.cc'
test_time = 2 # Default timeout

thread_dep = dependency('threads')

tests = [
    'test_backports.cc',
    'test_case_ascii.cc',
    'test_cat.cc',
    'test_escape.cc',
    'test_finalizer.cc',
    'test_hexdump.cc',
    'test_join_split.cc',
    'test_main.cc',
    'test_num_util.cc',
    'test_optional.cc',
    'test_replace.cc',
    'test_SlidingBuffer.cc',
    'test_statistics.cc',
    'test_substring_checks.cc',
    'test_time_util.cc',
    'test_tokenize.cc',
	'test_to_number.cc',
    'test_Trigger.cc',
    'test_trim.cc',
    'test_type_name.cc',
]

test('all',
     executable('libgul-test',
                tests + [ catch_main ],
                dependencies : thread_dep,
                include_directories : test_inc,
                link_with : libgul),
     timeout : test_time)

endif # if not get_option('tests') else
