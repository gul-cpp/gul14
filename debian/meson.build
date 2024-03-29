# All about debian packaging

deb_install = false
if build_machine.system() != 'linux'
    # disable packaging on all non linux platforms
    message('Debian packaging disabled (machine in not linux)')
    subdir_done()
elif meson.is_subproject()
    message('Debian packaging disabled (this is a subproject)')
    subdir_done()
elif get_option('prefix') == '/usr/local'
    message('Debian packaging disabled (Packets installing files to /usr/local forbidden)')
    message('                          Enable by specifying correct prefix e.g. --prefix=/usr')
    subdir_done()
elif git_full_api_version == 'unknown'
    message('Debian packaging disabled (can not access git tags and most probably git log)')
    subdir_done()
endif
deb_install = true

# These are the standard package names
deb_name_default = 'lib' + meson.project_name()
deb_dev_name_default = 'lib' + meson.project_name() + '-dev'

# Generate package name the user wants to have
# Thereby @0@ is substituted with the official package name
deb_name = get_option('deb-name').format('lib' + meson.project_name())
deb_dev_name = get_option('deb-dev-name').format('lib' + meson.project_name())

# Autogenerate debian/changelog
if not changelogger.found()
    error('Debian changelog generator not found')
endif

changelog_gen = meson.current_build_dir() / 'changelog.gen'
changelg_run = run_command(
    changelogger,
    get_option('deb-vers-pack') ? '-tp' : '-t',
    get_option('deb-vers-tag') + '*',
    changelog_gen,
    check: false,
)

if changelg_run.returncode() != 0
    error(changelg_run.stdout())
endif

debian_package_version = get_variable('git_version_num', libgul_api_version) + get_variable('git_patch', '')

# Find out if we run on ARM, and limit ninja if we do
if build_machine.cpu_family() == 'arm' or build_machine.cpu_family() == 'aarch64'
    deb_ninja_args = '-j 1'
endif

# Configure all the debian files
deb_conf = configuration_data()
deb_conf.set('PREFIX', get_option('prefix'))
deb_conf.set('LIBDIR', get_option('libdir'))
deb_conf.set('INCLUDEDIR', get_option('includedir'))
deb_conf.set('NAME', deb_name)
deb_conf.set('NAME-BASE', deb_name)
deb_conf.set('NAME-DEV', deb_dev_name)
deb_conf.set('CONFLICTS', '')
deb_conf.set('CONFLICTS-DEV', '')
deb_conf.set('VERSION', debian_package_version)
deb_conf.set('CONFIG_CHECKER', config_checker_path)
deb_conf.set('NINJA-ARGS', get_variable('deb_ninja_args', ''))

# Mark a conflict, if we generate a non standard debian package
# This is not a perfect, but at least some protection against
# having both installed.
if deb_name != deb_name_default or get_option('deb-vers-pack')
    git_version_suf = '-' + '-'.join(git_version_parts)
    deb_name = deb_name + git_version_suf
    deb_conf.set('NAME', deb_name)
    deb_conf.set('CONFLICTS', deb_name_default)
endif
if deb_dev_name != deb_dev_name_default
    deb_conf.set('NAME-DEV', deb_dev_name)
    deb_conf.set('CONFLICTS-DEV', deb_dev_name_default)
endif

debian_files = [
    'control',
    'compat',
    'copyright',
    'rules',
]

foreach f : debian_files
    configure_file(
        input : f + '.in',
        output : f,
        configuration : deb_conf
    )
endforeach

configure_file(
    input : changelog_gen,
    output : 'changelog',
    configuration : deb_conf
)

# The install files need to be renamed, if the package name is not standard,
# so we handle them individually
configure_file(
    input : deb_name_default + '.install.in',
    output : deb_name + '.install',
    configuration : deb_conf
)
configure_file(
    input : deb_dev_name_default + '.install.in',
    output : deb_dev_name + '.install',
    configuration : deb_conf
)

# vi:ts=4:sw=4:sts=4:et:syn=conf
