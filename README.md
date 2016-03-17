# [Crossbridge](www.crossbridge.io) (future branch)


### Getting started

The master branch contains all of the code necessary to build CrossBridge 1.0.1, this is the current stable version. 
The future branch contains some work in progress to upgrade the toolchain to LLVM 3.2 and switch to using the Clang compiler instead of GCC.

Both the mac and Cygwin SDKs are built on Mac, you will need XCode and some MacPorts packages installed to be able to build (pkg-config, glib, and mercurial).

Doing a "make" in the root directory should be sufficient to build just the Mac SDK, doing "make; make win; make deliverables" should build both Mac and Windows and package them as DMG and ZIPs

### Goals 

At present we have quite straight goal:

* LLVM-Clang-3.2 toolchain 
* Freebsd 9.1 Environment
* Libc++ with support of c++11 std.

### Status
By checking against samples, 10 out of 17 pass.
The failure cases fall into 5 categories:

* Exception handling.
* Several system APIs required by pthread and other libs are not implemented.
* Swig-gen'ed code fails at compiling.
* Object-C lib not suport (Does anyone use it?) 
* Debug doesn't work well.

[A precompiled SDK](https://github.com/adobe-flash/crossbridge/blob/futures/Crossbridge_1.1.0.devbuild.dmg) for current code base is under the top dir if you would like to give a try.   
Please note that it could be removed in future.  
Any issue/suggestion/feedback/... please go to issue list.

### Get started in using Crossbridge
Clang is designed to be a drop-in of GCC. And the one in this SDK also keeps the same.  
If you are familiar with Crossbridge SDK 1.0, just get and use.  
If not, please take a look at the document README.html at first to get a basic idea.

### Get involved in development

###### Build system  
The build system only supports Mac OS officially.  
(Well, building in Cygwin is possible, but please pay more patience.)  
So assume you have a machine like what I have 

* Mac OS 10.7+
* \*Xcode 4.6+  
* Memory 4G (8G+ would be great)
* Macport/Homebrew

_NOTE: XCode 6.0+ will fail to compile llvm-3.2. XCode 5.0.1 is suggested. Even with a higher OSX version (Yosemite or El Captain), you can still install XCode, and although the application itself will not run, the apple-llvm and clang compilers can still be used from within the .app packaging_


###### Tools and libs required for building  
Use macport/homebrew to install autoconf, automake, ccache, cmake, gettext,  
 glib, gmp, libmpc, mpfr, pkg-config, readline

For homebrew users, unzip [libmpc.tgz](https://github.com/adobe-flash/crossbridge/blob/futures/libmpc.tgz) to /opt/local/lib

###### Start Building it  
Once you've downloaded the code base, say:  
> $ git clone https://github.com/adobe-flash/crossbridge.git -b futures  
> $ cd crossbridge && make

Build log will be placed in ./build/mac/log. 
SDK will be placed in ./sdk.  

###### Get in developing  
Since the learning curve is not low, what I had were  
1 Read README.html and try examples to learn basic usage.  
2 Setup env and try to build it.
3 Get an overall idea on the process of compiler.  
4 Get a big picture on how Crossbridge compiler works, especially on generating AS/ABC.  
5 Walk through specific code path and verify your idea via debugging.  

###### Extra ref materials  

[Overview of compiler stages](COMPILER.md)

[Overview of LLVM TODOs](LLVM_UPGRADE.md)
