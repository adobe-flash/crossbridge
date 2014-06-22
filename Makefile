# ====================================================================================
# CrossBridge Makefile
# ====================================================================================
$?UNAME=$(shell uname -s)
$?SRCROOT=$(PWD)
ESCAPED_SRCROOT=$(shell echo $(SRCROOT) | sed -e 's/[\/&]/\\&/g')
$?SDK=$(PWD)/sdk
$?BUILDROOT=$(PWD)/build
$?WIN_BUILD=$(BUILDROOT)/win
$?MAC_BUILD=$(BUILDROOT)/mac
$?LINUX_BUILD=$(BUILDROOT)/linux
$?CYGWINMAC=$(SRCROOT)/cygwinmac/sdk/usr/bin

# ====================================================================================
# DEPENDENCIES
# ====================================================================================
$?DEPENDENCY_AVMPLUS=avmplus
$?DEPENDENCY_BINUTILS=binutils
$?DEPENDENCY_BMAKE=bmake
$?DEPENDENCY_CMAKE=cmake-2.8.12.2
$?DEPENDENCY_DMALLOC=dmalloc-5.5.2
$?DEPENDENCY_FFI=libffi-3.0.11
$?DEPENDENCY_GDB=gdb-7.3
$?DEPENDENCY_JPEG=jpeg-8c
$?DEPENDENCY_LIBAA=aalib-1.2
$?DEPENDENCY_LIBBZIP=bzip2-1.0.6
$?DEPENDENCY_LIBEIGEN=eigen-3.1.2
$?DEPENDENCY_LIBFLAC=flac-1.2.1
$?DEPENDENCY_LIBFREETYPE=freetype-2.5.3
$?DEPENDENCY_LIBGIF=giflib-5.0.5
$?DEPENDENCY_LIBGMP=gmp-6.0.0
$?DEPENDENCY_LIBICONV=libiconv-1.14
$?DEPENDENCY_LIBOGG=libogg-1.3.1
$?DEPENDENCY_LIBPNG=libpng-1.5.7
$?DEPENDENCY_LIBPROTOBUF=protobuf-2.5.0
$?DEPENDENCY_LIBPHYSFS=physfs-2.0.3
$?DEPENDENCY_LIBNCURSES=ncurses-5.9
$?DEPENDENCY_LIBREADLINE=readline-6.3
$?DEPENDENCY_LIBSNDFILE=libsndfile-1.0.25
$?DEPENDENCY_LIBSDL=SDL-1.2.14
$?DEPENDENCY_LIBSDLIMAGE=SDL_image-1.2.12
$?DEPENDENCY_LIBSDLMIXER=SDL_mixer-1.2.12
$?DEPENDENCY_LIBSDLTTF=SDL_ttf-2.0.11
$?DEPENDENCY_LIBTIFF=tiff-4.0.3
$?DEPENDENCY_LIBTOOL=libtool-2.4.2
$?DEPENDENCY_LIBVORBIS=libvorbis-1.3.4
$?DEPENDENCY_LIBWEBP=libwebp-0.4.0
$?DEPENDENCY_LIBXZ=xz-5.0.5
$?DEPENDENCY_LLVM=llvm-2.9
$?DEPENDENCY_LLVM_GCC=llvm-gcc-4.2-2.9
$?DEPENDENCY_MAKE=make-4.0
$?DEPENDENCY_OPENSSL=openssl-1.0.1g
$?DEPENDENCY_PKG_CFG=pkg-config-0.26
$?DEPENDENCY_SCIMARK=scimark2_1c
$?DEPENDENCY_SWIG=swig-3.0.0
$?DEPENDENCY_ZLIB=zlib-1.2.5
$?DEPENDENCY_DEJAGNU=dejagnu-1.5

# ====================================================================================
# HOST PLATFORM OPTIONS
# ====================================================================================
# Windows or OSX or Linux
ifneq (,$(findstring CYGWIN,$(UNAME)))
	$?PLATFORM="cygwin"
	$?RAWPLAT=cygwin
	$?THREADS=2
	$?nativepath=$(shell cygpath -at mixed $(1))
	$?BUILD_TRIPLE=i686-pc-cygwin
	$?FLASH_PLAYER_EXE=$(SRCROOT)/test/player/Debug/FlashPlayerDebugger.exe
	$?NOPIE=
	$?BIN_TRUE=/usr/bin/true
else ifneq (,$(findstring Darwin,$(UNAME)))
	$?PLATFORM="darwin"
	$?RAWPLAT=darwin
	$?THREADS=$(shell sysctl -n hw.ncpu)
	$?nativepath=$(1)
	$?BUILD_TRIPLE=x86_64-apple-darwin10
	$?FLASH_PLAYER_EXE=$(SRCROOT)/test/player/Debug/Flash Player.app
	$?NOPIE=-no_pie
	$?BIN_TRUE=/usr/bin/true
else
	$?PLATFORM="linux"
	$?RAWPLAT=linux
	$?THREADS=1
	$?nativepath=$(1)
	$?BUILD_TRIPLE=x86_64-unknown-linux-gnu
	$?FLASH_PLAYER_EXE=$(SRCROOT)/test/player/Debug/Flash Player.app
	$?NOPIE=
	$?BIN_TRUE=/bin/true
endif

# ====================================================================================
# TARGET PLATFORM OPTIONS
# ====================================================================================
# Windows
ifneq (,$(findstring cygwin,$(PLATFORM)))
	$?CC=gcc
	$?CXX=g++
	$?EXEEXT=.exe
	$?SOEXT=.dll
	$?SDLFLAGS=
	$?TAMARIN_CONFIG_FLAGS=--target=i686-linux
	$?TAMARINLDFLAGS=" -Wl,--stack,16000000"
	$?TAMARINOPTFLAGS=-Wno-unused-function -Wno-unused-local-typedefs -Wno-maybe-uninitialized -Wno-narrowing -Wno-sizeof-pointer-memaccess -Wno-unused-variable -Wno-unused-but-set-variable -Wno-deprecated-declarations 
	$?BUILD=$(WIN_BUILD)
	$?PLATFORM_NAME=win
	$?HOST_TRIPLE=i686-pc-cygwin
endif
# OSX 
ifneq (,$(findstring darwin,$(PLATFORM)))
	$?CC=gcc-4.2
	$?CXX=g++-4.2
	$?EXEEXT=
	$?SOEXT=.dylib
	$?SDLFLAGS=--build=i686-apple-darwin10
	$?TAMARIN_CONFIG_FLAGS=
	$?TAMARINLDFLAGS=" -m32 -arch=i686"
	$?TAMARINOPTFLAGS=-Wno-deprecated-declarations 
	$?BUILD=$(MAC_BUILD)
	$?PLATFORM_NAME=mac
	$?HOST_TRIPLE=x86_64-apple-darwin10
	export PATH:=$(BUILD)/ccachebin:$(PATH)
endif
# Linux
ifneq (,$(findstring linux,$(PLATFORM)))
	$?CC=gcc
	$?CXX=g++
	$?EXEEXT=
	$?SOEXT=.so
	$?SDLFLAGS=--build=i686-unknown-linux
	$?TAMARIN_CONFIG_FLAGS=
	$?TAMARINLDFLAGS=" -m32 -arch=i686"
	$?TAMARINOPTFLAGS=-Wno-deprecated-declarations 
	$?BUILD=$(LINUX_BUILD)
	$?PLATFORM_NAME=linux
	$?HOST_TRIPLE=x86_64-unknown-linux
	export PATH:=$(BUILD)/ccachebin:$(PATH)
endif

# Cross-Compile Options
$?CYGTRIPLE=i686-pc-cygwin
$?MINGWTRIPLE=i686-mingw32
$?TRIPLE=avm2-unknown-freebsd8

# ====================================================================================
# GNU Tool-chain and CC Options
# ====================================================================================
# Host Tools
#$?CC_FOR_BUILD=gcc
export CC:=$(CC)
export CXX:=$(CXX)
export CCACHE_DIR=$(SRCROOT)/ccache
# linker tool (symbolic force no-dereference)
$?LN=ln -sfn
# sync tool
$?RSYNC=rsync -az --no-p --no-g --chmod=ugo=rwX -l
# archive tool
$?NATIVE_AR=ar
# java tool
$?JAVA=$(call nativepath,$(shell which java))
# python tool
$?PYTHON=$(call nativepath,$(shell which python))
# Target Tools
$?AR=$(SDK)/usr/bin/ar scru -v
$?SDK_CC=$(SDK)/usr/bin/gcc
$?SDK_CXX=$(SDK)/usr/bin/g++
$?SDK_AR=$(SDK)/usr/bin/ar
$?SDK_NM=$(SDK)/usr/bin/nm
$?SDK_CMAKE=$(SDK)/usr/bin/cmake
$?SDK_MAKE=$(SDK)/usr/bin/make
# Extra Tool (Used by LLVM test)
$?FPCMP=$(BUILDROOT)/extra/fpcmp$(EXEEXT)
# Common Flags
$?DBGOPTS=
$?LIBHELPEROPTFLAGS=-O3
$?CFLAGS=-O4
$?CXXFLAGS=-O4

# ====================================================================================
# LLVM and Clang options
# ====================================================================================
$?LLVMASSERTIONS=OFF
$?LLVMTESTS=OFF
$?LLVMCMAKEOPTS= 
$?LLVMLDFLAGS=
$?LLVMCFLAGS=
$?LLVMCXXFLAGS=
$?LLVMINSTALLPREFIX=$(BUILD)
$?LLVM_ONLYLLC=false
$?LLVMBUILDTYPE=MinSizeRel
$?LLVMTARGETS=AVM2;AVM2Shim;X86;CBackend
$?CLANG=ON

# ====================================================================================
# AIR or Flex SDK options
# ====================================================================================
ifneq "$(wildcard $(AIR_HOME)/lib/compiler.jar)" ""
 $?FLEX_SDK_TYPE=AdobeAIR
 $?FLEX_SDK_HOME=$(AIR_HOME)
 $?FLEX_ASDOC=java -classpath "$(call nativepath,$(AIR_HOME)/lib/legacy/asdoc.jar)" -Dflex.compiler.theme= -Dflexlib=$(call nativepath,$(AIR_HOME)/frameworks) flex2.tools.ASDoc -compiler.fonts.local-fonts-snapshot=
else ifneq "$(wildcard $(FLEX_HOME)/lib/flex-compiler-oem.jar)" ""
 $?FLEX_SDK_TYPE=ApacheFlex
 $?FLEX_SDK_HOME=$(FLEX_HOME)
 $?FLEX_ASDOC=java -classpath "$(call nativepath,$(FLEX_SDK_HOME)/lib/asdoc.jar)" -Dflexlib=$(call nativepath,$(FLEX_SDK_HOME)/frameworks) flex2.tools.ASDoc
else 
 $?FLEX_SDK_TYPE=AdobeFlex
 $?FLEX_SDK_HOME=$(SRCROOT)/tools/flexsdk
 $?FLEX_ASDOC=java -classpath "$(call nativepath,$(FLEX_SDK_HOME)/lib/asdoc.jar)" -Dflexlib=$(call nativepath,$(FLEX_SDK_HOME)/frameworks) flex2.tools.ASDoc
endif

# ====================================================================================
# Tamarin options
# ====================================================================================
$?TAMARINCONFIG=CFLAGS=" -m32 -I$(SRCROOT)/avm2_env/misc -I/usr/local/Cellar/apple-gcc42/4.2.1-5666.3/lib/gcc/i686-apple-darwin11/4.2.1/include/ -DVMCFG_ALCHEMY_SDK_BUILD " CXXFLAGS=" -m32 -I$(SRCROOT)/avm2_env/misc -I/usr/local/Cellar/apple-gcc42/4.2.1-5666.3/lib/gcc/i686-apple-darwin11/4.2.1/include/ $(TAMARINOPTFLAGS) -DVMCFG_ALCHEMY_SDK_BUILD " LDFLAGS=$(TAMARINLDFLAGS) $(SRCROOT)/$(DEPENDENCY_AVMPLUS)/configure.py --enable-shell --enable-alchemy-posix $(TAMARIN_CONFIG_FLAGS)
$?ASC=$(call nativepath,$(SRCROOT)/$(DEPENDENCY_AVMPLUS)/utils/asc.jar)
#$?SCOMP=java -classpath $(ASC) macromedia.asc.embedding.ScriptCompiler -abcfuture -AS3 -import $(call nativepath,$(SRCROOT)/$(DEPENDENCY_AVMPLUS)/generated/builtin.abc)  -import $(call nativepath,$(SRCROOT)/$(DEPENDENCY_AVMPLUS)/generated/shell_toplevel.abc)
ifeq "$(FLEX_SDK_TYPE)" "AdobeAIR"
 $?SCOMPFALCON=java -classpath $(call nativepath,$(AIR_HOME)/lib/compiler.jar) com.adobe.flash.compiler.clients.ASC -merge -md -abcfuture -AS3 -import $(call nativepath,$(SRCROOT)/$(DEPENDENCY_AVMPLUS)/generated/builtin.abc)  -import $(call nativepath,$(SRCROOT)/$(DEPENDENCY_AVMPLUS)/generated/shell_toplevel.abc)
else
 $?SCOMPFALCON=java -jar $(call nativepath,$(SRCROOT)/tools/lib/asc2.jar) -merge -md -abcfuture -AS3 -import $(call nativepath,$(SRCROOT)/$(DEPENDENCY_AVMPLUS)/generated/builtin.abc)  -import $(call nativepath,$(SRCROOT)/$(DEPENDENCY_AVMPLUS)/generated/shell_toplevel.abc)
endif
$?ABCLIBOPTS=-config CONFIG::asdocs=false -config CONFIG::actual=true
$?AVMSHELL=$(SDK)/usr/bin/avmshell$(EXEEXT)

# ====================================================================================
# Other options
# ====================================================================================
$?FLASCC_VERSION_MAJOR:=1
$?FLASCC_VERSION_MINOR:=0
$?FLASCC_VERSION_PATCH:=3
$?FLASCC_VERSION_BUILD:=devbuild
$?SDKNAME=CrossBridge_$(FLASCC_VERSION_MAJOR).$(FLASCC_VERSION_MINOR).$(FLASCC_VERSION_PATCH).$(FLASCC_VERSION_BUILD)
BUILD_VER_DEFS"-DFLASCC_VERSION_MAJOR=$(FLASCC_VERSION_MAJOR) -DFLASCC_VERSION_MINOR=$(FLASCC_VERSION_MINOR) -DFLASCC_VERSION_PATCH=$(FLASCC_VERSION_PATCH) -DFLASCC_VERSION_BUILD=$(FLASCC_VERSION_BUILD)"

# ====================================================================================
# BMAKE
# ====================================================================================
#TODO are we done sweeping for asm?
#$?BMAKE=AR='/usr/bin/true ||' GENCAT=/usr/bin/true RANLIB=/usr/bin/true CC="$(SDK)/usr/bin/gcc -emit-llvm"' -DSTRIP_FBSDID -D__asm__\(X...\)="\error" -D__asm\(X...\)="\error"' MAKEFLAGS="" MFLAGS="" $(BUILD)/bmake/bmake -m $(BUILD)/lib/share/mk 
$?BMAKE=AR='/usr/bin/true ||' GENCAT=/usr/bin/true RANLIB=/usr/bin/true CC="$(SDK)/usr/bin/gcc -emit-llvm -DSTRIP_FBSDID" MAKEFLAGS="" MFLAGS="" $(BUILD)/bmake/bmake -m $(BUILD)/lib/share/mk 

# ====================================================================================
# ALL TARGETS
# ====================================================================================
BUILDORDER= cmake abclibs basictools llvm binutils plugins gcc bmake stdlibs gcclibs as3wig abcstdlibs
BUILDORDER+= sdkcleanup tr trd swig genfs gdb pkgconfig libtool extralibs finalcleanup submittests

TESTORDER= test_hello_c test_hello_cpp test_pthreads_c_shell test_pthreads_cpp_swf test_posix 
TESTORDER+= test_scimark_shell test_scimark_swf test_sjlj test_sjlj_opt test_eh test_eh_opt test_as3interop test_symbols 
#TESTORDER+= swigtests llvmtests checkasm 

# All Tests
submittests: $(TESTORDER)

# All Targets
all:
	@echo "Building $(SDKNAME) ..."
	@mkdir -p $(BUILD)/logs
	@$(MAKE) diagnostics &> $(BUILD)/logs/diagnostics.txt 2>&1
	@$(MAKE) install_libs &> $(BUILD)/logs/install_libs.txt 2>&1
	@$(MAKE) base &> $(BUILD)/logs/base.txt 2>&1
	@$(MAKE) make &> $(BUILD)/logs/make.txt 2>&1
	@$(SDK_MAKE) -s all_with_local_make

# Helper for 'all_with_local_make'
ifneq (,$(PRINT_LOGS_ON_ERROR))
	$?PRINT_LOGS_CMD=tail +1
else
	$?PRINT_LOGS_CMD=true
endif

# Macro for Targets with local Make
all_with_local_make:
	@for target in $(BUILDORDER) ; do \
		echo "-  $$target" ; \
		$(MAKE) $$target &> $(BUILD)/logs/$$target.txt 2>&1; \
		mret=$$? ; \
		logs="$$logs $(BUILD)/logs/$$target.txt" ; \
		grep -q "Resource temporarily unavailable" $(BUILD)/logs/$$target.txt ; \
		gret=$$? ; \
		rcount=1 ; \
		while [ $$gret == 0 ] && [ $$rcount -lt 6 ] ; do \
			echo "-  $$target (retry $$rcount)" ; \
			$(MAKE) $$target &> $(BUILD)/logs/$$target.txt 2>&1; \
			mret=$$? ; \
			grep -q "Resource temporarily unavailable" $(BUILD)/logs/$$target.txt ; \
			gret=$$? ; \
			let rcount=rcount+1 ; \
		done ; \
		if [ $$mret -ne 0 ] ; then \
			echo "Failed to build: $$target" ;\
			$(PRINT_LOGS_CMD) $$logs ;\
			exit 1 ; \
		fi ; \
	done 

# Build all with Windows 
# Notes: Ignoring some build errors but only documentation generation related
all_win:
	@echo "Building $(SDKNAME) ..."
	@mkdir -p $(BUILD)/logs
	@$(MAKE) diagnostics &> $(BUILD)/logs/diagnostics.txt 2>&1
	@$(MAKE) install_libs &> $(BUILD)/logs/install_libs.txt 2>&1
	@$(MAKE) base &> $(BUILD)/logs/base.txt 2>&1
	@$(MAKE) -i make &> $(BUILD)/logs/make.txt 2>&1
	@$(SDK_MAKE) cmake &> $(BUILD)/logs/cmake.txt 2>&1
	@$(SDK_MAKE) abclibs &> $(BUILD)/logs/abclibs.txt 2>&1
	@$(SDK_MAKE) basictools &> $(BUILD)/logs/basictools.txt 2>&1
	@$(SDK_MAKE) llvm &> $(BUILD)/logs/llvm.txt 2>&1
	@$(SDK_MAKE) -i binutils &> $(BUILD)/logs/binutils.txt 2>&1
	@$(SDK_MAKE) plugins &> $(BUILD)/logs/plugins.txt 2>&1
	@$(SDK_MAKE) gcc &> $(BUILD)/logs/gcc.txt 2>&1
	@$(SDK_MAKE) bmake &> $(BUILD)/logs/bmake.txt 2>&1
	@$(SDK_MAKE) stdlibs &> $(BUILD)/logs/stdlibs.txt 2>&1
	@$(SDK_MAKE) gcclibs &> $(BUILD)/logs/gcclibs.txt 2>&1
	@$(SDK_MAKE) as3wig &> $(BUILD)/logs/as3wig.txt 2>&1
	@$(SDK_MAKE) abcstdlibs &> $(BUILD)/logs/abcstdlibs.txt 2>&1
	@$(SDK_MAKE) sdkcleanup &> $(BUILD)/logs/sdkcleanup.txt 2>&1
	@$(SDK_MAKE) tr &> $(BUILD)/logs/tr.txt 2>&1
	@$(SDK_MAKE) trd &> $(BUILD)/logs/trd.txt 2>&1
	@$(SDK_MAKE) test_hello_cpp &> $(BUILD)/logs/test_hello_cpp.txt 2>&1
	@$(SDK_MAKE) swig &> $(BUILD)/logs/swig.txt 2>&1
	@$(SDK_MAKE) genfs &> $(BUILD)/logs/genfs.txt 2>&1
	@$(SDK_MAKE) gdb &> $(BUILD)/logs/gdb.txt 2>&1
	@$(SDK_MAKE) pkgconfig &> $(BUILD)/logs/pkgconfig.txt 2>&1
	@$(SDK_MAKE) libtool &> $(BUILD)/logs/libtool.txt 2>&1
	@$(SDK_MAKE) extralibs &> $(BUILD)/logs/extralibs.txt 2>&1
	@$(SDK_MAKE) finalcleanup &> $(BUILD)/logs/finalcleanup.txt 2>&1
	@$(SDK_MAKE) submittests &> $(BUILD)/logs/submittests.txt 2>&1
	@$(SDK_MAKE) samples &> $(BUILD)/logs/samples.txt 2>&1
	@echo "Done."

# Print debug information
diagnostics:
	@echo "~~~ $(SDKNAME) ~~~"
	@echo "User: $(UNAME)"
	@echo "Platform: $(PLATFORM)"
	@echo "Build: $(BUILD)"
	@echo "Triple: $(TRIPLE)"
	@echo "Host Triple: $(HOST_TRIPLE)"
	@echo "Build Triple: $(BUILD_TRIPLE)"
	@echo "CC: $(shell $(CC) --version)"
	@echo "CXX: $(shell $(CXX) --version)"
	@echo "FLEX_SDK_TYPE: $(FLEX_SDK_TYPE)"
	@echo "FLEX_SDK_HOME: $(FLEX_SDK_HOME)"

# Development target
all_dev:
	@$(SDK_MAKE) diagnostics

# Clean build outputs
clean:
	@echo "Cleaning ..."
	@rm -rf $(CCACHE_DIR)
	@rm -rf $(BUILDROOT)
	@rm -rf $(SDK)
	@rm -rf $(SRCROOT)/.redo
	@$(MAKE) -s clean_libs
	@cd samples && $(MAKE) -s clean
	@echo "Done."

# Install packaged dependency libraries
install_libs:
	tar xf packages/$(DEPENDENCY_BMAKE).tar.gz
	tar xf packages/$(DEPENDENCY_CMAKE).tar.gz
	tar xf packages/$(DEPENDENCY_DEJAGNU).tar.gz
	tar xf packages/$(DEPENDENCY_DMALLOC).tar.gz
	tar xf packages/$(DEPENDENCY_JPEG).tar.gz
	tar xf packages/$(DEPENDENCY_LIBAA).tar.gz
	tar xf packages/$(DEPENDENCY_LIBBZIP).tar.gz
	tar xf packages/$(DEPENDENCY_LIBEIGEN).tar.gz
	tar xf packages/$(DEPENDENCY_LIBFLAC).tar.gz
	tar xf packages/$(DEPENDENCY_LIBFREETYPE).tar.gz
	tar xf packages/$(DEPENDENCY_LIBGIF).tar.gz
	tar xf packages/$(DEPENDENCY_LIBGMP).tar.gz
	tar xf packages/$(DEPENDENCY_LIBICONV).tar.gz
	tar xf packages/$(DEPENDENCY_LIBNCURSES).tar.gz
	tar xf packages/$(DEPENDENCY_LIBPROTOBUF).tar.gz
	tar xf packages/$(DEPENDENCY_LIBPHYSFS).tar
	tar xf packages/$(DEPENDENCY_LIBREADLINE).tar.gz
	tar xf packages/$(DEPENDENCY_LIBOGG).tar.gz
	tar xf packages/$(DEPENDENCY_LIBPNG).tar.gz
	tar xf packages/$(DEPENDENCY_LIBSNDFILE).tar.gz
	tar xf packages/$(DEPENDENCY_LIBSDL).tar.gz
	tar xf packages/$(DEPENDENCY_LIBSDLIMAGE).tar.gz
	tar xf packages/$(DEPENDENCY_LIBSDLMIXER).tar.gz
	tar xf packages/$(DEPENDENCY_LIBSDLTTF).tar.gz
	tar xf packages/$(DEPENDENCY_LIBTIFF).tar.gz
	tar xf packages/$(DEPENDENCY_LIBTOOL).tar.gz
	tar xf packages/$(DEPENDENCY_LIBVORBIS).tar.gz
	tar xf packages/$(DEPENDENCY_LIBWEBP).tar.gz
	tar xf packages/$(DEPENDENCY_LIBXZ).tar.gz
	tar xf packages/$(DEPENDENCY_MAKE).tar.gz
	tar xf packages/$(DEPENDENCY_OPENSSL).tar.gz
	tar xf packages/$(DEPENDENCY_PKG_CFG).tar.gz
	mkdir -p $(DEPENDENCY_SCIMARK) && cd $(DEPENDENCY_SCIMARK) && unzip -q ../packages/$(DEPENDENCY_SCIMARK).zip
	unzip -q packages/avmplus-master.zip
	tar xf packages/$(DEPENDENCY_ZLIB).tar.gz
	# apply patches
	cp -r ./patches/$(DEPENDENCY_DEJAGNU) .
	cp -r ./patches/$(DEPENDENCY_DMALLOC) .
	cp -r ./patches/$(DEPENDENCY_LIBPHYSFS) .
	cp -r ./patches/$(DEPENDENCY_LIBPNG) .
	cp -r ./patches/$(DEPENDENCY_LIBSDL) .
	cp -r ./patches/$(DEPENDENCY_PKG_CFG) .
	cp -r ./patches/$(DEPENDENCY_SCIMARK) .
	cp -r ./patches/$(DEPENDENCY_ZLIB) .
	cp -r ./patches/avmplus-master .

# Clear depdendency libraries
clean_libs:
	rm -rf $(DEPENDENCY_BMAKE)
	rm -rf $(DEPENDENCY_CMAKE)
	rm -rf $(DEPENDENCY_DEJAGNU)
	rm -rf $(DEPENDENCY_DMALLOC)
	rm -rf $(DEPENDENCY_JPEG)
	rm -rf $(DEPENDENCY_LIBAA)
	rm -rf $(DEPENDENCY_LIBBZIP)
	rm -rf $(DEPENDENCY_LIBEIGEN)
	rm -rf eigen-eigen-5097c01bcdc4
	rm -rf $(DEPENDENCY_LIBFLAC)
	rm -rf $(DEPENDENCY_LIBFREETYPE)
	rm -rf $(DEPENDENCY_LIBGIF)
	rm -rf $(DEPENDENCY_LIBGMP)
	rm -rf $(DEPENDENCY_LIBICONV)
	rm -rf $(DEPENDENCY_LIBNCURSES)
	rm -rf $(DEPENDENCY_LIBPROTOBUF)
	rm -rf $(DEPENDENCY_LIBPHYSFS)
	rm -rf $(DEPENDENCY_LIBREADLINE)
	rm -rf $(DEPENDENCY_LIBOGG)
	rm -rf $(DEPENDENCY_LIBPNG)
	rm -rf $(DEPENDENCY_LIBSNDFILE)
	rm -rf $(DEPENDENCY_LIBSDL)
	rm -rf $(DEPENDENCY_LIBSDLIMAGE)
	rm -rf $(DEPENDENCY_LIBSDLMIXER)
	rm -rf $(DEPENDENCY_LIBSDLTTF)
	rm -rf $(DEPENDENCY_LIBTIFF)
	rm -rf $(DEPENDENCY_LIBTOOL)
	rm -rf $(DEPENDENCY_LIBVORBIS)
	rm -rf $(DEPENDENCY_LIBWEBP)
	rm -rf $(DEPENDENCY_LIBXZ)
	rm -rf $(DEPENDENCY_MAKE)
	rm -rf $(DEPENDENCY_OPENSSL)
	rm -rf $(DEPENDENCY_PKG_CFG)
	rm -rf $(DEPENDENCY_SCIMARK)
	rm -rf $(DEPENDENCY_ZLIB)
	rm -rf avmplus-master

# ====================================================================================
# BASE
# ====================================================================================
# Initialize the build
base:
	mkdir -p $(BUILDROOT)/extra
	mkdir -p $(BUILD)/abclibs
	mkdir -p $(SDK)/usr/lib/bfd-plugins
	mkdir -p $(SDK)/usr/share
	mkdir -p $(SDK)/usr/platform/$(PLATFORM)/bin
	mkdir -p $(SDK)/usr/platform/$(PLATFORM)/libexec/gcc/$(TRIPLE)

	$(LN) ../usr $(SDK)/usr/$(TRIPLE)
	$(LN) $(PLATFORM) $(SDK)/usr/platform/current
	$(LN) platform/current/bin $(SDK)/usr/bin
	$(LN) ../ $(SDK)/usr/platform/usr 
	$(LN) ../../lib $(SDK)/usr/platform/current/lib 
	$(LN) platform/current/libexec $(SDK)/usr/libexec
	$(LN) ../../../../../lib $(SDK)/usr/platform/current/libexec/gcc/$(TRIPLE)/lib

	cd $(SDK)/usr/platform/current/bin && $(LN) ar$(EXEEXT) avm2-unknown-freebsd8-ar$(EXEEXT)
	cd $(SDK)/usr/platform/current/bin && $(LN) nm$(EXEEXT) avm2-unknown-freebsd8-nm$(EXEEXT)
	cd $(SDK)/usr/platform/current/bin && $(LN) strip$(EXEEXT) avm2-unknown-freebsd8-strip$(EXEEXT)
	cd $(SDK)/usr/platform/current/bin && $(LN) ranlib$(EXEEXT) avm2-unknown-freebsd8-ranlib$(EXEEXT)
	cd $(SDK)/usr/platform/current/bin && $(LN) gcc$(EXEEXT) avm2-unknown-freebsd8-gcc$(EXEEXT)
	cd $(SDK)/usr/platform/current/bin && $(LN) g++$(EXEEXT) avm2-unknown-freebsd8-g++$(EXEEXT)
	cd $(SDK)/usr/platform/current/bin && $(LN) gcc$(EXEEXT) gcc-4.2$(EXEEXT)
	cd $(SDK)/usr/platform/current/bin && $(LN) g++$(EXEEXT) g++-4.2$(EXEEXT)

	mkdir -p $(BUILD)/ccachebin
	mkdir -p ccache
	$(LN) `which ccache` $(BUILD)/ccachebin/$(CC)
	$(LN) `which ccache` $(BUILD)/ccachebin/$(CXX)
	$(LN) `which ccache` $(BUILD)/ccachebin/$(CYGTRIPLE)-gcc
	$(LN) `which ccache` $(BUILD)/ccachebin/$(CYGTRIPLE)-g++
	$(LN) `which ccache` $(BUILD)/ccachebin/$(MINGTRIPLE)-gcc
	$(LN) `which ccache` $(BUILD)/ccachebin/$(MINGTRIPLE)-g++

	$(RSYNC) tools/playerglobal/14.0/playerglobal.abc $(SDK)/usr/lib/
	$(RSYNC) tools/playerglobal/14.0/playerglobal.swc $(SDK)/usr/lib/
	$(RSYNC) avm2_env/public-api.txt $(SDK)/
	cp -f $(DEPENDENCY_AVMPLUS)/generated/*.abc $(SDK)/usr/lib/

	$(RSYNC) --exclude '*iconv.h' avm2_env/usr/include/ $(SDK)/usr/include
	$(RSYNC) avm2_env/usr/lib/ $(SDK)/usr/lib
	cd $(BUILD) && $(SCOMPFALCON) $(call nativepath,$(SRCROOT)/$(DEPENDENCY_AVMPLUS)/utils/swfmake.as) -outdir . -out swfmake
	cd $(BUILD) && $(SCOMPFALCON) $(call nativepath,$(SRCROOT)/$(DEPENDENCY_AVMPLUS)/utils/projectormake.as) -outdir . -out projectormake
	cd $(BUILD) && $(SCOMPFALCON) $(call nativepath,$(SRCROOT)/$(DEPENDENCY_AVMPLUS)/utils/abcdump.as) -outdir . -out abcdump

# ====================================================================================
# MAKE
# ====================================================================================
# Assemble GNU Make
make:
	rm -rf $(BUILD)/make
	mkdir -p $(SDK)/usr/bin
	mkdir -p $(BUILD)/make
	cp -r $(SRCROOT)/$(DEPENDENCY_MAKE)/* $(BUILD)/make/
	cd $(BUILD)/make && CC=$(CC) CXX=$(CXX) ./configure --prefix=$(SDK)/usr --program-prefix="" \
                --build=$(BUILD_TRIPLE) --host=$(HOST_TRIPLE) --target=$(TRIPLE)
	cd $(BUILD)/make && CC=$(CC) CXX=$(CXX) $(MAKE) -j$(THREADS)
	cd $(BUILD)/make && CC=$(CC) CXX=$(CXX) $(MAKE) install

# ====================================================================================
# CMAKE
# ====================================================================================
# Assemble CMake
cmake:
	rm -rf $(BUILD)/cmake
	rm -rf $(SDK)/usr/cmake_junk
	mkdir -p $(SDK)/usr/bin
	mkdir -p $(BUILD)/cmake
	mkdir -p $(SDK)/usr/cmake_junk
	mkdir -p $(SDK)/usr/platform/$(PLATFORM)/share/$(DEPENDENCY_CMAKE)/
	cp -r $(SRCROOT)/$(DEPENDENCY_CMAKE)/* $(BUILD)/cmake/
	cd $(BUILD)/cmake && CC=$(CC) CXX=$(CXX) ./configure --prefix=$(SDK)/usr --datadir=share/$(DEPENDENCY_CMAKE) --docdir=cmake_junk --mandir=cmake_junk
	cd $(BUILD)/cmake && CC=$(CC) CXX=$(CXX) $(MAKE) -j$(THREADS)
	cd $(BUILD)/cmake && CC=$(CC) CXX=$(CXX) $(MAKE) install
	cp -r $(SDK)/usr/share/$(DEPENDENCY_CMAKE) $(SDK)/usr/platform/$(PLATFORM)/share/

# ====================================================================================
# ABCLIBS
# ====================================================================================
# Assemble builtin ABCs
# Use it if Tamarin AS3 code is modified
builtinabcs:
	cp $(BUILD)/abclibsposix/IKernel.as $(SRCROOT)/$(DEPENDENCY_AVMPLUS)/shell
	cp $(BUILD)/abclibsposix/ShellPosix.as $(SRCROOT)/$(DEPENDENCY_AVMPLUS)/shell
	cp $(BUILD)/abclibsposix/ShellPosixGlue.cpp $(SRCROOT)/$(DEPENDENCY_AVMPLUS)/shell
	cp $(BUILD)/abclibsposix/ShellPosixGlue.h $(SRCROOT)/$(DEPENDENCY_AVMPLUS)/shell
	cd $(SRCROOT)/$(DEPENDENCY_AVMPLUS)/core && $(PYTHON) ./builtin.py -abcfuture -config CONFIG::VMCFG_FLOAT=false -config CONFIG::VMCFG_ALCHEMY_SDK_BUILD=true -config CONFIG::VMCFG_ALCHEMY_POSIX=true
	cd $(SRCROOT)/$(DEPENDENCY_AVMPLUS)/shell && $(PYTHON) ./shell_toplevel.py -abcfuture -config CONFIG::VMCFG_FLOAT=false -config CONFIG::VMCFG_ALCHEMY_SDK_BUILD=true -config CONFIG::VMCFG_ALCHEMY_POSIX=true
	cp -f $(DEPENDENCY_AVMPLUS)/generated/*.abc $(SDK)/usr/lib/

# Assemble ABC library binaries and documentation
abclibs:
	$(MAKE) abclibs_compile
	$(MAKE) abclibs_asdocs && $(MAKE) asdocs_deploy

# Assemble ABC library binaries
# Change 04.05.14. DefaultPreloader and C_Run was compiled using ASC.jar, lets use ASC2.jar and test it: $(SCOMP) to $(SCOMPFALCON)
abclibs_compile:
	# Cleaning
	mkdir -p $(BUILD)/abclibs
	mkdir -p $(BUILD)/abclibsposix
	mkdir -p $(SDK)/usr/lib/abcs
	# Generating the Posix interface
	cd $(BUILD)/abclibsposix && $(PYTHON) $(SRCROOT)/posix/gensyscalls.py $(SRCROOT)/posix/syscalls.changed
	# Post-Processing IKernel
	# TODO: Do not print out files in the source folder (VPMedia)
	cat $(BUILD)/abclibsposix/IKernel.as | sed '1,1d' | sed '$$d' > $(SRCROOT)/posix/IKernel.as
	# Rebuild AVMPlus ABCs
	#$(MAKE) builtinabcs
	# Generating DefaultPreloader
	cd $(BUILD)/abclibs && $(SCOMPFALCON) $(ABCLIBOPTS) -import $(call nativepath,$(SDK)/usr/lib/playerglobal.abc) $(call nativepath,$(SRCROOT)/posix/DefaultPreloader.as) -swf com.adobe.flascc.preloader.DefaultPreloader,800,600,60 -outdir . -out DefaultPreloader
	# Generating ABC Libs
	cd $(BUILD)/abclibs && $(SCOMPFALCON) $(ABCLIBOPTS) -strict -optimize $(call nativepath,$(SRCROOT)/posix/ELF.as) -outdir . -out ELF
	cd $(BUILD)/abclibs && $(SCOMPFALCON) $(ABCLIBOPTS) -strict -optimize $(call nativepath,$(SRCROOT)/posix/Exit.as) -outdir . -out Exit
	cd $(BUILD)/abclibs && $(SCOMPFALCON) $(ABCLIBOPTS) -strict -optimize $(call nativepath,$(SRCROOT)/posix/LongJmp.as) -outdir . -out LongJmp
	cd $(BUILD)/abclibs && $(SCOMPFALCON) $(ABCLIBOPTS) -import Exit.abc $(call nativepath,$(SRCROOT)/posix/C_Run.as) -outdir . -out C_Run
	cd $(BUILD)/abclibs && $(SCOMPFALCON) $(ABCLIBOPTS) -strict -optimize $(call nativepath,$(SRCROOT)/posix/vfs/ISpecialFile.as) -outdir . -out ISpecialFile
	cd $(BUILD)/abclibs && $(SCOMPFALCON) $(ABCLIBOPTS) -strict -optimize $(call nativepath,$(SRCROOT)/posix/vfs/IBackingStore.as) -outdir . -out IBackingStore
	cd $(BUILD)/abclibs && $(SCOMPFALCON) $(ABCLIBOPTS) -strict -optimize -import $(call nativepath,$(SDK)/usr/lib/playerglobal.abc) -import IBackingStore.abc $(call nativepath,$(SRCROOT)/posix/vfs/InMemoryBackingStore.as) -outdir . -out InMemoryBackingStore
	cd $(BUILD)/abclibs && $(SCOMPFALCON) $(ABCLIBOPTS) -strict -optimize -import $(call nativepath,$(SDK)/usr/lib/playerglobal.abc) -import IBackingStore.abc -import ISpecialFile.abc $(call nativepath,$(SRCROOT)/posix/vfs/IVFS.as) -outdir . -out IVFS
	cd $(BUILD)/abclibs && $(SCOMPFALCON) $(ABCLIBOPTS) -strict -optimize -import $(call nativepath,$(SDK)/usr/lib/playerglobal.abc) -import ISpecialFile.abc -import IBackingStore.abc -import IVFS.abc -import InMemoryBackingStore.abc $(call nativepath,$(SRCROOT)/posix/vfs/DefaultVFS.as) -outdir . -out DefaultVFS
	cd $(BUILD)/abclibs && $(SCOMPFALCON) $(ABCLIBOPTS) -strict -optimize -import $(call nativepath,$(SDK)/usr/lib/playerglobal.abc) $(call nativepath, `find $(SRCROOT)/posix/vfs/nochump -name "*.as"`) -outdir . -out AlcVFSZip
	cd $(BUILD)/abclibs && $(SCOMPFALCON) $(ABCLIBOPTS) -import Exit.abc -import C_Run.abc -import IBackingStore.abc -import ISpecialFile.abc -import IVFS.abc -import LongJmp.abc $(call nativepath,$(SRCROOT)/posix/CModule.as) -outdir . -out CModule
	cd $(BUILD)/abclibs && $(SCOMPFALCON) $(ABCLIBOPTS) -strict -optimize -import IBackingStore.abc -import IVFS.abc -import ISpecialFile.abc -import CModule.abc -import C_Run.abc -import Exit.abc -import ELF.abc $(call nativepath,$(SRCROOT)/posix/AlcDbgHelper.as) -d -outdir . -out AlcDbgHelper
	cd $(BUILD)/abclibs && $(SCOMPFALCON) $(ABCLIBOPTS) -strict -optimize -import IBackingStore.abc -import IVFS.abc -import ISpecialFile.abc -import CModule.abc -import $(call nativepath,$(SDK)/usr/lib/playerglobal.abc) $(call nativepath,$(SRCROOT)/posix/BinaryData.as) -outdir . -out BinaryData
	cd $(BUILD)/abclibs && $(SCOMPFALCON) $(ABCLIBOPTS) -strict -optimize -import IBackingStore.abc -import IVFS.abc -import ISpecialFile.abc -import CModule.abc -import C_Run.abc -import $(call nativepath,$(SDK)/usr/lib/playerglobal.abc) $(call nativepath,$(SRCROOT)/posix/Console.as) -outdir . -out Console
	cd $(BUILD)/abclibs && $(SCOMPFALCON) $(ABCLIBOPTS) -strict -optimize -import IBackingStore.abc -import IVFS.abc -import ISpecialFile.abc -import CModule.abc -import C_Run.abc -import Exit.abc -import ELF.abc $(call nativepath,$(SRCROOT)/posix/startHack.as) -outdir . -out startHack
	cd $(BUILD)/abclibs && $(SCOMPFALCON) $(ABCLIBOPTS) -strict -optimize -import IBackingStore.abc -import IVFS.abc -import ISpecialFile.abc -import CModule.abc -import C_Run.abc $(call nativepath,$(SRCROOT)/posix/ShellCreateWorker.as) -outdir . -out ShellCreateWorker
	cd $(BUILD)/abclibs && $(SCOMPFALCON) $(ABCLIBOPTS) -strict -optimize -import IBackingStore.abc -import IVFS.abc -import ISpecialFile.abc -import CModule.abc -import C_Run.abc -import Exit.abc -import $(call nativepath,$(SDK)/usr/lib/playerglobal.abc) $(call nativepath,$(SRCROOT)/posix/PlayerCreateWorker.as) -outdir . -out PlayerCreateWorker
	cd $(BUILD)/abclibs && $(SCOMPFALCON) $(ABCLIBOPTS) -strict -optimize -import CModule.abc -import C_Run.abc -import Exit.abc -import IBackingStore.abc -import ISpecialFile.abc -import IVFS.abc -import DefaultVFS.abc -import $(call nativepath,$(SDK)/usr/lib/playerglobal.abc) $(call nativepath,$(SRCROOT)/posix/PlayerKernel.as) -outdir . -out PlayerKernel
	cp $(BUILD)/abclibs/*.abc $(SDK)/usr/lib
	cp $(BUILD)/abclibs/*.swf $(SDK)/usr/lib

# Assemble AS3 documentation
abclibs_asdocs:
	rm -rf $(BUILDROOT)/tempdita
	rm -rf $(BUILDROOT)/apidocs
	mkdir -p $(BUILDROOT)
	mkdir -p $(BUILDROOT)/apidocs
	mkdir -p $(BUILDROOT)/apidocs/tempdita
	mkdir -p $(BUILD)/logs
	cd $(BUILDROOT) && $(FLEX_ASDOC) \
				-load-config= \
				-external-library-path=$(call nativepath,$(FLEX_SDK_HOME)/frameworks/libs/player/14.0/playerglobal.swc) \
				-strict=false -define+=CONFIG::player,1 -define+=CONFIG::asdocs,true -define+=CONFIG::actual,false \
				-doc-sources+=$(call nativepath,$(SRCROOT)/posix/vfs) \
				-doc-sources+=$(call nativepath,$(SRCROOT)/posix) \
				-keep-xml=true \
				-exclude-sources+=$(call nativepath,$(SRCROOT)/posix/startHack.as) \
				-exclude-sources+=$(call nativepath,$(SRCROOT)/posix/IKernel.as) \
				-exclude-sources+=$(call nativepath,$(SRCROOT)/posix/vfs/nochump) \
				-package-description-file=$(call nativepath,$(SRCROOT)/test/aspackages.xml) \
				-main-title "CrossBridge API Reference" \
				-window-title "CrossBridge API Reference" \
				-output apidocs
	mv $(BUILDROOT)/apidocs/tempdita $(BUILDROOT)/

# Deploy AS3 documentation
asdocs_deploy:
	rm -rf $(SDK)/usr/share/asdocs
	mkdir -p $(SDK)/usr/share/asdocs
	$(RSYNC) --exclude "*.xslt" --exclude "*.html" --exclude ASDoc_Config.xml --exclude overviews.xml $(BUILDROOT)/tempdita/ $(SDK)/usr/share/asdocs

CROSS=PATH="$(BUILD)/ccachebin:$(CYGWINMAC):$(PATH):$(SDK)/usr/bin" $(MAKE) SDK=$(WIN_BUILD)/sdkoverlay PLATFORM=cygwin LLVMINSTALLPREFIX=$(WIN_BUILD) NATIVE_AR=$(CYGTRIPLE)-ar CC=$(CYGTRIPLE)-gcc CXX=$(CYGTRIPLE)-g++ RANLIB=$(CYGTRIPLE)-ranlib

# ====================================================================================
# BASICTOOLS
# ====================================================================================
# Assemble basic tools (C/CPP)
basictools:
	$(MAKE) uname noenv avm2-as alctool alcdb

# Assemble UName Helper
uname:
	mkdir -p $(SDK)/usr/bin
	$(CC) $(SRCROOT)/tools/uname/uname.c -o $(SDK)/usr/bin/uname$(EXEEXT)

# Assemble NoEnv Helper
noenv:
	mkdir -p $(SDK)/usr/bin
	$(CC) $(SRCROOT)/tools/noenv/noenv.c -o $(SDK)/usr/bin/noenv$(EXEEXT)

# Assemble TBD Tool
avm2-as:
	mkdir -p $(SDK)/usr/bin
	$(CXX) $(SRCROOT)/avm2_env/misc/SetAlchemySDKLocation.c $(SRCROOT)/tools/as/as.cpp -o $(SDK)/usr/bin/avm2-as$(EXEEXT)

# Assemble TBD Tool
alctool:
	rm -rf $(BUILD)/alctool
	mkdir -p $(BUILD)/alctool/flascc
	cp -f $(SRCROOT)/tools/lib/*.jar $(SDK)/usr/lib/
	cp -f $(SRCROOT)/tools/lib/falcon.txt $(SDK)/usr/lib/.
	rm -f $(SDK)/usr/lib/mxmlc.jar
	cp -f $(SRCROOT)/tools/aet/*.java $(BUILD)/alctool/flascc/.
	cp -f $(SRCROOT)/tools/common/java/flascc/*.java $(BUILD)/alctool/flascc/.
	cd $(BUILD)/alctool && javac flascc/*.java -cp $(call nativepath,$(SRCROOT)/tools/lib/aet.jar)
	cd $(BUILD)/alctool && echo "Main-Class: flascc.AlcTool" > MANIFEST.MF
	cd $(BUILD)/alctool && echo "Class-Path: aet.jar" >> MANIFEST.MF
	cd $(BUILD)/alctool && jar cmf MANIFEST.MF alctool.jar flascc/*.class
	cp $(BUILD)/alctool/alctool.jar $(SDK)/usr/lib/.

# Assemble Debugger Tool
alcdb:
	rm -rf $(BUILD)/alcdb
	mkdir -p $(BUILD)/alcdb/flascc
	cp -f $(SRCROOT)/tools/alcdb/*.java $(BUILD)/alcdb/flascc/.
	cp -f $(SRCROOT)/tools/common/java/flascc/*.java $(BUILD)/alcdb/flascc/.
	cd $(BUILD)/alcdb && javac flascc/*.java -cp $(call nativepath,$(SRCROOT)/tools/lib/fdb.jar)
	cd $(BUILD)/alcdb && echo "Main-Class: flascc.AlcDB" > MANIFEST.MF
	cd $(BUILD)/alcdb && echo "Class-Path: fdb.jar" >> MANIFEST.MF
	cd $(BUILD)/alcdb && jar cmf MANIFEST.MF alcdb.jar flascc/*.class 
	cp $(BUILD)/alcdb/alcdb.jar $(SDK)/usr/lib/.

# ====================================================================================
# LLVM
# ====================================================================================
# Assemble LLVM Tool-Chain
llvm:
	rm -rf $(BUILD)/llvm-debug
	mkdir -p $(BUILD)/llvm-debug
	cd $(BUILD)/llvm-debug && LDFLAGS="$(LLVMLDFLAGS)" CFLAGS="$(LLVMCFLAGS)" CXXFLAGS="$(LLVMCXXFLAGS)" $(SDK_CMAKE) -G "Unix Makefiles" \
		$(LLVMCMAKEOPTS) -DCMAKE_INSTALL_PREFIX=$(LLVMINSTALLPREFIX)/llvm-install -DCMAKE_BUILD_TYPE=$(LLVMBUILDTYPE) -DLLVM_BUILD_CLANG=$(CLANG) \
		-DLLVM_ENABLE_ASSERTIONS=$(LLVMASSERTIONS) -DLLVM_BUILD_GOLDPLUGIN=ON -DBINUTILS_INCDIR=$(SRCROOT)/$(DEPENDENCY_BINUTILS)/include \
		-DLLVM_TARGETS_TO_BUILD="$(LLVMTARGETS)" -DLLVM_NATIVE_ARCH="avm2" -DLLVM_INCLUDE_TESTS=$(LLVMTESTS) -DLLVM_INCLUDE_EXAMPLES=OFF \
		$(SRCROOT)/$(DEPENDENCY_LLVM) && $(MAKE) -j$(THREADS) && $(MAKE) install
	cp $(LLVMINSTALLPREFIX)/llvm-install/bin/llc$(EXEEXT) $(SDK)/usr/bin/llc$(EXEEXT)
ifeq ($(LLVM_ONLYLLC), false)
	cp $(LLVMINSTALLPREFIX)/llvm-install/bin/llvm-ar$(EXEEXT) $(SDK)/usr/bin/llvm-ar$(EXEEXT)
	cp $(LLVMINSTALLPREFIX)/llvm-install/bin/llvm-as$(EXEEXT) $(SDK)/usr/bin/llvm-as$(EXEEXT)
	cp $(LLVMINSTALLPREFIX)/llvm-install/bin/llvm-diff$(EXEEXT) $(SDK)/usr/bin/llvm-diff$(EXEEXT)
	cp $(LLVMINSTALLPREFIX)/llvm-install/bin/llvm-dis$(EXEEXT) $(SDK)/usr/bin/llvm-dis$(EXEEXT)
	cp $(LLVMINSTALLPREFIX)/llvm-install/bin/llvm-extract$(EXEEXT) $(SDK)/usr/bin/llvm-extract$(EXEEXT)
	cp $(LLVMINSTALLPREFIX)/llvm-install/bin/llvm-ld$(EXEEXT) $(SDK)/usr/bin/llvm-ld$(EXEEXT)
	cp $(LLVMINSTALLPREFIX)/llvm-install/bin/llvm-link$(EXEEXT) $(SDK)/usr/bin/llvm-link$(EXEEXT)
	cp $(LLVMINSTALLPREFIX)/llvm-install/bin/llvm-nm$(EXEEXT) $(SDK)/usr/bin/llvm-nm$(EXEEXT)
	cp $(LLVMINSTALLPREFIX)/llvm-install/bin/llvm-ranlib$(EXEEXT) $(SDK)/usr/bin/llvm-ranlib$(EXEEXT)
	cp $(LLVMINSTALLPREFIX)/llvm-install/bin/opt$(EXEEXT) $(SDK)/usr/bin/opt$(EXEEXT)
	cp $(LLVMINSTALLPREFIX)/llvm-install/lib/LLVMgold.* $(SDK)/usr/lib/LLVMgold$(SOEXT)
	cp -f $(BUILD)/llvm-debug/bin/fpcmp$(EXEEXT) $(BUILDROOT)/extra/fpcmp$(EXEEXT)
endif

# Assemble LLVM Tests
llvmtests:
	rm -rf $(BUILD)/llvm-tests
	mkdir -p $(BUILD)/llvm-tests
	cp -f $(SDK)/usr/bin/avmshell-release-debugger $(SDK)/usr/bin/avmshell
	cd $(BUILD)/llvm-tests && $(SRCROOT)/$(DEPENDENCY_LLVM)/configure --with-llvmgcc=$(SDK)/usr/bin/gcc --with-llvmgxx=$(SDK)/usr/bin/g++ --without-f2c --without-f95 --disable-clang --enable-jit=no --target=$(TRIPLE) --prefix=$(BUILD)/llvm-install
	cd $(BUILD)/llvm-tests && $(LN) $(SDK)/usr Release
	cd $(BUILD)/llvm-tests/projects/test-suite/MultiSource && (LANG=C && $(MAKE) TEST=nightly TARGET_LLCFLAGS=-jvm="$(JAVA)" -j$(THREADS) FPCMP=$(FPCMP) DISABLE_CBE=1)
	cd $(BUILD)/llvm-tests/projects/test-suite/SingleSource && (LANG=C && $(MAKE) TEST=nightly TARGET_LLCFLAGS=-jvm="$(JAVA)" -j$(THREADS) FPCMP=$(FPCMP) DISABLE_CBE=1)
	$(PYTHON) $(SRCROOT)/tools/llvmtestcheck.py --srcdir $(SRCROOT)/$(DEPENDENCY_LLVM)/projects/test-suite/ --builddir $(BUILD)/llvm-tests/projects/test-suite/ --fpcmp $(FPCMP)> $(BUILD)/llvm-tests/passfail.txt
	cp $(BUILD)/llvm-tests/passfail.txt $(BUILD)/passfail_llvm.txt

# Assemble LLVM SpecCPU2006 Test
llvmtests-speccpu2006: # works only on mac!
	rm -rf $(BUILD)/llvm-tests
	rm -rf $(BUILD)/llvm-spec-tests
	mkdir -p $(BUILD)/llvm-tests
	cp -f $(SDK)/usr/bin/avmshell-release-debugger $(SDK)/usr/bin/avmshell
	mkdir -p $(BUILD)/llvm-externals && cd $(BUILD)/llvm-externals && curl http://alchemy.corp.adobe.com/speccpu2006.tar.bz2 | tar xvjf -
	#mkdir -p $(BUILD)/llvm-externals && cd $(BUILD)/llvm-externals && cat $(SRCROOT)/speccpu2006.tar.bz2 | tar xvjf -
	cd $(BUILD)/llvm-tests && $(SRCROOT)/$(DEPENDENCY_LLVM)/configure --without-f2c --without-f95 --with-llvmgcc=$(SDK)/usr/bin/gcc --with-llvmgxx=$(SDK)/usr/bin/g++ --with-externals=$(BUILD)/llvm-externals --disable-clang --enable-jit=no --target=$(TRIPLE) --prefix=$(BUILD)/llvm-install
	cd $(BUILD)/llvm-tests && $(LN) $(SDK)/usr Release
	cd $(BUILD)/llvm-tests/projects/test-suite/External && (LANG=C && $(MAKE) TEST=nightly TARGET_LLCFLAGS=-jvm="$(JAVA)" -j$(THREADS) FPCMP=$(FPCMP) DISABLE_CBE=1 CXXFLAGS+='-DSPEC_CPU_MACOSX -DSPEC_CPU_NO_HAS_SIGSETJMP' CFLAGS+='-DSPEC_CPU_MACOSX -DSPEC_CPU_NO_HAS_SIGSETJMP')
	$(PYTHON) $(SRCROOT)/tools/llvmtestcheck.py --fpcmp $(FPCMP) --srcdir $(SRCROOT)/$(DEPENDENCY_LLVM)/projects/test-suite/ --builddir $(BUILD)/llvm-tests/projects/test-suite/ > $(BUILD)/llvm-tests/passfail.txt
	cp $(BUILD)/llvm-tests/passfail.txt $(BUILD)/passfail_spec.txt
	cp -r $(BUILD)/llvm-tests/projects $(BUILD)/llvm-spec-tests

# ====================================================================================
# BINUTILS
# ====================================================================================
# Assemble LLVM BinUtils
binutils:
	rm -rf $(BUILD)/binutils
	mkdir -p $(BUILD)/binutils
	mkdir -p $(SDK)/usr
	cd $(BUILD)/binutils && CC=$(CC) CXX=$(CXX) CFLAGS="-I$(SRCROOT)/avm2_env/misc/ $(DBGOPTS) " CXXFLAGS="-I$(SRCROOT)/avm2_env/misc/ $(DBGOPTS) " $(SRCROOT)/$(DEPENDENCY_BINUTILS)/configure \
		--disable-doc --disable-nls --enable-gold --disable-ld --enable-plugins \
		--build=$(BUILD_TRIPLE) --host=$(HOST_TRIPLE) --target=$(TRIPLE) --with-sysroot=$(SDK)/usr \
		--program-prefix="" --prefix=$(SDK)/usr --disable-werror \
		--enable-targets=$(TRIPLE)
	cd $(BUILD)/binutils && $(MAKE) -j$(THREADS) && $(MAKE) install
	mv $(SDK)/usr/bin/ld.gold$(EXEEXT) $(SDK)/usr/bin/ld$(EXEEXT)
	rm -rf $(SDK)/usr/bin/readelf$(EXEEXT) $(SDK)/usr/bin/elfedit$(EXEEXT) $(SDK)/usr/bin/ld.bfd$(EXEEXT) $(SDK)/usr/bin/objdump$(EXEEXT) $(SDK)/usr/bin/objcopy$(EXEEXT) $(SDK)/usr/share/info $(SDK)/usr/share/man

# ====================================================================================
# PLUGINS
# ====================================================================================
# Assemble LLVM Plug-ins
plugins:
	rm -rf $(BUILD)/makeswf $(BUILD)/multiplug $(BUILD)/zlib
	mkdir -p $(BUILD)/makeswf $(BUILD)/multiplug $(BUILD)/zlib
	cd $(BUILD)/makeswf && $(CXX) $(DBGOPTS) -I$(SRCROOT)/avm2_env/misc/ -DHAVE_ABCNM -DDEFTMPDIR=\"$(call nativepath,/tmp)\" -DDEFSYSROOT=\"$(call nativepath,$(SDK))\" -DHAVE_STDINT_H -I$(SRCROOT)/$(DEPENDENCY_ZLIB)/ -I$(SRCROOT)/$(DEPENDENCY_BINUTILS)/include -fPIC -c $(SRCROOT)/gold-plugins/makeswf.cpp
	cd $(BUILD)/makeswf && $(CXX) $(DBGOPTS) -shared -Wl,-headerpad_max_install_names,-undefined,dynamic_lookup -o makeswf$(SOEXT) makeswf.o
	cd $(BUILD)/multiplug && $(CXX) $(DBGOPTS) -I$(SRCROOT)/avm2_env/misc/  -DHAVE_STDINT_H -DSOEXT=\"$(SOEXT)\" -DDEFSYSROOT=\"$(call nativepath,$(SDK))\" -I$(SRCROOT)/$(DEPENDENCY_BINUTILS)/include -fPIC -c $(SRCROOT)/gold-plugins/multiplug.cpp
	cd $(BUILD)/multiplug && $(CXX) $(DBGOPTS) -shared -Wl,-headerpad_max_install_names,-undefined,dynamic_lookup -o multiplug$(SOEXT) multiplug.o
	cp -f $(BUILD)/makeswf/makeswf$(SOEXT) $(SDK)/usr/lib/makeswf$(SOEXT)
	cp -f $(BUILD)/multiplug/multiplug$(SOEXT) $(SDK)/usr/lib/multiplug$(SOEXT)
	cp -f $(BUILD)/multiplug/multiplug$(SOEXT) $(SDK)/usr/lib/bfd-plugins/multiplug$(SOEXT)

# ====================================================================================
# GCC
# ====================================================================================
# Assemble LLVM GCC 4.2
gcc:
	rm -rf $(BUILD)/llvm-gcc-42
	mkdir -p $(SDK)/usr/bin
	mkdir -p $(SDK)/usr/lib
	mkdir -p $(BUILD)/llvm-gcc-42
	cd $(BUILD)/llvm-gcc-42 && CFLAGS='$(NOPIE) -DSHARED_LIBRARY_EXTENSION=$(SOEXT) $(BUILD_VER_DEFS) -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS -Os $(DBGOPTS) -I$(SRCROOT)/avm2_env/misc/ ' \
		CC=$(CC) CXX=$(CXX) $(SRCROOT)/llvm-gcc-4.2-2.9/configure --enable-languages=c,c++,objc,obj-c++ \
		--enable-llvm=$(LLVMINSTALLPREFIX)/llvm-install/ --disable-bootstrap --disable-multilib --disable-libada --disable-doc --disable-nls \
		--enable-sjlj-exceptions --disable-shared --program-prefix="" \
		--prefix=$(SDK)/usr --with-sysroot="" --with-build-sysroot=$(SDK)/ \
		--build=$(BUILD_TRIPLE) --host=$(HOST_TRIPLE) --target=$(TRIPLE)
	cd $(BUILD)/llvm-gcc-42 && CC=$(CC) CXX=$(CXX)  $(MAKE) -j$(THREADS) all-gcc \
		CFLAGS_FOR_TARGET='$(NOPIE) -DSHARED_LIBRARY_EXTENSION=$(SOEXT) $(BUILD_VER_DEFS) -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS -Os -emit-llvm -I$(SRCROOT)/avm2_env/misc/ ' \
		CXXFLAGS_FOR_TARGET='$(NOPIE) -DSHARED_LIBRARY_EXTENSION=$(SOEXT) $(BUILD_VER_DEFS) -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS -Os -emit-llvm -I$(SRCROOT)/avm2_env/misc/ ' && $(MAKE) install-gcc
	rm -f $(SDK)/usr/bin/gccbug*
	rm -f $(BUILD)/llvm-gcc-42/gcc/gccbug*
	rm -rf $(SDK)/usr/lib/avm2-unknown-freebsd8
	mv $(SDK)/usr/lib/gcc/* $(SDK)/usr/lib/
	mv $(SDK)/usr/lib/avm2-unknown-freebsd8/4.2.1/*.a $(SDK)/usr/lib/
	rmdir $(SDK)/usr/lib/gcc
	$(RSYNC) $(SDK)/usr/libexec/gcc/avm2-unknown-freebsd8/4.2.1/ $(SDK)/usr/bin/
	rm -rf $(SDK)/usr/libexec

# ====================================================================================
# BMAKE
# ====================================================================================
# Assemble BMake
bmake:
	rm -rf $(BUILD)/bmake
	mkdir -p $(BUILD)/bmake
	cd $(BUILD)/bmake && $(SRCROOT)/$(DEPENDENCY_BMAKE)/configure && bash make-bootstrap.sh

# ====================================================================================
# STDLIBS
# ====================================================================================
# TBD
stdlibs:
	$(MAKE) csu libc libthr libm libBlocksRuntime

# TBD
csu:
	$(RSYNC) avm2_env/usr/ $(BUILD)/lib/
	cd $(BUILD)/lib/src/lib/csu/avm2 && $(BMAKE) SSP_CFLAGS="" MACHINE_ARCH=avm2 crt1_c.o
	cp -f $(BUILD)/lib/src/lib/csu/avm2/crt1_c.o $(SDK)/usr/lib/.

# TBD
# TODO: We are already calling gensyscalls.py in abclibs_compile phase, is second time really necessary?! (VPMedia)
libc:
	mkdir -p $(BUILD)/posix/
	rm -f $(BUILD)/posix/*.o
	mkdir -p $(BUILD)/lib/src/lib/libc/
	$(RSYNC) avm2_env/usr/ $(BUILD)/lib/
	cd $(BUILD)/posix && $(PYTHON) $(SRCROOT)/posix/gensyscalls.py $(SRCROOT)/posix/syscalls.changed
	cp $(BUILD)/posix/IKernel.as $(SRCROOT)/$(DEPENDENCY_AVMPLUS)/shell
	cp $(BUILD)/posix/ShellPosix.as $(SRCROOT)/$(DEPENDENCY_AVMPLUS)/shell
	cp $(BUILD)/posix/ShellPosixGlue.cpp $(SRCROOT)/$(DEPENDENCY_AVMPLUS)/shell
	cp $(BUILD)/posix/ShellPosixGlue.h $(SRCROOT)/$(DEPENDENCY_AVMPLUS)/shell
	cd $(SRCROOT)/$(DEPENDENCY_AVMPLUS)/shell && $(PYTHON) ./shell_toplevel.py -config CONFIG::VMCFG_ALCHEMY_POSIX=true
	cd $(BUILD)/posix && $(SDK)/usr/bin/gcc -emit-llvm -fno-stack-protector $(LIBHELPEROPTFLAGS) -c posix.c
	cd $(BUILD)/posix && $(SDK)/usr/bin/gcc -emit-llvm -fno-stack-protector $(LIBHELPEROPTFLAGS) -c $(SRCROOT)/posix/vgl.c
	cd $(BUILD)/posix && $(SDK)/usr/bin/gcc -emit-llvm -fno-stack-protector $(LIBHELPEROPTFLAGS) -D_KERNEL -c $(SRCROOT)/avm2_env/usr/src/kern/kern_umtx.c
	cd $(BUILD)/posix && $(SDK)/usr/bin/gcc -emit-llvm -fno-stack-protector $(LIBHELPEROPTFLAGS) -I $(SRCROOT)/avm2_env/usr/src/lib/libc/include/ -c $(SRCROOT)/posix/thrStubs.c
	cd $(BUILD)/posix && $(SDK)/usr/bin/gcc -emit-llvm -fno-stack-protector $(LIBHELPEROPTFLAGS) -c $(SRCROOT)/posix/kpmalloc.c
	cd $(BUILD)/posix && cp *.o $(BUILD)/lib/src/lib/libc/
	cd $(BUILD)/lib/src/lib/libc && $(BMAKE) -j$(THREADS) SSP_CFLAGS="" MACHINE_ARCH=avm2 libc.a
	# find bitcode (and ignore non-bitcode genned from .s files) and put
	# it in our lib
	rm -f $(BUILD)/lib/src/lib/libc/tmp/*
	$(AR) $(SDK)/usr/lib/libssp.a $(BUILD)/lib/src/lib/libc/stack_protector.o && cp $(SDK)/usr/lib/libssp.a $(SDK)/usr/lib/libssp_nonshared.a
	# we override these in thrStubs.c but leave them weak
	cd $(BUILD)/lib/src/lib/libc && $(SDK)/usr/bin/llvm-dis -o=_pthread_stubs.ll _pthread_stubs.o && sed -E 's/@pthread_(key_create|key_delete|getspecific|setspecific|once) =/@_d_u_m_m_y_\1 =/g' _pthread_stubs.ll | $(SDK)/usr/bin/llvm-as -o _pthread_stubs.o
	cd $(BUILD)/lib/src/lib/libc && rm -f libc.a && find . -name '*.o' -exec sh -c 'file {} | grep -v 86 > /dev/null' \; -print | xargs $(AR) libc.a
	cd $(BUILD)/posix && $(SDK)/usr/bin/gcc -emit-llvm -fno-stack-protector $(LIBHELPEROPTFLAGS) -I $(SRCROOT)/avm2_env/usr/src/lib/libc/include/ -fexceptions -c $(SRCROOT)/posix/libcHack.c
	cp -f $(BUILD)/lib/src/lib/libc/libc.a $(BUILD)/posix/libcHack.o $(SDK)/usr/lib/.

# TBD
libthr:
	rm -rf $(BUILD)/libthr
	mkdir -p $(BUILD)/libthr
	$(RSYNC) avm2_env/usr/src/lib/ $(BUILD)/libthr/
	cd $(BUILD)/libthr/libthr && $(SDK)/usr/bin/gcc -emit-llvm -fno-stack-protector $(LIBHELPEROPTFLAGS) -c $(SRCROOT)/posix/thrHelpers.c
	# CWARNFLAGS= because thr_exit() can return and pthread_exit() is marked noreturn (where?)...
	cd $(BUILD)/libthr/libthr && $(BMAKE) -j$(THREADS) SSP_CFLAGS="" MACHINE_ARCH=avm2 CWARNFLAGS= libthr.a
	# find bitcode (and ignore non-bitcode genned from .s files) and put
	# it in our lib
	cd $(BUILD)/libthr/libthr && rm -f libthr.a && find . -name '*.o' -exec sh -c 'file {} | grep -v 86 > /dev/null' \; -print | xargs $(AR) libthr.a
	cp -f $(BUILD)/libthr/libthr/libthr.a $(SDK)/usr/lib/.

# TBD
libm:
	cd compiler_rt && $(MAKE) clean && $(MAKE) avm2 CC="$(SDK)/usr/bin/gcc -emit-llvm" RANLIB=$(SDK)/usr/bin/ranlib AR=$(SDK_AR) VERBOSE=1
	$(SDK)/usr/bin/llvm-link -o $(BUILD)/libcompiler_rt.o compiler_rt/avm2/avm2/avm2/SubDir.lib/*.o
	$(SDK_NM) $(BUILD)/libcompiler_rt.o  | grep "T _" | sed 's/_//' | awk '{print $$3}' | sort | uniq > $(BUILD)/compiler_rt.txt
	cat $(BUILD)/compiler_rt.txt >> $(SDK)/public-api.txt
	cat $(SRCROOT)/$(DEPENDENCY_LLVM)/lib/CodeGen/SelectionDAG/TargetLowering.cpp | grep "Names\[RTLIB::" | awk '{print $$3}' | sed 's/"//g' | sed 's/;//' | sort | uniq > $(BUILD)/rtlib.txt
	cat avm2_env/rtlib-extras.txt >> $(BUILD)/rtlib.txt

	rm -rf $(BUILD)/msun/ $(BUILD)/libmbc $(SDK)/usr/lib/libm.a $(SDK)/usr/lib/libm.o
	mkdir -p $(BUILD)/msun
	$(RSYNC) avm2_env/usr/src/lib/ $(BUILD)/msun/
	cd $(BUILD)/msun/msun && $(BMAKE) -j$(THREADS) SSP_CFLAGS="" MACHINE_ARCH=avm2 libm.a
	# find bitcode (and ignore non-bitcode genned from .s files) and put
	# it in our lib
	cd $(BUILD)/msun/msun && rm -f libm.a && find . -name '*.o' -exec sh -c 'file {} | grep -v 86 > /dev/null' \; -print | xargs $(AR) libm.a
	# remove symbols for sin, cos, other things we support as intrinsics
	cd $(BUILD)/msun/msun && $(SDK_AR) sd libm.a s_cos.o s_sin.o e_pow.o e_sqrt.o
	$(SDK_AR) r $(SDK)/usr/lib/libm.a
	mkdir -p $(BUILD)/libmbc
	cd $(BUILD)/libmbc && $(SDK_AR) x $(BUILD)/msun/msun/libm.a
	cd $(BUILD)/libmbc && $(SDK)/usr/bin/llvm-link -o $(BUILD)/libmbc/libm.o $(BUILD)/libcompiler_rt.o *.o
	$(SDK)/usr/bin/opt -O3 -o $(SDK)/usr/lib/libm.o $(BUILD)/libmbc/libm.o
	$(SDK_NM) $(SDK)/usr/lib/libm.o | grep "T _" | sed 's/_//' | awk '{print $$3}' | sort | uniq > $(BUILD)/libm.bc.txt

# TBD
libBlocksRuntime:
	cd compiler_rt/BlocksRuntime && echo '#define HAVE_SYNC_BOOL_COMPARE_AND_SWAP_INT' > config.h && echo '#define HAVE_SYNC_BOOL_COMPARE_AND_SWAP_LONG' >> config.h
	cd compiler_rt/BlocksRuntime && $(SDK)/usr/bin/gcc -emit-llvm -c data.c -o data.o
	cd compiler_rt/BlocksRuntime && $(SDK)/usr/bin/gcc -emit-llvm -c runtime.c -o runtime.o
	cd compiler_rt/BlocksRuntime && $(AR) $(SDK)/usr/lib/libBlocksRuntime.a data.o runtime.o
	cp compiler_rt/BlocksRuntime/Block*.h $(SDK)/usr/include/

# ====================================================================================
# GCCLIBS
# ====================================================================================
# TBD
gcclibs:
	rm -rf $(BUILD)/llvm-gcc-42/$(TRIPLE)
	cd $(BUILD)/llvm-gcc-42 \
		&& $(MAKE) -j$(THREADS) FLASCC_INTERNAL_SDK_ROOT=$(SDK) CFLAGS_FOR_TARGET='-O2 -emit-llvm ' CXXFLAGS_FOR_TARGET='-O2 -emit-llvm ' all-target-libstdc++-v3 all-target-libgomp \
		&& $(MAKE) -j$(THREADS) FLASCC_INTERNAL_SDK_ROOT=$(SDK) install-target-libstdc++-v3 install-target-libgomp \
		&& find $(SDK) -name '*.gch' -type d | xargs rm -rf
	$(SDK)/usr/bin/ranlib $(SDK)/usr/lib/libstdc++.a
	$(SDK)/usr/bin/ranlib $(SDK)/usr/lib/libsupc++.a
	$(SDK)/usr/bin/ranlib $(SDK)/usr/lib/libgomp.a
	cp -f $(SDK)/usr/lib/gcc/$(TRIPLE)/4.2.1/include/omp.h $(SDK)/usr/include/
	rm -rf $(SDK)/usr/lib/gcc
	$(MAKE) libobjc
	mkdir -p $(SDK)/usr/lib/stdlibs_abc
	cd $(BUILD)/posix && $(SDK)/usr/bin/g++ -emit-llvm -fno-stack-protector $(LIBHELPEROPTFLAGS) -c $(SRCROOT)/posix/AS3++.cpp
	cd $(BUILD)/posix && $(SDK)/usr/bin/llc -gendbgsymtable -jvm="$(JAVA)" -falcon-parallel -filetype=obj AS3++.o -o AS3++.abc
	cd $(BUILD)/posix && $(SDK_AR) crus $(SDK)/usr/lib/libAS3++.a AS3++.o
	cd $(BUILD)/posix && $(SDK_AR) crus $(SDK)/usr/lib/stdlibs_abc/libAS3++.a AS3++.abc

# TBD
libobjc_configure:
	cd $(BUILD)/llvm-gcc-42 \
		&& $(MAKE) -j1 FLASCC_INTERNAL_SDK_ROOT=$(SDK) CFLAGS_FOR_TARGET='-O2 -emit-llvm -DSJLJ_EXCEPTIONS=1 ' CXXFLAGS_FOR_TARGET='-O2 -emit-llvm -DSJLJ_EXCEPTIONS=1 ' TARGET-target-libobjc="all OBJC_THREAD_FILE=thr-posix" all-target-libobjc > $(SRCROOT)/cached_build/libobjc/compile.log
	cd $(BUILD)/llvm-gcc-42 \
		&& $(MAKE) -j1 FLASCC_INTERNAL_SDK_ROOT=$(SDK) CFLAGS_FOR_TARGET='-O2 -emit-llvm -DSJLJ_EXCEPTIONS=1 ' CXXFLAGS_FOR_TARGET='-O2 -emit-llvm -DSJLJ_EXCEPTIONS=1 ' install-target-libobjc > $(SRCROOT)/cached_build/libobjc/install.log
	perl -p -i -e 's~$(SRCROOT)~FLASCC_SRC_DIR~g' `grep -ril $(SRCROOT) cached_build/libobjc`

# TBD
libobjc:
	rm -rf $(BUILD)/libobjc
	mkdir -p $(BUILD)/libobjc
	$(PYTHON) $(SRCROOT)/tools/build-objc.py $(SRCROOT)/cached_build/libobjc/compile.log $(SRCROOT)/cached_build/libobjc/install.log $(SRCROOT) > $(BUILD)/libobjc/build.sh
	cd $(BUILD)/libobjc && PATH="$(SDK)/usr/bin:$(PATH)" bash -x build.sh
	# link bitcode
	cd $(BUILD)/libobjc && rm -f libobjc.a && mkdir NXConstStr && mv NXConstStr.o NXConstStr/. && $(SDK)/usr/bin/llvm-link -o libobjc.o *.o && $(AR) libobjc.a libobjc.o NXConstStr/*.o && cp libobjc.a $(SDK)/usr/lib/.

# TBD
abclibobjc:
	mkdir -p $(BUILD)/libobjc_abc
	cd $(BUILD)/libobjc_abc && $(SDK_AR) x $(SDK)/usr/lib/libobjc.a
	cd $(BUILD)/libobjc_abc && cp -f $(SRCROOT)/avm2_env/misc/abcarchive.mk Makefile && SDK=$(SDK) $(MAKE) LLCOPTS=-jvm="$(JAVA)" -j$(THREADS)
	mv $(BUILD)/libobjc_abc/test.a $(SDK)/usr/lib/stdlibs_abc/libobjc.a

# ====================================================================================
# AS3WIG
# ====================================================================================
# TBD
as3wig:
	rm -rf $(BUILD)/as3wig
	mkdir -p $(BUILD)/as3wig/flascc
	cp -f $(SRCROOT)/tools/aet/AS3Wig.java $(BUILD)/as3wig/flascc/.
	cp -f $(SRCROOT)/tools/common/java/flascc/*.java $(BUILD)/as3wig/flascc/.
	cd $(BUILD)/as3wig && javac flascc/*.java -cp $(call nativepath,$(SDK)/usr/lib/aet.jar)
	cd $(BUILD)/as3wig && echo "Main-Class: flascc.AS3Wig" > MANIFEST.MF
	cd $(BUILD)/as3wig && echo "Class-Path: aet.jar" >> MANIFEST.MF
	cd $(BUILD)/as3wig && jar cmf MANIFEST.MF as3wig.jar flascc/*.class
	cp $(BUILD)/as3wig/as3wig.jar $(SDK)/usr/lib/.
	mkdir -p $(SDK)/usr/include/AS3++/
	cp -f $(SRCROOT)/tools/aet/AS3Wig.h $(SDK)/usr/include/AS3++/AS3Wig.h
	java -jar $(call nativepath,$(SDK)/usr/lib/as3wig.jar) -builtins -i $(call nativepath,$(SDK)/usr/lib/builtin.abc) -o $(call nativepath,$(SDK)/usr/include/AS3++/builtin)
	java -jar $(call nativepath,$(SDK)/usr/lib/as3wig.jar) -builtins -i $(call nativepath,$(SDK)/usr/lib/playerglobal.abc) -o $(call nativepath,$(SDK)/usr/include/AS3++/playerglobal)
	cp -f $(SRCROOT)/tools/aet/AS3Wig.cpp $(BUILD)/as3wig/
	echo "#include <AS3++/builtin.h>\n" > $(BUILD)/as3wig/AS3WigIncludes.h
	echo "#include <AS3++/playerglobal.h>\n" >> $(BUILD)/as3wig/AS3WigIncludes.h
	cd $(BUILD)/as3wig && $(SDK)/usr/bin/g++ -c -emit-llvm -I. AS3Wig.cpp -o Flash++.o
	cd $(BUILD)/as3wig && $(SDK_AR) crus $(SDK)/usr/lib/libFlash++.a Flash++.o

# ====================================================================================
# ABCSDTLIBS
# ====================================================================================
# TBD
abcstdlibs:
	$(MAKE) abcflashpp abcstdlibs_more

# TBD
abcflashpp:
	$(SDK)/usr/bin/llc -gendbgsymtable -jvmopt=-Xmx4G -jvm="$(JAVA)" -falcon-parallel -target-player -filetype=obj $(BUILD)/as3wig/Flash++.o -o $(BUILD)/as3wig/Flash++.abc
	$(SDK_AR) crus $(SDK)/usr/lib/stdlibs_abc/libFlash++.a $(BUILD)/as3wig/Flash++.abc

# TBD
abcstdlibs_more:
	mkdir -p $(SDK)/usr/lib/stdlibs_abc
	$(SDK)/usr/bin/llc -gendbgsymtable -jvm="$(JAVA)" -falcon-parallel -filetype=obj $(SDK)/usr/lib/crt1_c.o -o $(SDK)/usr/lib/stdlibs_abc/crt1_c.o
	$(SDK)/usr/bin/llc -gendbgsymtable -jvm="$(JAVA)" -falcon-parallel -filetype=obj $(SDK)/usr/lib/libm.o -o $(SDK)/usr/lib/stdlibs_abc/libm.o
	$(SDK)/usr/bin/llc -gendbgsymtable -jvm="$(JAVA)" -falcon-parallel -filetype=obj $(SDK)/usr/lib/libcHack.o -o $(SDK)/usr/lib/stdlibs_abc/libcHack.o

	mkdir -p $(BUILD)/libc_abc
	cd $(BUILD)/libc_abc && $(SDK_AR) x $(SDK)/usr/lib/libc.a
	cd $(BUILD)/libc_abc && cp -f $(SRCROOT)/avm2_env/misc/abcarchive.mk Makefile && SDK=$(SDK) $(MAKE) LLCOPTS=-jvm="$(JAVA)" -j$(THREADS)
	mv $(BUILD)/libc_abc/test.a $(SDK)/usr/lib/stdlibs_abc/libc.a

	mkdir -p $(BUILD)/libthr_abc
	cd $(BUILD)/libthr_abc && $(SDK_AR) x $(SDK)/usr/lib/libthr.a
	cd $(BUILD)/libthr_abc && cp -f $(SRCROOT)/avm2_env/misc/abcarchive.mk Makefile && SDK=$(SDK) $(MAKE) LLCOPTS=-jvm="$(JAVA)" -j$(THREADS)
	mv $(BUILD)/libthr_abc/test.a $(SDK)/usr/lib/stdlibs_abc/libthr.a

	mkdir -p $(BUILD)/libgcc_abc
	cd $(BUILD)/libgcc_abc && $(SDK_AR) x $(SDK)/usr/lib/libgcc.a
	cd $(BUILD)/libgcc_abc && cp -f $(SRCROOT)/avm2_env/misc/abcarchive.mk Makefile && SDK=$(SDK) $(MAKE) LLCOPTS=-jvm="$(JAVA)" -j$(THREADS)
	mv $(BUILD)/libgcc_abc/test.a $(SDK)/usr/lib/stdlibs_abc/libgcc.a

	mkdir -p $(BUILD)/libstdcpp_abc
	cd $(BUILD)/libstdcpp_abc && $(SDK_AR) x $(SDK)/usr/lib/libstdc++.a
	cd $(BUILD)/libstdcpp_abc && cp -f $(SRCROOT)/avm2_env/misc/abcarchive.mk Makefile && SDK=$(SDK) $(MAKE) LLCOPTS=-jvm="$(JAVA)" -j$(THREADS)
	mv $(BUILD)/libstdcpp_abc/test.a $(SDK)/usr/lib/stdlibs_abc/libstdc++.a

	mkdir -p $(BUILD)/libsupcpp_abc
	cd $(BUILD)/libsupcpp_abc && $(SDK_AR) x $(SDK)/usr/lib/libsupc++.a
	cd $(BUILD)/libsupcpp_abc && cp -f $(SRCROOT)/avm2_env/misc/abcarchive.mk Makefile && SDK=$(SDK) $(MAKE) LLCOPTS=-jvm="$(JAVA)" -j$(THREADS)
	mv $(BUILD)/libsupcpp_abc/test.a $(SDK)/usr/lib/stdlibs_abc/libsupc++.a

	# disable this until aliases work in our abc
	# mkdir -p $(BUILD)/libgomp_abc
	# cd $(BUILD)/libgomp_abc && $(SDK_AR) x $(SDK)/usr/lib/libgomp.a
	# cd $(BUILD)/libgomp_abc && cp -f $(SRCROOT)/avm2_env/misc/abcarchive.mk Makefile && SDK=$(SDK) $(MAKE) LLCOPTS=-jvm="$(JAVA)" -j$(THREADS)
	# mv $(BUILD)/libgomp_abc/test.a $(SDK)/usr/lib/stdlibs_abc/libgomp.a

	mkdir -p $(BUILD)/libobjc_abc
	cd $(BUILD)/libobjc_abc && $(SDK_AR) x $(SDK)/usr/lib/libobjc.a
	cd $(BUILD)/libobjc_abc && cp -f $(SRCROOT)/avm2_env/misc/abcarchive.mk Makefile && SDK=$(SDK) $(MAKE) LLCOPTS=-jvm="$(JAVA)" -j$(THREADS)
	mv $(BUILD)/libobjc_abc/test.a $(SDK)/usr/lib/stdlibs_abc/libobjc.a

	mkdir -p $(BUILD)/libBlocksRuntime_abc
	cd $(BUILD)/libBlocksRuntime_abc && $(SDK_AR) x $(SDK)/usr/lib/libBlocksRuntime.a
	cd $(BUILD)/libBlocksRuntime_abc && cp -f $(SRCROOT)/avm2_env/misc/abcarchive.mk Makefile && SDK=$(SDK) $(MAKE) LLCOPTS=-jvm="$(JAVA)" -j$(THREADS)
	mv $(BUILD)/libBlocksRuntime_abc/test.a $(SDK)/usr/lib/stdlibs_abc/libBlocksRuntime.a

# ====================================================================================
# CLEANUP
# ====================================================================================
# TBD
sdkcleanup:
	mv $(SDK)/usr/share/$(DEPENDENCY_CMAKE) $(SDK)/usr/share_cmake
	rm -rf $(SDK)/usr/share $(SDK)/usr/info $(SDK)/usr/man $(SDK)/usr/lib/x86_64 $(SDK)/usr/cmake_junk $(SDK)/usr/make_junk
	mkdir -p $(SDK)/usr/share
	mv $(SDK)/usr/share_cmake $(SDK)/usr/share/$(DEPENDENCY_CMAKE)
	rm -f $(SDK)/usr/lib/*.la
	rm -f $(SDK)/usr/lib/crt1.o $(SDK)/usr/lib/crtbegin.o $(SDK)/usr/lib/crtbeginS.o $(SDK)/usr/lib/crtbeginT.o $(SDK)/usr/lib/crtend.o $(SDK)/usr/lib/crtendS.o $(SDK)/usr/lib/crti.o $(SDK)/usr/lib/crtn.o

# TBD
finalcleanup:
	perl -p -i -e 's~$(SRCROOT)~\$$\{flascc_sdk_root\}~g' `grep -ril $(SRCROOT) $(SDK)/usr/lib/pkgconfig`
	rm -f $(SDK)/usr/lib/*.la
	rm -rf $(SDK)/usr/share/aclocal $(SDK)/usr/share/doc $(SDK)/usr/share/man $(SDK)/usr/man $(SDK)/usr/share/info
	@$(LN) ../../share $(SDK)/usr/platform/$(PLATFORM)/share
	$(RSYNC) $(SRCROOT)/tools/add-opt-in.py $(SDK)/usr/bin/
	$(RSYNC) $(SRCROOT)/tools/projector-dis.py $(SDK)/usr/bin/
	$(RSYNC) $(SRCROOT)/tools/swfdink.py $(SDK)/usr/bin/
	$(RSYNC) $(SRCROOT)/tools/swf-info.py $(SDK)/usr/bin/
	$(RSYNC) $(SRCROOT)/posix/avm2_tramp.cpp $(SDK)/usr/share/
	$(RSYNC) $(SRCROOT)/posix/vgl.c $(SDK)/usr/share/
	$(RSYNC) $(SRCROOT)/posix/Console.as $(SDK)/usr/share/
	$(RSYNC) $(SRCROOT)/posix/DefaultPreloader.as $(SDK)/usr/share/
	$(RSYNC) $(SRCROOT)/posix/vfs/HTTPBackingStore.as $(SDK)/usr/share/
	$(RSYNC) $(SRCROOT)/posix/vfs/DefaultVFS.as $(SDK)/usr/share/
	$(RSYNC) $(SRCROOT)/posix/vfs/ISpecialFile.as $(SDK)/usr/share/
	$(RSYNC) $(SRCROOT)/posix/vfs/IBackingStore.as $(SDK)/usr/share/
	$(RSYNC) $(SRCROOT)/posix/vfs/IVFS.as $(SDK)/usr/share/
	$(RSYNC) $(SRCROOT)/posix/vfs/InMemoryBackingStore.as $(SDK)/usr/share/
	$(RSYNC) $(SRCROOT)/posix/vfs/LSOBackingStore.as $(SDK)/usr/share/
	$(RSYNC) --exclude "*.xslt" --exclude "*.html" --exclude ASDoc_Config.xml --exclude overviews.xml $(BUILDROOT)/tempdita/ $(SDK)/usr/share/asdocs

# ====================================================================================
# EXTRA TOOLS
# ====================================================================================

# Tamarin Shell built without debugging
tr:
	rm -rf $(BUILD)/tr
	mkdir -p $(BUILD)/tr
	mkdir -p $(SDK)/usr/bin
	cd $(BUILD)/tr && rm -f Makefile && AR=$(NATIVE_AR) CC=$(CC) CXX=$(CXX) $(TAMARINCONFIG) --disable-debugger
	cd $(BUILD)/tr && AR=$(NATIVE_AR) CC=$(CC) CXX=$(CXX) $(MAKE) -j$(THREADS)
	cp -f $(BUILD)/tr/shell/avmshell $(SDK)/usr/bin/avmshell
	cd $(SRCROOT)/$(DEPENDENCY_AVMPLUS)/utils && curdir=$(SRCROOT)/$(DEPENDENCY_AVMPLUS)/utils ASC=$(ASC) $(MAKE) -f manifest.mk utils
	cd $(BUILD)/abclibs && $(SCOMPFALCON) $(call nativepath,$(SRCROOT)/$(DEPENDENCY_AVMPLUS)/utils/projectormake.as) -outdir . -out projectormake
ifneq (,$(findstring cygwin,$(PLATFORM)))
	$(SDK)/usr/bin/avmshell $(BUILD)/abclibs/projectormake.abc -- -o $(SDK)/usr/bin/abcdump$(EXEEXT) $(SDK)/usr/bin/avmshell $(BUILD)/abcdump.abc -- -Djitordie
	chmod a+x $(SDK)/usr/bin/abcdump$(EXEEXT)
endif

# Tamarin Shell built with debugging
trd:
	rm -rf $(BUILD)/trd
	mkdir -p $(BUILD)/trd
	mkdir -p $(SDK)/usr/bin
	cd $(BUILD)/trd && rm -f Makefile && AR=$(NATIVE_AR) CC=$(CC) CXX=$(CXX) $(TAMARINCONFIG) --enable-debugger
	cd $(BUILD)/trd && AR=$(NATIVE_AR) CC=$(CC) CXX=$(CXX) $(MAKE) -j$(THREADS)
	cp -f $(BUILD)/trd/shell/avmshell $(SDK)/usr/bin/avmshell-release-debugger

SWIG_LDFLAGS=-L$(BUILD)/llvm-debug/lib
SWIG_LIBS=-lLLVMAVM2ShimInfo -lLLVMAVM2ShimCodeGen -lclangFrontend -lclangCodeGen -lclangDriver -lclangParse -lclangSema -lclangAnalysis -lclangLex -lclangAST -lclangBasic -lLLVMSelectionDAG -lLLVMCodeGen -lLLVMTarget -lLLVMMC -lLLVMScalarOpts -lLLVMTransformUtils -lLLVMAnalysis -lclangSerialization -lLLVMCore -lLLVMSupport 
SWIG_CXXFLAGS=-I$(SRCROOT)/avm2_env/misc/ -I$(SRCROOT)/$(DEPENDENCY_LLVM)/include -I$(BUILD)/llvm-debug/include -I$(SRCROOT)/$(DEPENDENCY_LLVM)/tools/clang/include -I$(BUILD)/llvm-debug/tools/clang/include -I$(SRCROOT)/$(DEPENDENCY_LLVM)/tools/clang/lib -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS -fno-rtti -g -Wno-long-long
# VPMedia: Why delete, I would want a full featured swig shipped as possible, so deletion is disabled below
SWIG_DIRS_TO_DELETE=allegrocl chicken clisp csharp d gcj go guile java lua modula3 mzscheme ocaml octave perl5 php pike python r ruby tcl

# Build SWIG
swig:
	rm -rf $(BUILD)/swig
	mkdir -p $(BUILD)/swig
	#unpack PCRE dependency
	cp -f packages/pcre-8.20.tar.gz $(BUILD)/swig
	#configure PCRE dependency
	cd $(BUILD)/swig && $(SRCROOT)/$(DEPENDENCY_SWIG)/Tools/pcre-build.sh --build=$(BUILD_TRIPLE) --host=$(HOST_TRIPLE) --target=$(HOST_TRIPLE)
	#initialize SWIG
	#cd $(SRCROOT)/$(DEPENDENCY_SWIG) && ./autogen.sh --build=$(BUILD_TRIPLE) --host=$(HOST_TRIPLE) --target=$(HOST_TRIPLE)
	#configure SWIG
	cd $(BUILD)/swig && CFLAGS=-g LDFLAGS="$(SWIG_LDFLAGS)" LIBS="$(SWIG_LIBS)" CXXFLAGS="$(SWIG_CXXFLAGS)" $(SRCROOT)/$(DEPENDENCY_SWIG)/configure --prefix=$(SDK)/usr --disable-ccache --without-maximum-compile-warnings --build=$(BUILD_TRIPLE) --host=$(HOST_TRIPLE) --target=$(HOST_TRIPLE)
	#make and install SWIG
	cd $(BUILD)/swig && $(MAKE) && $(MAKE) install
	#$(foreach var, $(SWIG_DIRS_TO_DELETE), rm -rf $(SDK)/usr/share/swig/3.0.0/$(var);)

# Run SWIG Tests
swigtests:
	# reconfigure so that makefile is up to date (in case Makefile.in changed)
	cd $(BUILD)/swig && CFLAGS=-g LDFLAGS="$(SWIG_LDFLAGS)" LIBS="$(SWIG_LIBS)" \
		CXXFLAGS="$(SWIG_CXXFLAGS)" $(SRCROOT)/$(DEPENDENCY_SWIG)/configure --prefix=$(SDK)/usr --disable-ccache
	rm -rf $(BUILD)/swig/Examples/as3
	cp -R $(SRCROOT)/$(DEPENDENCY_SWIG)/Examples/as3 $(BUILD)/swig/Examples
	rm -rf $(BUILD)/swig/Lib/
	mkdir -p $(BUILD)/swig/Lib/as3
	cp -R $(SRCROOT)/$(DEPENDENCY_SWIG)/Lib/as3/* $(BUILD)/swig/Lib/as3
	cp $(SRCROOT)/$(DEPENDENCY_SWIG)/Lib/*.i $(BUILD)/swig/Lib
	cp $(SRCROOT)/$(DEPENDENCY_SWIG)/Lib/*.swg $(BUILD)/swig/Lib
	cd $(BUILD)/swig && $(MAKE) check-as3-examples

# Run SWIG Automation Tests (Legacy / Deprecated)
swigtestsautomation:
	cd $(SRCROOT)/qa/swig/framework && $(MAKE) SWIG_SOURCE=$(SRCROOT)/$(DEPENDENCY_SWIG)

# 04.06. Removed -Werror after -Wall from $CC for Maverick compatibility
# Generate Virtual File System ZLib Dependency
genfs:
	rm -rf $(BUILD)/zlib-native
	mkdir -p $(BUILD)/zlib-native
	$(RSYNC) $(SRCROOT)/$(DEPENDENCY_ZLIB)/ $(BUILD)/zlib-native
	cd $(BUILD)/zlib-native && AR=$(NATIVE_AR) CC=$(CC) CXX=$(CXX) ./configure --static && $(MAKE) 
	cd $(BUILD)/zlib-native/contrib/minizip/ && $(MAKE) 
	$$CC -Wall -I$(BUILD)/zlib-native/contrib/minizip -o $(SDK)/usr/bin/genfs$(EXEEXT) $(BUILD)/zlib-native/contrib/minizip/zip.o $(BUILD)/zlib-native/contrib/minizip/ioapi.o $(BUILD)/zlib-native/libz.a $(SRCROOT)/tools/vfs/genfs.c

# Build GDB Debugger
gdb:
	rm -rf $(BUILD)/$(DEPENDENCY_GDB)
	mkdir -p $(BUILD)/$(DEPENDENCY_GDB)
	cd $(BUILD)/$(DEPENDENCY_GDB) && CFLAGS="-I$(SRCROOT)/avm2_env/misc" $(SRCROOT)/$(DEPENDENCY_GDB)/configure \
		--build=$(BUILD_TRIPLE) --host=$(HOST_TRIPLE) --target=avm2-elf && $(MAKE)
	cp -f $(BUILD)/$(DEPENDENCY_GDB)/gdb/gdb$(EXEEXT) $(SDK)/usr/bin/
	cp -f $(SRCROOT)/tools/flascc.gdb $(SDK)/usr/share/
	cp -f $(SRCROOT)/tools/flascc-run.gdb $(SDK)/usr/share/
	cp -f $(SRCROOT)/tools/flascc-init.gdb $(SDK)/usr/share/

# pkg-config is a helper tool used when compiling applications and libraries. 
# It is language-agnostic, so it can be used for defining the location of documentation tools, for instance. 
pkgconfig:
	rm -rf $(BUILD)/pkgconfig
	mkdir -p $(BUILD)/pkgconfig
	cd $(BUILD)/pkgconfig && CFLAGS="-I$(SRCROOT)/avm2_env/misc" $(SRCROOT)/$(DEPENDENCY_PKG_CFG)/configure \
		--prefix=$(SDK)/usr --build=$(BUILD_TRIPLE) --host=$(HOST_TRIPLE) --target=$(TRIPLE) --disable-shared \
		--disable-dependency-tracking
	cd $(BUILD)/pkgconfig && $(MAKE) && $(MAKE) install

# GNU libtool is a generic library support script. 
# Libtool hides the complexity of using shared libraries behind a consistent, portable interface. 
libtool:
	rm -rf $(BUILD)/libtool
	mkdir -p $(BUILD)/libtool
	cd $(BUILD)/libtool && CC=$(CC) CXX=$(CXX) $(SRCROOT)/$(DEPENDENCY_LIBTOOL)/configure \
		--build=$(BUILD_TRIPLE) --host=$(HOST_TRIPLE) --target=$(TRIPLE) \
		--prefix=$(SDK)/usr --enable-static --disable-shared --disable-ltdl-install
	cd $(BUILD)/libtool && $(MAKE) && $(MAKE) install-exec

# Converts GLSL Shaders to Stage3D AGAL format
# About 'peflags' see: http://www.cygwin.com/cygwin-ug-net/setup-maxmem.html
glsl2agal:
	rm -rf $(BUILD)/glsl2agal
	mkdir -p $(BUILD)/glsl2agal
	$(RSYNC) $(SRCROOT)/tools/glsl2agal/ $(BUILD)/glsl2agal
	cd $(BUILD)/glsl2agal/agaloptimiser/src && SDK="$(call nativepath, $(SDK))" ./genabc.sh
	cd $(BUILD)/glsl2agal/swc && PATH=$(SDK)/usr/bin:$(PATH) $(SDK_CMAKE) -G "Unix Makefiles" $(BUILD)/glsl2agal/swc
	cd $(BUILD)/glsl2agal/swc && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) -j$(THREADS)
ifneq (,$(findstring cygwin,$(PLATFORM)))
	peflags --cygwin-heap=4096 $(SDK)/usr/bin/llc$(EXEEXT)
endif
	cd $(BUILD)/glsl2agal/swc && PATH=$(SDK)/usr/bin:$(PATH) $(CXX) -fno-exceptions -fno-rtti -O4 -flto-api=exports.txt -emit-swc=com.adobe.glsl2agal -o glsl2agal.swc agaloptimiser.abc swc.cpp libglsl2agal.a -I../include -I../src/mesa -I../src/mapi -I../src/glsl
	cd $(BUILD)/glsl2agal/swc && PATH=$(SDK)/usr/bin:$(PATH) $(CXX) -DCMDLINE=1 -fno-exceptions -fno-rtti --enable-debug -O4 -flto-api=exports.txt -o glsl2agalopt agaloptimiser.abc swc.cpp libglsl2agal.a -I../include -I../src/mesa -I../src/mapi -I../src/glsl
ifneq (,$(findstring cygwin,$(PLATFORM)))
	peflags --cygwin-heap=0 $(SDK)/usr/bin/llc$(EXEEXT)
endif
	cd $(BUILD)/glsl2agal/swc && $(PYTHON) $(SRCROOT)/tools/projector-dis.py $(BUILD)/glsl2agal/swc/glsl2agalopt
	cd $(BUILD)/glsl2agal/swc && $(SDK)/usr/bin/avmshell $(BUILD)/projectormake.abc -- -o $(BUILD)/glsl2agal/swc/glsl2agalopt$(EXEEXT) \
		$(SDK)/usr/bin/avmshell $(BUILD)/glsl2agal/swc/output.swf --  -osr=1
	#cp -f glsl2agal.swc glsl2agalopt.* $(SDK)/usr/bin/

#glsl2agal_example:
#	cd examples/basic && $(FLEX)/bin/mxmlc -omit-trace-statements=false -library-path+=$(BUILD)/glsl2agal/swc/glsl2agal.swc \
#	GLSLCompiler.mxml -o GLSLCompiler.swf

# ====================================================================================
# EXTRA LIBS
# ====================================================================================
# TBD
extralibs:
	$(MAKE) zlib libbzip libxz libeigen dmalloc libffi libgmp libiconv libvgl libjpeg libpng libgif libtiff libwebp \
		libogg libvorbis libflac libsndfile libsdl libfreetype libsdl_ttf libsdl_mixer libsdl_image libphysfs

# A Massively Spiffy Yet Delicately Unobtrusive Compression Library
zlib:
	rm -rf $(BUILD)/zlib
	cp -r $(SRCROOT)/$(DEPENDENCY_ZLIB) $(BUILD)/zlib
	cd $(BUILD)/zlib && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) -j$(THREADS) libz.a CFLAGS=$(CFLAGS) CXXFLAGS=$(CXXFLAGS) SFLAGS=-O4
	$(RSYNC) $(BUILD)/zlib/zlib.h $(SDK)/usr/include/
	$(RSYNC) $(BUILD)/zlib/libz.a $(SDK)/usr/lib/

# BZip data compression
libbzip:
	cd $(SRCROOT)/$(DEPENDENCY_LIBBZIP) && PATH=$(SDK)/usr/bin:$(PATH) CC=$(CC) CXX=$(CXX) CFLAGS=$(CFLAGS) CXXFLAGS=$(CXXFLAGS) $(MAKE) PREFIX=$(SDK)/usr install
	$(RSYNC) $(SRCROOT)/$(DEPENDENCY_LIBBZIP)/bzlib.h $(SDK)/usr/include/
	$(RSYNC) $(SRCROOT)/$(DEPENDENCY_LIBBZIP)/libbz2.a $(SDK)/usr/lib/

# XZ data compression
libxz:
	rm -rf $(BUILD)/libxz
	mkdir -p $(BUILD)/libxz
	cd $(BUILD)/libxz && PATH=$(SDK)/usr/bin:$(PATH) CC=$(CC) CXX=$(CXX) CFLAGS=$(CFLAGS) CXXFLAGS=$(CXXFLAGS) $(SRCROOT)/$(DEPENDENCY_LIBXZ)/configure \
		--prefix=$(SDK)/usr --build=$(BUILD_TRIPLE) --host=$(TRIPLE) --target=$(TRIPLE) --enable-static --disable-shared \
		--enable-encoders=lzma1,lzma2,delta --enable-decoders=lzma1,lzma2,delta 
	cd $(BUILD)/libxz && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install

# Eigen is a C++ template library for linear algebra: matrices, vectors, numerical solvers, and related algorithms.
libeigen:
	rm -rf $(BUILD)/libeigen
	mkdir -p $(BUILD)/libeigen
	cd $(BUILD)/libeigen && PATH=$(SDK)/usr/bin:$(PATH) $(SDK_CMAKE) -G "Unix Makefiles" \
		$(SRCROOT)/eigen-eigen-5097c01bcdc4 -DCMAKE_INSTALL_PREFIX="$(SDK)/usr"
	cd $(BUILD)/libeigen && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install

# The debug memory allocation or dmalloc library has been designed as a drop in replacement for the system's malloc, realloc, calloc, free and other memory management routines while providing powerful debugging facilities configurable at runtime. 
# These facilities include such things as memory-leak tracking, fence-post write detection, file/line number reporting, and general logging of statistics. 
dmalloc:
	rm -rf $(BUILD)/dmalloc
	mkdir -p $(BUILD)/dmalloc
	cd $(BUILD)/dmalloc && PATH=$(SDK)/usr/bin:$(PATH) CC=$(CC) CXX=$(CXX) $(SRCROOT)/$(DEPENDENCY_DMALLOC)/configure \
		--prefix=$(SDK)/usr --disable-shared --enable-static --build=$(BUILD_TRIPLE) --host=$(TRIPLE) --target=$(TRIPLE)
	cd $(BUILD)/dmalloc && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) -j1 threads cxx
	cd $(BUILD)/dmalloc && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) -j1 installcxx installth
	cd $(BUILD)/dmalloc && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) -j1 heavy

# Compilers for high level languages generate code that follows certain conventions. 
# These conventions are necessary, in part, for separate compilation to work. 
libffi:
	mkdir -p $(BUILD)/libffi
	cd $(BUILD)/libffi && PATH=$(SDK)/usr/bin:$(PATH) $(SRCROOT)/$(DEPENDENCY_FFI)/configure \
		--prefix=$(SDK)/usr --enable-static --disable-shared
	cd $(BUILD)/libffi && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install

# TBD
libfficheck:
	cd $(BUILD)/libffi/testsuite && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) check

# GMP is a free library for arbitrary precision arithmetic, operating on signed integers, rational numbers, and floating-point numbers. 
libgmp:
	rm -rf $(BUILD)/libgmp
	mkdir -p $(BUILD)/libgmp
	cd $(BUILD)/libgmp && PATH=$(SDK)/usr/bin:$(PATH) CC=$(CC) CXX=$(CXX) CFLAGS=$(CFLAGS) CXXFLAGS=$(CXXFLAGS) $(SRCROOT)/$(DEPENDENCY_LIBGMP)/configure \
		--prefix=$(SDK)/usr --build=$(BUILD_TRIPLE) --host=$(TRIPLE) --target=$(TRIPLE) --enable-static --disable-shared 
	cd $(BUILD)/libgmp && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install

# For historical reasons, international text is often encoded using a language or country dependent character encoding. 
libiconv:
	rm -rf $(BUILD)/libiconv
	mkdir -p $(BUILD)/libiconv
	cd $(BUILD)/libiconv && PATH=$(SDK)/usr/bin:$(PATH) CC=$(CC) CXX=$(CXX) CFLAGS=$(CFLAGS) CXXFLAGS=$(CXXFLAGS) $(SRCROOT)/$(DEPENDENCY_LIBICONV)/configure \
		--prefix=$(SDK)/usr --host=$(TRIPLE) --enable-static --disable-shared \
		--disable-dependency-tracking
	cd $(BUILD)/libiconv && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install

# The Ncurses (new curses) library is a free software emulation of curses in System V Release 4.0, and more. 
libncurses:
	rm -rf $(BUILD)/libncurses
	mkdir -p $(BUILD)/libncurses
	cd $(BUILD)/libncurses && PATH=$(SDK)/usr/bin:$(PATH) CC=$(CC) CXX=$(CXX) CFLAGS=$(CFLAGS) CXXFLAGS=$(CXXFLAGS) $(SRCROOT)/$(DEPENDENCY_LIBNCURSES)/configure \
		--prefix=$(SDK)/usr --host=$(TRIPLE) --enable-static --disable-shared \
		--disable-pthread --without-shared --without-debug --without-tests \
		--without-progs --without-dlsym
	cd $(BUILD)/libncurses && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install

# The GNU Readline library provides a set of functions for use by applications that allow users to edit command lines as they are typed in. 
libreadline:
	rm -rf $(BUILD)/libreadline
	mkdir -p $(BUILD)/libreadline
	cd $(BUILD)/libreadline && PATH=$(SDK)/usr/bin:$(PATH) CC=$(CC) CXX=$(CXX) CFLAGS=$(CFLAGS) CXXFLAGS=$(CXXFLAGS) $(SRCROOT)/$(DEPENDENCY_LIBREADLINE)/configure \
		--prefix=$(SDK)/usr --build=$(BUILD_TRIPLE) --host=$(TRIPLE) --target=$(TRIPLE) --enable-static --disable-shared --with-curses 
	cd $(BUILD)/libreadline && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install

# OpenGL-based programs must link with the libGL library. libGL implements the GLX interface as well as the main OpenGL API entrypoints. 
libvgl:
	$(RSYNC) avm2_env/usr/ $(BUILD)/lib/
	cd $(BUILD)/lib/src/lib/libvgl && $(BMAKE) -j$(THREADS) SSP_CFLAGS="" MACHINE_ARCH=avm2 libvgl.a
	rm -f $(SDK)/usr/lib/libvgl.a
	$(AR) $(SDK)/usr/lib/libvgl.a $(BUILD)/lib/src/lib/libvgl/*.o

# JPEG image format library. 
libjpeg:
	rm -rf $(BUILD)/libjpeg
	mkdir -p $(BUILD)/libjpeg
	cd $(BUILD)/libjpeg && PATH=$(SDK)/usr/bin:$(PATH) CC=$(CC) CXX=$(CXX) CFLAGS=$(CFLAGS) CXXFLAGS=$(CXXFLAGS) $(SRCROOT)/$(DEPENDENCY_JPEG)/configure \
		--prefix=$(SDK)/usr --build=$(BUILD_TRIPLE) --host=$(TRIPLE) --target=$(TRIPLE) --disable-shared \
		--disable-dependency-tracking
	cd $(BUILD)/libjpeg && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) -j$(THREADS) libjpeg.la && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install-libLTLIBRARIES install-includeHEADERS
	cp -f $(BUILD)/libjpeg/jconfig.h $(SDK)/usr/include/
	rm -f $(SDK)/usr/lib/libjpeg.so
	rm -f $(SDK)/usr/bin/avm2-unknown-freebsd8-jpegtran
	rm -f $(SDK)/usr/bin/avm2-unknown-freebsd8-rdjpgcom
	rm -f $(SDK)/usr/bin/avm2-unknown-freebsd8-wrjpgcom
	rm -f $(SDK)/usr/bin/avm2-unknown-freebsd8-cjpeg
	rm -f $(SDK)/usr/bin/avm2-unknown-freebsd8-djpeg

# PNG image format library. 
libpng:
	rm -rf $(BUILD)/libpng
	mkdir -p $(BUILD)/libpng
	cd $(BUILD)/libpng && PATH=$(SDK)/usr/bin:$(PATH) CC=$(CC) CXX=$(CXX) CFLAGS=$(CFLAGS) CXXFLAGS=$(CXXFLAGS) $(SRCROOT)/$(DEPENDENCY_LIBPNG)/configure \
		--prefix=$(SDK)/usr --build=$(BUILD_TRIPLE) --host=$(TRIPLE) --target=$(TRIPLE) --disable-shared \
		--disable-dependency-tracking
	cd $(BUILD)/libpng && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) -j$(THREADS) && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install
	rm -f $(SDK)/usr/bin/libpng-config
	cp -f $(SDK)/usr/bin/libpng15-config $(SDK)/usr/bin/libpng-config
	rm -f $(SDK)/usr/lib/libpng.a
	cp -f $(SDK)/usr/lib/libpng15.a $(SDK)/usr/lib/libpng.a

# GIF image format library.
libgif:
	rm -rf $(BUILD)/libgif
	mkdir -p $(BUILD)/libgif
	cd $(BUILD)/libgif && PATH=$(SDK)/usr/bin:$(PATH) $(SRCROOT)/$(DEPENDENCY_LIBGIF)/configure \
		--prefix=$(SDK)/usr --build=$(BUILD_TRIPLE) --host=$(TRIPLE) --target=$(TRIPLE) --enable-static --disable-shared \
		--disable-dependency-tracking 
	cd $(BUILD)/libgif && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install

# TIFF image format library.
libtiff:
	rm -rf $(BUILD)/libtiff
	mkdir -p $(BUILD)/libtiff
	cd $(BUILD)/libtiff && PATH=$(SDK)/usr/bin:$(PATH) $(SRCROOT)/$(DEPENDENCY_LIBTIFF)/configure \
		--prefix=$(SDK)/usr --build=$(BUILD_TRIPLE) --host=$(TRIPLE) --target=$(TRIPLE) --enable-static --disable-shared \
		--disable-dependency-tracking 
	cd $(BUILD)/libtiff && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install

# WebP is an image format that does lossy compression of digital photographic images. 
libwebp:
	rm -rf $(BUILD)/libwebp
	mkdir -p $(BUILD)/libwebp
	cd $(BUILD)/libwebp && PATH=$(SDK)/usr/bin:$(PATH) $(SRCROOT)/$(DEPENDENCY_LIBWEBP)/configure \
		--prefix=$(SDK)/usr --build=$(BUILD_TRIPLE) --host=$(TRIPLE) --target=$(TRIPLE) --enable-static --disable-shared \
		--disable-dependency-tracking 
	cd $(BUILD)/libwebp && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install

# FreeType is a freely available software library to render fonts.
libfreetype:
	rm -rf $(BUILD)/libfreetype
	mkdir -p $(BUILD)/libfreetype
	cd $(BUILD)/libfreetype && PATH=$(SDK)/usr/bin:$(PATH) $(SRCROOT)/$(DEPENDENCY_LIBFREETYPE)/configure \
		--prefix=$(SDK)/usr --build=$(BUILD_TRIPLE) --host=$(TRIPLE) --target=$(TRIPLE) --enable-static --disable-shared \
		--disable-mmap --without-bzip2 --without-ats --without-old-mac-fonts
	cd $(BUILD)/libfreetype && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE)
	cd $(BUILD)/libfreetype && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install

# Simple DirectMedia Layer provide low level access to audio, keyboard, mouse, joystick, and graphics hardware. 
libsdl:
	rm -rf $(BUILD)/libsdl
	mkdir -p $(BUILD)/libsdl
	cd $(BUILD)/libsdl && PATH=$(SDK)/usr/bin:$(PATH) CC=$(CC) CXX=$(CXX) CFLAGS=$(CFLAGS) CXXFLAGS=$(CXXFLAGS) $(SRCROOT)/$(DEPENDENCY_LIBSDL)/configure \
		--host=$(TRIPLE) --prefix=$(SDK)/usr --disable-pthreads --disable-alsa --disable-video-x11 \
		--disable-cdrom --disable-loadso --disable-assembly --disable-esd --disable-arts --disable-nas \
		--disable-nasm --disable-altivec --disable-dga --disable-screensaver --disable-sdl-dlopen \
		--disable-directx --enable-joystick --enable-video-vgl --enable-static --disable-shared
	rm $(BUILD)/libsdl/config.status
	cd $(BUILD)/libsdl && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) -j$(THREADS)
	cd $(BUILD)/libsdl && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install
	$(MAKE) libsdl_install
	rm $(SDK)/usr/include/SDL/SDL_opengl.h

# Install SDL with our custom sdl-config
libsdl_install:
	cp $(SRCROOT)/tools/sdl-config $(SDK)/usr/bin/.
	chmod a+x $(SDK)/usr/bin/sdl-config

# TBD
libsdl_image:
	mkdir -p $(BUILD)/libsdlimage
	cd $(BUILD)/libsdlimage && PATH=$(SDK)/usr/bin:$(PATH) $(SRCROOT)/$(DEPENDENCY_LIBSDLIMAGE)/configure \
		--prefix=$(SDK)/usr --with-freetype-prefix=$(SDK)/usr --enable-static --disable-shared \
		--disable-dependency-tracking --disable-sdltest --without-x
	cd $(BUILD)/libsdlimage && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install

# TBD
libsdl_mixer:
	mkdir -p $(BUILD)/libsdlmixer
	cd $(BUILD)/libsdlmixer && PATH=$(SDK)/usr/bin:$(PATH) $(SRCROOT)/$(DEPENDENCY_LIBSDLMIXER)/configure \
		--prefix=$(SDK)/usr --with-freetype-prefix=$(SDK)/usr --enable-static --disable-shared \
		--disable-dependency-tracking --disable-sdltest --without-x
	cd $(BUILD)/libsdlmixer && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install

# TBD
libsdl_ttf:
	mkdir -p $(BUILD)/libsdlttf
	cd $(BUILD)/libsdlttf && PATH=$(SDK)/usr/bin:$(PATH) $(SRCROOT)/$(DEPENDENCY_LIBSDLTTF)/configure \
		--prefix=$(SDK)/usr --with-sdl-prefix=$(SDK)/usr --with-freetype-prefix=$(SDK)/usr --enable-static --disable-shared \
		--disable-dependency-tracking --disable-sdltest --without-x
	cd $(BUILD)/libsdlttf && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install

# Ogg is a multimedia container format, and the native file and stream format for the Xiph.org multimedia codecs. 
libogg:
	rm -rf $(BUILD)/libogg
	mkdir -p $(BUILD)/libogg
	cd $(BUILD)/libogg && PATH=$(SDK)/usr/bin:$(PATH) CC=$(CC) CXX=$(CXX) CFLAGS=$(CFLAGS) CXXFLAGS=$(CXXFLAGS) $(SRCROOT)/$(DEPENDENCY_LIBOGG)/configure \
		--prefix=$(SDK)/usr --build=$(BUILD_TRIPLE) --host=$(TRIPLE) --target=$(TRIPLE) --enable-static --disable-shared \
		--disable-dependency-tracking
	cd $(BUILD)/libogg && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install

# Ogg Vorbis is a completely open, patent-free, professional audio encoding and streaming technology with all the benefits of Open Source.
libvorbis:
	rm -rf $(BUILD)/libvorbis
	mkdir -p $(BUILD)/libvorbis
	cd $(BUILD)/libvorbis && PATH=$(SDK)/usr/bin:$(PATH) CC=$(CC) CXX=$(CXX) CFLAGS=$(CFLAGS) CXXFLAGS=$(CXXFLAGS) $(SRCROOT)/$(DEPENDENCY_LIBVORBIS)/configure \
		--prefix=$(SDK)/usr --build=$(BUILD_TRIPLE) --host=$(TRIPLE) --target=$(TRIPLE) --enable-static --disable-shared \
		--disable-dependency-tracking
	cd $(BUILD)/libvorbis && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install

# TBD
libflac:
	rm -rf $(BUILD)/libflac
	mkdir -p $(BUILD)/libflac
	mkdir -p $(SDK)/usr/share/doc/$(DEPENDENCY_LIBFLAC)/html/api
	cd $(BUILD)/libflac && PATH=$(SDK)/usr/bin:$(PATH) CC=$(CC) CXX=$(CXX) CFLAGS=$(CFLAGS) CXXFLAGS=$(CXXFLAGS) $(SRCROOT)/$(DEPENDENCY_LIBFLAC)/configure \
		--prefix=$(SDK)/usr --build=$(BUILD_TRIPLE) --host=$(TRIPLE) --target=$(TRIPLE) --enable-static --disable-shared \
		--disable-dependency-tracking --disable-doxygen-docs --disable-xmms-plugin --disable-thorough-tests --disable-oggtest --disable-largefile
	cd $(BUILD)/libflac && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) -i install

# Libsndfile is a C library for reading and writing files containing sampled sound.
libsndfile:
	rm -rf $(BUILD)/libsndfile
	mkdir -p $(BUILD)/libsndfile
	cd $(BUILD)/libsndfile && PATH=$(SDK)/usr/bin:$(PATH) CC=$(CC) CXX=$(CXX) CFLAGS=$(CFLAGS) CXXFLAGS=$(CXXFLAGS) $(SRCROOT)/$(DEPENDENCY_LIBSNDFILE)/configure \
		--prefix=$(SDK)/usr --build=$(BUILD_TRIPLE) --host=$(TRIPLE) --target=$(TRIPLE) --enable-static --disable-shared \
		--disable-dependency-tracking
	cd $(BUILD)/libsndfile && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install

# AAlib is an portable ascii art GFX library.
libaa:
	rm -rf $(BUILD)/libaa
	mkdir -p $(BUILD)/libaa
	cd $(BUILD)/libaa && PATH=$(SDK)/usr/bin:$(PATH) CC=$(CC) CXX=$(CXX) CFLAGS=$(CFLAGS) CXXFLAGS=$(CXXFLAGS) $(SRCROOT)/$(DEPENDENCY_LIBAA)/configure \
		--prefix=$(SDK)/usr --build=$(BUILD_TRIPLE) --host=$(TRIPLE) --target=$(TRIPLE) --enable-static --disable-shared \
		--without-x --with-curses-driver=no
	cd $(BUILD)/libaa && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install

# Cryptography library.
libopenssl:
	rm -rf $(BUILD)/openssl
	mkdir -p $(BUILD)/openssl
	cd $(SRCROOT)/$(DEPENDENCY_OPENSSL) && PATH=$(SDK)/usr/bin:$(PATH) CC=$(CC) CXX=$(CXX) CFLAGS=$(CFLAGS) CXXFLAGS=$(CXXFLAGS) ./configure \
		BSD-x86 --prefix=$(SDK)/usr --build=$(BUILD_TRIPLE) --host=$(TRIPLE) --target=$(TRIPLE) \
		-no-hw -no-asm -no-threads -no-shared -no-dso
	cd $(SRCROOT)/$(DEPENDENCY_OPENSSL) && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install

# Protocol Buffers are a way of encoding structured data in an efficient yet extensible format. 
# Google uses Protocol Buffers for almost all of its internal RPC protocols and file formats. 
libprotobuf:
	rm -rf $(BUILD)/libprotobuf
	mkdir -p $(BUILD)/libprotobuf
	cd $(BUILD)/libprotobuf && PATH=$(SDK)/usr/bin:$(PATH) CC=$(CC) CXX=$(CXX) CFLAGS=$(CFLAGS) CXXFLAGS=$(CXXFLAGS) $(SRCROOT)/$(DEPENDENCY_LIBPROTOBUF)/configure \
		--prefix=$(SDK)/usr --build=$(BUILD_TRIPLE) --host=$(TRIPLE) --target=$(TRIPLE) --disable-shared
	cd $(BUILD)/libprotobuf && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install

# Physics FS
libphysfs:
	rm -rf $(BUILD)/libphysfs
	mkdir -p $(BUILD)/libphysfs
	cd $(BUILD)/libphysfs && PATH=$(SDK)/usr/bin:$(PATH) $(SDK_CMAKE) -G "Unix Makefiles" \
		$(SRCROOT)/$(DEPENDENCY_LIBPHYSFS) -DCMAKE_INSTALL_PREFIX="$(SDK)/usr" \
		-DPHYSFS_BUILD_TEST=0 -DPHYSFS_HAVE_THREAD_SUPPORT=0 -DPHYSFS_HAVE_CDROM_SUPPORT=0 -DPHYSFS_BUILD_STATIC=1 -DPHYSFS_BUILD_SHARED=0 -DOTHER_LDFLAGS=-lz -DCMAKE_INCLUDE_PATH="$(SDK)/usr/include" \
		-DCMAKE_LIBRARY_PATH="$(SDK)/usr/lib"
	cd $(BUILD)/libphysfs && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install

# ====================================================================================
# Submit tests
# ====================================================================================

# Test HelloWorld.C
test_hello_c:
	# Cleaning test folder
	@rm -rf $(BUILD)/test_hello_c
	@mkdir -p $(BUILD)/test_hello_c
	# Assembling BitCode Output (BC)
	cd $(BUILD)/test_hello_c && $(SDK_CC) -c -g -O0 $(SRCROOT)/test/hello.c -emit-llvm -o hello.bc
	# Assembling ABC Output (OBJ)
	cd $(BUILD)/test_hello_c && $(SDK)/usr/bin/llc -jvm="$(JAVA)" hello.bc -o hello.abc -filetype=obj
	# Assembling AS3 Output (ASM)
	cd $(BUILD)/test_hello_c && $(SDK)/usr/bin/llc -jvm="$(JAVA)" hello.bc -o hello.as -filetype=asm
	# Assembling SWF Output
	cd $(BUILD)/test_hello_c && $(SDK_CC) -emit-swf -swf-size=320x240 -O0 -g hello.abc -o hello.swf
	# Assembling SWF Output (Optimized)
	cd $(BUILD)/test_hello_c && $(SDK_CC) -emit-swf -swf-size=320x240 -O4 $(SRCROOT)/test/hello.c -o hello-opt.swf
	# Assembling SWF Output (Use AS3 Assembly instead of intrinsics - Does not work)
	# cd $(BUILD)/test_hello_c && $(SDK_CC) -emit-swf -swf-size=320x240 -O4 -muse-legacy-asc $(SRCROOT)/test/hello.c -o hello-muse.swf

# Test HelloWorld.CPP
test_hello_cpp:
	# Cleaning test folder
	@rm -rf $(BUILD)/test_hello_cpp
	@mkdir -p $(BUILD)/test_hello_cpp
	# Assembling Native Output
	cd $(BUILD)/test_hello_cpp && $(SDK_CXX) -g -O0 $(SRCROOT)/test/hello.cpp -o hello-cpp && ./hello-cpp
	# Assembling SWF Output
	cd $(BUILD)/test_hello_cpp && $(SDK_CXX) -emit-swf -swf-size=320x240 -O0 $(SRCROOT)/test/hello.cpp -o hello-cpp.swf
	# Assembling SWF Output (Optimized)
	cd $(BUILD)/test_hello_cpp && $(SDK_CXX) -emit-swf -swf-size=320x240 -O4 $(SRCROOT)/test/hello.cpp -o hello-cpp-opt.swf

# Test POSIX Threads - C
test_pthreads_c_shell:
	# Cleaning test folder
	@rm -rf $(BUILD)/test_pthreads_c_shell
	@mkdir -p $(BUILD)/test_pthreads_c_shell
	# Assembling SWF Output
	cd $(BUILD)/test_pthreads_c_shell && $(SDK_CC) -O0 -pthread -save-temps $(SRCROOT)/test/pthread_test.c -o pthread_test
	# Assembling SWF Output (Optimized)
	cd $(BUILD)/test_pthreads_c_shell && $(SDK_CC) -O4 -pthread -save-temps $(SRCROOT)/test/pthread_test.c -o pthread_test_optimized
	# Running Output
	cd $(BUILD)/test_pthreads_c_shell && ./pthread_test &> $(BUILD)/test_pthreads_c_shell/pthread_test.txt
	# Running Output (Optimized)
	cd $(BUILD)/test_pthreads_c_shell && ./pthread_test_optimized &> $(BUILD)/test_pthreads_c_shell/pthread_test_optimized.txt

# Test POSIX Threads - C
test_pthreads_c_swf:
	# Cleaning test folder
	@rm -rf $(BUILD)/test_pthreads_c_swf
	@mkdir -p $(BUILD)/test_pthreads_c_swf
	# Assembling SWC
	cd $(BUILD)/test_pthreads_c_swf && $(SDK_CC) -O4 -pthread -save-temps $(SRCROOT)/test/pthread_test.c -emit-swc=com.adobe.flascc -o pthread_test_optimized.swc
	# Assembling SWF
	cd $(BUILD)/test_pthreads_c_swf && $(SDK_CC) -O0 -pthread -emit-swf -save-temps $(SRCROOT)/test/pthread_test.c -o pthread_test.swf
	# Assembling SWF (Optimized)
	cd $(BUILD)/test_pthreads_c_swf && $(SDK_CC) -O4 -pthread -emit-swf -save-temps $(SRCROOT)/test/pthread_test.c -o pthread_test_optimized.swf
	# Assembling SWFs (Optimized)
	cd $(BUILD)/test_pthreads_c_swf && $(SDK_CC) -O4 -pthread -emit-swf $(SRCROOT)/test/pthread_cancel.c -o pthread_cancel.swf
	cd $(BUILD)/test_pthreads_c_swf && $(SDK_CC) -O4 -pthread -emit-swf $(SRCROOT)/test/pthread_async_cancel.c -o pthread_async_cancel.swf
	cd $(BUILD)/test_pthreads_c_swf && $(SDK_CC) -O4 -pthread -emit-swf $(SRCROOT)/test/pthread_create.c -o pthread_create.swf
	cd $(BUILD)/test_pthreads_c_swf && $(SDK_CC) -O4 -pthread -emit-swf $(SRCROOT)/test/pthread_create_test.c -o pthread_create_test.swf
	cd $(BUILD)/test_pthreads_c_swf && $(SDK_CC) -O4 -pthread -emit-swf $(SRCROOT)/test/pthread_mutex_test.c -o pthread_mutex_test.swf
	cd $(BUILD)/test_pthreads_c_swf && $(SDK_CC) -O4 -pthread -emit-swf $(SRCROOT)/test/pthread_mutex_test2.c -o pthread_mutex_test2.swf
	cd $(BUILD)/test_pthreads_c_swf && $(SDK_CC) -O4 -pthread -emit-swf $(SRCROOT)/test/pthread_malloc_test.c -o pthread_malloc_test.swf
	cd $(BUILD)/test_pthreads_c_swf && $(SDK_CC) -O4 -pthread -emit-swf $(SRCROOT)/test/pthread_specific.c -o pthread_specific.swf
	cd $(BUILD)/test_pthreads_c_swf && $(SDK_CC) -O4 -pthread -emit-swf $(SRCROOT)/test/pthread_suspend.c -o pthread_suspend.swf
	cd $(BUILD)/test_pthreads_c_swf && $(SDK_CC) -O4 -pthread -emit-swf $(SRCROOT)/test/thr_kill.c -o thr_kill.swf
	cd $(BUILD)/test_pthreads_c_swf && $(SDK_CC) -O4 -pthread -emit-swf $(SRCROOT)/test/peterson.c -o peterson.swf
	cd $(BUILD)/test_pthreads_c_swf && $(SDK_CC) -O4 -pthread -emit-swf -DORDER_STRENGTH=1 $(SRCROOT)/test/peterson.c -o peterson_nofence.swf
	cd $(BUILD)/test_pthreads_c_swf && $(SDK_CC) -O4 -emit-swf $(SRCROOT)/test/newThread.c -o newThread.swf
	cd $(BUILD)/test_pthreads_c_swf && $(SDK_CC) -O4 -pthread -emit-swf $(SRCROOT)/test/avm2_conc.c -o avm2_conc.swf
	cd $(BUILD)/test_pthreads_c_swf && $(SDK_CC) -O4 -pthread -emit-swf $(SRCROOT)/test/avm2_mutex.c -o avm2_mutex.swf
	cd $(BUILD)/test_pthreads_c_swf && $(SDK_CC) -O4 -pthread -emit-swf $(SRCROOT)/test/avm2_mutex2.c -o avm2_mutex2.swf

# Test POSIX Threads - CPP
test_pthreads_cpp_swf:
	# Cleaning test folder
	@rm -rf $(BUILD)/test_pthreads_cpp_swf
	@mkdir -p $(BUILD)/test_pthreads_cpp_swf
	# Assembling SWFs
	cd $(BUILD)/test_pthreads_cpp_swf && $(SDK_CXX) -O4 -emit-swf -pthread $(SRCROOT)/test/AS3++mt.cpp -lAS3++ -o AS3++mt.swf
	cd $(BUILD)/test_pthreads_cpp_swf && $(SDK_CXX) -O4 -emit-swf -pthread $(SRCROOT)/test/AS3++mt1.cpp -lAS3++ -o AS3++mt1.swf
	cd $(BUILD)/test_pthreads_cpp_swf && $(SDK_CXX) -O4 -emit-swf -pthread $(SRCROOT)/test/AS3++mt2.cpp -lAS3++ -o AS3++mt2.swf
	cd $(BUILD)/test_pthreads_cpp_swf && $(SDK_CXX) -O4 -emit-swf -pthread $(SRCROOT)/test/AS3++mt3.cpp -lAS3++ -o AS3++mt3.swf

# Test POSIX VFS
test_posix:
	# Cleaning test folder
	@rm -rf $(BUILD)/test_posix
	@mkdir -p $(BUILD)/test_posix
	# Assembling VFS
	$(SDK)/usr/bin/genfs --name my.test.BackingStore $(SRCROOT)/test/zipfsroot $(BUILD)/test_posix/alcfs
	# Assembling ABC
	cd $(BUILD)/test_posix && $(SCOMPFALCON) \
		-import $(call nativepath,$(SDK)/usr/lib/BinaryData.abc) \
		-import $(call nativepath,$(SDK)/usr/lib/playerglobal.abc) \
		-import $(call nativepath,$(SDK)/usr/lib/ISpecialFile.abc) \
		-import $(call nativepath,$(SDK)/usr/lib/IBackingStore.abc) \
		-import $(call nativepath,$(SDK)/usr/lib/IVFS.abc) \
		-import $(call nativepath,$(SDK)/usr/lib/AlcVFSZip.abc) \
		-import $(call nativepath,$(SDK)/usr/lib/InMemoryBackingStore.abc) \
		-import $(call nativepath,$(SDK)/usr/lib/PlayerKernel.abc) \
		$(call nativepath, $(BUILD)/test_posix/alcfsBackingStore.as) -outdir . -out alcfs
	# Assembling SWF
	cd $(BUILD)/test_posix && $(SDK_CC) -emit-swf -O0 -swf-version=15 $(call nativepath,$(SDK)/usr/lib/AlcVFSZip.abc) alcfs.abc $(SRCROOT)/test/fileio.c -o posixtest.swf

# Test with SciMark
test_scimark_shell:
	# Cleaning test folder
	@rm -rf $(BUILD)/test_scimark_shell
	@mkdir -p $(BUILD)/test_scimark_shell
	# Assembling Native
	cd $(BUILD)/test_scimark_shell && $(SDK_CC) -O4 $(SRCROOT)/scimark2_1c/*.c -o scimark2 -save-temps
	# Running Native
	$(BUILD)/test_scimark_shell/scimark2 &> $(BUILD)/test_scimark_shell/result.txt

# Test with SciMark SWF
test_scimark_swf:
	# Cleaning test folder
	@rm -rf $(BUILD)/test_scimark_swf
	@mkdir -p $(BUILD)/test_scimark_swf
	# Assembling SWFs
	cd $(BUILD)/test_scimark_swf && $(SDK_CC) -O4 -swf-version=17 $(SRCROOT)/scimark2_1c/*.c -emit-swf -swf-size=400x400 -o scimark2-SWF17.swf
	cd $(BUILD)/test_scimark_swf && $(SDK_CC) -O4 $(SRCROOT)/scimark2_1c/*.c -emit-swf -swf-size=400x400 -o scimark2.swf
	cd $(BUILD)/test_scimark_swf && $(SDK_CC) -O4 $(SRCROOT)/scimark2_1c/*.c -emit-swf -swf-size=400x400 -o scimark2v18.swf

# TBD
test_sjlj:
	# Cleaning test folder
	@rm -rf $(BUILD)/test_sjlj
	@mkdir -p $(BUILD)/test_sjlj
	# Assembling Native
	cd $(BUILD)/test_sjlj && $(SDK_CXX) -O0 $(SRCROOT)/test/sjljtest.c -v -o sjljtest -save-temps
	# Running Native
	$(BUILD)/test_sjlj/sjljtest &> $(BUILD)/test_sjlj/result.txt
	diff --strip-trailing-cr $(BUILD)/test_sjlj/result.txt $(SRCROOT)/test/sjljtest.expected.txt

# TBD
test_sjlj_opt:
	# Cleaning test folder
	@rm -rf $(BUILD)/test_sjlj_opt
	@mkdir -p $(BUILD)/test_sjlj_opt
	# Assembling Native
	cd $(BUILD)/test_sjlj_opt && $(SDK_CXX) -O4 $(SRCROOT)/test/sjljtest.c -o sjljtest -save-temps
	# Running Native
	$(BUILD)/test_sjlj_opt/sjljtest &> $(BUILD)/test_sjlj_opt/result.txt
	diff --strip-trailing-cr $(BUILD)/test_sjlj_opt/result.txt $(SRCROOT)/test/sjljtest.expected.txt

# TBD
test_eh:
	# Cleaning test folder
	@rm -rf $(BUILD)/test_eh
	@mkdir -p $(BUILD)/test_eh
	# Assembling Native
	cd $(BUILD)/test_eh && $(SDK_CXX) -O0 $(SRCROOT)/test/ehtest.cpp -o ehtest -save-temps
	# Running Native
	-$(BUILD)/test_eh/ehtest &> $(BUILD)/test_eh/result.txt
	diff --strip-trailing-cr $(BUILD)/test_eh/result.txt $(SRCROOT)/test/ehtest.expected.txt

# TBD
test_eh_opt:
	# Cleaning test folder
	@rm -rf $(BUILD)/test_eh_opt
	@mkdir -p $(BUILD)/test_eh_opt
	# Assembling Native
	cd $(BUILD)/test_eh_opt && $(SDK_CXX) -O4 $(SRCROOT)/test/ehtest.cpp -o ehtest -save-temps
	# Running Native
	-$(BUILD)/test_eh_opt/ehtest &> $(BUILD)/test_eh_opt/result.txt
	diff --strip-trailing-cr $(BUILD)/test_eh_opt/result.txt $(SRCROOT)/test/ehtest.expected.txt

# TBD
test_as3interop:
	# Cleaning test folder
	@rm -rf $(BUILD)/test_as3interop
	@mkdir -p $(BUILD)/test_as3interop
	# Assembling Native
	cd $(BUILD)/test_as3interop && $(SDK_CXX) -O4 $(SRCROOT)/test/as3interoptest.c -o as3interoptest -save-temps
	# Running Native
	$(BUILD)/test_as3interop/as3interoptest &> $(BUILD)/test_as3interop/result.txt

# Tests Listing of Symbols from ASM and ABC Object Formats
# 'llvm-as': Reads from human readable LLVM assembly language, translates it to LLVM byte-code
# 'llc': Compiles LLVM byte-code into assembly language
# 'nm': Lists the symbols from object files
test_symbols:
	# Cleaning test folder
	@rm -rf $(BUILD)/test_symbols
	mkdir -p $(BUILD)/test_symbols
	# Assembling Native
	cd $(BUILD)/test_symbols && $(SDK)/usr/bin/llvm-as $(SRCROOT)/test/symboltest.ll -o symboltest.bc
	cd $(BUILD)/test_symbols && $(SDK)/usr/bin/llc -jvm=$(JAVA) symboltest.bc -filetype=asm -o symboltest.s
	cd $(BUILD)/test_symbols && $(SDK)/usr/bin/llc -jvm=$(JAVA) symboltest.bc -filetype=obj -o symboltest.abc
	cd $(BUILD)/test_symbols && $(SDK_NM) symboltest.abc | grep symbolTest > syms.abc.txt
	cd $(BUILD)/test_symbols && $(SDK_NM) symboltest.bc | grep symbolTest > syms.bc.txt
	# Generating Result
	diff --strip-trailing-cr $(BUILD)/test_symbols/*.txt

# Run GDB tests
#TODO: implement auto run gdb with commands on SWFs (SWF17 and SWF18 both)
$?GDBUNIT_ROOT=$(SRCROOT)/test/gdbunit/media/swfs
test_gdb:
	cd $(GDBUNIT_ROOT) && make -f ./ALC58.make SWFS_DIR=$(GDBUNIT_ROOT) FLASCC_DIR=$(SRCROOT)
	cd $(GDBUNIT_ROOT) && make -f ./as3Commands.make SWFS_DIR=$(GDBUNIT_ROOT) FLASCC_DIR=$(SRCROOT)
	cd $(GDBUNIT_ROOT) && make -f ./call.make SWFS_DIR=$(GDBUNIT_ROOT) FLASCC_DIR=$(SRCROOT)
	cd $(GDBUNIT_ROOT) && make -f ./flashPlusPlus.make SWFS_DIR=$(GDBUNIT_ROOT) FLASCC_DIR=$(SRCROOT)
	cd $(GDBUNIT_ROOT) && make -f ./hellointerop.make SWFS_DIR=$(GDBUNIT_ROOT) FLASCC_DIR=$(SRCROOT)
	cd $(GDBUNIT_ROOT) && make -f ./helloWorld.make SWFS_DIR=$(GDBUNIT_ROOT) FLASCC_DIR=$(SRCROOT)
	cd $(GDBUNIT_ROOT) && make -f ./helloWorldCPP.make SWFS_DIR=$(GDBUNIT_ROOT) FLASCC_DIR=$(SRCROOT)
	#cd $(GDBUNIT_ROOT) && make -f ./loadMultipleSWFsMain.make SWFS_DIR=$(GDBUNIT_ROOT) FLASCC_DIR=$(SRCROOT)
	cd $(GDBUNIT_ROOT) && make -f ./mixeddebug.make SWFS_DIR=$(GDBUNIT_ROOT) FLASCC_DIR=$(SRCROOT)
	cd $(GDBUNIT_ROOT) && make -f ./multifile.make SWFS_DIR=$(GDBUNIT_ROOT) FLASCC_DIR=$(SRCROOT)
	#cd $(GDBUNIT_ROOT) && make -f ./multipleSWC.make SWFS_DIR=$(GDBUNIT_ROOT) FLASCC_DIR=$(SRCROOT)
	cd $(GDBUNIT_ROOT) && make -f ./nestedFunctions.make SWFS_DIR=$(GDBUNIT_ROOT) FLASCC_DIR=$(SRCROOT)
	cd $(GDBUNIT_ROOT) && make -f ./printfLoop.make SWFS_DIR=$(GDBUNIT_ROOT) FLASCC_DIR=$(SRCROOT)
	#cd $(GDBUNIT_ROOT) && make -f ./Quake1.make SWFS_DIR=$(GDBUNIT_ROOT) FLASCC_DIR=$(SRCROOT)
	cd $(GDBUNIT_ROOT) && make -f ./setjmp.make SWFS_DIR=$(GDBUNIT_ROOT) FLASCC_DIR=$(SRCROOT)


# Run Virtual File System (VFS) tests
test_vfs:
	@cd qa/vfs/framework && $(MAKE) FLASCC=$(FLASCC)

# ====================================================================================
# Samples and Examples
# ====================================================================================

# Samples shipped with the SDK
samples:
	cd samples && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) UNAME=$(UNAME) FLASCC=$(SDK) FLEX=$(FLEX_SDK_HOME) -j$(THREADS)
	#find samples -iname "*.swf" -exec cp -f '{}' $(BUILDROOT)/extra/ \;

# Examples used to test the SDK
examples:
	cd samples && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) UNAME=$(UNAME) FLASCC=$(SDK) FLEX=$(FLEX_SDK_HOME) -j$(THREADS) examples
	#find samples -iname "*.swf" -exec cp -f '{}' $(BUILDROOT)/extra/ \;

# ====================================================================================
# Extra Tests
# ====================================================================================

# Check headers for ASM
checkasm:
	rm -rf $(BUILD)/libtoabc
	@mkdir -p $(BUILD)/logs/libtoabc
	@libs=`find $(SDK) -name "*.a"`; \
	omittedlibs="libjpeg.a\nlibpng\nlibz.a" ; \
	omittedlibs=`echo $$omittedlibs` ; \
	libs=`echo "$$libs" | grep -F -v "$$omittedlibs"` ; \
	echo "Compiling SDK libraries to ABC" ; \
	for lib in $$libs ; do \
		shortlib=`basename $$lib` ; \
		echo "- checking $$lib" ; \
		$(MAKE) libtoabc LIB=$$lib &> $(BUILD)/logs/libtoabc/$$shortlib.txt ; \
		mret=$$? ; \
		if [ $$mret -ne 0 ] ; then \
		echo "Failed to build abc: $$lib" ;\
		cat $(BUILD)/logs/libtoabc/$$shortlib.txt ;\
		exit 1 ; \
		fi ; \
	done
	@echo "Checking headers for asm"
	$(PYTHON) $(SRCROOT)/tools/search_headers.py $(SDK) $(BUILD)/header-search

# Helper target for 'checkasm'
libtoabc:
	mkdir -p $(BUILD)/libtoabc/`basename $(LIB)`
	cd $(BUILD)/libtoabc/`basename $(LIB)` && $(SDK_AR) x $(LIB)
	@abcdir=$(BUILD)/libtoabc/`basename $(LIB)` ; \
	numos=`find $$abcdir -maxdepth 1 -name '*.o' | wc -l` ; \
	if [$$numos -gt 0 ] ; then \
	cd $(BUILD)/libtoabc/`basename $(LIB)` && cp -f $(SRCROOT)/avm2_env/misc/abcarchive.mk Makefile && SDK=$(SDK) $(MAKE) LLCOPTS=-jvm="$(JAVA)" -j$(THREADS) ; \
	fi 

# Deprecated test (Source missing)
speccpu2006: # works on mac only! (and probably requires local tweaks to alchemy.cfg and mac32.cfg)
	@rm -rf $(BUILD)/speccpu2006
	@mkdir -p $(BUILD)/speccpu2006
	cd $(BUILD)/speccpu2006 && curl http://alchemy.corp.adobe.com/speccpu2006.tar.bz2 | tar xvf -
	cd $(BUILD)/speccpu2006/speccpu2006 && cat $(SRCROOT)/test/speccpu2006/install.sh.ed | ed install.sh # build install2.sh w/ hardcoded arch=mac32-x86
	cd $(BUILD)/speccpu2006/speccpu2006 && chmod +x install2.sh && chmod +x tools/bin/macosx-x86/spec* && chmod +w MANIFEST && echo y | SPEC= ./install2.sh
	cd $(BUILD)/speccpu2006/speccpu2006 && cp $(SRCROOT)/test/speccpu2006/*.cfg config/. && chmod +w config/*.cfg
	cd $(BUILD)/speccpu2006/speccpu2006 && (source shrc && time runspec --config=alchemy.cfg --tune=base --loose --action build int fp | tee alchemy.build.log)
	cd $(BUILD)/speccpu2006/speccpu2006 && (source shrc && time runspec --config=mac32.cfg --tune=base --loose --action build int fp | tee mac32.build.log)
	cd $(BUILD)/speccpu2006/speccpu2006 && (source shrc && time runspec --config=alchemy.cfg --tune=base --loose --action validate int fp | tee -a alchemy.run.log)
	cd $(BUILD)/speccpu2006/speccpu2006 && (source shrc && time runspec --config=mac32.cfg --tune=base --loose --action validate int fp | tee -a mac32.run.log)

# Helper lib for GCC tests
dejagnu:
	mkdir -p $(BUILD)/dejagnu
	cd $(BUILD)/dejagnu && $(SRCROOT)/dejagnu-1.5/configure --prefix=$(BUILD)/dejagnu && $(MAKE) install

RUNGCCTESTS=mkdir -p $(BUILD)/gcctests/$@ && cd $(BUILD)/gcctests/$@ && LD_LIBRARY_PATH="/" PATH="$(SDK)/usr/bin:$(PATH)" $(BUILD)/dejagnu/bin/runtest --all --srcdir $(SRCROOT)/llvm-gcc-4.2-2.9/gcc/testsuite --target_board=$(TRIPLE)

CTORTUREDIRS= \
compat \
compile \
execute \
unsorted

GCCTESTDIRS= \
g++.apple \
g++.dg \
g++.old-deja \
gcc.apple \
gcc.dg \
gcc.misc-tests \
gcc.target \
gcc.test-framework \
llvm.obj-c++ \
llvm.objc \
obj-c++.dg \
objc \
objc.dg

gcctorture/%:
	-$(RUNGCCTESTS) --tool gcc --directory $(SRCROOT)/llvm-gcc-4.2-2.9/gcc/testsuite/gcc.c-torture $(@:gcctorture/%=%).exp

gxxtorture/%:
	-$(RUNGCCTESTS) --tool g++ --directory $(SRCROOT)/llvm-gcc-4.2-2.9/gcc/testsuite/gcc.c-torture $(@:gxxtorture/%=%).exp

gccrun/%:
	-$(RUNGCCTESTS) --tool gcc --directory $(SRCROOT)/llvm-gcc-4.2-2.9/gcc/testsuite/$(@:gccrun/%=%)

gxxrun/%:
	-$(RUNGCCTESTS) --tool g++ --directory $(SRCROOT)/llvm-gcc-4.2-2.9/gcc/testsuite/$(@:gxxrun/%=%)

# TBD
gcctests:
	$(MAKE) dejagnu
	cp -f $(SRCROOT)/tools/$(TRIPLE).exp $(BUILD)/dejagnu/share/dejagnu/baseboards/
	chmod u+rw $(BUILD)/dejagnu/share/dejagnu/baseboards/*
	$(MAKE) -j$(THREADS) allgcctests

# TBD
allgcctests: $(CTORTUREDIRS:%=gcctorture/%) $(CTORTUREDIRS:%=gxxtorture/%) $(GCCTESTDIRS:%=gccrun/%) $(GCCTESTDIRS:%=gxxrun/%)
	cat $(BUILD)/gcctests/*/*/gcc.log  > $(BUILD)/gcctests/gcc.log
	cat $(BUILD)/gcctests/*/*/g++.log  > $(BUILD)/gcctests/g++.log

# TBD
ieeetests_conversion:
	rm -rf $(BUILD)/ieeetests_conversion
	mkdir -p $(BUILD)/ieeetests_conversion
	$(RSYNC) $(SRCROOT)/test/IeeeCC754/ $(BUILD)/ieeetests_conversion
	echo "b\nb\na" > $(BUILD)/ieeetests_conversion/answers
	cd $(BUILD)/ieeetests_conversion && PATH=$(SDK)/usr/bin:$(PATH) ./dotests.sh < answers

# TBD
ieeetests_basicops:
	rm -rf $(BUILD)/ieeetests_basicops
	mkdir -p $(BUILD)/ieeetests_basicops
	$(RSYNC) $(SRCROOT)/test/IeeeCC754/ $(BUILD)/ieeetests_basicops
	echo "a\nb\na" > $(BUILD)/ieeetests_basicops/answers
	cd $(BUILD)/ieeetests_basicops && PATH=$(SDK)/usr/bin:$(PATH) ./dotests.sh < answers

# ====================================================================================
# DEPLOY
# ====================================================================================
# Deploy SDK 
deploy:
	$(MAKE) deliverables

# Deploy SDK
deliverables:
	$(MAKE) staging
	$(MAKE) flattensymlinks
ifneq (,$(findstring cygwin,$(PLATFORM)))
		$(MAKE) zip
else
		$(MAKE) dmg
		$(MAKE) staging
		$(MAKE) winstaging
		$(MAKE) flattensymlinks
		$(MAKE) zip
#		$(MAKE) diffdeliverables
endif

# Staging
staging:
	rm -rf $(BUILDROOT)/staging
	mkdir -p $(BUILDROOT)/staging
	$(RSYNC) $(SDK) $(BUILDROOT)/staging/
	$(RSYNC) --exclude '*.PAK' --exclude '*.pak' $(SRCROOT)/samples $(BUILDROOT)/staging/
	$(RSYNC) $(SRCROOT)/README.html $(BUILDROOT)/staging/
	$(RSYNC) $(SRCROOT)/docs $(BUILDROOT)/staging/
	$(RSYNC) $(BUILDROOT)/apidocs $(BUILDROOT)/staging/docs/
	rm -f $(BUILDROOT)/staging/sdk/usr/bin/gccbug*
	find $(BUILDROOT)/staging/ | grep "\.DS_Store$$" | xargs rm -f 
	echo $(FLASCC_VERSION_BUILD) > $(BUILDROOT)/staging/sdk/ver.txt

# Flatten Symbolic Links
flattensymlinks:
	find $(BUILDROOT)/staging/sdk -type l | xargs rm
	$(RSYNC) $(BUILDROOT)/staging/sdk/usr/platform/*/ $(BUILDROOT)/staging/sdk/usr
	rm -rf $(BUILDROOT)/staging/sdk/usr/platform

# Assemble ZIP
zip:
	cd $(BUILDROOT)/staging/ && zip -qr $(BUILDROOT)/$(SDKNAME).zip *
	find $(BUILDROOT)/staging > $(BUILDROOT)/zipcontents.txt

# Assemble DMG
dmg:
	mkdir -p $(BUILDROOT)/dmgmount
	rm -f $(BUILDROOT)/$(SDKNAME).dmg $(BUILDROOT)/$(SDKNAME)-tmp.dmg
	cp -f $(SRCROOT)/tools/Base.dmg $(BUILDROOT)/$(SDKNAME)-tmp.dmg
	chmod u+rw $(BUILDROOT)/$(SDKNAME)-tmp.dmg
	hdiutil resize -size 1G $(BUILDROOT)/$(SDKNAME)-tmp.dmg
	hdiutil attach $(BUILDROOT)/$(SDKNAME)-tmp.dmg -readwrite -mountpoint $(BUILDROOT)/dmgmount
	rm -f $(BUILDROOT)/staging/.DS_Store
	$(RSYNC) $(BUILDROOT)/staging/ $(BUILDROOT)/dmgmount/
	mv $(BUILDROOT)/dmgmount/.fseventsd $(BUILDROOT)/
	hdiutil detach $(BUILDROOT)/dmgmount
	hdiutil convert $(BUILDROOT)/$(SDKNAME)-tmp.dmg -format UDZO -imagekey zlib-level=9 -o $(BUILDROOT)/$(SDKNAME).dmg
	rm -f $(BUILDROOT)/$(SDKNAME)-tmp.dmg
	find $(BUILDROOT)/staging > $(BUILDROOT)/dmgcontents.txt

# Cross-Deploy Windows Staging
winstaging:
	$(LN) cygwin $(BUILDROOT)/staging/sdk/usr/platform/current
	# temporarily $(MAKE) sdk cygwin-ish
	$(LN) cygwin $(SDK)/usr/platform/current
	mkdir -p $(SDK)/usr/platform/cygwin/bin
	$(MAKE) libsdl_install
	# copy some parts of the mac bin dir which are actually xplatform
	cp -f $(BUILDROOT)/staging/sdk/usr/platform/darwin/bin/libtool* $(SDK)/usr/platform/cygwin/bin/
	cp -f $(BUILDROOT)/staging/sdk/usr/platform/darwin/bin/libpng* $(SDK)/usr/platform/cygwin/bin/
	# run post cleanup processes
	$(MAKE) sdkcleanup
	$(MAKE) finalcleanup
	@rm -rf $(SDK)/usr/platform/darwin/share
	$(RSYNC) $(SRCROOT)/tools/run.bat $(BUILDROOT)/staging/
	$(RSYNC) $(SRCROOT)/cygwin $(BUILDROOT)/staging/
	$(RSYNC) $(SDK) $(BUILDROOT)/staging/
	rm -rf $(BUILDROOT)/staging/sdk/usr/libexec
	rm -rf $(BUILDROOT)/staging/sdk/usr/share/$(DEPENDENCY_CMAKE)
	$(RSYNC) $(WIN_BUILD)/sdkoverlay/usr/platform/cygwin $(BUILDROOT)/staging/sdk/usr/platform/
	$(RSYNC) $(WIN_BUILD)/sdkoverlay/usr/lib/*.dll $(BUILDROOT)/staging/sdk/usr/lib/
	$(RSYNC) $(WIN_BUILD)/sdkoverlay/usr/lib/bfd-plugins/*.dll $(BUILDROOT)/staging/sdk/usr/lib/bfd-plugins/
	rm -rf $(BUILDROOT)/staging/sdk/usr/platform/darwin
	rm -f $(BUILDROOT)/staging/sdk/usr/lib/*.dylib
	rm -f $(BUILDROOT)/staging/sdk/usr/lib/*.la
	rm -f $(BUILDROOT)/staging/sdk/usr/lib/bfd-plugins/*.dylib
	$(LN) darwin $(SDK)/usr/platform/current
	# nuke cygwin from sdk
	rm -rf $(SDK)/usr/platform/cygwin
	find $(BUILDROOT)/staging/ | grep "\.DS_Store$$" | xargs rm -f 

# Cross-Deploy SDK (OSX-Windows)
diffdeliverables:
		cat $(BUILDROOT)/zipcontents.txt | grep -v staging/cygwin | grep -v "run.bat" | sed -e 's/\.exe//g' -e 's/\.dll/\.~SO~/g' -e 's/\/cygwin/\/~PLAT~/g' | sort > $(BUILDROOT)/zipcontents_munge.txt
		cat $(BUILDROOT)/dmgcontents.txt | grep -v "share/cmake" | grep -v "bin/cmake" | grep -v "bin/ctest" | grep -v "bin/cpack" | grep -v "bin/ccmake" | sed -e 's/\.exe//g' -e 's/\.dylib/\.~SO~/g' -e 's/\/darwin/\/~PLAT~/g' | sort > $(BUILDROOT)/dmgcontents_munge.txt	
		diff $(BUILDROOT)/dmgcontents_munge.txt $(BUILDROOT)/zipcontents_munge.txt

# ====================================================================================
# CROSS
# ====================================================================================

# TBD
cxxfiltmingw:
	# install the version of mingw for osx from here: http://crossgcc.rts-software.org/doku.php
	rm -rf $(BUILD)/cxxfiltmingw
	mkdir -p $(BUILD)/cxxfiltmingw
	cd $(BUILD)/cxxfiltmingw && CC=$(SRCROOT)/mingwmac/sdk/usr/bin/$(MINGWTRIPLE)-gcc \
		AR=$(SRCROOT)/mingwmac/sdk/usr/bin/$(MINGWTRIPLE)-ar  CXX=$(SRCROOT)/mingwmac/sdk/usr/bin/$(MINGWTRIPLE)-g++ \
		CFLAGS="-I$(SRCROOT)/avm2_env/misc/ $(DBGOPTS) -DMINGW_MONOCLE_HACKS " \
		CXXFLAGS="-I$(SRCROOT)/avm2_env/misc/ $(DBGOPTS) -DMINGW_MONOCLE_HACKS " $(SRCROOT)/$(DEPENDENCY_BINUTILS)/configure \
		--disable-doc --disable-nls --disable-gold --disable-ld --disable-plugins \
		--build=$(BUILD_TRIPLE) --host=$(MINGWTRIPLE) --target=$(MINGWTRIPLE) \
		--program-prefix="" --disable-werror \
		--enable-targets=$(TRIPLE)
	-cd $(BUILD)/cxxfiltmingw && $(MAKE) -j$(THREADS)
	-cd $(BUILD)/cxxfiltmingw && $(SRCROOT)/mingwmac/sdk/usr/bin/$(MINGWTRIPLE)-gcc -DMINGW_MONOCLE_HACKS  \
		-I$(BUILD)/cxxfiltmingw/binutils -I$(BUILD)/cxxfiltmingw/bfd -I$(SRCROOT)/$(DEPENDENCY_BINUTILS)/include  \
		-I$(BUILD)/cxxfiltmingw/intl $(SRCROOT)/$(DEPENDENCY_BINUTILS)/binutils/cxxfilt.c   \
		$(BUILD)/cxxfiltmingw/libiberty/*.o $(BUILD)/cxxfiltmingw/intl/*.o $(BUILD)/cxxfiltmingw/binutils/version.o \
		$(BUILD)/cxxfiltmingw/binutils/bucomm.o  $(BUILD)/cxxfiltmingw/bfd/*.o -o c++filt.exe

# TBD
win:
	@if [ -d $(CYGWINMAC) ] ; then true ; \
		else echo "Couldn't locate cygwin mac directory, please invoke $(MAKE) with \"$(MAKE) CYGWINMAC=/path/to/cygwinmac/sdk/usr/bin ...\"" ; exit 1; \
	fi
	@mkdir -p $(WIN_BUILD)/logs
	@echo "-  base (win)"
	@$(CROSS) base  &> $(WIN_BUILD)/logs/base_win.txt
	@echo "-  make (win)"
	@$(CROSS) make &> $(WIN_BUILD)/logs/make_win.txt
	@echo "-  uname (win)"
	@$(CROSS) uname  &> $(WIN_BUILD)/logs/uname_win.txt
	@echo "-  noenv (win)"
	@$(CROSS) noenv &> $(WIN_BUILD)/logs/noenv_win.txt
	@echo "-  avm2-as (win)"
	@$(CROSS) avm2-as &> $(WIN_BUILD)/logs/avm2-as_win.txt
	@echo "-  pkgconfig (win)"
	@$(CROSS) GLIB_CFLAGS="-I$(CYGWINMAC)/../include/glib-2.0/ -I$(CYGWINMAC)/../lib/glib-2.0/include/" GLIB_LIBS="$(CYGWINMAC)/../lib/libglib-2.0.a -lintl -liconv" pkgconfig -j1 &> $(WIN_BUILD)/logs/pkgconfig_win.txt
	@echo "-  llvm (win/cygwin)"
	@$(MAKE) cross_llvm_cygwin &> $(WIN_BUILD)/logs/llvm_win_cygwin.txt
	@echo "-  binutils (win)"
	@$(CROSS) binutils &> $(WIN_BUILD)/logs/binutils_win.txt
	@echo "-  plugins (win)"
	@$(CROSS) plugins &> $(WIN_BUILD)/logs/plugins_win.txt
	@echo "-  gcc (win)"
	@$(CROSS) gcc &> $(WIN_BUILD)/logs/gcc_win.txt
	@echo "-  swig (win)"
	@$(CROSS) swig &> $(WIN_BUILD)/logs/swig_win.txt
	@echo "-  llvm (win/mingw)"
	@$(MAKE) cross_llvm_mingw &> $(WIN_BUILD)/logs/llvm_win_mingw.txt
	@echo "-  tr (win)"
	@$(CROSS) tr &> $(WIN_BUILD)/logs/tr_win.txt
	@echo "-  trd (win)"
	@$(CROSS) trd &> $(WIN_BUILD)/logs/trd_win.txt
	@echo "-  gdb (win)"
	@$(CROSS) gdb &> $(WIN_BUILD)/logs/gdb_win.txt
	@echo "-  genfs (win)"
	@$(CROSS) genfs &> $(WIN_BUILD)/logs/genfs_win.txt

# TBD
cross_llvm_mingw:
	echo "# Cmake Toolchain file:" > $(BUILD)/llvmcross.toolchain
	echo  "set(CMAKE_SYSTEM_NAME Windows)" >> $(BUILD)/llvmcross.toolchain
	echo  "set(CMAKE_RC_COMPILER $(MINGWTRIPLE)-gcc)" >> $(BUILD)/llvmcross.toolchain
	PATH="$(BUILD)/ccachebin:$(SRCROOT)/mingwmac/sdk/usr/bin:$(PATH)" $(MAKE) \
		SDK=$(WIN_BUILD)/sdkoverlay PLATFORM=cygwin NATIVE_AR=$(MINGWTRIPLE)-ar LLVMINSTALLPREFIX=$(WIN_BUILD) \
		CC=$(MINGWTRIPLE)-gcc CXX=$(MINGWTRIPLE)-g++ \
		LLVMCFLAGS="-march=pentium4 -mfpmath=sse -D_GLIBCXX_HAVE_FENV_H=1" \
		LLVMCXXFLAGS="-march=pentium4 -mfpmath=sse -D_GLIBCXX_HAVE_FENV_H=1" LLVMLDFLAGS="-lrpcrt4 -Wl,--stack,16000000" \
		LLVMCMAKEOPTS="-DCMAKE_TOOLCHAIN_FILE=$(BUILD)/llvmcross.toolchain -DLLVM_TABLEGEN=$(MAC_BUILD)/llvm-install/bin/tblgen" \
		llvm LLVMTESTS=ON LLVM_ONLYLLC=true CLANG=OFF

# TBD
cross_llvm_cygwin:
	echo "# Cmake Toolchain file:" > $(BUILD)/llvmcross.toolchain
	echo  "set(CMAKE_SYSTEM_NAME Windows)" >> $(BUILD)/llvmcross.toolchain
	echo  "set(CMAKE_RC_COMPILER $(CC))" >> $(BUILD)/llvmcross.toolchain
	PATH="$(BUILD)/ccachebin:$(CYGWINMAC)/../altbin:$(CYGWINMAC):$(PATH)" $(MAKE) \
		SDK=$(WIN_BUILD)/sdkoverlay PLATFORM=cygwin NATIVE_AR=$(CYGTRIPLE)-ar LLVMINSTALLPREFIX=$(WIN_BUILD) \
		CC=$(CYGTRIPLE)-gcc CXX=$(CYGTRIPLE)-g++ LLVMLDFLAGS="-Wl,--stack,16000000" \
		LLVMCMAKEOPTS="-DCMAKE_TOOLCHAIN_FILE=$(BUILD)/llvmcross.toolchain -DLLVM_TABLEGEN=$(MAC_BUILD)/llvm-install/bin/tblgen" \
		llvm LLVMTESTS=OFF

.PHONY: bmake posix binutils docs gcc samples
