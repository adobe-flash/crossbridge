Building from sources
=====================
   
## Building from source on Windows (Cygwin x86-32bit)

* Install Cygwin Core
* Install Cygwin Packages:
    * autoconf
    * automake
    * bison
    * cmake
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
* Downgrade Cygwin Package: TexInfo to 4.X (>=5.0 Not supported!)
* Edit "$CYGWIN_HOME\etc\fstab" with "noacl" resulting "none /cygdrive cygdrive binary,noacl,posix=0,user 0 0"
* Run Cygwin with pre-set variables
    * "set FLASCC_ROOT=\CROSSBRIDGE_SOURCE_ROOT"
    * "set SHELLOPTS=braceexpand:emacs:hashall:histexpand:history:igncr:interactive-comments:monitor"
* Run "cd $FLASCC_ROOT"
* Run "make"

