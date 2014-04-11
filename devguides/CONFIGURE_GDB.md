`configure' configures this package to adapt to many kinds of systems.

Usage: ./configure [OPTION]... [VAR=VALUE]...

To assign environment variables (e.g., CC, CFLAGS...), specify them as
VAR=VALUE.  See below for descriptions of some of the useful variables.

Defaults for the options are specified in brackets.

Configuration:
  -h, --help              display this help and exit
      --help=short        display options specific to this package
      --help=recursive    display the short help of all the included packages
  -V, --version           display version information and exit
  -q, --quiet, --silent   do not print `checking...' messages
      --cache-file=FILE   cache test results in FILE [disabled]
  -C, --config-cache      alias for `--cache-file=config.cache'
  -n, --no-create         do not create output files
      --srcdir=DIR        find the sources in DIR [configure dir or `..']

Installation directories:
  --prefix=PREFIX         install architecture-independent files in PREFIX
                          [/usr/local]
  --exec-prefix=EPREFIX   install architecture-dependent files in EPREFIX
                          [PREFIX]

By default, `make install' will install all the files in
`/usr/local/bin', `/usr/local/lib' etc.  You can specify
an installation prefix other than `/usr/local' using `--prefix',
for instance `--prefix=$HOME'.

For better control, use the options below.

Fine tuning of the installation directories:
  --bindir=DIR            user executables [EPREFIX/bin]
  --sbindir=DIR           system admin executables [EPREFIX/sbin]
  --libexecdir=DIR        program executables [EPREFIX/libexec]
  --sysconfdir=DIR        read-only single-machine data [PREFIX/etc]
  --sharedstatedir=DIR    modifiable architecture-independent data [PREFIX/com]
  --localstatedir=DIR     modifiable single-machine data [PREFIX/var]
  --libdir=DIR            object code libraries [EPREFIX/lib]
  --includedir=DIR        C header files [PREFIX/include]
  --oldincludedir=DIR     C header files for non-gcc [/usr/include]
  --datarootdir=DIR       read-only arch.-independent data root [PREFIX/share]
  --datadir=DIR           read-only architecture-independent data [DATAROOTDIR]
  --infodir=DIR           info documentation [DATAROOTDIR/info]
  --localedir=DIR         locale-dependent data [DATAROOTDIR/locale]
  --mandir=DIR            man documentation [DATAROOTDIR/man]
  --docdir=DIR            documentation root [DATAROOTDIR/doc/PACKAGE]
  --htmldir=DIR           html documentation [DOCDIR]
  --dvidir=DIR            dvi documentation [DOCDIR]
  --pdfdir=DIR            pdf documentation [DOCDIR]
  --psdir=DIR             ps documentation [DOCDIR]

Program names:
  --program-prefix=PREFIX            prepend PREFIX to installed program names
  --program-suffix=SUFFIX            append SUFFIX to installed program names
  --program-transform-name=PROGRAM   run sed PROGRAM on installed program names

System types:
  --build=BUILD     configure for building on BUILD [guessed]
  --host=HOST       cross-compile to build programs to run on HOST [BUILD]
  --target=TARGET   configure for building compilers for TARGET [HOST]

Optional Features:
  --disable-option-checking  ignore unrecognized --enable/--with options
  --disable-FEATURE       do not include FEATURE (same as --enable-FEATURE=no)
  --enable-FEATURE[=ARG]  include FEATURE [ARG=yes]
  --enable-gold[=ARG]     build gold [ARG={default,yes,no}]
  --enable-ld[=ARG]       build ld [ARG={default,yes,no}]
  --disable-libquadmath   do not build libquadmath directory
  --disable-libquadmath-support
                          disable libquadmath support for Fortran
  --enable-libada         build libada directory
  --enable-libssp         build libssp directory
  --enable-build-with-cxx build with C++ compiler instead of C compiler
  --disable-ppl-version-check
                          disable check for PPL version
  --enable-cloog-backend[=BACKEND]
                          set the CLooG BACKEND used to either isl, ppl or
                          ppl-legacy (default)
  --disable-cloog-version-check
                          disable check for CLooG version
  --enable-lto            enable link time optimization support
  --enable-stage1-languages[=all]
                          choose additional languages to build during stage1.
                          Mostly useful for compiler development
  --enable-objc-gc        enable use of Boehm's garbage collector with the GNU
                          Objective-C runtime
  --enable-bootstrap      enable bootstrapping [yes if native build]
  --enable-serial-[{host,target,build}-]configure
                          force sequential configuration of sub-packages for
                          the host, target or build machine, or all
                          sub-packages
  --enable-maintainer-mode
                          enable make rules and dependencies not useful (and
                          sometimes confusing) to the casual installer
  --enable-stage1-checking[=all]
                          choose additional checking for stage1 of the
                          compiler
  --enable-werror         enable -Werror in bootstrap stage2 and later

Optional Packages:
  --with-PACKAGE[=ARG]    use PACKAGE [ARG=yes]
  --without-PACKAGE       do not use PACKAGE (same as --with-PACKAGE=no)
  --with-build-libsubdir=DIR  Directory where to find libraries for build system
  --with-mpc=PATH         specify prefix directory for installed MPC package.
                          Equivalent to --with-mpc-include=PATH/include plus
                          --with-mpc-lib=PATH/lib
  --with-mpc-include=PATH specify directory for installed MPC include files
  --with-mpc-lib=PATH     specify directory for the installed MPC library
  --with-mpfr-dir=PATH    this option has been REMOVED
  --with-mpfr=PATH        specify prefix directory for installed MPFR package.
                          Equivalent to --with-mpfr-include=PATH/include plus
                          --with-mpfr-lib=PATH/lib
  --with-mpfr-include=PATH
                          specify directory for installed MPFR include files
  --with-mpfr-lib=PATH    specify directory for the installed MPFR library
  --with-gmp-dir=PATH     this option has been REMOVED
  --with-gmp=PATH         specify prefix directory for the installed GMP
                          package. Equivalent to
                          --with-gmp-include=PATH/include plus
                          --with-gmp-lib=PATH/lib
  --with-gmp-include=PATH specify directory for installed GMP include files
  --with-gmp-lib=PATH     specify directory for the installed GMP library
  --with-host-libstdcxx=L use linker arguments L to link with libstdc++ when
                          linking with PPL
  --with-stage1-ldflags=FLAGS
                          linker flags for stage1
  --with-stage1-libs=LIBS libraries for stage1
  --with-boot-libs=LIBS   libraries for stage2 and later
  --with-boot-ldflags=FLAGS
                          linker flags for stage2 and later
  --with-ppl=PATH         specify prefix directory for the installed PPL
                          package. Equivalent to
                          --with-ppl-include=PATH/include plus
                          --with-ppl-lib=PATH/lib
  --with-ppl-include=PATH specify directory for installed PPL include files
  --with-ppl-lib=PATH     specify directory for the installed PPL library
  --with-cloog=PATH       Specify prefix directory for the installed CLooG-PPL
                          package. Equivalent to
                          --with-cloog-include=PATH/include plus
                          --with-cloog-lib=PATH/lib
  --with-cloog-include=PATH
                          Specify directory for installed CLooG include files
  --with-cloog-lib=PATH   Specify the directory for the installed CLooG
                          library
  --with-build-sysroot=SYSROOT
                          use sysroot as the system root during the build
  --with-debug-prefix-map='A=B C=D ...'
                          map A to B, C to D ... in debug information
  --with-build-config='NAME NAME2...'
                          use config/NAME.mk build configuration
  --with-build-time-tools=PATH
                          use given path to find target tools during the build

Some influential environment variables:
  CC          C compiler command
  CFLAGS      C compiler flags
  LDFLAGS     linker flags, e.g. -L<lib dir> if you have libraries in a
              nonstandard directory <lib dir>
  LIBS        libraries to pass to the linker, e.g. -l<library>
  CPPFLAGS    C/C++/Objective C preprocessor flags, e.g. -I<include dir> if
              you have headers in a nonstandard directory <include dir>
  CXX         C++ compiler command
  CXXFLAGS    C++ compiler flags
  build_configargs
              additional configure arguments for build directories
  host_configargs
              additional configure arguments for host directories
  target_configargs
              additional configure arguments for target directories
  AR          AR for the host
  AS          AS for the host
  DLLTOOL     DLLTOOL for the host
  LD          LD for the host
  LIPO        LIPO for the host
  NM          NM for the host
  RANLIB      RANLIB for the host
  STRIP       STRIP for the host
  WINDRES     WINDRES for the host
  WINDMC      WINDMC for the host
  OBJCOPY     OBJCOPY for the host
  OBJDUMP     OBJDUMP for the host
  CC_FOR_TARGET
              CC for the target
  CXX_FOR_TARGET
              CXX for the target
  GCC_FOR_TARGET
              GCC for the target
  GCJ_FOR_TARGET
              GCJ for the target
  GFORTRAN_FOR_TARGET
              GFORTRAN for the target
  GOC_FOR_TARGET
              GOC for the target
  AR_FOR_TARGET
              AR for the target
  AS_FOR_TARGET
              AS for the target
  DLLTOOL_FOR_TARGET
              DLLTOOL for the target
  LD_FOR_TARGET
              LD for the target
  LIPO_FOR_TARGET
              LIPO for the target
  NM_FOR_TARGET
              NM for the target
  OBJDUMP_FOR_TARGET
              OBJDUMP for the target
  RANLIB_FOR_TARGET
              RANLIB for the target
  STRIP_FOR_TARGET
              STRIP for the target
  WINDRES_FOR_TARGET
              WINDRES for the target
  WINDMC_FOR_TARGET
              WINDMC for the target

Use these variables to override the choices made by `configure' or to help
it to find libraries and programs with nonstandard names/locations.

Report bugs to the package provider.
