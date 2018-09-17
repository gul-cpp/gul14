# This is not a standalone project
# Only called if deb_install == true

deb_conf = configuration_data()
deb_conf.set('LIBGUL_API_VERSION', libgul_api_version)
# Not applied to any file, do we need this anywhere?

empty_conf = configuration_data() # for <0.47 compat
empty_conf.set('FOR_MESON_0.47', '@FOR_MESON_0.47@') # Empty rule to suppress warning in meson 0.47+

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
    # should rather be: configure_file(input : f, output : f, copy : true)
    configure_file(input : f, output : f,
                   configuration : empty_conf)
endforeach
