Changes
=======

## 08/04/14 

* Updated CMake to 2.8.12.2
  \cmake-2.8.12.2
* Changed Cygwin compiler names ("gcc-4" => "gcc")
  * \Makefile
* Fixed ASDoc generation on windows ("$(SRCROOT)/posix/vfs" => "$(call nativepath,$(SRCROOT)/posix/vfs)")
  * \Makefile
* Fixed compiler error (argument type)
  * \tools\noenv\noenv.c
* Fixed compiler error (multi-line rows)
  * \posix\syscalls.changed
  * \posix\syscalls.master
* Fixed compiler error (missing header: unistd.h)
  * \tools\as\as.cpp
  * \llvm-2.9\lib\Target\AVM2\AVM2MCAsmStreamer.cpp
  
## 09/04/14

* Disabled dependency documentation generation
  * \Makefile
* Fixed compiler error (missing header: unistd.h)
  * \gold-plugins\makeswf.cpp