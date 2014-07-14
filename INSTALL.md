Install
=======

# Building CrossBridge

## OSX

### Build a Mac only SDK and package as DMG

1. make clean
1. make all 
1. make deploy

### OSX 10.9+ Notes

In order to compile the Master branch you will need Apple GCC 4.2:

1. brew install apple-gcc42

## Windows

### Bootstrap Cygwin

1. Download [Cygwin for SDK developers](http://sourceforge.net/projects/crossbridge-community/files/cygwin-for-sdk-devs.zip/download)
1. Launch *cygwin-dev.bat* from DOS prompt
1. Launch *cd $FLASCC_ROOT* from shell

### Build a Windows only SDK and package as ZIP

1. *make clean*
1. *make all*
1. *make deploy*

# Core Dependencies

* Java JDK x64
* Adobe AIR SDK

# Cygwin Dependencies

* autoconf
* automake
* bison
* cmbzip2
* cmake
* expat
* flex
* gcc-core
* gcc-g++
* gettext
* glib
* help2man
* libarchive
* libiconv
* libmpfr
* libgmp
* libuuid
* libncurses
* libxml2
* libzip
* libzip2
* make
* ncurses
* pkg-config
* python
* rsync
* termcap
* unzip

> Install *dev* or *devel* suffixed packages where possible

## Notes

* Please set *AIR_HOME* environment variable pointing to the SDK location

* It's recommended to set \_JAVA\_OPTIONS environment variable according to your system memory:
  
> Example (Windows): set \_JAVA\_OPTIONS=-Xms512m -Xmx4096m
  
> Example (Linux): export \_JAVA\_OPTIONS="-Xms512m -Xmx4096m"

* Disable anti-virus software for faster builds

* To build a Light SDK without extra libraries use: *make all LIGHTSDK=1*

# Hacking

* To discover CrossBridge related changes/patches in 3rd party sources, search for keywords: 'crossbridge', 'flascc', 'alchemy', 'avm2', 'as3'
* Diff tool is great
* Take a look at commits and other forks
* Use dis-assembling ABCs if you need to compare outputs: https://github.com/CyberShadow/RABCDAsm

# Updating Flash Player Libs

* AIR-SDK-HOME\lib\aot\lib\avmglue.abc => \CROSSBRIDGE-SDK-HOME\tools\playerglobal\VERSION\playerglobal.abc
* AIR-SDK-HOME\frameworks\libs\player\VERSION\playerglobal.swc => \CROSSBRIDGE-SDK-HOME\tools\playerglobal\VERSION\playerglobal.swc
