# Find all out tools and their sub-dependencies

# This is possibly used by the debian installation stuff
changelogger = find_program('changelogger', required : false)

# This is used by the Doxygen generation stuff
# doxygen is used by doxywrap; if we have both we are ready to rumble
doxygen = find_program('doxygen', required : false)
if get_option('docs') and doxygen.found()
    graphviz = find_program('dot', required : false)
    if graphviz.found()
        doxy_run = find_program('doxywrap')
    else
        message('Hint: dot is usually a part of the graphviz package')
    endif
endif

# Script to check the configuration validity
check_conf = configuration_data()
check_conf.set('GIT_FULL_API_VERSION', git_full_api_version)
check_conf.set('GIT_VERSION', git_version)
check_conf.set('SOURCE_ROOT', meson.current_build_dir())
check_conf.set('VERSION_TAG', get_option('deb-vers-tag'))
config_checker = configure_file(
    input : 'config_checker.in',
    output : 'config_checker',
    configuration : check_conf,
)
config_checker_path = meson.current_build_dir() / '/config_checker'

# vi:ts=4:sw=4:sts=4:et:syn=conf
