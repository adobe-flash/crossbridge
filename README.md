# [CrossBridge](http://www.crossbridge.io) 

[![Build Status](https://travis-ci.org/vpmedia/crossbridge.svg?branch=master)](https://travis-ci.org/vpmedia/crossbridge)

CrossBridge is the open-source version of Adobe FlasCC (formerly the [Alchemy] (http://labs.adobe.com/technologies/alchemy/) project). 
It provides a complete C/C++ development environment for targeting the Adobe Flash Runtime.  

Features:

* [LLVM-GCC 4.2](http://llvm.org) compiler with Flash backend
* [GDB](http://www.sourceware.org/gdb) debugger that debugs your code while running in the Flash Player
* [SWIG](http://www.swig.org) backend that auto-generates inter-operability code, allowing you to build Flash libraries (SWCs) that wrap C/C++ libs
* Multi-threading based on [POSIX Threads](https://en.wikipedia.org/wiki/POSIX_Threads)

### Getting started using CrossBridge

* [Download the latest stable SDK for your OS](http://sourceforge.net/projects/crossbridge/files/)
* Open README.html to get a basic idea of CrossBridge usage
* For more information about the project, please visit http://www.crossbridge.io

### Maintenance releases by VPMedia

* [Changes] (https://github.com/vpmedia/crossbridge/blob/master/CHANGES.md)

#### CrossBridge 1.0.2 - Windows only

* [Download] (https://dl.dropboxusercontent.com/u/1375050/cb_master_sdk-win.zip)

### Getting involved in development 

Check out the [Install Guide](https://github.com/vpmedia/crossbridge/blob/master/INSTALL.md)

#### Master branch

1. Create a fork
1. Check out [TODOs](https://github.com/vpmedia/crossbridge/blob/master/TODO.md)
1. Report a new issue, properly describing the situation - backed up with logs
1. Submit a patch for the issue or ask others to do so
1. Submit pull requests with complete set of new features

#### Futures branch

The main goal is to support C++11 by upgrading the compiler infrastructure to the latest LLVM tool-chain (3.X) with FreeBSD 9+

1. Create a fork
1. Check out [TODOs](https://github.com/vpmedia/crossbridge/blob/futures/TODO.md)
1. Check out [LLVM Upgrade](https://github.com/vpmedia/crossbridge/blob/futures/LLVM_UPGRADE.md)
1. Solve issues and submit pull requests

