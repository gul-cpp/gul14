# Generate the documentation with Doxygen

# We use configuration data to rewrite the Doxyfile
# Only PROJECT_NUMBER is rewritten to be compatible to make builds
# OUTPUT_DIRECTORY can't be rewritten without Makefile modifications,
# so this is done by the doxywrap

if get_option('docs')
message('Run \'ninja resources/docs\' to generate documentation.')

docs_data = configuration_data()
docs_data.set('PROJECT_NUMBER', meson.project_version())

dox = configure_file(
      input : 'Doxyfile.in',
      output : 'Doxyfile',
      configuration : docs_data)

custom_target('doxy_generate', # This is not the ninja target name
    command : [ doxy_run,
                # "Usage: doxywrap source_root output_root config_file [doxygen parameters]"
                meson.source_root(),
                meson.current_build_dir() + '/doxygenerated', # join_paths()... sigh
                '@INPUT@' ],
    input : dox,
    #depend_files : inc,
    capture : true,
    output : 'docs', # This is how the target is called
    build_by_default : false,
    )

# Installation of doxygenerated-tree still missing
#
# install_subdir(....)

endif # if get_option('docs')
