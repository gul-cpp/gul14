# This is not a standalone project
# Only called if deb_install == true

# These are the standard package names
deb_name_default = meson.project_name()
deb_dev_name_default = meson.project_name() + '-dev'

# Generate package name the user wants to have
# Thereby @0@ is substituted with the official package name
deb_name = get_option('deb-name').format(meson.project_name())
deb_dev_name = get_option('deb-dev-name').format(meson.project_name())

# If we use an obscure package name, the builder might want an
# obscure version number also
# Expected tag is in the form 'TEXT_M_m_p'
if get_option('deb-vers-ext')
    message('Debian package versioning: external tag')
    git_external_parts = git_external.split('_')
    git_external_num = git_external_parts[1] + '.' + git_external_parts[2] + '.' + git_external_parts[3]
    git_external_suf = '-' + git_external_parts[1] + '-' + git_external_parts[2] + '-' + git_external_parts[3]
    if not changelogger.found()
        error('Debian changelog generator not found')
    endif

    changelog_gen = join_paths(meson.current_build_dir(), 'changelog.gen')
    run_command(changelogger, '[^v]*', changelog_gen)
    changelog_in = changelog_gen
else
    git_external_suf = ''
    changelog_in = 'changelog.in'
endif

deb_conf = configuration_data()
deb_conf.set('PREFIX', get_option('prefix'))
deb_conf.set('LIBDIR', get_option('libdir'))
deb_conf.set('INCLUDEDIR', get_option('includedir'))
deb_conf.set('NAME', deb_name)
deb_conf.set('NAME-BASE', deb_name)
deb_conf.set('NAME-DEV', deb_dev_name)
deb_conf.set('CONFLICTS', '')
deb_conf.set('CONFLICTS-DEV', '')
deb_conf.set('LIBGUL_API_VERSION', get_variable('git_external_num', libgul_api_version))

# Mark a conflict, if we generate a non standard debian package
# This is not a perfect, but at least some protection against
# having both installed.
if deb_name != deb_name_default
    deb_name = deb_name + git_external_suf
    deb_conf.set('NAME', deb_name)
    deb_conf.set('CONFLICTS', deb_name_default)
endif
if deb_dev_name != deb_dev_name_default
    deb_conf.set('NAME-DEV', deb_dev_name)
    deb_conf.set('CONFLICTS-DEV', deb_dev_name_default)
endif

message('Debian package names: ' + deb_name + ', ' + deb_dev_name)

debian_files = [
  'control',
  'compat',
  'copyright',
  'rules',
]

foreach f : debian_files
    configure_file(input : f + '.in', output : f,
                   configuration : deb_conf)
endforeach

configure_file(input : changelog_in, output : 'changelog',
               configuration : deb_conf)

# The install files need to be renamed, if the package name is not standard,
# so we handle them individually
configure_file(input : deb_name_default + '.install.in',
               output : deb_name + '.install',
               configuration : deb_conf)
configure_file(input : deb_dev_name_default + '.install.in',
               output : deb_dev_name + '.install',
               configuration : deb_conf)
