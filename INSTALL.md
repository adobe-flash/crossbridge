Install
=======

# Building CrossBridge

## OSX

### Build a Mac only SDK.

> "make" 

### Build both Mac and Windows (Cross-Compiled) SDK and package them as DMG and ZIP.

> "make; make win; make deliverables" 

## Windows

### Bootstrap Cygwin

> "cygwin-dev.bat"

> "cd $FLASCC_ROOT"

### Build a Windows only SDK.

> "make all_win"

### Build Windows only SDK and package as ZIP.

> "make all_win; make deliverables" 

## Linux

### Build a Linux only SDK.

> "make"

# Core Dependencies

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
 
# Hacking

* To discover Crossbridge related changes/patches in 3rd party sources, search for keywords: 'crossbridge', 'flascc', 'alchemy', 'avm2', 'as3'

* Diff tool is great

* Take a look at commits and other forks

* Use dis-assembling ABCs if you need to compare outputs: https://github.com/CyberShadow/RABCDAsm

# Upgrading

## Adobe PlayerGlobal ASC + SWC

* AIR_SDK_HOME\lib\aot\lib\avmglue.abc => \CROSSBRIDGE_SDK_HOME\tools\playerglobal\playerglobal.abc
* AIR_SDK_HOME\frameworks\libs\player\VERSION\playerglobal.swc => \CROSSBRIDGE_SDK_HOME\tools\playerglobal\playerglobal.swc

## Mozilla Tamarin ASC

* ftp://ftp.mozilla.org/pub/js/tamarin/builds/asc/latest/asc.jar => patches\tamarin-redux-VERSION\utils\asc.jar
  * @see: https://developer.mozilla.org/en-US/docs/Archive/Mozilla/Tamarin/Tamarin_Build_Documentation