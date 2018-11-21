# Find all out tools and their sub-dependencies

# This is possibly used by the debian installation stuff
changelogger = find_program('changelogger', required : false)

# This is used by the Doxygen generation stuff
# doxygen is used by doxywrap; if we have both we are ready to rumble
doxygen = find_program('doxygen', required : false)
if get_option('docs') and doxygen.found()
    doxy_run = find_program('doxywrap')
endif # if get_option('docs')
