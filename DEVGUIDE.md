Developer guide
===============

# Building from source

## MacOSX

The master branch contains all of the code necessary to build crossbridge 1.0.1, this is the current stable version. The future branch contains some work in progress to upgrade the toolchain to LLVM 3.2 and switch to using the Clang compiler instead of GCC.

Both the mac and cygwin SDKs are built on mac, you will need XCode and some macports packages installed to be able to build (pkg-config, glib, and mercurial).

Doing a "make" in the root directory should be sufficient to build just the mac sdk, doing "make; make win; make deliverables" should build both mac and windows and package them as dmg and zips

## Windows (Cygwin x86-32bit)

* Install Cygwin Core
* Install Cygwin Packages:
    * autoconf
    * automake
    * bison
    * ccache
    * cmbzip2
    * cmake
    * expat
    * flex
    * gcc-core
    * gcc-g++
    * gettext
    * gettext-devel
    * glib
    * help2man
    * libiconv
    * libmpfr
    * libuuid
    * libuuid-devel
    * libncurses
    * libncurses-devel
    * make
    * pkg-config
    * python
    * rsync
    * termcap
    * unzip
* Downgrade Cygwin Package: TexInfo to 4.X (>=5.0 Not supported!)
* Edit "$CYGWIN_HOME\etc\fstab" with "noacl" resulting "none /cygdrive cygdrive binary,noacl,posix=0,user 0 0"
* Run Cygwin with pre-set variables
    * "set FLASCC_ROOT=\CROSSBRIDGE_SOURCE_ROOT"
    * "set SHELLOPTS=braceexpand:emacs:hashall:histexpand:history:igncr:interactive-comments:monitor"
* Run "cd $FLASCC_ROOT"
* Run "make"

## Linux

TBD


# Build steps

## base

* create folders
* link executables
* cache executables
* export swfmake and projectormake ABCs

## make

* create own gnu make

## cmake

* create own cmake

## abclibs

* generate posix interface using gensyscalls.py
* build ABCs

## basictools

* compile 'uname.c'
* compile 'noenv.c'
* compile 'as.cpp'
* compile 'alctool.java'
* compile 'alcdb.java'

## llvm

* assemble llvm tool chain

## binutils

* create own binutils

## plugins

* compile 'makeswf.cpp'
* compile 'multiplug.cpp'

## bmake

* create own bmake

## stdlibs

* csu
* libc
* libthr
* libm
* libBlocksRuntime
* libcxx

## as3xx

* compile 'AS3++.cpp'

## as3wig

* compile 'AS3Wig.java'

## abcstdlibs

* abcflashpp 
* abcstdlibs_more
  * libcHack
  * libc
  * libthr
  * libgcc
  * libstdcpp
  * libsupcpp
  * libobjc
  * libBlocksRuntime
  * libcxx_abc

## sdkcleanup

* clean

## tr

* assemble 'projectormake'

## trd

* assemble 'avmshell-release-debugger'

## extralibs

* zlib 
* libvgl 
* libjpeg 
* libpng 
* libsdl (TODO)
* dmalloc (TODO)
* libffi(TODO)

## extratools

* genfs 
* gdb 
* swig 
* pkgconfig 
* libtool

## finalcleanup

* clean
* sync posix tools and sources

## submittests

* pthreadsubmittests_shell 
* pthreadsubmittests_swf 
* helloswf 


# Useful links

## CMake

* http://www.cmake.org/cmake/help/install.html

## GNU Make

* https://www.gnu.org/software/make/manual/

## LLVM

* http://llvm.org/releases/3.2/docs/CMake.html
* http://llvm.org/docs/GoldPlugin.html
* http://llvm.org/docs/WritingAnLLVMBackend.html
* http://llvm.org/docs/TableGen/index.html
* https://github.com/maidsafe/MaidSafe/wiki/Hacking-with-Clang-llvm-abi-and-llvm-libc
* http://libcxx.llvm.org/

## SWIG

* http://www.swig.org/Doc2.0/SWIGDocumentation.html

## Tamarin

* https://developer.mozilla.org/en-US/docs/Tamarin/Tamarin_Build_Documentation

