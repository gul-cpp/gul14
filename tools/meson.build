# Find all out tools and their sub-dependencies

# This is possibly used by the debian installation stuff
changelogger = find_program('changelogger', required : false)

# This is used by the Doxygen generation stuff
if get_option('docs')
    find_program('doxygen') # Used by doxywrap
    doxy_run = find_program('doxywrap')
endif # if get_option('docs')
