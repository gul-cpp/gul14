# This is not a standalone project
# Only called if deb_install == true

deb_conf = configuration_data()
deb_conf.set('LIBGUL_API_VERSION', libgul_api_version)
# Not applied to any file, do we need this anywhere?

empty_conf = configuration_data() # for <0.47 compat

debian_files = [
  'changelog',
  'control',
  'compat',
  'copyright',
  'rules',
  'libgul-dev.install',
  'libgul.install',
]

foreach f : debian_files
    if meson.version().version_compare('<0.47')
        configure_file(input: f, output: f,
                       configuration: empty_conf)
    else
        configure_file(input: f, output: f, copy:true)
    endif
endforeach
