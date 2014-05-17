Install
=======

# Building CrossBridge

## OSX

### Build a Mac only SDK.

1. "make" 

### Build both Mac and Windows (Cross-Compiled) SDK and package them as DMG and ZIP.

1. make all
1. make win
1. make deploy

### OSX 10.9+ Notes

In order to compile the master branch you need Apple GCC 4.2:

1. brew install apple-gcc42
1. sudo ln -s /usr/local/bin/gcc-4.2 /usr/bin/gcc-4.2
1. sudo ln -s /usr/local/bin/g++-4.2 /usr/bin/++-4.2

## Windows

### Bootstrap Cygwin

1. cygwin-dev.bat
1. cd $FLASCC_ROOT

### Build a Windows only SDK and package as ZIP.

1. make all_win
1. make deploy

## Linux

### Build a Linux only SDK and package as ZIP.

1. make all
1. make deploy

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
* glib
* help2man
* java-jdk
* libiconv
* libmpfr
* libgmp
* libuuid
* libncurses
* libxml2
* libzip
* libzip2
* make
* pkg-config
* python
* rsync
* termcap
* unzip

> Install *dev* or *devel* suffixed packages where possible

## Notes

* It's recommended to set \_JAVA\_OPTIONS environment variable according to your system memory:
  
> Example (Windows): set \_JAVA\_OPTIONS=-Xms512m -Xmx4096m
  
> Example (Linux): export \_JAVA\_OPTIONS="-Xms512m -Xmx4096m"

# Hacking

* To discover CrossBridge related changes/patches in 3rd party sources, search for keywords: 'crossbridge', 'flascc', 'alchemy', 'avm2', 'as3'
* Diff tool is great
* Take a look at commits and other forks
* Use dis-assembling ABCs if you need to compare outputs: https://github.com/CyberShadow/RABCDAsm

# Updating

## Adobe PlayerGlobal ASC + SWC

* AIR-SDK-HOME\lib\aot\lib\avmglue.abc => \CROSSBRIDGE-SDK-HOME\tools\playerglobal\VERSION\playerglobal.abc
* AIR-SDK-HOME\frameworks\libs\player\VERSION\playerglobal.swc => \CROSSBRIDGE-SDK-HOME\tools\playerglobal\VERSION\playerglobal.swc

## Mozilla Tamarin ASC

* ftp://ftp.mozilla.org/pub/js/tamarin/builds/asc/latest/asc.jar => \CROSSBRIDGE-SDK-HOME\patches\tamarin-redux-VERSION\utils\asc.jar
  * @see: https://developer.mozilla.org/en-US/docs/Archive/Mozilla/Tamarin/Tamarin_Build_Documentation
