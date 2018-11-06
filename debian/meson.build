# This is not a standalone project
# Only called if deb_install == true

# These are the standard package names
deb_name_default = meson.project_name()
deb_dev_name_default = meson.project_name() + '-dev'

# Generate package name the user wants to have
# Thereby @0@ is substituted with the official package name
deb_name = get_option('deb-name').format(meson.project_name())
deb_dev_name = get_option('deb-dev-name').format(meson.project_name())

deb_conf = configuration_data()
deb_conf.set('PREFIX', get_option('prefix'))
deb_conf.set('LIBDIR', get_option('libdir'))
deb_conf.set('INCLUDEDIR', get_option('includedir'))
deb_conf.set('NAME', deb_name)
deb_conf.set('NAME-DEV', deb_dev_name)

# Mark a conflict, if we generate a non standard debian package
# This is not a perfect, but at least some protection against
# having both installed.
if deb_name != deb_name_default
    deb_conf.set('CONFLICTS', deb_name_default)
endif
if deb_dev_name != deb_dev_name_default
    deb_conf.set('CONFLICTS-DEV', deb_dev_name_default)
endif

debian_files = [
  'changelog',
  'control',
  'compat',
  'copyright',
  'rules',
]

foreach f : debian_files
    configure_file(input : f, output : f,
                   configuration : deb_conf)
endforeach

# The install files need to be renamed, if the package name is not standard,
# so we handle them individually
configure_file(input : deb_name_default + '.install',
               output : deb_name + '.install',
               configuration : deb_conf)
configure_file(input : deb_dev_name_default + '.install',
               output : deb_dev_name + '.install',
               configuration : deb_conf)
