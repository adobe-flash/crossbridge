`configure' configures swig 2.0.4 to adapt to many kinds of systems.

Usage: ./configure [OPTION]... [VAR=VALUE]...

To assign environment variables (e.g., CC, CFLAGS...), specify them as
VAR=VALUE.  See below for descriptions of some of the useful variables.

Defaults for the options are specified in brackets.

Configuration:
  -h, --help              display this help and exit
      --help=short        display options specific to this package
      --help=recursive    display the short help of all the included packages
  -V, --version           display version information and exit
  -q, --quiet, --silent   do not print `checking ...' messages
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
  --docdir=DIR            documentation root [DATAROOTDIR/doc/swig]
  --htmldir=DIR           html documentation [DOCDIR]
  --dvidir=DIR            dvi documentation [DOCDIR]
  --pdfdir=DIR            pdf documentation [DOCDIR]
  --psdir=DIR             ps documentation [DOCDIR]

Program names:
  --program-prefix=PREFIX            prepend PREFIX to installed program names
  --program-suffix=SUFFIX            append SUFFIX to installed program names
  --program-transform-name=PROGRAM   run sed PROGRAM on installed program names

X features:
  --x-includes=DIR    X include files are in DIR
  --x-libraries=DIR   X library files are in DIR

System types:
  --build=BUILD     configure for building on BUILD [guessed]
  --host=HOST       cross-compile to build programs to run on HOST [BUILD]

Optional Features:
  --disable-option-checking  ignore unrecognized --enable/--with options
  --disable-FEATURE       do not include FEATURE (same as --enable-FEATURE=no)
  --enable-FEATURE[=ARG]  include FEATURE [ARG=yes]
  --disable-dependency-tracking  speeds up one-time build
  --enable-dependency-tracking   do not reject slow dependency extractors
  --disable-ccache        disable building and installation of ccache-swig
                          executable (default enabled)

Optional Packages:
  --with-PACKAGE[=ARG]    use PACKAGE [ARG=yes]
  --without-PACKAGE       do not use PACKAGE (same as --with-PACKAGE=no)
  --without-maximum-compile-warnings
                          Disable maximum warning verbosity
  --without-popen         Disable popen
  --without-pcre          Disable support for regular expressions using PCRE
  --with-pcre-prefix=PREFIX
                          Prefix where pcre is installed (optional)
  --with-pcre-exec-prefix=EPREFIX
                          Exec prefix where pcre is installed (optional)
  --with-boost[=ARG]      use Boost library from a standard location
                          (ARG=yes), from the specified location (ARG=<path>),
                          or disable it (ARG=no) [ARG=yes]
  --with-boost-libdir=LIB_DIR
                          Force given directory for boost libraries. Note that
                          this will override library path detection, so use
                          this parameter only if default library detection
                          fails and you know exactly where your boost
                          libraries are located.
  --with-libm=STRING      math library
  --with-libc=STRING      C library
  --with-x                use the X Window System
  --without-alllang       Disable all languages
  --without-tcl           Disable Tcl
  --with-tclconfig=path   Set location of tclConfig.sh
  --with-tcl=path         Set location of Tcl package
  --with-tclincl=path     Set location of Tcl include directory
  --with-tcllib=path      Set location of Tcl library directory
  --without-python        Disable Python
  --with-python=path      Set location of Python executable
  --without-python3       Disable Python 3.x support
  --with-python3=path     Set location of Python 3.x executable
  --without-perl5         Disable Perl5
  --with-perl5=path       Set location of Perl5 executable
  --without-octave        Disable Octave
  --with-octave=path      Set location of Octave executable
  --without-java          Disable Java
  --with-java=path        Set location of java executable
  --with-javac=path       Set location of javac executable
  --with-javaincl=path    Set location of Java include directory
  --without-gcj           Disable GCJ
  --with-gcj=path         Set location of gcj executable
  --with-gcjh=path        Set location of gcjh executable
  --without-guile         Disable Guile
  --with-guile-config=path
                          Set location of guile-config
  --with-guile-prefix=path
                          Set location of Guile tree
  --with-guile=path       Set location of Guile executable
  --with-guileincl=path   Set location of Guile include directory
  --with-guilelib=path    Set location of Guile library directory
  --without-mzscheme      Disable MzScheme
  --with-mzscheme=path    Set location of MzScheme executable
  --with-mzc=path         Set location of MzScheme's mzc
  --without-ruby          Disable Ruby
  --with-ruby=path        Set location of Ruby executable
  --without-php           Disable PHP
  --with-php=path         Set location of PHP executable
  --without-ocaml         Disable OCaml
  --with-ocaml=path       Set location of ocaml executable
  --with-ocamlc=path      Set location of ocamlc executable
  --with-ocamldlgen=path  Set location of ocamldlgen
  --with-ocamlfind=path   Set location of ocamlfind
  --with-ocamlmktop=path  Set location of ocamlmktop executable
  --without-pike          Disable Pike
  --with-pike=path        Set location of Pike executable
  --with-pike-config=path Set location of pike-config script
  --with-pikeincl=path    Set location of Pike include directory
  --without-chicken       Disable CHICKEN
  --with-chicken=path     Set location of CHICKEN executable
  --with-chickencsc=path  Set location of csc executable
  --with-chickencsi=path  Set location of csi executable
  --with-chickenopts=args Set compiler options for static CHICKEN generated code
  --with-chickensharedlib=args    Set linker options for shared CHICKEN generated code
  --with-chickenlib=args  Set linker options for static CHICKEN generated code
  --without-csharp        Disable CSharp
  --with-cil-interpreter=path     Set location of CIL interpreter for CSharp
  --with-csharp-compiler=path     Set location of CSharp compiler
  --without-lua           Disable Lua
  --with-lua=path         Set location of Lua executable
  --with-luaincl=path     Set location of Lua include directory
  --with-lualib=path      Set location of Lua library directory
  --without-allegrocl     Disable Allegro CL
  --with-allegrocl=path   Set location of Allegro CL executable (alisp)
  --without-clisp         Disable CLISP
  --with-clisp=path       Set location of CLISP executable (clisp)
  --without-r             Disable R
  --with-r=path           Set location of R executable (r)
  --without-go            Disable Go
  --with-go=path          Set location of Go compiler
  --without-d             Disable D
  --with-d1-compiler=path  Set location of D1/Tango compiler (DMD compatible)
  --with-d2-compiler=path  Set location of D2 compiler (DMD compatible)
  --with-swiglibdir=DIR   Put SWIG system-independent libraries into DIR.

Some influential environment variables:
  CC          C compiler command
  CFLAGS      C compiler flags
  LDFLAGS     linker flags, e.g. -L<lib dir> if you have libraries in a
              nonstandard directory <lib dir>
  LIBS        libraries to pass to the linker, e.g. -l<library>
  CPPFLAGS    (Objective) C/C++ preprocessor flags, e.g. -I<include dir> if
              you have headers in a nonstandard directory <include dir>
  CXX         C++ compiler command
  CXXFLAGS    C++ compiler flags
  CPP         C preprocessor
  PCRE_CONFIG config script used for pcre
  PCRE_CFLAGS CFLAGS used for pcre
  PCRE_LIBS   LIBS used for pcre
  YACC        The `Yet Another Compiler Compiler' implementation to use.
              Defaults to the first program found out of: `bison -y', `byacc',
              `yacc'.
  YFLAGS      The list of arguments that will be passed by default to $YACC.
              This script will default YFLAGS to the empty string to avoid a
              default value of `-d' given by some make applications.
  XMKMF       Path to xmkmf, Makefile generator for X Window System

Use these variables to override the choices made by `configure' or to help
it to find libraries and programs with nonstandard names/locations.

Report bugs to <http://www.swig.org>.
