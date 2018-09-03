# Specification for all tests that are provided with libgul
# Run the tests with `ninja test` in the build dir

test_inc = inc + [ include_directories('.') ]
catch_main = 'test_main.cc'

test('Backports',
    executable('backports-test',
            [ 'test_backports.cc', catch_main ],
            include_directories : test_inc,
            link_with : libgul))

test('cat()',
    executable('cat-test',
            [ 'test_cat.cc', catch_main ],
            include_directories : test_inc,
            link_with : libgul))

test('split() and join()',
	executable('test-string-split',
		[ 'test_string_split.cc', catch_main ],
		include_directories : test_inc,
		link_with : libgul),
	timeout : 2)

test('escape()',
	executable('test-string-escape',
		[ 'test_string_escape.cc', catch_main ],
		include_directories : test_inc,
		link_with : libgul))

test('replace() and replace_inplace()',
	executable('test-string-replace',
		[ 'test_string_replace.cc', catch_main ],
		include_directories : test_inc,
		link_with : libgul))

t3 = executable('tokenize-test',
                [ 'test_tokenize.cc', catch_main ],
                include_directories : test_inc,
                link_with : libgul
)

# Run the tests

test('Testing tokenize(), tokenize_string_view()', t3)
