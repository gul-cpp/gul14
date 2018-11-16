# Generate the documentation with Doxygen

# We use configuration data to rewrite the Doxyfile
# Only PROJECT_NUMBER is rewritten to be compatible to make builds
# OUTPUT_DIRECTORY can't be rewritten without Makefile modifications,
# so this is done by the doxywrap

if get_option('docs') and find_program('doxygen', required : false).found() and doxy_run.found()
    rsync = find_program('rsync', required : false)

    docs_data = configuration_data()
    docs_data.set('PROJECT_NUMBER', meson.project_version())
    docs_data.set('PROJECT_NAME', meson.project_name())

    dox = configure_file(
          input : 'Doxyfile.in',
          output : 'Doxyfile',
          configuration : docs_data)

    docs_outdir = join_paths(meson.current_build_dir(), 'doxygenerated')

    docs_gen = custom_target('doxy_generate', # This is not the ninja target name
        command : [ doxy_run,
                    # "Usage: doxywrap source_root output_root config_file [doxygen parameters]"
                    meson.source_root(),
                    docs_outdir,
                    '@INPUT@' ],
        input : dox,
        capture : true,
        output : 'docs', # This is how the target is called
        # build_by_default : false,  This is the default default
        )

    if rsync.found()
        docdir = join_paths(get_option('prefix'),
                            get_option('datadir'),
                            meson.project_name())

        custom_target('copy_doxygenerated',
            command : [ rsync,
                        '-rt',
                        '--chmod=Da=rx,ug+w,Fa=r,ug+w',
                        join_paths(docs_outdir, 'html'),
                        docdir ],
            input : docs_gen,
            capture : true,
            output : 'install_docs',
            # build_by_default : false,  This is the default default
        )
    endif # have rsync

    message('Run \'ninja resources/docs\' to generate documentation.')
    if rsync.found()
        message('Run \'ninja resources/install_docs\' to install documentation in ' + docdir)
    endif


endif # option docs and have doxygen
