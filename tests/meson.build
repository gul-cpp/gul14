# Specification for all tests that are provided with libgul
# Run the tests with `ninja test` in the build dir

test_inc = inc + [ include_directories('.') ]
catch_main = 'test_main.cc'

# Build all the tests

t1 = executable('backports-test',
                [ 'test_backports.cc', catch_main ],
                include_directories : test_inc,
)

t2 = executable('cat-test',
                [ 'test_cat.cc', catch_main ],
                include_directories : test_inc,
                link_with : libgul
)

# Run the tests

test('Backports test', t1)
test('Testing cat()', t2)

test('String split and join',
	executable('test-string-split',
		[ 'test_string_split.cc', catch_main ],
		include_directories : test_inc,
		link_with : libgul),
	timeout : 2)
