# This is not a standalone project
# Only called if deb_install == true

deb_conf = configuration_data()
deb_conf.set('PREFIX', get_option('prefix'))
deb_conf.set('LIBDIR', get_option('libdir'))
deb_conf.set('INCLUDEDIR', get_option('includedir'))

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
    configure_file(input : f, output : f,
                   configuration : deb_conf)
endforeach
