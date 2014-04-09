Building from sources
=====================

## Development

The master branch contains all of the code necessary to build crossbridge 1.0.1, this is the current stable version. The future branch contains some work in progress to upgrade the toolchain to LLVM 3.2 and switch to using the Clang compiler instead of GCC.

Both the mac and cygwin SDKs are built on mac, you will need XCode and some macports packages installed to be able to build (pkg-config, glib, and mercurial).

Doing a "make" in the root directory should be sufficient to build just the mac sdk, doing "make; make win; make deliverables" should build both mac and windows and package them as dmg and zips
    
## Building from source

### Windows (Cygwin x86-32bit)

* Install Cygwin Core
* Install Cygwin Packages:
    * autoconf
    * automake
    * cmake
    * gcc-core
    * gcc-g++
    * gettext
    * gettext-devel
    * glib
    * libuuid
    * make
    * pkg-config
    * python
    * rsync
* Downgrade Cygwin Package - TexInfo 4.X (>=5.0 Not supported!)
* Edit "$CYGWIN_HOME\etc\fstab" with "noacl" resulting "none /cygdrive cygdrive binary,noacl,posix=0,user 0 0"
* Run Cygwin with pre-set variables
    * "set FLASCC_ROOT=\CROSSBRIDGE_SOURCE_ROOT"
    * "set SHELLOPTS=braceexpand:emacs:hashall:histexpand:history:igncr:interactive-comments:monitor"
* Run "cd $FLASCC_ROOT"
* Run "make"
   
### Mac

* Take a look at .travis.yml