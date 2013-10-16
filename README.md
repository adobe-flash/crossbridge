# [Crossbridge][http://www.crossbridge.io] 

Crossbridge is the open-source version of Adobe FlasCC (formerly the Alchemy project). It provides a complete C/C++ development environment for targetting the Adobe Flash Runtime.  
For more information about this project, please visit http://adobe-flash.github.io/crossbridge/.  
To get the latest release version, please visit the http://sourceforge.net/projects/crossbridge/files/.

Features:

* GCC 4.2 compiler with Flash backend.
* GDB debugger that debugs your code while running in the Flash player.
* SWIG backend that auto generates interop code allowing you to build Flash libs (SWCs) that wrap C/C++ libraries.
* pthread based multithreading (in Flash 11.5+)

### Development
The master branch contains all of the code necessary to build crossbridge 1.0.1, this is the current stable version. The future branch contains some work in progress to upgrade the toolchain to LLVM 3.2 and switch to using the Clang compiler instead of GCC.

Both the mac and cygwin SDKs are built on mac, you will need XCode and some macports packages installed to be able to build (pkg-config, glib, and mercurial).

Doing a "make" in the root directory should be sufficient to build just the mac sdk, doing "make; make win; make deliverables" should build both mac and windows and package them as dmg and zips

### Future
Futures branch contains the upgrading code.
Comparing to master, it will mainly has
* Clang/LLVM 3.2 toolchain 
* Freebsd 9.1 Environment
* Libc++ with support of c++11 std.

### Start using crossbridge
Download a copy of SDK for your OS at http://sourceforge.net/projects/crossbridge/files/
Open README.html to get a basic idea of crossbridge usage.

