Usage: configure [options] [host]
Options: [defaults in brackets after descriptions]
Configuration:
  --cache-file=FILE       cache test results in FILE
  --help                  print this message
  --no-create             do not create output files
  --quiet, --silent       do not print `checking...' messages
  --version               print the version of autoconf that created configure
Directory and file names:
  --prefix=PREFIX         install architecture-independent files in PREFIX
                          [/usr/local]
  --exec-prefix=EPREFIX   install architecture-dependent files in EPREFIX
                          [same as prefix]
  --bindir=DIR            user executables in DIR [EPREFIX/bin]
  --sbindir=DIR           system admin executables in DIR [EPREFIX/sbin]
  --libexecdir=DIR        program executables in DIR [EPREFIX/libexec]
  --datadir=DIR           read-only architecture-independent data in DIR
                          [PREFIX/share]
  --sysconfdir=DIR        read-only single-machine data in DIR [PREFIX/etc]
  --sharedstatedir=DIR    modifiable architecture-independent data in DIR
                          [PREFIX/com]
  --localstatedir=DIR     modifiable single-machine data in DIR [PREFIX/var]
  --libdir=DIR            object code libraries in DIR [EPREFIX/lib]
  --includedir=DIR        C header files in DIR [PREFIX/include]
  --oldincludedir=DIR     C header files for non-gcc in DIR [/usr/include]
  --infodir=DIR           info documentation in DIR [PREFIX/info]
  --mandir=DIR            man documentation in DIR [PREFIX/man]
  --srcdir=DIR            find the sources in DIR [configure dir or ..]
  --program-prefix=PREFIX prepend PREFIX to installed program names
  --program-suffix=SUFFIX append SUFFIX to installed program names
  --program-transform-name=PROGRAM
                          run sed PROGRAM on installed program names
Host type:
  --build=BUILD           configure for building on BUILD [BUILD=HOST]
  --host=HOST             configure for HOST [guessed]
  --target=TARGET         configure for TARGET [TARGET=HOST]
Features and packages:
  --disable-FEATURE       do not include FEATURE (same as --enable-FEATURE=no)
  --enable-FEATURE[=ARG]  include FEATURE [ARG=yes]
  --with-PACKAGE[=ARG]    use PACKAGE [ARG=yes]
  --without-PACKAGE       do not use PACKAGE (same as --with-PACKAGE=no)
  --x-includes=DIR        X include files are in DIR
  --x-libraries=DIR       X library files are in DIR
--enable and --with options recognized:
  --with-build-libsubdir=[DIR]  Directory where to find libraries for build system
  --enable-libada        Builds libada directory
  --enable-libssp        Builds libssp directory
  --with-mpfr-dir=PATH    This option has been REMOVED
  --with-mpfr=PATH        Specify prefix directory for installed MPFR package
                          Equivalent to --with-mpfr-include=PATH/include
                          plus --with-mpfr-lib=PATH/lib
  --with-mpfr-include=PATH
                          Specify directory for installed MPFR include files
  --with-mpfr-lib=PATH    Specify the directory for the installed MPFR library
  --with-gmp-dir=PATH     This option has been REMOVED
  --with-gmp=PATH         Specify prefix directory for the installed GMP package
                          Equivalent to --with-gmp-include=PATH/include
                          plus --with-gmp-lib=PATH/lib
  --with-gmp-include=PATH Specify directory for installed GMP include files
  --with-gmp-lib=PATH     Specify the directory for the installed GMP library
  --enable-objc-gc       enable the use of Boehm's garbage collector with
                          the GNU Objective-C runtime.
  --with-build-sysroot=sysroot
                          use sysroot as the system root during the build
  --enable-bootstrap           Enable bootstrapping [yes if native build]
  --enable-serial-[{host,target,build}-]configure
                          Force sequential configuration of
                          sub-packages for the host, target or build
			  machine, or all sub-packages
  --with-build-time-tools=path
                          use given path to find target tools during the build
  --enable-maintainer-mode enable make rules and dependencies not useful
                          (and sometimes confusing) to the casual installer
  --enable-stage1-checking[=all]   choose additional checking for stage1
                          of the compiler.
  --enable-werror         enable -Werror in bootstrap stage2 and later
  --with-datarootdir	Use datarootdir as the data root directory.
  --with-docdir	Install documentation in this directory.
  --with-htmldir	Install html in this directory.
