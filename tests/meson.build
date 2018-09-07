# Specification for all tests that are provided with libgul
# Run the tests with `ninja test` in the build dir

test_inc = inc + [ include_directories('.') ]
catch_main = 'test_main.cc'
test_time = 2 # Default timeout

test('Backports',
     executable('backports-test',
                [ 'test_backports.cc', catch_main ],
                include_directories : test_inc,
                link_with : libgul),
     timeout : test_time)

test('cat()',
     executable('cat-test',
                [ 'test_cat.cc', catch_main ],
                include_directories : test_inc,
                link_with : libgul),
     timeout : test_time)

test('split() and join()',
     executable('string-split-test',
                [ 'test_string_split.cc', catch_main ],
                include_directories : test_inc,
                link_with : libgul),
     timeout : test_time)

test('escape()',
     executable('string-escape-test',
                [ 'test_string_escape.cc', catch_main ],
                include_directories : test_inc,
                link_with : libgul),
     timeout : test_time)

test('replace() and replace_inplace()',
     executable('string-replace-test',
                [ 'test_string_replace.cc', catch_main ],
                include_directories : test_inc,
                link_with : libgul),
     timeout : test_time)

test('Testing time utility functions',
     executable('time-util-test',
                [ 'test_time_util.cc', catch_main ],
                include_directories : test_inc,
                link_with : libgul),
     timeout : test_time)

test('Testing tokenize(), tokenize_string_view()',
     executable('tokenize-test',
                [ 'test_tokenize.cc', catch_main ],
                include_directories : test_inc,
                link_with : libgul),
     timeout : test_time)
