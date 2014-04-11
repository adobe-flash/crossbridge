Building from sources
=====================
   
## Building from source on MacOSX

The master branch contains all of the code necessary to build crossbridge 1.0.1, this is the current stable version. The future branch contains some work in progress to upgrade the toolchain to LLVM 3.2 and switch to using the Clang compiler instead of GCC.

Both the mac and cygwin SDKs are built on mac, you will need XCode and some macports packages installed to be able to build (pkg-config, glib, and mercurial).

Doing a "make" in the root directory should be sufficient to build just the mac sdk, doing "make; make win; make deliverables" should build both mac and windows and package them as dmg and zips
 