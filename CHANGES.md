Changes
=======

## Makefile

* Changed Cygwin compiler names ("gcc-4" => "gcc")

* Fixed ASDoc generation on windows ("$(SRCROOT)/posix/vfs" => "$(call nativepath,$(SRCROOT)/posix/vfs)")

* Fixed Cygwin FreeBSD issues with CR/LF (dos2unix)

* Disabled documentation and i18n generation (disable-doc and disable-nls)
  
## Code

* Fixed compiler error (argument type)
  * \tools\noenv\noenv.c

* Fixed compiler error (multi-line rows) [new version of python tool should solve this change set]
  * \posix\syscalls.changed
  * \posix\syscalls.master
  
* Fixed compiler error (missing header: unistd.h)
  * \tools\as\as.cpp
  * \llvm-2.9\lib\Target\AVM2\AVM2MCAsmStreamer.cpp
  * \gold-plugins\makeswf.cpp
  
* Changed typos due gettex>5 errors ( @colophon => @@colophon, @cyrus => @@cyrus)
  * \binutils\bfd\doc\bfd.texinfo
  
* Removed default argument definition: %{!jvmopt=*:-jvmopt=-Xmx1500M}
  * \llvm-gcc-4.2-2.9\gcc\config\avm2.hu
  
* Changed from "cygpath -at unix" to "cygpath -at mixed"
  * \avm2_env\misc\SeAlchemySDKLocation.c
  
* Changed from "cygpath -m" to "cygpath -at mixed"
  * \tools\as\as.cpp
  
* Changed from "cygpath -at windows" to "cygpath -at mixed"
  * \llvm-2.9\tools\gold\gold-plugin.cpp
  * \llvm-gcc-4.2-2.9\gcc\llvm-backend.cpp

## Dependency

* Introduced tar-d untouched packages
  * \packages\..
  
* Upgraded BMake
  * \bmake-20140214
  
* Upgraded CMake
  * \cmake-2.8.12.2
  
## Other

* Fixed gitignore, CMake has a 'build' folder with sources!!!
./ccache/
./build/
./sdk/

