# Run all tests provided by Ryu, also the Ryu-printf ones

tests += files([
    'd2s_intrinsics_test.cc',
    'd2s_table_test.cc',
    'd2s_test.cc',
    'f2s_test.cc',
    'd2fixed_test.cc',
    'generic_128_test.cc',
])

# We need d2fixed for the d2fixed test ;-)
# Too lazy to build ryu2
# And we need generic_128 for the generic_128 test ;-)
tests += ryu_additionals
