# Generate the documentation with Doxygen

# We use configuration data to rewrite the Doxyfile
# Only PROJECT_NUMBER is rewritten to be compatible to make builds
# OUTPUT_DIRECTORY can't be rewritten without Makefile modifications,
# so this is done by the doxywrap

if get_option('doxy')
message('Run \'ninja resources/doxy\' to generate documenation.')

doxy_data = configuration_data()
doxy_data.set('PROJECT_NUMBER', meson.project_version())

doxy_conf = files('Doxyfile')
doxy_run = find_program('doxywrap')

dox = configure_file(
      input : 'Doxyfile',
      output : 'Doxyfile',
      configuration : doxy_data)

custom_target('doxy_generate', # This is not the ninja target name
    command : [ doxy_run,
                # "Usage: doxywrap source_root output_root config_file [doxygen parameters]"
                meson.source_root(),
                meson.current_build_dir() + '/doxygenerated', # join_paths()... sigh
                '@INPUT@' ],
    input : doxy_conf,
    #depend_files : inc,
    capture : true,
    output : 'doxy', # This is how the target is called
    build_by_default : false,
    )

# Installation of doxygenerated-tree still missing
#
# install_subdir(....)

endif # if get_option('doxy')
