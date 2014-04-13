Developer guide
===============

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

## SWIG

* http://www.swig.org/Doc2.0/SWIGDocumentation.html

## Tamarin

* https://developer.mozilla.org/en-US/docs/Tamarin/Tamarin_Build_Documentation

