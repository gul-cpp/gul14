# Specification for all tests that are provided with libgul
# Run the tests with `ninja test` in the build dir

test_inc = inc + [ include_directories('.') ]
catch_main = 'test_main.cc'

t1= executable('backports-test',
                [ 'test_backports.cc', catch_main ],
                include_directories : test_inc,
             #   link_with : libgul))
	     )

test('Backports test', t1)
