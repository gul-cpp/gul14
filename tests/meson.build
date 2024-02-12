# Specification for all tests that are provided with libgul
# Run the tests with `ninja test` in the build dir

if not get_option('tests')
    message('This build is defined to have no tests.')
    message('Use \'meson configure -Dtests=true\' to enable building tests (mesonconf for ancient meson versions).')
    subdir_done()
endif

# Find out if we run the tests on ARM (i.e. Raspberry)
if host_machine.cpu_family() == 'arm' or host_machine.cpu_family() == 'aarch64'
    test_cpp_args = [ '-DARM' ]
else
    test_cpp_args = []
endif

catch_main = 'test_main.cc'
test_time = 10 # Default timeout

thread_dep = dependency('threads')

tests = [
    'test_backports.cc',
    'test_bit_manip.cc',
    'test_case_ascii.cc',
    'test_cat.cc',
    'test_escape.cc',
    'test_expected.cc',
    'test_finalizer.cc',
    'test_gcd_lcm.cc',
    'test_hexdump.cc',
    'test_join_split.cc',
    'test_main.cc',
    'test_num_util.cc',
    'test_optional.cc',
    'test_replace.cc',
    'test_SlidingBuffer.cc',
    'test_SmallVector.cc',
    'test_statistics.cc',
    'test_string_util.cc',
    'test_substring_checks.cc',
    'test_time_util.cc',
    'test_to_number.cc',
    'test_tokenize.cc',
    'test_Trigger.cc',
    'test_trim.cc',
    'test_traits.cc',
    'test_type_name.cc',
    'test_variant.cc',
]

test('all',
    executable('libgul-test',
        tests + [ catch_main ],
        cpp_args : test_cpp_args,
        dependencies : [ thread_dep, libgul_dep ],
    ),
    timeout : test_time
)

######
# Test if all headers are self-contained (Core Guidelines SF.11)
# This is automated over all standalone_headers

message('Generating self-containment tests')
all_header_checks = [ ]
loop_number = 0
foreach header_to_check : standalone_headers
    loop_number += 1
    if header_to_check.endswith('.h')
        header_name = header_to_check.split('.h')[0]

        header_conf = configuration_data()
        header_conf.set('HEADER_TO_INCLUDE', meson.project_name() / header_to_check)
        header_conf.set('DUMMY_FUNCTION_NAME', 'a@0@'.format(loop_number))

        header_check = configure_file(
            input : 'test_self_containment.cc.in',
            output : 'incl_' + header_name + '.cc',
            configuration : header_conf,
        )
        all_header_checks += [ header_check ]
    endif
endforeach

static_library(
    '_ignore_me', all_header_checks,
    cpp_args : add_cpp_args,
    include_directories : inc,
    install : false,
)

# vi:ts=4:sw=4:sts=4:et:syn=conf
