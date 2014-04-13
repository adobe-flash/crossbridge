`configure' configures llvm 2.9 to adapt to many kinds of systems.

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
  --bindir=DIR           user executables [EPREFIX/bin]
  --sbindir=DIR          system admin executables [EPREFIX/sbin]
  --libexecdir=DIR       program executables [EPREFIX/libexec]
  --sysconfdir=DIR       read-only single-machine data [PREFIX/etc]
  --sharedstatedir=DIR   modifiable architecture-independent data [PREFIX/com]
  --localstatedir=DIR    modifiable single-machine data [PREFIX/var]
  --libdir=DIR           object code libraries [EPREFIX/lib]
  --includedir=DIR       C header files [PREFIX/include]
  --oldincludedir=DIR    C header files for non-gcc [/usr/include]
  --datarootdir=DIR      read-only arch.-independent data root [PREFIX/share]
  --datadir=DIR          read-only architecture-independent data [DATAROOTDIR]
  --infodir=DIR          info documentation [DATAROOTDIR/info]
  --localedir=DIR        locale-dependent data [DATAROOTDIR/locale]
  --mandir=DIR           man documentation [DATAROOTDIR/man]
  --docdir=DIR           documentation root [DATAROOTDIR/doc/-llvm-]
  --htmldir=DIR          html documentation [DOCDIR]
  --dvidir=DIR           dvi documentation [DOCDIR]
  --pdfdir=DIR           pdf documentation [DOCDIR]
  --psdir=DIR            ps documentation [DOCDIR]

System types:
  --build=BUILD     configure for building on BUILD [guessed]
  --host=HOST       cross-compile to build programs to run on HOST [BUILD]
  --target=TARGET   configure for building compilers for TARGET [HOST]

Optional Features:
  --disable-FEATURE       do not include FEATURE (same as --enable-FEATURE=no)
  --enable-FEATURE[=ARG]  include FEATURE [ARG=yes]
  --enable-polly          Use polly if available (default is YES)
  --enable-optimized      Compile with optimizations enabled (default is YES)
  --enable-profiling      Compile with profiling enabled (default is NO)
  --enable-assertions     Compile with assertion checks enabled (default is NO)
  --enable-expensive-checks
                          Compile with expensive debug checks enabled (default
                          is NO)
  --enable-debug-runtime  Build runtime libs with debug symbols (default is
                          NO)
  --enable-debug-symbols  Build compiler with debug symbols (default is NO if
                          optimization is on and YES if it's off)
  --enable-jit            Enable Just In Time Compiling (default is YES)
  --enable-docs           Build documents (default is YES)
  --enable-doxygen        Build doxygen documentation (default is NO)
  --enable-threads        Use threads if available (default is YES)
  --enable-pthreads       Use pthreads if available (default is YES)
  --enable-pic            Build LLVM with Position Independent Code (default
                          is YES)
  --enable-shared         Build a shared library and link tools against it
                          (default is NO)
  --enable-embed-stdcxx   Build a shared library with embedded libstdc++ for
                          Win32 DLL (default is YES)
  --enable-timestamps     Enable embedding timestamp information in build
                          (default is YES)
  --enable-targets        Build specific host targets: all or
                          target1,target2,... Valid targets are: host, x86,
                          x86_64, sparc, powerpc, alpha, arm, mips, spu,
                          xcore, msp430, systemz, blackfin, ptx, cbe, and cpp
                          (default=all)
  --enable-cbe-printf-a   Enable C Backend output with hex floating point via
                          %a (default is YES)
  --enable-bindings       Build specific language bindings:
                          all,auto,none,{binding-name} (default=auto)
  --enable-libffi         Check for the presence of libffi (default is NO)
  --enable-ltdl-install   install libltdl

Optional Packages:
  --with-PACKAGE[=ARG]    use PACKAGE [ARG=yes]
  --without-PACKAGE       do not use PACKAGE (same as --with-PACKAGE=no)
  --with-llvmgccdir       Specify location of llvm-gcc install dir (default
                          searches PATH)
  --with-llvmgcc          Specify location of llvm-gcc driver (default
                          searches PATH)
  --with-llvmgxx          Specify location of llvm-g++ driver (default
                          searches PATH)
  --with-clang            Specify location of clang compiler (default is
                          --with-built-clang)
  --with-built-clang      Use the compiled Clang as the LLVM compiler
                          (default=check)
  --with-optimize-option  Select the compiler options to use for optimized
                          builds
  --with-extra-options    Specify additional options to compile LLVM with
  --with-ocaml-libdir     Specify install location for ocaml bindings (default
                          is stdlib)
  --with-clang-resource-dir
                          Relative directory from the Clang binary for
                          resource files
  --with-c-include-dirs   Colon separated list of directories clang will
                          search for headers
  --with-cxx-include-root Directory with the libstdc++ headers.
  --with-cxx-include-arch Architecture of the libstdc++ headers.
  --with-cxx-include-32bit-dir
                          32 bit multilib dir.
  --with-cxx-include-64bit-dir
                          64 bit multilib directory.
  --with-binutils-include Specify path to binutils/include/ containing
                          plugin-api.h file for gold plugin.
  --with-tclinclude       directory where tcl headers are
  --with-llvmcc=<name>    Choose the LLVM capable compiler to use (llvm-gcc,
                          clang, or none; default=check)
  --with-udis86=<path>    Use udis86 external x86 disassembler library
  --with-oprofile=<prefix>
                          Tell OProfile >= 0.9.4 how to symbolize JIT output

Some influential environment variables:
  CC          C compiler command
  CFLAGS      C compiler flags
  LDFLAGS     linker flags, e.g. -L<lib dir> if you have libraries in a
              nonstandard directory <lib dir>
  CPPFLAGS    C/C++/Objective C preprocessor flags, e.g. -I<include dir> if
              you have headers in a nonstandard directory <include dir>
  CPP         C preprocessor
  CXX         C++ compiler command
  CXXFLAGS    C++ compiler flags

Use these variables to override the choices made by `configure' or to help
it to find libraries and programs with nonstandard names/locations.

Report bugs to <llvmbugs@cs.uiuc.edu>.
