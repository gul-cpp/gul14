# Generate the documentation with Doxygen

# We use configuration data to rewrite the Doxyfile
# Only PROJECT_NUMBER is rewritten to be compatible to make builds
# OUTPUT_DIRECTORY can't be rewritten without Makefile modifications,
# so this is done by the doxywrap

if get_option('docs') and doxygen.found() and graphviz.found() and doxy_run.found()
    rsync = find_program('rsync', required : false)

    docs_data = configuration_data()
    docs_data.set('PROJECT_NUMBER', meson.project_version())

    dox = configure_file(
        input : 'Doxyfile.in',
        output : 'Doxyfile',
        configuration : docs_data,
    )

    docs_outdir = meson.current_build_dir() / 'doxygenerated'

    docs_gen = custom_target('doxy_generate', # This is not the ninja target name
        command : [
            doxy_run, # "Usage: doxywrap source_root output_root config_file [doxygen parameters]"
            meson.current_source_dir() / '..',
            docs_outdir,
            '@INPUT@',
        ],
        input : dox,
        capture : true,
        output : 'docs', # This is how the target is called
        build_by_default : false,
        depend_files : libgul_sub_headers + libgul_src,
        )

    if rsync.found()
        docdir = get_option('prefix') / get_option('datadir') / meson.project_name()

        custom_target('copy_doxygenerated',
            command : [
                rsync,
                '-rt',
                '--chmod=Da=rx,ug+w,Fa=r,ug+w',
                docs_outdir / 'html',
                docdir,
            ],
            input : docs_gen,
            capture : true,
            output : 'install_docs',
            build_by_default : false,
        )
    endif # have rsync

    message('Run \'ninja data/docs\' to generate documentation.')
    if rsync.found()
        message('Run \'ninja data/install_docs\' to install documentation in ' + docdir)
    endif


elif get_option('docs')
    message('Documentation requested but tools missing, \'doc\' target not generated')
endif # option docs and have all tools incl doxy_run (doxywrap)

# vi:ts=4:sw=4:sts=4:et:syn=conf
