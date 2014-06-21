Install
=======

# Building CrossBridge

## OSX

### Build a Mac only SDK and package as DMG

1. make clean
1. make all 
1. make staging 
1. make flattensymlinks
1. make dmg

### Build both Mac and Windows (Cross-Compiled) SDK and package them as DMG and ZIP

1. make clean
1. make all
1. make cygwinmac
1. make win
1. make deploy

### OSX 10.9+ Notes

In order to compile the Master branch you will need Apple GCC 4.2:

1. brew install apple-gcc42

## Windows

### Bootstrap Cygwin

1. cygwin-dev.bat
1. cd $FLASCC_ROOT

### Build a Windows only SDK and package as ZIP

1. make clean
1. make all_win
1. make deploy

## Linux

### Build a Linux only SDK

1. make clean
1. make all

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

* Disable anti-virus software for faster builds

# Hacking

* To discover CrossBridge related changes/patches in 3rd party sources, search for keywords: 'crossbridge', 'flascc', 'alchemy', 'avm2', 'as3'
* Diff tool is great
* Take a look at commits and other forks
* Use dis-assembling ABCs if you need to compare outputs: https://github.com/CyberShadow/RABCDAsm

# Updating Flash Player Libs

* AIR-SDK-HOME\lib\aot\lib\avmglue.abc => \CROSSBRIDGE-SDK-HOME\tools\playerglobal\VERSION\playerglobal.abc
* AIR-SDK-HOME\frameworks\libs\player\VERSION\playerglobal.swc => \CROSSBRIDGE-SDK-HOME\tools\playerglobal\VERSION\playerglobal.swc
