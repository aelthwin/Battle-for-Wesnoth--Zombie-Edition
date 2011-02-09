# vi: syntax=python:et:ts=4
#
# SCons build description for the Wesnoth project
#
# Prerequisites are:
# 1. Subversion command-line client programs svnstatus and svnversion.
# 2. Unix file(1), on installation only.
# 3. Unix convert(1), on installation only, if using tinygui.
# 4. msgfmt(1) for making builds with i18n support.
# 5. graph-includes for making the project dependency graph.

EnsureSConsVersion(0,98,3)

import os, sys, shutil, re, commands
from glob import glob
from subprocess import Popen, PIPE, call
from os import access, F_OK

# Warn user of current set of build options.
if os.path.exists('.scons-option-cache'):
    optfile = file('.scons-option-cache')
    print "Saved options:", optfile.read().replace("\n", ", ")[:-2]
    optfile.close()

#
# Get the Wesnoth version number
#

config_h_re = re.compile(r"^.*#define\s*(\S*)\s*\"(\S*)\".*$", re.MULTILINE)
build_config = dict( config_h_re.findall(File("src/wesconfig.h").get_contents()) )
try:
    version = build_config["VERSION"]
    print "Building Wesnoth version %s" % version
except KeyError:
    print "Couldn't determin the Wesnoth version number, bailing out!"
    sys.exit(1)

#
# Build-control options
#

opts = Variables('.scons-option-cache')

def OptionalPath(key, val, env):
    if val:
        PathVariable.PathIsDir(key, val, env)

opts.AddVariables(
    ListVariable('default_targets', 'Targets that will be built if no target is specified in command line.',
        "wesnoth,wesnothd", Split("wesnoth wesnothd campaignd cutter exploder test")),
    EnumVariable('build', 'Build variant: debug, release profile or base (no subdirectory)', "release", ["release", "debug", "glibcxx_debug", "profile","base"]),
    PathVariable('build_dir', 'Build all intermediate files(objects, test programs, etc) under this dir', "build", PathVariable.PathAccept),
    ('extra_flags_config', 'Extra compiler and linker flags to use for configuration and all builds', ""),
    ('extra_flags_base', 'Extra compiler and linker flags to use for release builds', ""),
    ('extra_flags_release', 'Extra compiler and linker flags to use for release builds', ""),
    ('extra_flags_debug', 'Extra compiler and linker flags to use for debug builds', ""),
    ('extra_flags_profile', 'Extra compiler and linker flags to use for profile builds', ""),
    PathVariable('bindir', 'Where to install binaries', "bin", PathVariable.PathAccept),
    ('cachedir', 'Directory that contains a cache of derived files.', ''),
    PathVariable('datadir', 'read-only architecture-independent game data', "$datarootdir/$datadirname", PathVariable.PathAccept),
    PathVariable('fifodir', 'directory for the wesnothd fifo socket file', "/var/run/wesnothd", PathVariable.PathAccept),
    BoolVariable('fribidi','Clear to disable bidirectional-language support', True),
    BoolVariable('desktop_entry','Clear to disable desktop-entry', True),
    PathVariable('datarootdir', 'sets the root of data directories to a non-default location', "share", PathVariable.PathAccept),
    PathVariable('datadirname', 'sets the name of data directory', "wesnoth$version_suffix", PathVariable.PathAccept),
    PathVariable('desktopdir', 'sets the desktop entry directory to a non-default location', "$datarootdir/applications", PathVariable.PathAccept),
    PathVariable('icondir', 'sets the icons directory to a non-default location', "$datarootdir/icons", PathVariable.PathAccept),
    BoolVariable('internal_data', 'Set to put data in Mac OS X application fork', False),
    PathVariable('localedirname', 'sets the locale data directory to a non-default location', "translations", PathVariable.PathAccept),
    PathVariable('mandir', 'sets the man pages directory to a non-default location', "$datarootdir/man", PathVariable.PathAccept),
    PathVariable('docdir', 'sets the doc directory to a non-default location', "$datarootdir/doc/wesnoth", PathVariable.PathAccept),
    PathVariable('python_site_packages_dir', 'sets the directory where python modules are installed', "lib/python/site-packages/wesnoth", PathVariable.PathAccept),
    BoolVariable('editor', 'Enable editor', True),
    BoolVariable('lowmem', 'Set to reduce memory usage by removing extra functionality', False),
    BoolVariable('notifications', 'Enable support for desktop notifications', True),
    BoolVariable('nls','enable compile/install of gettext message catalogs',True),
    PathVariable('prefix', 'autotools-style installation prefix', "/usr/local", PathVariable.PathAccept),
    PathVariable('prefsdir', 'user preferences directory', "", PathVariable.PathAccept),
    PathVariable('destdir', 'prefix to add to all installation paths.', "/", PathVariable.PathAccept),
    BoolVariable('prereqs','abort if prerequisites cannot be detected',True),
    ('program_suffix', 'suffix to append to names of installed programs',"$version_suffix"),
    ('version_suffix', 'suffix that will be added to default values of prefsdir, program_suffix and datadirname', ""),
    BoolVariable('raw_sockets', 'Set to use raw receiving sockets in the multiplayer network layer rather than the SDL_net facilities', False),
    BoolVariable('forum_user_handler', 'Enable forum user handler in wesnothd', False),
    BoolVariable('use_network_ana', 'Use the new network api', True),
    BoolVariable('pool_alloc', 'Enable custom pool malloc', False),
    ('server_gid', 'group id of the user who runs wesnothd', ""),
    ('server_uid', 'user id of the user who runs wesnothd', ""),
    EnumVariable('gui', 'Set for GUI reductions for resolutions down to 320x240 (PDAs)', "normal", ["normal", "tiny"]),
    BoolVariable('strict', 'Set to strict compilation', False),
    BoolVariable('static_test', 'Staticaly build against boost test (Not supported yet)', False),
    BoolVariable('verbose', 'Emit progress messages during data installation.', False),
    PathVariable('sdldir', 'Directory of SDL installation.', "", OptionalPath),
    PathVariable('boostdir', 'Directory of boost installation.', "", OptionalPath),
    PathVariable('boostlibdir', 'Directory where boost libraries are installed.', "", OptionalPath),
    ('boost_suffix', 'Suffix of boost libraries.'),
    PathVariable('gettextdir', 'Root directory of Gettext\'s installation.', "", OptionalPath), 
    PathVariable('gtkdir', 'Directory where GTK SDK is installed.', "", OptionalPath),
    PathVariable('luadir', 'Directory where Lua binary package is unpacked.', "", OptionalPath),
    ('host', 'Cross-compile host.', ''),
    ('jobs', 'Set the number of parallel compilations', "1", lambda key, value, env: int(value), int),
    BoolVariable('distcc', 'Use distcc', False),
    BoolVariable('ccache', "Use ccache", False),
    ('cxxtool', 'Set c++ compiler command if not using standard compiler.'),
    BoolVariable("fast", "Make scons faster at cost of less precise dependency tracking.", False),
    BoolVariable("lockfile", "Create a lockfile to prevent multiple instances of scons from being run at the same time on this working copy.", False)
    )

#
# Setup
#

sys.path.insert(0, "./scons")
env = Environment(tools=["tar", "gettext", "install", "python_devel", "scanreplace"], options = opts, toolpath = ["scons"])

if env["lockfile"]:
    print "Creating lockfile"
    lockfile = os.path.abspath("scons.lock")
    open(lockfile, "wx").write(str(os.getpid()))
    import atexit
    atexit.register(os.remove, lockfile)

opts.Save('.scons-option-cache', env)
env.SConsignFile("$build_dir/sconsign.dblite")

# Make sure the user's environment is always available
env['ENV']['PATH'] = os.environ["PATH"]
if env["PLATFORM"] == "win32":
    env.Tool("mingw")
elif env["PLATFORM"] == "sunos":
    env.Tool("sunc++")
    env.Tool("suncc")
    env.Tool("sunar")
    env.Tool("sunlink")
    env.Append(CXXFLAGS = Split("-library=stlport4 -staticlib=stlport4 -norunpath -features=tmplife -features=tmplrefstatic -features=extensions"))
    env.Append(LINKFLAGS = Split("-library=stlport4 -staticlib=stlport4 -lsocket -lnsl -lboost_iostreams -L. -R."))
    env['CC'] = env['CXX']
else:
    from cross_compile import *
    setup_cross_compile(env)

if env.get('cxxtool',""):
    env['CXX'] = env['cxxtool']
    if 'HOME' in os.environ:
        env['ENV']['HOME'] = os.environ['HOME']

if env['jobs'] > 1:
    SetOption("num_jobs", env['jobs'])

if env['distcc']: 
    env.Tool('distcc')

if env['ccache']: env.Tool('ccache')


Help("""Arguments may be a mixture of switches and targets in any order.
Switches apply to the entire build regardless of where they are in the order.
Important switches include:

    prefix=/usr     probably what you want for production tools
    build=base      build directly in the distribution root;
                    you'll need this if you're trying to use Emacs compile mode.
    build=release   build the release build variant with appropriate flags
                        in build/release and copy resulting binaries
                        into distribution/working copy root.
    build=debug     same for debug build variant, binaries will be copied with -debug suffix

With no arguments, the recipe builds wesnoth and wesnothd.  Available
build targets include the individual binaries:

    wesnoth wesnothd campaignd exploder cutter test

You can make the following special build targets:

    all = wesnoth exploder cutter wesnothd campaignd (*).
    TAGS = build tags for Emacs (*).
    wesnoth-deps.png = project dependency graph
    install = install all executables that currently exist, and any data needed
    install-wesnothd = install the Wesnoth multiplayer server.
    install-campaignd = install the Wesnoth campaign server.
    install-cutter = install the castle cutter
    install-exploder = install the castle exploder
    install-pytools = install all Python tools and modules
    uninstall = uninstall all executables, tools, modules, and servers.
    pot-update = generate gettext message catalog templates and merge them with localized message catalogs
    update-po = merge message catalog templates with localized message catalogs for particular lingua
    update-po4a = update translations of manual and manpages
    af bg ca ... = linguas for update-po
    dist = make distribution tarball as wesnoth.tar.bz2 (*).
    data-dist = make data tarball as wesnoth-data.tar.bz2 (*).
    binary-dist = make data tarball as wesnoth-binaries.tar.bz2 (*).
    wesnoth-bundle = make Mac OS application bundle from game (*)
    sanity-check = run a pre-release sanity check on the distribution.
    manual = regenerate English-language manual and, possibly, localized manuals if appropriate xmls exist.

Files made by targets marked with '(*)' are cleaned by 'scons -c all'

Options are cached in a file named .scons-option-cache and persist to later
invocations.  The file is editable.  Delete it to start fresh.  Current option
values can be listed with 'scons -h'.

If you set CXXFLAGS and/or LDFLAGS in the environment, the values will
be appended to the appropriate variables within scons.
""" + opts.GenerateHelpText(env, sort=cmp))

if GetOption("help"):
    Return()

if env["cachedir"] and not env['ccache']:
    CacheDir(env["cachedir"])

if env["fast"]:
    env.Decider('MD5-timestamp')
    SetOption('max_drift', 1)
    SetOption('implicit_cache', 1)

if not os.path.isabs(env["prefix"]):
    print "Warning: prefix is set to relative dir. destdir setting will be ignored."
    env["destdir"] = ""

#
# work around long command line problem on windows
# see http://www.scons.org/wiki/LongCmdLinesOnWin32
#
if sys.platform == 'win32':
    try:
        import win32file
        import win32event
        import win32process
        import win32security
        import string

        def my_spawn(sh, escape, cmd, args, spawnenv):
            for var in spawnenv:
                spawnenv[var] = spawnenv[var].encode('ascii', 'replace')

            sAttrs = win32security.SECURITY_ATTRIBUTES()
            StartupInfo = win32process.STARTUPINFO()
            newargs = string.join(map(escape, args[1:]), ' ')
            cmdline = cmd + " " + newargs

            # check for any special operating system commands
            if cmd == 'del':
                for arg in args[1:]:
                    win32file.DeleteFile(arg)
                exit_code = 0
            else:
                # otherwise execute the command.
                hProcess, hThread, dwPid, dwTid = win32process.CreateProcess(None, cmdline, None, None, 1, 0, spawnenv, None, StartupInfo)
                win32event.WaitForSingleObject(hProcess, win32event.INFINITE)
                exit_code = win32process.GetExitCodeProcess(hProcess)
                win32file.CloseHandle(hProcess);
                win32file.CloseHandle(hThread);
            return exit_code

        env['SPAWN'] = my_spawn
    except ImportError:
        def subprocess_spawn(sh, escape, cmd, args, env):
            return call(' '.join(args))
        env['SPAWN'] = subprocess_spawn

#
# Check some preconditions
#

def Warning(message):
    print message
    return False

from metasconf import init_metasconf
configure_args = dict(custom_tests = init_metasconf(env, ["cplusplus", "python_devel", "sdl", "boost", "pango", "pkgconfig", "gettext", "lua"]), config_h = "config.h",
    log_file="$build_dir/config.log", conf_dir="$build_dir/sconf_temp")

env.MergeFlags(env["extra_flags_config"])
if env["prereqs"]:
    conf = env.Configure(**configure_args)

    if env["PLATFORM"] == "posix":
        conf.CheckCHeader("poll.h", "<>")
        conf.CheckCHeader("sys/poll.h", "<>")
        conf.CheckCHeader("sys/select.h", "<>")
        if conf.CheckCHeader("sys/sendfile.h", "<>"):
            conf.CheckFunc("sendfile")
    conf.CheckLib("m")
    conf.CheckFunc("round")

    have_server_prereqs = \
        conf.CheckCPlusPlus(gcc_version = "3.3") and \
        conf.CheckGettextLibintl() and \
        conf.CheckBoost("iostreams", require_version = "1.34.1") and \
        conf.CheckBoostIostreamsGZip() and \
        conf.CheckBoost("smart_ptr", header_only = True) and \
        conf.CheckSDL(require_version = '1.2.7') and \
        conf.CheckSDL('SDL_net') or Warning("Base prerequisites are not met.")
    if have_server_prereqs and env["use_network_ana"]:
        if env["PLATFORM"] == 'win32':
            env.Append(LIBS = ["libws2_32"])
        have_server_prereqs = \
            conf.CheckBoost("system") and \
            conf.CheckBoost("thread") and \
            conf.CheckBoost("asio", header_only = True) or Warning("Base prerequisites are not met.")

    env = conf.Finish()
    client_env = env.Clone()
    conf = client_env.Configure(**configure_args)
    have_client_prereqs = have_server_prereqs and \
        conf.CheckPango("cairo") and \
        conf.CheckPKG("fontconfig") and \
        conf.CheckBoost("regex", require_version = "1.35.0") and \
        conf.CheckSDL("SDL_ttf", require_version = "2.0.8") and \
        conf.CheckSDL("SDL_mixer", require_version = '1.2.0') and \
        conf.CheckSDL("SDL_image", require_version = '1.2.0') and \
        conf.CheckOgg() or Warning("Client prerequisites are not met. wesnoth, cutter and exploder cannot be built.")

    have_X = False
    if have_client_prereqs:
        if env["PLATFORM"] != "win32":
            have_X = conf.CheckLib('X11')

        env["notifications"] = env["notifications"] and conf.CheckPKG("dbus-1")
        if env["notifications"]:
            client_env.Append(CPPDEFINES = ["HAVE_LIBDBUS"])

        if client_env['fribidi']:
            client_env['fribidi'] = conf.CheckPKG('fribidi >= 0.10.9') or Warning("Can't find libfribidi, disabling freebidi support.")

    if env["forum_user_handler"]:
        env.ParseConfig("mysql_config --libs --cflags")
        env.Append(CPPDEFINES = ["HAVE_MYSQLPP"])

    client_env = conf.Finish()

    test_env = client_env.Clone()
    conf = test_env.Configure(**configure_args)

    have_test_prereqs = have_client_prereqs and have_server_prereqs and conf.CheckBoost('unit_test_framework') or Warning("Unit tests are disabled because their prerequisites are not met.")
    test_env = conf.Finish()
    if not have_test_prereqs and "test" in env["default_targets"]:
        env["default_targets"].remove("test")

    print env.subst("If any config checks fail, look in $build_dir/config.log for details")
    print "If a check fails spuriously due to caching, use --config=force to force its rerun"

else:
    have_client_prereqs = True
    have_X = False
    if env["PLATFORM"] != "win32":
        have_X = True
    have_server_prereqs = True
    have_test_prereqs = True
    test_env = env.Clone()
    client_env = env.Clone()

have_msgfmt = env["MSGFMT"]
if not have_msgfmt:
     env["nls"] = False
if not have_msgfmt:
     print "NLS tools are not present..."
if not env['nls']:
     print "NLS catalogue installation is disabled."

#
# Implement configuration switches
#

for env in [test_env, client_env, env]:
    env.Append(CPPPATH = ["#/", "#/src"])

    env.Append(CPPDEFINES = ["HAVE_CONFIG_H"])

    if "gcc" in env["TOOLS"]:
        env.AppendUnique(CCFLAGS = Split("-W -Wall"), CFLAGS = ["-std=c99"], CXXFLAGS="-std=c++98")
        if env['strict']:
            # The current networking code breaks strict aliasing in g++ 4.5.
            # Once the code is converted to boost::asio it might be reenabled.
            env.AppendUnique(CCFLAGS = Split("-Werror -Wno-strict-aliasing"))

        env["OPT_FLAGS"] = "-O2"
        env["DEBUG_FLAGS"] = Split("-O0 -DDEBUG -ggdb3")

    if "suncc" in env["TOOLS"]:
        env["OPT_FLAGS"] = "-g0"
        env["DEBUG_FLAGS"] = "-g"

    if env['gui'] == 'tiny':
        env.Append(CPPDEFINES = "USE_TINY_GUI")

    if env['lowmem']:
        env.Append(CPPDEFINES = "LOW_MEM")

    if env['internal_data']:
        env.Append(CPPDEFINES = "USE_INTERNAL_DATA")

    if not env["editor"]:
        env.Append(CPPDEFINES = "DISABLE_EDITOR")

    if env["PLATFORM"] == "win32":
        env["pool_alloc"] = False

    if have_X:
        env.Append(CPPDEFINES = "_X11")

# Simulate autools-like behavior of prefix on various paths
    installdirs = Split("bindir datadir fifodir icondir desktopdir mandir docdir python_site_packages_dir")
    for d in installdirs:
        env[d] = os.path.join(env["prefix"], env[d])

    if env["PLATFORM"] == 'win32':
        env.Append(LIBS = ["wsock32", "intl", "z"], CCFLAGS = ["-mthreads"], LINKFLAGS = ["-mthreads"], CPPDEFINES = ["_WIN32_WINNT=0x0500"])
    if env["PLATFORM"] == 'darwin':            # Mac OS X
        env.Append(FRAMEWORKS = "Carbon")            # Carbon GUI

if not env['static_test']:
    test_env.Append(CPPDEFINES = "BOOST_TEST_DYN_LINK")

if os.path.exists('.git'):
    try:
        env["svnrev"] = Popen(Split("git svn find-rev refs/remotes/trunk"), stdout=PIPE, stderr=PIPE).communicate()[0].rstrip("\n")
        if not env["svnrev"]:
            # If you use git svn for one svn path only there's no refs/remotes/trunk, only git svn branch
            env["svnrev"] = Popen(Split("git svn find-rev git svn"), stdout=PIPE, stderr=PIPE).communicate()[0].rstrip("\n")
        # if git svn can't find HEAD it's a local commit
        if Popen(Split("git svn find-rev HEAD"), stdout=PIPE).communicate()[0].rstrip("\n") == "":
            env["svnrev"] += "L"
        if Popen(Split("git diff --exit-code --quiet")).wait() == 1:
            env["svnrev"] += "M"
    except:
        env["svnrev"] = ""
else:
    try:
        env["svnrev"] = Popen(Split("svnversion -n ."), stdout=PIPE).communicate()[0]
    except:
        env["svnrev"] = ""

Export(Split("env client_env test_env have_client_prereqs have_server_prereqs have_test_prereqs"))
SConscript(dirs = Split("po doc packaging/windows"))

binaries = Split("wesnoth wesnothd cutter exploder campaignd test")
builds = {
    "base"          : dict(CCFLAGS   = "$OPT_FLAGS"),    # Don't build in subdirectory
    "debug"         : dict(CCFLAGS   = Split("$DEBUG_FLAGS")),
    "glibcxx_debug" : dict(CPPDEFINES = Split("_GLIBCXX_DEBUG _GLIBCXX_DEBUG_PEDANTIC")),
    "release"       : dict(CCFLAGS   = "$OPT_FLAGS"),
    "profile"       : dict(CCFLAGS   = "-pg", LINKFLAGS = "-pg")
    }
builds["glibcxx_debug"].update(builds["debug"])
build = env["build"]

for env in [test_env, client_env, env]:
    env["extra_flags_glibcxx_debug"] = env["extra_flags_debug"]
    env.AppendUnique(**builds[build])
    env.Append(CXXFLAGS = Split(os.environ.get('CXXFLAGS', [])), LINKFLAGS = Split(os.environ.get('LDFLAGS', [])))
    env.MergeFlags(env["extra_flags_" + build])

if build == "base":
    build_dir = ""
else:
    build_dir = os.path.join("$build_dir", build)

if build == "release" : build_suffix = ""
else                  : build_suffix = "-" + build
Export("build_suffix")
env.SConscript("src/SConscript", variant_dir = build_dir, duplicate = False)
Import(binaries + ["sources"])
binary_nodes = map(eval, binaries)
all = env.Alias("all", map(Alias, binaries))
env.Default(map(Alias, env["default_targets"]))

if have_client_prereqs and env["nls"]:
    env.Requires("wesnoth", Dir("translations"))

#
# Utility productions (Unix-like systems only)
#

# Make a tags file for Emacs
# Exuberant Ctags doesn't understand the -l c++ flag so if the etags fails try the ctags version
env.Command("TAGS", sources, 'etags -l c++ $SOURCES.srcpath || (ctags --tag-relative=yes -f src/tags $SOURCES.srcpath)')
env.Clean(all, 'TAGS')

#
# Unix installation productions
#
# These will not be portable to Windows or Mac. They assume a Unix-like
# directory structure and FreeDesktop standard locations foicon, app,
# and doc files.
#

for d in installdirs:
    exec d + ' = os.path.join(env["destdir"], env[d].lstrip("/"))'
installable_subs = Split('data fonts images sounds')
if os.path.isabs(env["localedirname"]):
    env["localedir"] = env["localedirname"]
else:
    env["localedir"] = "$datadir/$localedirname"
        
pythontools = Split("wmlscope wmllint wmlindent wesnoth_addon_manager")
pythonmodules = Split("wmltools.py wmlparser.py wmldata.py wmliterator.py campaignserver_client.py libsvn.py __init__.py")

def CopyFilter(fn):
    "Filter out data-tree things that shouldn't be installed."
    return not ".svn" in str(fn) and not "Makefile" in str(fn)

env["copy_filter"] = CopyFilter

linguas = Split(open("po/LINGUAS").read())

def InstallManpages(env, component):
    env.InstallData("mandir", component, os.path.join("doc", "man", component + ".6"), "man6")
    for lingua in linguas:
        manpage = os.path.join("doc", "man", lingua, component + ".6")
        env.InstallData("mandir", component, manpage, os.path.join(lingua, "man6"))

# Now the actual installation productions

# The game and associated resources
env.InstallBinary(wesnoth)
env.InstallData("datadir", "wesnoth", map(Dir, installable_subs))
env.InstallData("docdir",  "wesnoth", [Glob("doc/manual/*.html"), Dir("doc/manual/styles"), Dir("doc/manual/images")])
if env["nls"]:
    env.InstallData("localedir", "wesnoth", Dir("translations"))
    env.InstallData("datadir", "wesnoth", "l10n-track")
InstallManpages(env, "wesnoth")
if have_client_prereqs and have_X and env["desktop_entry"]:
     if sys.platform == "darwin":
         env.InstallData("icondir", "wesnoth", "icons/wesnoth-icon-Mac.png")
     else:
         env.InstallData("icondir", "wesnoth", "icons/wesnoth-icon.png")
     env.InstallData("desktopdir", "wesnoth", "icons/wesnoth.desktop")

# Python tools
env.InstallData("bindir", "pytools", map(lambda tool: os.path.join("data", "tools", tool), pythontools))
env.InstallData("python_site_packages_dir", "pytools", map(lambda module: os.path.join("data", "tools", "wesnoth", module), pythonmodules))

# Wesnoth MP server install
env.InstallBinary(wesnothd)
InstallManpages(env, "wesnothd")
if not access(fifodir, F_OK):
    fifodir = env.Command(fifodir, [], [
        Mkdir(fifodir),
        Chmod(fifodir, 0700),
        Action("chown %s:%s %s" %
               (env["server_uid"], env["server_gid"], fifodir)),
        ])
    AlwaysBuild(fifodir)
    env.Alias("install-wesnothd", fifodir)

# Wesnoth campaign server
env.InstallBinary(campaignd)

# And the artists' tools
env.InstallBinary(cutter)
env.InstallBinary(exploder)

# Compute things for default install based on which targets have been created.
install = env.Alias('install', [])
for installable in ('wesnoth',
                    'wesnothd', 'campaignd',
                    'exploder', 'cutter'):
    if os.path.exists(installable + build_suffix) or installable in COMMAND_LINE_TARGETS or "all" in COMMAND_LINE_TARGETS:
        env.Alias('install', env.Alias('install-'+installable))

#
# Un-installation
#
def Uninstall(nodes):
    deletes = []
    for node in nodes:
        if node.__class__ == install[0].__class__:
            deletes.append(Uninstall(node.sources))
        else:
            deletes.append(Delete(str(node)))
    return deletes
uninstall = env.Command('uninstall', '', Flatten(Uninstall(Alias("install"))) or "")
env.AlwaysBuild(uninstall)
env.Precious(uninstall)

#
# Making a distribution tarball.
#
env["version"] = build_config.get("VERSION")
if 'dist' in COMMAND_LINE_TARGETS:    # Speedup, the manifest is expensive
    def dist_manifest():
        "Get an argument list suitable for passing to a distribution archiver."
        # Start by getting a list of all files under version control
        lst = commands.getoutput("svn -v status | grep -v 'data\/test\/.*' | awk '/^[^?]/ {print $4;}'").split()
        lst = filter(os.path.isfile, lst)
        return lst
    dist_tarball = env.Tar('wesnoth-${version}.tar.bz2', [])
    open("dist.manifest", "w").write("\n".join(dist_manifest() + ["src/revision.hpp"]))
    env.Append(TARFLAGS='-j -T dist.manifest --transform "s,^,wesnoth-$version/,"',
               TARCOMSTR="Making distribution tarball...")
    env.AlwaysBuild(dist_tarball)
    env.Clean(all, 'wesnoth.tar.bz2')
    env.Alias('dist', dist_tarball)

#
# Make binary distribution (from installed client side)
#
bin_tar_env = env.Clone()
bin_tarball = bin_tar_env.Tar('wesnoth-binary.tar.bz2',
                              os.path.join(bindir,"wesnoth"))
bin_tar_env.Append(TARFLAGS='-j', TARCOMSTR="Making binary tarball...")
env.Clean(all, 'wesnoth-binary.tar.bz2')
env.Alias('binary-dist', bin_tarball)

#
# Make data distribution (from installed client side)
#
data_tar_env = env.Clone()
data_tarball = data_tar_env.Tar('wesnoth-data.tar.bz2', datadir)
data_tar_env.Append(TARFLAGS='-j', TARCOMSTR="Making data tarball...")
env.Clean(all, 'wesnoth-data.tar.bz2')
env.Alias('data-dist', data_tarball)

#
# Windows installer
#

text_builder = Builder(action = Copy("$TARGET", "$SOURCE"), single_source = True, suffix = ".txt")
env.WindowsInstaller([
    wesnoth, wesnothd,
    Dir(installable_subs), env["nls"] and Dir("translations") or [],
    glob("*.dll"),
    text_builder(env, source = Split("README copyright COPYING changelog players_changelog"))
    ])

#
# Making Mac OS X application bundles
#
env.Alias("wesnoth-bundle",
          env.Command("Battle For Wesnoth.app", "wesnoth", [
              Mkdir("${TARGET}/Contents"),
              Mkdir("${TARGET}/Contents/MacOS"),
              Mkdir("${TARGET}/Contents/Resources"),
              Action('echo "APPL????" > "${TARGET}/Contents/PkgInfo"'),
              Copy("${TARGET}/Contents/MacOS/wesnoth", "wesnoth"),
              ]))
env.Clean(all, "Battle For Wesnoth.app")    

#
# Sanity checking
#
sanity_check = env.Command('sanity-check', '', [
    Action("cd utils; ./sanity_check"),
    Action("cd data/tools; make sanity-check"),
    ])
env.AlwaysBuild(sanity_check)
env.Precious(sanity_check)

#
# Make the project dependency graph (requires graph-includes).
#
env.Command("wesnoth-deps.dot", [],
            "graph-includes -verbose --class wesnoth \
            -sysI /usr/include/c++/4.0 -sysI /usr/include -sysI /usr/include/SDL \
            --prefixstrip src/ -I src src > ${TARGET}")
env.Command("wesnoth-deps.png", "wesnoth-deps.dot",
            "dot -Tpng -o ${TARGET} ${SOURCE}")
env.Clean(all, ["wesnoth-deps.dot", "wesnoth-deps.png"])

# Local variables:
# mode: python
# end:
