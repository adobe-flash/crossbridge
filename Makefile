# ====================================================================================
# CrossBridge Makefile
# ====================================================================================

$?UNAME=$(shell uname -s)

# ====================================================================================
# DIRECTORIES
# ====================================================================================
$?SRCROOT=$(PWD)
$?SDK=$(PWD)/sdk
$?BUILDROOT=$(PWD)/build
$?WIN_BUILD=$(BUILDROOT)/win
$?MAC_BUILD=$(BUILDROOT)/mac
$?LINUX_BUILD=$(BUILDROOT)/linux
$?CYGWINMAC=$(SRCROOT)/cygwinmac/sdk/usr/bin

# ====================================================================================
# THREAD TEST CONFIG
# ====================================================================================
$?EMITSWF=
$?SWFDIR=
$?SWFEXT=

# ====================================================================================
# DEPENDENCIES
# ====================================================================================
$?DEPENDENCY_BINUTILS=binutils
$?DEPENDENCY_BMAKE=bmake
$?DEPENDENCY_CMAKE=cmake-2.8.12.2
$?DEPENDENCY_DEJAGNU=dejagnu-1.5
$?DEPENDENCY_DMALLOC=dmalloc-5.5.2
$?DEPENDENCY_FFI=libffi-3.0.11
$?DEPENDENCY_GDB=gdb-7.3
$?DEPENDENCY_ICONV=libiconv-1.13.1
$?DEPENDENCY_JPEG=jpeg-8c
$?DEPENDENCY_LIBOGG=libogg-1.3.0
$?DEPENDENCY_LIBPNG=libpng-1.5.7
$?DEPENDENCY_LIBTOOL=libtool-2.4.2
$?DEPENDENCY_LIBVORBIS=libvorbis-1.3.2
$?DEPENDENCY_LLVM=llvm-3.2
$?DEPENDENCY_LLVM_VERSION=3.2
$?DEPENDENCY_MAKE=make-4.0
$?DEPENDENCY_PKG_CFG=pkg-config-0.26
$?DEPENDENCY_SCIMARK=scimark2_1c
$?DEPENDENCY_SDL=SDL-1.2.14
$?DEPENDENCY_SWIG=swig-3.0.0
$?DEPENDENCY_SWIG_PCRE=pcre-8.20
$?DEPENDENCY_ZLIB=zlib-1.2.5

# ====================================================================================
# HOST PLATFORM OPTIONS
# ====================================================================================
ifneq (,$(findstring CYGWIN,$(UNAME)))
	$?PLATFORM="cygwin"
	$?RAWPLAT=cygwin
	ifdef NUMBER_OF_PROCESSORS
		$?THREADS=$(NUMBER_OF_PROCESSORS)
	else
		$?THREADS=2
	endif
	$?nativepath=$(shell cygpath -at mixed $(1))
	$?BUILD_TRIPLE=i686-pc-cygwin
	$?PLAYER=$(SRCROOT)/qa/runtimes/player/Debug/FlashPlayerDebugger.exe
	$?FPCMP=$(BUILDROOT)/extra/fpcmp.exe
	$?NOPIE=
	$?BIN_TRUE=/usr/bin/true
else ifneq (,$(findstring Darwin,$(UNAME)))
	$?PLATFORM="darwin"
	$?RAWPLAT=darwin
	$?THREADS=$(shell sysctl -n hw.ncpu)
	$?nativepath=$(1)
	$?BUILD_TRIPLE=x86_64-apple-darwin10
	$?PLAYER=$(SRCROOT)/qa/runtimes/player/Debug/Flash Player.app
	$?FPCMP=$(BUILDROOT)/extra/fpcmp
	$?NOPIE=-no_pie
	$?BIN_TRUE=/usr/bin/true
else
	$?PLATFORM="linux"
	$?RAWPLAT=linux
	$?THREADS=1
	$?nativepath=$(1)
	$?BUILD_TRIPLE=x86_64-unknown-linux-gnu
	$?PLAYER=$(SRCROOT)/qa/runtimes/player/Debug/Flash Player.app
	$?FPCMP=$(BUILDROOT)/extra/fpcmp
	$?NOPIE=
	$?BIN_TRUE=/bin/true
endif

# ====================================================================================
# TOOLCHAIN
# ====================================================================================
$?CC=gcc
$?CXX=g++
$?NATIVE_AR=ar
export CC:=$(CC)
export CXX:=$(CXX)
$?DBGOPTS=
$?ABCLIBOPTS=-config CONFIG::asdocs=false -config CONFIG::actual=true
$?LIBHELPEROPTFLAGS=-O3

# ====================================================================================
# TARGET PLATFORM OPTIONS
# ====================================================================================
ifneq (,$(findstring cygwin,$(PLATFORM)))
	$?EXEEXT=.exe
	$?SOEXT=.dll
	$?SDLFLAGS=
	$?TAMARIN_CONFIG_FLAGS=--target=i686-linux
	$?TAMARINLDFLAGS=" -Wl,--stack,16000000"
	$?TAMARINOPTFLAGS=-Wno-unused-function -Wno-unused-local-typedefs -Wno-maybe-uninitialized -Wno-narrowing -Wno-sizeof-pointer-memaccess -Wno-unused-variable -Wno-unused-but-set-variable -Wno-deprecated-declarations 
	$?SDKEXT=.zip
	$?BUILD=$(WIN_BUILD)
	$?PLATFORM_NAME=win
	$?HOST_TRIPLE=i686-pc-cygwin
endif

ifneq (,$(findstring darwin,$(PLATFORM)))
	$?EXEEXT=
	$?SOEXT=.dylib
	$?SDLFLAGS=--build=i686-apple-darwin9
	$?TAMARIN_CONFIG_FLAGS=
	$?TAMARINLDFLAGS=" -m32 -arch=i686"
	$?TAMARINOPTFLAGS=-Wno-deprecated-declarations 
	$?SDKEXT=.dmg
	$?BUILD=$(MAC_BUILD)
	$?PLATFORM_NAME=mac
	$?HOST_TRIPLE=x86_64-apple-darwin10
endif

ifneq (,$(findstring linux,$(PLATFORM)))
	$?EXEEXT=
	$?SOEXT=.so
	$?SDLFLAGS=--build=i686-unknown-linux
	$?TAMARIN_CONFIG_FLAGS=
	$?TAMARINLDFLAGS=" -m32 -arch=i686"
	$?TAMARINOPTFLAGS=-Wno-deprecated-declarations 
	$?SDKEXT=.dmg
	$?BUILD=$(LINUX_BUILD)
	$?PLATFORM_NAME=linux
	$?HOST_TRIPLE=x86_64-unknown-linux
endif

ESCAPED_SRCROOT=$(shell echo $(SRCROOT) | sed -e 's/[\/&]/\\&/g')
$?JAVA=$(call nativepath,$(shell which java))
$?JAVAFLAGS=
$?PYTHON=$(call nativepath,$(shell which python))
$?TAMARINCONFIG=CFLAGS=" -m32 -I$(SRCROOT)/avm2_env/misc -DVMCFG_ALCHEMY_SDK_BUILD " CXXFLAGS=" -m32 -I$(SRCROOT)/avm2_env/misc $(TAMARINOPTFLAGS) -DVMCFG_ALCHEMY_SDK_BUILD " LDFLAGS=$(TAMARINLDFLAGS) $(SRCROOT)/avmplus/configure.py --enable-shell --enable-alchemy-posix $(TAMARIN_CONFIG_FLAGS)
$?LN=ln -sfn

# ====================================================================================
# LLVM
# ====================================================================================
$?LLVMASSERTIONS=OFF
$?LLVMTESTS=OFF
$?LLVMCMAKEOPTS= 
$?LLVMLDFLAGS=
$?LLVMCXXFLAGS=
$?LLVMINSTALLPREFIX=$(BUILD)
$?LLVM_ONLYLLC=false

$?GCCLANGFLAG=
ifneq (,$(findstring 3.3svn,$(shell g++ --version)))
	$?GCCLANGFLAG+=-stdlib=libstdc++
endif
$?LLVMCXXFLAGS+=$(GCCLANGFLAG)

LLVMTARGETS=AVM2;X86
LLVMCMAKEFLAGS=-DLLVM_DEFAULT_TARGET_TRIPLE=avm2-unknown-freebsd8 \
	-DLLVM_BINUTILS_INCDIR=$(SRCROOT)/binutils/include \
	-DLLVM_BUILD_RUNTIME=OFF
#Possible values: Release, Debug, RelWithDebInfo and MinSizeRel
LLVMBUILDTYPE=MinSizeRel
FLASCC_CC=clang
FLASCC_CXX=clang++
CP_CLANG= cp $(LLVMINSTALLPREFIX)/llvm-debug/bin/clang$(EXEEXT) \
	$(SDK)/usr/bin/clang$(EXEEXT) && \
	cp $(LLVMINSTALLPREFIX)/llvm-debug/bin/clang$(EXEEXT) \
	$(SDK)/usr/bin/clang++$(EXEEXT)

$?FLEX=$(SRCROOT)/tools/flex/
$?RSYNC=rsync -az --no-p --no-g --chmod=ugo=rwX -l
$?ASDOC=$(SRCROOT)/tools/flex/bin/asdoc
$?ASC=$(call nativepath,$(SRCROOT)/avmplus/utils/asc.jar)
$?SCOMP=java $(JAVAFLAGS) -classpath $(ASC) macromedia.asc.embedding.ScriptCompiler -abcfuture -AS3 -import $(call nativepath,$(SRCROOT)/avmplus/generated/builtin.abc) -import $(call nativepath,$(SRCROOT)/avmplus/generated/shell_toplevel.abc)
$?SCOMPFALCON=java $(JAVAFLAGS) -jar $(call nativepath,$(SRCROOT)/tools/lib/asc2.jar) -merge -md -abcfuture -AS3 -import $(call nativepath,$(SRCROOT)/avmplus/generated/builtin.abc) -import $(call nativepath,$(SRCROOT)/avmplus/generated/shell_toplevel.abc)
$?CLANG=ON
$?CYGTRIPLE=i686-pc-cygwin
$?MINGWTRIPLE=i686-mingw32
$?TRIPLE=avm2-unknown-freebsd8
$?AVMSHELL=$(SDK)/usr/bin/avmshell$(EXEEXT)
$?AR=$(SDK)/usr/bin/ar scru -v
$?CMAKE=$(SDK)/usr/bin/cmake

# ====================================================================================
# VERSIONING
# ====================================================================================
$?FLASCC_VERSION_MAJOR:=1
$?FLASCC_VERSION_MINOR:=1
$?FLASCC_VERSION_PATCH:=0
$?FLASCC_VERSION_BUILD:=devbuild
$?SDKNAME=Crossbridge_$(FLASCC_VERSION_MAJOR).$(FLASCC_VERSION_MINOR).$(FLASCC_VERSION_PATCH).$(FLASCC_VERSION_BUILD)
BUILD_VER_DEFS"-DFLASCC_VERSION_MAJOR=$(FLASCC_VERSION_MAJOR) -DFLASCC_VERSION_MINOR=$(FLASCC_VERSION_MINOR) -DFLASCC_VERSION_PATCH=$(FLASCC_VERSION_PATCH) -DFLASCC_VERSION_BUILD=$(FLASCC_VERSION_BUILD)"

# ====================================================================================
# BMAKE
# ====================================================================================

#TODO are we done sweeping for asm?
#BMAKE=AR='/usr/bin/true ||' GENCAT=/usr/bin/true RANLIB=/usr/bin/true CC="$(SDK)/usr/bin/$(FLASCC_CC) -emit-llvm"' -DSTRIP_FBSDID -D__asm__\(X...\)="\error" -D__asm\(X...\)="\error"' MAKEFLAGS="" MFLAGS="" NO_WERROR=true $(BUILD)/bmake/bmake -m $(BUILD)/lib/share/mk 

#BMake Tool (Add -v flag to CC and CXX to verbose mode for debugging)
BMAKE= AR="$(BIN_TRUE) ||" GENCAT=$(BIN_TRUE) RANLIB=$(BIN_TRUE)
BMAKE+= CC="$(SDK)/usr/bin/$(FLASCC_CC) -I $(SDK)/usr/include -emit-llvm -fno-builtin -DSTRIP_FBSDID " 
BMAKE+= CXX="$(SDK)/usr/bin/$(FLASCC_CXX) -I $(SDK)/usr/include -emit-llvm -fno-builtin -DSTRIP_FBSDID "
BMAKE+= MAKEFLAGS="" MFLAGS="" MK_ICONV= WITHOUT_PROFILE=
BMAKE+= MACHINE_ARCH=avm2 MACHINE_CPUARCH=AVM2 NO_WERROR=true SSP_CFLAGS=
BMAKE+= $(BUILD)/bmake/bmake -m $(BUILD)/lib/share/mk 

# ====================================================================================
# ALL TARGET
# ====================================================================================
BUILDORDER= cmake abclibs uname noenv avm2-as alctool alcdb llvm binutils plugins bmake 
BUILDORDER+= csu libc libthr libm libBlocksRuntime libcxx libunwind libcxxrt
BUILDORDER+= as3xx as3wig abcflashpp abcstdlibs_more
BUILDORDER+= sdkcleanup 
BUILDORDER+= tr trd genfs swig gdb pkgconfig libtool 
BUILDORDER+= zlib libvgl libjpeg libpng dejagnu libsdl dmalloc libffi libiconv 
BUILDORDER+= finalcleanup
BUILDORDER+= helloswf helloswf_opt hellocpp_shell hellocpp_swf hellocpp_swf_opt 
BUILDORDER+= scimark scimark_swf aliastest pthreadsubmittests_shell pthreadsubmittests_swf 
BUILDORDER+= sjljtest sjljtest_opt ehtest ehtest_opt as3interoptest posixtest symboltest
BUILDORDER+= samples

all:
	@echo "~~~ Crossbridge $(SDKNAME) ~~~"
	@echo "User: $(UNAME)"
	@echo "Platform: $(PLATFORM)"
	@echo "Build: $(BUILD)"
	@mkdir -p $(BUILD)/logs
	@$(MAKE) diagnostics > $(BUILD)/logs/diagnostics.txt 2>&1
	@$(MAKE) install_libs > $(BUILD)/logs/install_libs.txt 2>&1
	@$(MAKE) base > $(BUILD)/logs/base.txt 2>&1
	@$(MAKE) make > $(BUILD)/logs/make.txt 2>&1
	@$(SDK)/usr/bin/make -s all_with_local_make

all_with_local_make:
	@for target in $(BUILDORDER) ; do \
		echo "-  $$target" ; \
		$(MAKE) $$target > $(BUILD)/logs/$$target.txt 2>&1; \
		mret=$$? ; \
		logs="$$logs $(BUILD)/logs/$$target.txt" ; \
		grep -q "Resource temporarily unavailable" $(BUILD)/logs/$$target.txt ; \
		gret=$$? ; \
		rcount=1 ; \
		while [ $$gret == 0 ] && [ $$rcount -lt 6 ] ; do \
			echo "-  $$target (retry $$rcount)" ; \
			$(MAKE) $$target > $(BUILD)/logs/$$target.txt 2>&1; \
			mret=$$? ; \
			grep -q "Resource temporarily unavailable" $(BUILD)/logs/$$target.txt ; \
			gret=$$? ; \
			let rcount=rcount+1 ; \
		done ; \
		if [ $$mret -ne 0 ] ; then \
			echo "Failed to build: $$target" ;\
			tail $$logs ;\
			exit 1 ; \
		fi ; \
	done

# Print debug information
diagnostics:
	@echo "~~~ $(SDKNAME) ~~~"
	@echo "Threads: $(THREADS)"
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

# Development
all_dev:
	@$(SDK)/usr/bin/make dmalloc

# ====================================================================================
# CORE
# ====================================================================================
clean:
	$(MAKE) clean_libs
	rm -rf $(BUILDROOT)
	rm -rf $(SDK)
	rm -rf $(SRCROOT)/.redo
	cd samples && $(MAKE) clean

docs:
	$(MAKE) base
	$(MAKE) abclibs_asdocs

# ====================================================================================
# DEPENDENCY LIBS
# ====================================================================================
install_libs:
	# untar packages
	tar xf packages/$(DEPENDENCY_BMAKE).tar.gz
	tar xf packages/$(DEPENDENCY_CMAKE).tar.gz
	tar xf packages/$(DEPENDENCY_DMALLOC).tar.gz
	tar xf packages/$(DEPENDENCY_DEJAGNU).tar.gz
	tar xf packages/$(DEPENDENCY_ICONV).tar.gz
	tar xf packages/$(DEPENDENCY_JPEG).tar.gz
	tar xf packages/$(DEPENDENCY_LIBPNG).tar.gz
	tar xf packages/$(DEPENDENCY_MAKE).tar.gz
	tar xf packages/$(DEPENDENCY_PKG_CFG).tar.gz
	tar xf packages/$(DEPENDENCY_SDL).tar.gz
	tar xf packages/$(DEPENDENCY_ZLIB).tar.gz
	# unzip packages
	mkdir -p $(DEPENDENCY_SCIMARK)
	cd $(DEPENDENCY_SCIMARK) && unzip -q ../packages/$(DEPENDENCY_SCIMARK).zip
	# apply patches
	cp -r ./patches/$(DEPENDENCY_DEJAGNU) .
	cp -r ./patches/$(DEPENDENCY_DMALLOC) .
	cp -r ./patches/$(DEPENDENCY_LIBPNG) .
	cp -r ./patches/$(DEPENDENCY_PKG_CFG) .
	cp -r ./patches/$(DEPENDENCY_SCIMARK) .
	cp -r ./patches/$(DEPENDENCY_SDL) .
	cp -r ./patches/$(DEPENDENCY_ZLIB) .

clean_libs:
	rm -rf $(DEPENDENCY_BMAKE)
	rm -rf $(DEPENDENCY_CMAKE)
	rm -rf $(DEPENDENCY_DMALLOC)
	rm -rf $(DEPENDENCY_DEJAGNU)
	rm -rf $(DEPENDENCY_ICONV)
	rm -rf $(DEPENDENCY_JPEG)
	rm -rf $(DEPENDENCY_LIBPNG)
	rm -rf $(DEPENDENCY_MAKE)
	rm -rf $(DEPENDENCY_PKG_CFG)
	rm -rf $(DEPENDENCY_SCIMARK)
	rm -rf $(DEPENDENCY_SDL)
	rm -rf $(DEPENDENCY_ZLIB)

# ====================================================================================
# BASE
# ====================================================================================
# Initialize the build
base:
	mkdir -p $(BUILDROOT)/extra
	mkdir -p $(BUILD)/abclibs
	mkdir -p $(SDK)/usr
	mkdir -p $(SDK)/usr/bin
	mkdir -p $(SDK)/usr/lib
	mkdir -p $(SDK)/usr/lib/bfd-plugins
	mkdir -p $(SDK)/usr/share
	mkdir -p $(SDK)/usr/platform/$(PLATFORM)/libexec/gcc/$(TRIPLE)

	$(LN) ../usr $(SDK)/usr/$(TRIPLE)
	$(LN) $(PLATFORM) $(SDK)/usr/platform/current
	$(LN) ../ $(SDK)/usr/platform/usr 
	$(LN) ../../bin $(SDK)/usr/platform/current/bin
	$(LN) ../../lib $(SDK)/usr/platform/current/lib 
	$(LN) ../../share $(SDK)/usr/platform/current/share
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

	$(RSYNC) tools/playerglobal/14.0/playerglobal.abc $(SDK)/usr/lib/
	$(RSYNC) tools/playerglobal/14.0/playerglobal.swc $(SDK)/usr/lib/
	$(RSYNC) avm2_env/public-api.txt $(SDK)/
	cp -f avmplus/generated/*.abc $(SDK)/usr/lib/

	$(RSYNC) --exclude '*iconv.h' avm2_env/usr/src/include/ $(SDK)/usr/include
	#$(RSYNC) avm2_env/usr/lib/ $(SDK)/usr/lib

	cd $(BUILD) && $(SCOMPFALCON) $(call nativepath,$(SRCROOT)/avmplus/utils/swfmake.as) -outdir . -out swfmake
	cd $(BUILD) && $(SCOMPFALCON) $(call nativepath,$(SRCROOT)/avmplus/utils/projectormake.as) -outdir . -out projectormake

# ====================================================================================
# MAKE
# ====================================================================================
make:
	rm -rf $(BUILD)/make
	mkdir -p $(BUILD)/make
	$(RSYNC) $(SRCROOT)/$(DEPENDENCY_MAKE)/ $(BUILD)/make/
	cd $(BUILD)/make && CC=$(CC) CXX=$(CXX) ./configure --prefix=$(SDK)/usr --program-prefix="" \
		--build=$(BUILD_TRIPLE) --host=$(HOST_TRIPLE) --target=$(TRIPLE)
	cd $(BUILD)/make && CC=$(CC) CXX=$(CXX) $(MAKE) -j$(THREADS)
	cd $(BUILD)/make && CC=$(CC) CXX=$(CXX) $(MAKE) install

# ====================================================================================
# CMAKE
# ====================================================================================
#TBD
cmake:
	rm -rf $(BUILD)/cmake
	rm -rf $(SDK)/usr/cmake_junk
	mkdir -p $(BUILD)/cmake
	mkdir -p $(SDK)/usr/cmake_junk
	$(RSYNC) $(SRCROOT)/$(DEPENDENCY_CMAKE)/ $(BUILD)/cmake/
	cd $(BUILD)/cmake && CC=$(CC) CXX=$(CXX) ./configure \
		--prefix=$(SDK)/usr --datadir=share/$(DEPENDENCY_CMAKE) --docdir=cmake_junk --mandir=cmake_junk
	cd $(BUILD)/cmake && CC=$(CC) CXX=$(CXX) $(MAKE) -j$(THREADS)
	cd $(BUILD)/cmake && CC=$(CC) CXX=$(CXX) $(MAKE) install

# ====================================================================================
# ABC LIBS
# ====================================================================================
#TBD
#TODO: cannot find copy_docs target
abclibs:
	$(MAKE) abclibs_compile abclibs_asdocs

#TBD
abclibs_compile:
	mkdir -p $(BUILD)/abclibs
	mkdir -p $(BUILD)/abclibsposix
	mkdir -p $(SDK)/usr/lib/abcs
	# Just use this to get the Posix interface
	cd $(BUILD)/abclibsposix && $(PYTHON) $(SRCROOT)/posix/gensyscalls.py $(SRCROOT)/posix/syscalls.changed
	cat $(BUILD)/abclibsposix/IKernel.as | sed '1,1d' | sed '$$d' > $(SRCROOT)/posix/IKernel.as
	# Assemble Preloader
	cd $(BUILD)/abclibs && $(SCOMP) $(ABCLIBOPTS) -import $(call nativepath,$(SDK)/usr/lib/playerglobal.abc) $(call nativepath,$(SRCROOT)/posix/DefaultPreloader.as) -swf com.adobe.flascc.preloader.DefaultPreloader,1024,768,60 -outdir . -out DefaultPreloader
	# TBD
	cd $(BUILD)/abclibs && $(SCOMPFALCON) $(ABCLIBOPTS) -strict -optimize $(call nativepath,$(SRCROOT)/posix/ELF.as) -outdir . -out ELF
	cd $(BUILD)/abclibs && $(SCOMPFALCON) $(ABCLIBOPTS) -strict -optimize $(call nativepath,$(SRCROOT)/posix/Exit.as) -outdir . -out Exit
	cd $(BUILD)/abclibs && $(SCOMPFALCON) $(ABCLIBOPTS) -strict -optimize $(call nativepath,$(SRCROOT)/posix/LongJmp.as) -outdir . -out LongJmp
	cd $(BUILD)/abclibs && $(SCOMP) $(ABCLIBOPTS)       -import Exit.abc $(call nativepath,$(SRCROOT)/posix/C_Run.as) -outdir . -out C_Run
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

#TBD
abclibs_asdocs:
	mkdir -p $(BUILDROOT)
	mkdir -p $(BUILDROOT)/tempdita
	mkdir -p $(BUILDROOT)/apidocs
	mkdir -p $(BUILDROOT)/apidocs/tempdita
	mkdir -p $(BUILD)/logs
	cd $(BUILDROOT) && $(ASDOC) \
				-load-config= \
				-external-library-path=$(call nativepath,$(SRCROOT)/tools/flex/frameworks/libs/player/11.1/playerglobal.swc) \
				-strict=false -define+=CONFIG::player,1 -define+=CONFIG::asdocs,true -define+=CONFIG::actual,false \
				-doc-sources+=$(call nativepath,$(SRCROOT)/posix/vfs) \
				-doc-sources+=$(call nativepath,$(SRCROOT)/posix) \
				-keep-xml=true \
				-exclude-sources+=$(call nativepath,$(SRCROOT)/posix/startHack.as) \
				-exclude-sources+=$(call nativepath,$(SRCROOT)/posix/IKernel.as) \
				-exclude-sources+=$(call nativepath,$(SRCROOT)/posix/vfs/nochump) \
				-package-description-file=$(call nativepath,$(SRCROOT)/test/aspackages.xml) \
				-main-title "Crossbridge API Reference" \
				-window-title "Crossbridge API Reference" \
				-output apidocs
	if [ -d $(BUILDROOT)/tempdita ]; then rm -rf $(BUILDROOT)/tempdita; fi
	mv $(BUILDROOT)/apidocs/tempdita $(BUILDROOT)/

# ====================================================================================
# BASIC TOOLS
# ====================================================================================

#TBD
uname:
	$(CC) $(SRCROOT)/tools/uname/uname.c -o $(SDK)/usr/bin/uname$(EXEEXT)

#TBD
noenv:
	$(CC) $(SRCROOT)/tools/noenv/noenv.c -o $(SDK)/usr/bin/noenv$(EXEEXT)

#TBD
avm2-as:
	$(CXX) $(SRCROOT)/avm2_env/misc/SetAlchemySDKLocation.c $(SRCROOT)/tools/as/as.cpp -o $(SDK)/usr/bin/avm2-as$(EXEEXT)

#TBD
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

#TBD
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
# This build the tool chain
llvm:
	rm -rf $(BUILD)/llvm-debug
	mkdir -p $(BUILD)/llvm-debug
	cd $(BUILD)/llvm-debug && LDFLAGS="$(LLVMLDFLAGS)" CFLAGS="$(LLVMCFLAGS)" CXXFLAGS="$(LLVMCXXFLAGS)" $(SDK)/usr/bin/cmake -G "Unix Makefiles" \
		$(LLVMCMAKEOPTS) -DCMAKE_INSTALL_PREFIX=$(LLVMINSTALLPREFIX)/llvm-install -DCMAKE_BUILD_TYPE=$(LLVMBUILDTYPE) $(LLVMCMAKEFLAGS) \
		-DLLVM_ENABLE_ASSERTIONS=$(LLVMASSERTIONS) -DLLVM_ENABLE_ZLIB=OFF \
		-DLLVM_TARGETS_TO_BUILD="$(LLVMTARGETS)" -DLLVM_NATIVE_ARCH="avm2" -DLLVM_INCLUDE_TESTS=$(LLVMTESTS) -DLLVM_BUILD_TESTS=$(LLVMTESTS) -DLLVM_INCLUDE_EXAMPLES=OFF \
		$(SRCROOT)/$(DEPENDENCY_LLVM) && $(MAKE) -j$(THREADS) 
	cp $(LLVMINSTALLPREFIX)/llvm-debug/bin/llc$(EXEEXT) $(SDK)/usr/bin/llc$(EXEEXT)
	$(MAKE) llvmcopyheaders
ifeq ($(LLVM_ONLYLLC), false)
	$(MAKE) llvm-install
endif

# This installs clang include headers
llvmcopyheaders:
	mkdir -p $(SDK)/usr/lib/clang/$(DEPENDENCY_LLVM_VERSION)/include
	cp -f $(BUILD)/llvm-debug/lib/clang/$(DEPENDENCY_LLVM_VERSION)/include/*.* $(SDK)/usr/lib/clang/$(DEPENDENCY_LLVM_VERSION)/include

# This re-build the tool chain
llvmdev:
	cd $(BUILD)/llvm-debug && $(MAKE) -j$(THREADS) && $(MAKE) install
	cp $(LLVMINSTALLPREFIX)/llvm-install/bin/llc$(EXEEXT) $(SDK)/usr/bin/llc$(EXEEXT)
	$(MAKE) llvmcopyheaders
	$(MAKE) llvm-install

# This install the tool chain
llvm-install:
	cp $(LLVMINSTALLPREFIX)/llvm-debug/bin/llvm-ar$(EXEEXT) $(SDK)/usr/bin/llvm-ar$(EXEEXT)
	cp $(LLVMINSTALLPREFIX)/llvm-debug/bin/llvm-as$(EXEEXT) $(SDK)/usr/bin/llvm-as$(EXEEXT)
	cp $(LLVMINSTALLPREFIX)/llvm-debug/bin/llvm-diff$(EXEEXT) $(SDK)/usr/bin/llvm-diff$(EXEEXT)
	cp $(LLVMINSTALLPREFIX)/llvm-debug/bin/llvm-dis$(EXEEXT) $(SDK)/usr/bin/llvm-dis$(EXEEXT)
	cp $(LLVMINSTALLPREFIX)/llvm-debug/bin/llvm-extract$(EXEEXT) $(SDK)/usr/bin/llvm-extract$(EXEEXT)
	$(LLVMLDCP)
	cp $(LLVMINSTALLPREFIX)/llvm-debug/bin/llvm-link$(EXEEXT) $(SDK)/usr/bin/llvm-link$(EXEEXT)
	cp $(LLVMINSTALLPREFIX)/llvm-debug/bin/llvm-nm$(EXEEXT) $(SDK)/usr/bin/llvm-nm$(EXEEXT)
	cp $(LLVMINSTALLPREFIX)/llvm-debug/bin/llvm-ranlib$(EXEEXT) $(SDK)/usr/bin/llvm-ranlib$(EXEEXT)
	cp $(LLVMINSTALLPREFIX)/llvm-debug/bin/opt$(EXEEXT) $(SDK)/usr/bin/opt$(EXEEXT)
	$(CP_CLANG)
	cp $(LLVMINSTALLPREFIX)/llvm-debug/lib/LLVMgold.* $(SDK)/usr/lib/LLVMgold$(SOEXT)
	cp -f $(BUILD)/llvm-debug/bin/fpcmp$(EXEEXT) $(BUILDROOT)/extra/fpcmp$(EXEEXT)

# This run the test suite
llvmtests:
	rm -rf $(BUILD)/llvm-tests
	mkdir -p $(BUILD)/llvm-tests
	#cp -f $(SDK)/usr/bin/avmshell-release-debugger $(SDK)/usr/bin/avmshell
	cd $(BUILD)/llvm-tests && PATH=$(SDK)/usr/bin:$(PATH) CC=$(FLASCC_CC) CXX=$(FLASCC_CXX) $(SRCROOT)/$(DEPENDENCY_LLVM)/configure \
		--disable-polly --without-f2c --without-f95 --enable-jit=no --target=$(TRIPLE) --prefix=$(BUILD)/llvm-debug
	cd $(BUILD)/llvm-tests && $(LN) $(SDK)/usr Release
	cd $(BUILD)/llvm-tests/projects/test-suite/tools && (LANG=C && PATH=$(SDK)/usr/bin:$(PATH) CC=$(FLASCC_CC) CXX=$(FLASCC_CXX) $(MAKE) TEST=nightly TARGET_LLCFLAGS=-jvm="$(JAVA)" -j$(THREADS) FPCMP=$(FPCMP) DISABLE_CBE=1)
	cd $(BUILD)/llvm-tests/projects/test-suite/MultiSource && (LANG=C && PATH=$(SDK)/usr/bin:$(PATH) CC=$(FLASCC_CC) CXX=$(FLASCC_CXX) $(MAKE) TEST=nightly TARGET_LLCFLAGS=-jvm="$(JAVA)" -j$(THREADS) FPCMP=$(FPCMP) DISABLE_CBE=1)
	cd $(BUILD)/llvm-tests/projects/test-suite/SingleSource && (LANG=C && PATH=$(SDK)/usr/bin:$(PATH) CC=$(FLASCC_CC) CXX=$(FLASCC_CXX) $(MAKE) TEST=nightly TARGET_LLCFLAGS=-jvm="$(JAVA)" -j$(THREADS) FPCMP=$(FPCMP) DISABLE_CBE=1)
	$(PYTHON) $(SRCROOT)/tools/llvmtestcheck.py --srcdir $(SRCROOT)/$(DEPENDENCY_LLVM)/projects/test-suite/ --builddir $(BUILD)/llvm-tests/projects/test-suite/ --fpcmp $(FPCMP)> $(BUILD)/llvm-tests/passfail.txt
	cp $(BUILD)/llvm-tests/passfail.txt $(BUILD)/passfail_llvm.txt

# ====================================================================================
# BINUTILS
# ====================================================================================
binutils:
ifneq (,$(findstring cygwin,$(PLATFORM)))
	$(MAKE) -i binutils_build
else
	$(MAKE) binutils_build
endif

# Assemble LLVM BinUtils
binutils_build:
	rm -rf $(BUILD)/binutils
	mkdir -p $(BUILD)/binutils
	mkdir -p $(SDK)/usr
	cd $(BUILD)/binutils && CC=$(CC) CXX=$(CXX) CFLAGS="-I$(SRCROOT)/avm2_env/misc/ $(DBGOPTS) " CXXFLAGS="$(GCCLANGFLAG) -I$(SRCROOT)/avm2_env/misc/ $(DBGOPTS) " $(SRCROOT)/binutils/configure \
		--disable-doc --enable-gold --disable-ld --enable-plugins \
		--build=$(BUILD_TRIPLE) --host=$(HOST_TRIPLE) --target=$(TRIPLE) --with-sysroot=$(SDK)/usr \
		--program-prefix="" --prefix=$(SDK)/usr --disable-werror \
		--enable-targets=$(TRIPLE)
	cd $(BUILD)/binutils && $(MAKE) -j$(THREADS) && $(MAKE) install
	mv $(SDK)/usr/bin/ld.gold$(EXEEXT) $(SDK)/usr/bin/ld$(EXEEXT)
	rm -rf $(SDK)/usr/bin/readelf$(EXEEXT) $(SDK)/usr/bin/elfedit$(EXEEXT) $(SDK)/usr/bin/ld.bfd$(EXEEXT) $(SDK)/usr/bin/objdump$(EXEEXT) $(SDK)/usr/bin/objcopy$(EXEEXT) $(SDK)/usr/share/info $(SDK)/usr/share/man

# ====================================================================================
# PLUGINS
# ====================================================================================
plugins:
	rm -rf $(BUILD)/makeswf $(BUILD)/multiplug $(BUILD)/zlib
	mkdir -p $(BUILD)/makeswf $(BUILD)/multiplug $(BUILD)/zlib
	cd $(BUILD)/makeswf && $(CXX) $(DBGOPTS) -I$(SRCROOT)/avm2_env/misc/ -DHAVE_ABCNM -DDEFTMPDIR=\"$(call nativepath,/tmp)\" -DDEFSYSROOT=\"$(call nativepath,$(SDK))\" -DHAVE_STDINT_H -I$(SRCROOT)/$(DEPENDENCY_ZLIB)/ -I$(SRCROOT)/binutils/include -fPIC -c $(SRCROOT)/gold-plugins/makeswf.cpp
	cd $(BUILD)/makeswf && $(CXX) $(DBGOPTS) -shared -Wl,-headerpad_max_install_names,-undefined,dynamic_lookup -o makeswf$(SOEXT) makeswf.o
	cd $(BUILD)/multiplug && $(CXX) $(DBGOPTS) -I$(SRCROOT)/avm2_env/misc/ -DHAVE_STDINT_H -DSOEXT=\"$(SOEXT)\" -DDEFSYSROOT=\"$(call nativepath,$(SDK))\" -I$(SRCROOT)/binutils/include -fPIC -c $(SRCROOT)/gold-plugins/multiplug.cpp
	cd $(BUILD)/multiplug && $(CXX) $(DBGOPTS) -shared -Wl,-headerpad_max_install_names,-undefined,dynamic_lookup -o multiplug$(SOEXT) multiplug.o
	cp -f $(BUILD)/makeswf/makeswf$(SOEXT) $(SDK)/usr/lib/makeswf$(SOEXT)
	cp -f $(BUILD)/multiplug/multiplug$(SOEXT) $(SDK)/usr/lib/multiplug$(SOEXT)
	cp -f $(BUILD)/multiplug/multiplug$(SOEXT) $(SDK)/usr/lib/bfd-plugins/multiplug$(SOEXT)

# ====================================================================================
# BMAKE
# ====================================================================================
bmake:
	rm -rf $(BUILD)/bmake
	mkdir -p $(BUILD)/bmake
	cd $(BUILD)/bmake && $(SRCROOT)/bmake/configure
	cd $(BUILD)/bmake && bash make-bootstrap.sh

# ====================================================================================
# STD LIBS
# ====================================================================================

# TBD
csu:
	$(RSYNC) avm2_env/usr/ $(BUILD)/lib/
	cd $(BUILD)/lib/src/lib/csu/avm2 && $(BMAKE) crt1_c.o
	mv -f $(BUILD)/lib/src/lib/csu/avm2/crt1_c.o $(SDK)/usr/lib/.

# C
libc:
	mkdir -p $(BUILD)/posix/
	rm -f $(BUILD)/posix/*.o
	mkdir -p $(BUILD)/lib/src/lib/libc/
	$(RSYNC) avm2_env/usr/ $(BUILD)/lib/
	cd $(BUILD)/posix && $(PYTHON) $(SRCROOT)/posix/gensyscalls.py $(SRCROOT)/posix/syscalls.changed
	cp $(BUILD)/posix/IKernel.as $(SRCROOT)/avmplus/shell
	cp $(BUILD)/posix/ShellPosix.as $(SRCROOT)/avmplus/shell
	cp $(BUILD)/posix/ShellPosixGlue.cpp $(SRCROOT)/avmplus/shell
	cp $(BUILD)/posix/ShellPosixGlue.h $(SRCROOT)/avmplus/shell
	cd $(SRCROOT)/avmplus/shell && $(PYTHON) ./shell_toplevel.py -config CONFIG::VMCFG_ALCHEMY_POSIX=true
	cd $(BUILD)/posix && $(SDK)/usr/bin/$(FLASCC_CC) -emit-llvm -fno-stack-protector $(LIBHELPEROPTFLAGS) -I $(SDK)/usr/include -c posix.c
	cd $(BUILD)/posix && $(SDK)/usr/bin/$(FLASCC_CC) -emit-llvm -fno-stack-protector $(LIBHELPEROPTFLAGS) -I $(SDK)/usr/include -c $(SRCROOT)/posix/vgl.c
	cd $(BUILD)/posix && $(SDK)/usr/bin/$(FLASCC_CC) -emit-llvm -fno-stack-protector $(LIBHELPEROPTFLAGS) -I $(SDK)/usr/include -D_KERNEL -c $(SRCROOT)/avm2_env/usr/src/kern/kern_umtx.c
	cd $(BUILD)/posix && $(SDK)/usr/bin/$(FLASCC_CC) -emit-llvm -fno-stack-protector $(LIBHELPEROPTFLAGS) -I $(SDK)/usr/include -I $(SRCROOT)/avm2_env/usr/src/lib/libc/include/ -c $(SRCROOT)/posix/thrStubs.c
	cd $(BUILD)/posix && $(SDK)/usr/bin/$(FLASCC_CC) -emit-llvm -fno-stack-protector $(LIBHELPEROPTFLAGS) -I $(SDK)/usr/include -c $(SRCROOT)/posix/kpmalloc.c
	cd $(BUILD)/posix && cp *.o $(BUILD)/lib/src/lib/libc/
	cd $(BUILD)/lib/src/lib/libc && $(BMAKE) -j$(THREADS) libc.a
	# find bitcode (and ignore non-bitcode genned from .s files) and put
	# it in our lib
	rm -f $(BUILD)/lib/src/lib/libc/tmp/*
	$(AR) $(SDK)/usr/lib/libssp.a $(BUILD)/lib/src/lib/libc/stack_protector.o && cp $(SDK)/usr/lib/libssp.a $(SDK)/usr/lib/libssp_nonshared.a
	# we override these in thrStubs.c but leave them weak
	cd $(BUILD)/lib/src/lib/libc && $(SDK)/usr/bin/llvm-dis -o=_pthread_stubs.ll _pthread_stubs.o && sed -E 's/@pthread_(key_create|key_delete|getspecific|setspecific|once) =/@_d_u_m_m_y_\1 =/g' _pthread_stubs.ll | $(SDK)/usr/bin/llvm-as -o _pthread_stubs.o
	cd $(BUILD)/lib/src/lib/libc && rm -f libc.a && find . -name '*.o' -exec sh -c 'file {} | grep -v 86 > /dev/null' \; -print | xargs $(AR) libc.a
	cd $(BUILD)/posix && $(SDK)/usr/bin/$(FLASCC_CC) -emit-llvm -fno-stack-protector $(LIBHELPEROPTFLAGS) -I $(SRCROOT)/avm2_env/usr/src/lib/libc/include/ -fexceptions -c $(SRCROOT)/posix/libcHack.c
	cp -f $(BUILD)/lib/src/lib/libc/libc.a $(BUILD)/posix/libcHack.o $(SDK)/usr/lib/.

libc.abc:
	mkdir -p $(BUILD)/libc_abc
	cd $(BUILD)/libc_abc && $(SDK)/usr/bin/ar x $(SDK)/usr/lib/libc.a
	cd $(BUILD)/libc_abc && cp -f $(SRCROOT)/avm2_env/misc/abcarchive.mk Makefile && SDK=$(SDK) $(MAKE) LLCOPTS='-jvm="$(JAVA)"' -j$(THREADS)
	mv $(BUILD)/libc_abc/test.a $(SDK)/usr/lib/stdlibs_abc/libc.a

# Threads
libthr:
	rm -rf $(BUILD)/libthr
	mkdir -p $(BUILD)/libthr
	$(RSYNC) avm2_env/usr/src/lib/ $(BUILD)/libthr/
	cd $(BUILD)/libthr/libthr && $(SDK)/usr/bin/$(FLASCC_CC) -emit-llvm -fno-stack-protector $(LIBHELPEROPTFLAGS) -c $(SRCROOT)/posix/thrHelpers.c
	# CWARNFLAGS= because thr_exit() can return and pthread_exit() is marked noreturn (where?)...
	cd $(BUILD)/libthr/libthr && $(BMAKE) -j$(THREADS) CWARNFLAGS= libthr.a
	# find bitcode (and ignore non-bitcode genned from .s files) and put
	# it in our lib
	cd $(BUILD)/libthr/libthr && rm -f libthr.a && find . -name '*.o' -exec sh -c 'file {} | grep -v 86 > /dev/null' \; -print | xargs $(AR) libthr.a
	cp -f $(BUILD)/libthr/libthr/libthr.a $(SDK)/usr/lib/.

# Threads
libthr.abc:
	mkdir -p $(BUILD)/libthr_abc
	cd $(BUILD)/libthr_abc && $(SDK)/usr/bin/ar x $(SDK)/usr/lib/libthr.a
	cd $(BUILD)/libthr_abc && cp -f $(SRCROOT)/avm2_env/misc/abcarchive.mk Makefile && SDK=$(SDK) $(MAKE) LLCOPTS='-jvm="$(JAVA)"' -j$(THREADS)
	mv $(BUILD)/libthr_abc/test.a $(SDK)/usr/lib/stdlibs_abc/libthr.a

# Math
libm:
	cd compiler_rt && $(MAKE) clean && $(MAKE) avm2 CC="$(SDK)/usr/bin/$(FLASCC_CC) -emit-llvm" RANLIB=$(SDK)/usr/bin/ranlib AR=$(SDK)/usr/bin/ar VERBOSE=1
	$(SDK)/usr/bin/llvm-link -o $(BUILD)/libcompiler_rt.o compiler_rt/avm2/avm2/avm2/SubDir.lib/*.o
	$(SDK)/usr/bin/nm $(BUILD)/libcompiler_rt.o  | grep "T _" | sed 's/_//' | awk '{print $$3}' | sort | uniq > $(BUILD)/compiler_rt.txt
	cat $(BUILD)/compiler_rt.txt >> $(SDK)/public-api.txt
	cat $(SRCROOT)/$(DEPENDENCY_LLVM)/lib/CodeGen/SelectionDAG/TargetLowering.cpp | grep "Names\[RTLIB::" | awk '{print $$3}' | sed 's/"//g' | sed 's/;//' | sort | uniq > $(BUILD)/rtlib.txt
	cat avm2_env/rtlib-extras.txt >> $(BUILD)/rtlib.txt

	rm -rf $(BUILD)/msun/ $(BUILD)/libmbc $(SDK)/usr/lib/libm.a $(SDK)/usr/lib/libm.o
	mkdir -p $(BUILD)/msun
	$(RSYNC) avm2_env/usr/src/lib/ $(BUILD)/msun/
	cd $(BUILD)/msun/msun && $(BMAKE) -j$(THREADS) libm.a
	# find bitcode (and ignore non-bitcode genned from .s files) and put
	# it in our lib
	cd $(BUILD)/msun/msun && rm -f libm.a && find . -name '*.o' -exec sh -c 'file {} | grep -v 86 > /dev/null' \; -print | xargs $(AR) libm.a
	# remove symbols for sin, cos, other things we support as intrinsics
	cd $(BUILD)/msun/msun && $(SDK)/usr/bin/ar sd libm.a s_cos.o s_sin.o e_pow.o e_sqrt.o
	$(SDK)/usr/bin/ar r $(SDK)/usr/lib/libm.a
	mkdir -p $(BUILD)/libmbc
	cd $(BUILD)/libmbc && $(SDK)/usr/bin/ar x $(BUILD)/msun/msun/libm.a
	cd $(BUILD)/libmbc && $(SDK)/usr/bin/llvm-link -o $(BUILD)/libmbc/libm.o $(BUILD)/libcompiler_rt.o *.o
	cp -f $(BUILD)/libmbc/libm.o $(SDK)/usr/lib/libm.o
	$(SDK)/usr/bin/opt -O3 -o $(SDK)/usr/lib/libm.o $(BUILD)/libmbc/libm.o
	$(SDK)/usr/bin/nm $(SDK)/usr/lib/libm.o | grep "T _" | sed 's/_//' | awk '{print $$3}' | sort | uniq > $(BUILD)/libm.bc.txt

# Math
libm.abc:
	mkdir -p $(BUILD)/libm_abc
	cp $(BUILD)/msun/msun/*.o $(BUILD)/libm_abc
	cd $(BUILD)/libm_abc && cp -f $(SRCROOT)/avm2_env/misc/abcarchive.mk Makefile && SDK=$(SDK) $(MAKE) LLCOPTS='-jvm="$(JAVA)"' -j$(THREADS)

libBlocksRuntime:
	cd compiler_rt/BlocksRuntime && echo '#define HAVE_SYNC_BOOL_COMPARE_AND_SWAP_INT' > config.h && echo '#define HAVE_SYNC_BOOL_COMPARE_AND_SWAP_LONG' >> config.h
	cd compiler_rt/BlocksRuntime && $(SDK)/usr/bin/$(FLASCC_CC) -emit-llvm -c data.c -o data.o
	cd compiler_rt/BlocksRuntime && $(SDK)/usr/bin/$(FLASCC_CC) -emit-llvm -c runtime.c -o runtime.o
	cd compiler_rt/BlocksRuntime && $(AR) $(SDK)/usr/lib/libBlocksRuntime.a data.o runtime.o
	cp compiler_rt/BlocksRuntime/Block*.h $(SDK)/usr/include/

libcxx: libsupcxx libgcceh
	$(RSYNC) avm2_env/usr/ $(BUILD)/lib/
	cd $(BUILD)/lib/src/lib/libc++ && $(BMAKE) clean && $(BMAKE) -j$(THREADS) libc++.a
	cd $(BUILD)/lib/src/lib/libc++ && $(AR) libc++.a *.o && mv libc++.a $(SDK)/usr/lib/.

libcxx.abc:
	mkdir -p $(BUILD)/libcxx_abc
	cd $(BUILD)/libcxx_abc && $(SDK)/usr/bin/ar x $(SDK)/usr/lib/libc++.a
	cd $(BUILD)/libcxx_abc && cp -f $(SRCROOT)/avm2_env/misc/abcarchive.mk Makefile && SDK=$(SDK) $(MAKE) LLCOPTS='-jvm="$(JAVA)"' -j$(THREADS)
	mv $(BUILD)/libcxx_abc/test.a $(SDK)/usr/lib/stdlibs_abc/libc++.a

libsupcxx:
	$(RSYNC) avm2_env/usr/ $(BUILD)/lib/
	cd $(BUILD)/lib/src/lib/libsupc++/ && $(BMAKE) clean && $(BMAKE) -j$(THREADS) libsupc++.a
	cd $(BUILD)/lib/src/lib/libsupc++/ && $(SDK)/usr/bin/llvm-link -o libsupc++.o *.o && mv libsupc++.o ../libc++

libgcceh:
	$(RSYNC) avm2_env/usr/ $(BUILD)/lib/
	cd $(BUILD)/lib/src/lib/libgcceh/ && $(BMAKE) clean && $(BMAKE) -j$(THREADS) libgcceh.a
	cd $(BUILD)/lib/src/lib/libgcceh/ && $(SDK)/usr/bin/llvm-link -o libgcceh.o *.o && mv libgcceh.o ../libc++

# TBD
libunwind:
	$(RSYNC) avm2_env/usr/ $(BUILD)/lib/
	cd $(BUILD)/lib/src/lib/libunwind/ && $(BMAKE) clean && $(BMAKE) -j$(THREADS) libunwind.a
	cd $(BUILD)/lib/src/lib/libunwind/ && $(SDK)/usr/bin/llvm-link -o libunwind.o *.o && mv libunwind.o ../libc++

# TBD
libcxxrt:
	$(RSYNC) avm2_env/usr/ $(BUILD)/lib/
	cd $(BUILD)/lib/src/lib/libcxxrt/ && $(BMAKE) clean && $(BMAKE) -j$(THREADS) libcxxrt.a
	cd $(BUILD)/lib/src/lib/libcxxrt/ && $(SDK)/usr/bin/llvm-link -o libcxxrt.o *.o && mv libcxxrt.o ../libc++

# TBD
# TODO: Solve build error
libcxxabi:
	$(RSYNC) avm2_env/usr/ $(BUILD)/lib/
	cd $(BUILD)/lib/src/lib/libc++abi/ && $(BMAKE) clean && $(BMAKE) libc++abi.a
	cd $(BUILD)/lib/src/lib/libc++abi/ && $(SDK)/usr/bin/llvm-link -o libc++abi.o *.o && mv libc++abi.o ../libc++

# ====================================================================================
# AS3XX
# ====================================================================================
as3xx:
	mkdir -p $(SDK)/usr/lib/stdlibs_abc
	cd $(BUILD)/posix && $(SDK)/usr/bin/$(FLASCC_CXX) -emit-llvm -fno-stack-protector $(LIBHELPEROPTFLAGS) -c $(SRCROOT)/posix/AS3++.cpp
	cd $(BUILD)/posix && $(SDK)/usr/bin/llc -gendbgsymtable -jvm="$(JAVA)" -falcon-parallel -filetype=obj AS3++.o -o AS3++.abc
	cd $(BUILD)/posix && $(SDK)/usr/bin/ar crus $(SDK)/usr/lib/libAS3++.a AS3++.o
	cd $(BUILD)/posix && $(SDK)/usr/bin/ar crus $(SDK)/usr/lib/stdlibs_abc/libAS3++.a AS3++.abc

# ====================================================================================
# AS3WIG
# ====================================================================================
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
	cd $(BUILD)/as3wig && $(SDK)/usr/bin/$(FLASCC_CXX) -c -emit-llvm -I. AS3Wig.cpp -o Flash++.o
	cd $(BUILD)/as3wig && $(SDK)/usr/bin/ar crus $(SDK)/usr/lib/libFlash++.a Flash++.o

# ====================================================================================
# ABCSTDLIBS
# ====================================================================================

# TBD
abcflashpp:
	$(SDK)/usr/bin/llc -gendbgsymtable -jvmopt=-Xmx4G -jvm="$(JAVA)" -falcon-parallel -target-player -filetype=obj $(BUILD)/as3wig/Flash++.o -o $(BUILD)/as3wig/Flash++.abc
	$(SDK)/usr/bin/ar crus $(SDK)/usr/lib/stdlibs_abc/libFlash++.a $(BUILD)/as3wig/Flash++.abc

# TBD
# TODO: Not in build
single.abc:
	mkdir -p $(SDK)/usr/lib/stdlibs_abc
	$(SDK)/usr/bin/llc -gendbgsymtable -jvm="$(JAVA)" -falcon-parallel -filetype=obj $(SDK)/usr/lib/crt1_c.o -o $(SDK)/usr/lib/stdlibs_abc/crt1_c.o
	$(SDK)/usr/bin/llc -gendbgsymtable -jvm="$(JAVA)" -falcon-parallel -filetype=obj $(SDK)/usr/lib/libm.o -o $(SDK)/usr/lib/stdlibs_abc/libm.o
	$(SDK)/usr/bin/llc -gendbgsymtable -jvm="$(JAVA)" -falcon-parallel -filetype=obj $(SDK)/usr/lib/libcHack.o -o $(SDK)/usr/lib/stdlibs_abc/libcHack.o

# TBD
abcstdlibs_more:
	mkdir -p $(SDK)/usr/lib/stdlibs_abc
	$(SDK)/usr/bin/llc -gendbgsymtable -jvm="$(JAVA)" -falcon-parallel -filetype=obj $(SDK)/usr/lib/crt1_c.o -o $(SDK)/usr/lib/stdlibs_abc/crt1_c.o
	$(SDK)/usr/bin/llc -gendbgsymtable -jvm="$(JAVA)" -falcon-parallel -filetype=obj $(SDK)/usr/lib/libm.o -o $(SDK)/usr/lib/stdlibs_abc/libm.o
	$(SDK)/usr/bin/llc -gendbgsymtable -jvm="$(JAVA)" -falcon-parallel -filetype=obj $(SDK)/usr/lib/libcHack.o -o $(SDK)/usr/lib/stdlibs_abc/libcHack.o

	mkdir -p $(BUILD)/libc_abc
	cd $(BUILD)/libc_abc && $(SDK)/usr/bin/ar x $(SDK)/usr/lib/libc.a
	cd $(BUILD)/libc_abc && cp -f $(SRCROOT)/avm2_env/misc/abcarchive.mk Makefile && SDK=$(SDK) $(MAKE) LLCOPTS='-jvm="$(JAVA)"' -j$(THREADS)
	mv $(BUILD)/libc_abc/test.a $(SDK)/usr/lib/stdlibs_abc/libc.a

	mkdir -p $(BUILD)/libthr_abc
	cd $(BUILD)/libthr_abc && $(SDK)/usr/bin/ar x $(SDK)/usr/lib/libthr.a
	cd $(BUILD)/libthr_abc && cp -f $(SRCROOT)/avm2_env/misc/abcarchive.mk Makefile && SDK=$(SDK) $(MAKE) LLCOPTS='-jvm="$(JAVA)"' -j$(THREADS)
	mv $(BUILD)/libthr_abc/test.a $(SDK)/usr/lib/stdlibs_abc/libthr.a
#ifneq (,$(findstring 2.9,$(LLVMVERSION)))
	#mkdir -p $(BUILD)/libgcc_abc
	#cd $(BUILD)/libgcc_abc && $(SDK)/usr/bin/ar x $(SDK)/usr/lib/libgcc.a
	#cd $(BUILD)/libgcc_abc && cp -f $(SRCROOT)/avm2_env/misc/abcarchive.mk Makefile && SDK=$(SDK) $(MAKE) LLCOPTS='-jvm="$(JAVA)"' -j$(THREADS)
	#mv $(BUILD)/libgcc_abc/test.a $(SDK)/usr/lib/stdlibs_abc/libgcc.a
	#mkdir -p $(BUILD)/libstdcpp_abc
	#cd $(BUILD)/libstdcpp_abc && $(SDK)/usr/bin/ar x $(SDK)/usr/lib/libstdc++.a
	#cd $(BUILD)/libstdcpp_abc && cp -f $(SRCROOT)/avm2_env/misc/abcarchive.mk Makefile && SDK=$(SDK) $(MAKE) LLCOPTS='-jvm="$(JAVA)"' -j$(THREADS)
	#mv $(BUILD)/libstdcpp_abc/test.a $(SDK)/usr/lib/stdlibs_abc/libstdc++.a
	#mkdir -p $(BUILD)/libsupcpp_abc
	#cd $(BUILD)/libsupcpp_abc && $(SDK)/usr/bin/ar x $(SDK)/usr/lib/libsupc++.a
	#cd $(BUILD)/libsupcpp_abc && cp -f $(SRCROOT)/avm2_env/misc/abcarchive.mk Makefile && SDK=$(SDK) $(MAKE) LLCOPTS='-jvm="$(JAVA)"' -j$(THREADS)
	#mv $(BUILD)/libsupcpp_abc/test.a $(SDK)/usr/lib/stdlibs_abc/libsupc++.a
	#mkdir -p $(BUILD)/libobjc_abc
	#cd $(BUILD)/libobjc_abc && $(SDK)/usr/bin/ar x $(SDK)/usr/lib/libobjc.a
	#cd $(BUILD)/libobjc_abc && cp -f $(SRCROOT)/avm2_env/misc/abcarchive.mk Makefile && SDK=$(SDK) $(MAKE) LLCOPTS='-jvm="$(JAVA)"' -j$(THREADS)
	#mv $(BUILD)/libobjc_abc/test.a $(SDK)/usr/lib/stdlibs_abc/libobjc.a
#endif
	mkdir -p $(BUILD)/libBlocksRuntime_abc
	cd $(BUILD)/libBlocksRuntime_abc && $(SDK)/usr/bin/ar x $(SDK)/usr/lib/libBlocksRuntime.a
	cd $(BUILD)/libBlocksRuntime_abc && cp -f $(SRCROOT)/avm2_env/misc/abcarchive.mk Makefile && SDK=$(SDK) $(MAKE) LLCOPTS='-jvm="$(JAVA)"' -j$(THREADS)
	mv $(BUILD)/libBlocksRuntime_abc/test.a $(SDK)/usr/lib/stdlibs_abc/libBlocksRuntime.a

	mkdir -p $(BUILD)/libcxx_abc
	cd $(BUILD)/libcxx_abc && $(SDK)/usr/bin/ar x $(SDK)/usr/lib/libc++.a
	cd $(BUILD)/libcxx_abc && cp -f $(SRCROOT)/avm2_env/misc/abcarchive.mk Makefile && SDK=$(SDK) $(MAKE) LLCOPTS='-jvm="$(JAVA)"' -j$(THREADS)
	mv $(BUILD)/libcxx_abc/test.a $(SDK)/usr/lib/stdlibs_abc/libc++.a

# ====================================================================================
# SDKCLEANUP
# ====================================================================================

# TBD
sdkcleanup:
	mv $(SDK)/usr/share/$(DEPENDENCY_CMAKE) $(SDK)/usr/share_cmake
	rm -rf $(SDK)/usr/share $(SDK)/usr/info $(SDK)/usr/man $(SDK)/usr/lib/x86_64 $(SDK)/usr/cmake_junk $(SDK)/usr/make_junk
	mkdir -p $(SDK)/usr/share
	mv $(SDK)/usr/share_cmake $(SDK)/usr/share/$(DEPENDENCY_CMAKE)
	rm -f $(SDK)/usr/lib/*.la
	rm -f $(SDK)/usr/lib/crt1.o $(SDK)/usr/lib/crtbegin.o $(SDK)/usr/lib/crtbeginS.o $(SDK)/usr/lib/crtbeginT.o $(SDK)/usr/lib/crtend.o $(SDK)/usr/lib/crtendS.o $(SDK)/usr/lib/crti.o $(SDK)/usr/lib/crtn.o

# ====================================================================================
# TR
# ====================================================================================
# Assemble Tamarin Release
tr:
	rm -rf $(BUILD)/tr
	mkdir -p $(BUILD)/tr
	cd $(BUILD)/tr && rm -f Makefile && AR=$(NATIVE_AR) CC=$(CC) CXX=$(CXX) $(TAMARINCONFIG) --disable-debugger
	cd $(BUILD)/tr && AR=$(NATIVE_AR) CC=$(CC) CXX=$(CXX) $(MAKE) -j$(THREADS)
	cp -f $(BUILD)/tr/shell/avmshell $(SDK)/usr/bin/avmshell
	cd $(SRCROOT)/avmplus/utils && curdir=$(SRCROOT)/avmplus/utils ASC=$(ASC) $(MAKE) -f manifest.mk utils
	cd $(BUILD)/abclibs && $(SCOMPFALCON) $(call nativepath,$(SRCROOT)/avmplus/utils/projectormake.as) -outdir . -out projectormake
#ifeq (,$(findstring cygwin,$(PLATFORM)))
#	$(SDK)/usr/bin/avmshell $(BUILD)/abclibs/projectormake.abc -- -o $(SDK)/usr/bin/abcdump$(EXEEXT) $(SDK)/usr/bin/avmshell $(SRCROOT)/avmplus/utils/abcdump.abc -- -Djitordie
#	chmod a+x $(SDK)/usr/bin/abcdump$(EXEEXT)
#endif

# ====================================================================================
# TRD
# ====================================================================================
# Assemble Tamarin Debugger
trd:
	rm -rf $(BUILD)/trd
	mkdir -p $(BUILD)/trd
	cd $(BUILD)/trd && rm -f Makefile && AR=$(NATIVE_AR) CC=$(CC) CXX=$(CXX) $(TAMARINCONFIG) --enable-debugger
	cd $(BUILD)/trd && AR=$(NATIVE_AR) CC=$(CC) CXX=$(CXX) $(MAKE) -j$(THREADS)
	cp -f $(BUILD)/trd/shell/avmshell $(SDK)/usr/bin/avmshell-release-debugger

# ====================================================================================
# EXTRA LIBS
# ====================================================================================

# Library ZLib
zlib:
	rm -rf $(BUILD)/zlib
	cp -r $(SRCROOT)/$(DEPENDENCY_ZLIB) $(BUILD)/zlib
	cd $(BUILD)/zlib && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) -j$(THREADS) libz.a CFLAGS=-O4 CXXFLAGS=-O4 SFLAGS=-O4
	$(RSYNC) $(BUILD)/zlib/zlib.h $(SDK)/usr/include/
	$(RSYNC) $(BUILD)/zlib/libz.a $(SDK)/usr/lib/

# Library VGL
libvgl:
	$(RSYNC) avm2_env/usr/ $(BUILD)/lib/
	cd $(BUILD)/lib/src/lib/libvgl && $(BMAKE) -j$(THREADS) libvgl.a
	rm -f $(SDK)/usr/lib/libvgl.a
	$(AR) $(SDK)/usr/lib/libvgl.a $(BUILD)/lib/src/lib/libvgl/*.o

# Library JPEG
libjpeg:
	rm -rf $(BUILD)/libjpeg
	mkdir -p $(BUILD)/libjpeg
	cd $(BUILD)/libjpeg && PATH=$(SDK)/usr/bin:$(PATH) CC=$(FLASCC_CC) CXX=$(FLASCC_CXX) CFLAGS=-O4 CXXFLAGS=-O4 $(SRCROOT)/$(DEPENDENCY_JPEG)/configure \
		--prefix=$(SDK)/usr --disable-shared --build=$(BUILD_TRIPLE) --host=$(TRIPLE) --target=$(TRIPLE)
	cd $(BUILD)/libjpeg && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) -j$(THREADS) libjpeg.la && PATH=$(SDK)/usr/bin:$(PATH) CC=$(FLASCC_CC) CXX=$(FLASCC_CXX) $(MAKE) install-libLTLIBRARIES install-includeHEADERS
	cp -f $(BUILD)/libjpeg/jconfig.h $(SDK)/usr/include/
	rm -f $(SDK)/usr/lib/libjpeg.so
	rm -f $(SDK)/usr/bin/avm2-unknown-freebsd8-jpegtran
	rm -f $(SDK)/usr/bin/avm2-unknown-freebsd8-rdjpgcom
	rm -f $(SDK)/usr/bin/avm2-unknown-freebsd8-wrjpgcom
	rm -f $(SDK)/usr/bin/avm2-unknown-freebsd8-cjpeg
	rm -f $(SDK)/usr/bin/avm2-unknown-freebsd8-djpeg

# Library PNG
libpng:
	rm -rf $(BUILD)/libpng
	mkdir -p $(BUILD)/libpng
	cd $(BUILD)/libpng && PATH=$(SDK)/usr/bin:$(PATH) CC=$(FLASCC_CC) CXX=$(FLASCC_CXX) CFLAGS=-O4 CXXFLAGS=-O4 $(SRCROOT)/libpng-1.5.7/configure \
		--prefix=$(SDK)/usr --disable-shared --build=$(BUILD_TRIPLE) --host=$(TRIPLE) --target=$(TRIPLE) --disable-dependency-tracking
	cd $(BUILD)/libpng && PATH=$(SDK)/usr/bin:$(PATH) CC=$(FLASCC_CC) CXX=$(FLASCC_CXX) $(MAKE) -j$(THREADS) && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install
	rm -f $(SDK)/usr/bin/libpng-config
	cp -f $(SDK)/usr/bin/libpng15-config $(SDK)/usr/bin/libpng-config
	rm -f $(SDK)/usr/lib/libpng.a
	cp -f $(SDK)/usr/lib/libpng15.a $(SDK)/usr/lib/libpng.a

# Library SDL
libsdl:
	rm -rf $(BUILD)/libsdl
	mkdir -p $(BUILD)/libsdl
	cd $(BUILD)/libsdl && PATH='$(SDK)/usr/bin:$(PATH)' CC=$(FLASCC_CC) CXX=$(FLASCC_CXX) CFLAGS=-O4 CXXFLAGS=-O4 $(SRCROOT)/$(DEPENDENCY_SDL)/configure \
		--host=$(TRIPLE) --prefix=$(SDK)/usr --disable-pthreads --disable-alsa --disable-video-x11 \
		--disable-cdrom --disable-loadso --disable-assembly --disable-esd --disable-arts --disable-nas \
		--disable-nasm --disable-altivec --disable-dga --disable-screensaver --disable-sdl-dlopen \
		--disable-directx --enable-joystick --enable-video-vgl --enable-static --disable-shared
	rm $(BUILD)/libsdl/config.status
	cd $(BUILD)/libsdl && PATH='$(SDK)/usr/bin:$(PATH)' $(MAKE) -j$(THREADS)
	cd $(BUILD)/libsdl && PATH='$(SDK)/usr/bin:$(PATH)' $(MAKE) install
	$(MAKE) libsdl-install
	rm $(SDK)/usr/include/SDL/SDL_opengl.h

# TBD
libsdl-install:
	cp $(SRCROOT)/tools/sdl-config $(SDK)/usr/bin/. # install our custom sdl-config
	chmod a+x $(SDK)/usr/bin/sdl-config

# Debug Memory Allocations
dmalloc:
	rm -rf $(BUILD)/dmalloc
	mkdir -p $(BUILD)/dmalloc
	cd $(BUILD)/dmalloc && PATH=$(SDK)/usr/bin:$(PATH) CFLAGS=" -O3 " CXXFLAGS=" -O3 " CC=$(FLASCC_CC) CXX=$(FLASCC_CXX) $(SRCROOT)/$(DEPENDENCY_DMALLOC)/configure \
		--prefix=$(SDK)/usr --disable-shared --enable-static --build=$(BUILD_TRIPLE) --host=$(TRIPLE) --target=$(TRIPLE)
	cd $(BUILD)/dmalloc && PATH=$(SDK)/usr/bin:$(PATH) CC=$(FLASCC_CC) CXX=$(FLASCC_CXX) $(MAKE) -j1 threads cxx
	cd $(BUILD)/dmalloc && PATH=$(SDK)/usr/bin:$(PATH) CC=$(FLASCC_CC) CXX=$(FLASCC_CXX) $(MAKE) -j1 installcxx installth
	cd $(BUILD)/dmalloc && PATH=$(SDK)/usr/bin:$(PATH) CC=$(FLASCC_CC) CXX=$(FLASCC_CXX) $(MAKE) -j1 heavy

# Foreign Function Interface
# TODO: error: no member names 'trampoline_table_entry' in 'ffi_closure'
libffi:
	mkdir -p $(BUILD)/libffi
	cd $(BUILD)/libffi && PATH=$(SDK)/usr/bin:$(PATH) CC=$(FLASCC_CC) CXX=$(FLASCC_CXX) CFLAGS="-I$(SDK)/usr/include" CXXFLAGS="-I$(SDK)/usr/include" $(SRCROOT)/$(DEPENDENCY_FFI)/configure \
		--prefix=$(SDK)/usr --disable-shared --enable-static --build=$(BUILD_TRIPLE) --host=$(TRIPLE) --target=$(TRIPLE)
	cd $(BUILD)/libffi && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install

# TBD
libfficheck:
	cd $(BUILD)/libffi/testsuite && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) check

# TBD
libiconv:
	mkdir -p $(BUILD)/libiconv
	cd $(BUILD)/libiconv && PATH=$(SDK)/usr/bin:$(PATH) CFLAGS=" -O3 " CXXFLAGS=" -O3 " CC=$(FLASCC_CC) CXX=$(FLASCC_CXX) $(SRCROOT)/$(DEPENDENCY_ICONV)/configure \
		--prefix=$(SDK)/usr
	cd $(BUILD)/libiconv && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) install

# TBD
dejagnu:
	mkdir -p $(BUILD)/dejagnu
	cd $(BUILD)/dejagnu && PATH=$(SDK)/usr/bin:$(PATH) CC=$(FLASCC_CC) CXX=$(FLASCC_CXX) $(SRCROOT)/$(DEPENDENCY_DEJAGNU)/configure \
		--prefix=$(BUILD)/dejagnu
	cd $(BUILD)/dejagnu && $(MAKE) install

# TBD
cxxfiltmingw:
	# install the version of mingw for osx from ere: http://crossgcc.rts-software.org/doku.php
	rm -rf $(BUILD)/cxxfiltmingw
	mkdir -p $(BUILD)/cxxfiltmingw
	cd $(BUILD)/cxxfiltmingw && CC=$(SRCROOT)/mingwmac/sdk/usr/bin/$(MINGWTRIPLE)-gcc \
		AR=$(SRCROOT)/mingwmac/sdk/usr/bin/$(MINGWTRIPLE)-ar  CXX=$(SRCROOT)/mingwmac/sdk/usr/bin/$(MINGWTRIPLE)-g++ \
		CFLAGS="-I$(SRCROOT)/avm2_env/misc/ $(DBGOPTS) -DMINGW_MONOCLE_HACKS " \
		CXXFLAGS="-I$(SRCROOT)/avm2_env/misc/ $(DBGOPTS) -DMINGW_MONOCLE_HACKS " $(SRCROOT)/binutils/configure \
		--disable-doc --disable-gold --disable-ld --disable-plugins \
		--build=$(BUILD_TRIPLE) --host=$(MINGWTRIPLE) --target=$(MINGWTRIPLE) \
		--program-prefix="" --disable-werror \
		--enable-targets=$(TRIPLE)
	-cd $(BUILD)/cxxfiltmingw && $(MAKE) -j$(THREADS)
	-cd $(BUILD)/cxxfiltmingw && $(SRCROOT)/mingwmac/sdk/usr/bin/$(MINGWTRIPLE)-gcc -DMINGW_MONOCLE_HACKS  \
		-I$(BUILD)/cxxfiltmingw/binutils -I$(BUILD)/cxxfiltmingw/bfd -I$(SRCROOT)/binutils/include  \
		-I$(BUILD)/cxxfiltmingw/intl $(SRCROOT)/binutils/binutils/cxxfilt.c  \
		$(BUILD)/cxxfiltmingw/libiberty/*.o $(BUILD)/cxxfiltmingw/intl/*.o $(BUILD)/cxxfiltmingw/binutils/version.o \
		$(BUILD)/cxxfiltmingw/binutils/bucomm.o  $(BUILD)/cxxfiltmingw/bfd/*.o -o c++filt.exe

# TBD
libtoabc:
	mkdir -p $(BUILD)/libtoabc/`basename $(LIB)`
	cd $(BUILD)/libtoabc/`basename $(LIB)` && $(SDK)/usr/bin/ar x $(LIB)
	@abcdir=$(BUILD)/libtoabc/`basename $(LIB)` ; \
	numos=`find $$abcdir -maxdepth 1 -name '*.o' | wc -l` ; \
	if [$$numos -gt 0 ] ; then \
	cd $(BUILD)/libtoabc/`basename $(LIB)` && cp -f $(SRCROOT)/avm2_env/misc/abcarchive.mk Makefile && SDK=$(SDK) $(MAKE) LLCOPTS=-jvm="$(JAVA)" -j$(THREADS) ; \
	fi 

# TODO: Solve Error
builtinabcs:
	cd $(SRCROOT)/avmplus/core && ./builtin.py
	cd $(SRCROOT)/avmplus/shell && ./shell_toplevel.py

# ====================================================================================
# EXTRA TOOLS
# ====================================================================================

# TBD
genfs:
	rm -rf $(BUILD)/zlib-native
	mkdir -p $(BUILD)/zlib-native
	$(RSYNC) $(SRCROOT)/$(DEPENDENCY_ZLIB)/ $(BUILD)/zlib-native
	cd $(BUILD)/zlib-native && PATH=$(SDK)/usr/bin:$(PATH) CC=$(FLASCC_CC) CXX=$(FLASCC_CXX) ./configure \
		--static && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) 
	cd $(BUILD)/zlib-native/contrib/minizip/ && PATH=$(SDK)/usr/bin:$(PATH) CC=$(FLASCC_CC) CXX=$(FLASCC_CXX) $(MAKE) 
	PATH=$(SDK)/usr/bin:$(PATH) $(FLASCC_CC) -Wall -I$(BUILD)/zlib-native/contrib/minizip -o $(SDK)/usr/bin/genfs$(EXEEXT) $(BUILD)/zlib-native/contrib/minizip/zip.o $(BUILD)/zlib-native/contrib/minizip/ioapi.o $(BUILD)/zlib-native/libz.a $(SRCROOT)/tools/vfs/genfs.c

# SWIG Phase 0

# LD Flags
SWIG_LDFLAGS=-L$(BUILD)/llvm-debug/lib
# Libs
SWIG_LIBS=-lLLVMAVM2Info -lLLVMAVM2CodeGen -lLLVMAVM2AsmParser -lLLVMAsmPrinter -lLLVMMCParser -lLLVMSelectionDAG -lLLVMCodeGen -lLLVMTarget -lLLVMMC -lLLVMScalarOpts -lLLVMTransformUtils -lLLVMAnalysis -lLLVMCore -lLLVMSupport
SWIG_LIBS+= -lclangEdit -lclangFrontend -lclangCodeGen -lclangDriver -lclangParse -lclangSema -lclangAnalysis -lclangLex -lclangAST -lclangBasic -lclangSerialization
SWIG_LIBS+= $(GCCLANGFLAG)
# C++ Flags
SWIG_CXXFLAGS=-I$(SRCROOT)/avm2_env/misc/
#SWIG_CXXFLAGS+= -I$(BUILD)/llvm-debug/lib/clang/$(DEPENDENCY_LLVM_VERSION)/include/
SWIG_CXXFLAGS+= -I$(SRCROOT)/$(DEPENDENCY_LLVM)/include -I$(BUILD)/llvm-debug/include 
SWIG_CXXFLAGS+= -I$(SRCROOT)/$(DEPENDENCY_LLVM)/tools/clang/include -I$(BUILD)/llvm-debug/tools/clang/include
SWIG_CXXFLAGS+= -I$(SRCROOT)/$(DEPENDENCY_LLVM)/tools/clang/lib
SWIG_CXXFLAGS+= -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS -fno-rtti -Wno-long-long 
#SWIG_CXXFLAGS+= -g

# SWIG Phase 1
swig-clean:
	rm -rf $(BUILD)/swig
	mkdir -p $(BUILD)/swig

# SWIG Phase 2
swig-pcre:
	cp -f packages/$(DEPENDENCY_SWIG_PCRE).tar.gz $(BUILD)/swig
	cd $(BUILD)/swig && PATH=$(SDK)/usr/bin:$(PATH) CC=$(FLASCC_CC) CXX=$(FLASCC_CXX) $(SRCROOT)/$(DEPENDENCY_SWIG)/Tools/pcre-build.sh \
		--build=$(BUILD_TRIPLE) --host=$(HOST_TRIPLE) --target=$(HOST_TRIPLE)

# SWIG Phase 3
swig-configure:
	cd $(BUILD)/swig && PATH=$(SDK)/usr/bin:$(PATH) CC=$(FLASCC_CC) CXX=$(FLASCC_CXX) CFLAGS=-g LDFLAGS="$(SWIG_LDFLAGS)" LIBS="$(SWIG_LIBS)" CXXFLAGS="$(SWIG_CXXFLAGS)" $(SRCROOT)/$(DEPENDENCY_SWIG)/configure \
		--prefix=$(SDK)/usr --disable-ccache --without-maximum-compile-warnings --build=$(BUILD_TRIPLE) --host=$(HOST_TRIPLE) --target=$(HOST_TRIPLE)

# SWIG Phase 4
swig-build:
	rm -rf $(SDK)/usr/lib/clang
	cp -R $(BUILD)/llvm-debug/lib/clang $(SDK)/usr/lib/clang
	cd $(BUILD)/swig && PATH=$(SDK)/usr/bin:$(PATH) CC=$(FLASCC_CC) CXX=$(FLASCC_CXX) $(MAKE) && $(MAKE) install

# SWIG All
swig:
	$(MAKE) swig-clean
	$(MAKE) swig-pcre
	$(MAKE) swig-configure
	$(MAKE) swig-build

# SWIG Tests
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

# TBD
swigtestsautomation:
	cd $(SRCROOT)/qa/swig/framework && $(MAKE) SWIG_SOURCE=$(SRCROOT)/$(DEPENDENCY_SWIG)

# TBD
gdb:
	rm -rf $(BUILD)/$(DEPENDENCY_GDB)
	mkdir -p $(BUILD)/$(DEPENDENCY_GDB)
	cd $(BUILD)/$(DEPENDENCY_GDB) && CFLAGS="-I$(SRCROOT)/avm2_env/misc -Wno-error" $(SRCROOT)/$(DEPENDENCY_GDB)/configure \
		--build=$(BUILD_TRIPLE) --host=$(HOST_TRIPLE) --target=avm2-elf && $(MAKE) -j$(THREADS)
	cp -f $(BUILD)/$(DEPENDENCY_GDB)/gdb/gdb$(EXEEXT) $(SDK)/usr/bin/
	cp -f $(SRCROOT)/tools/flascc.gdb $(SDK)/usr/share/
	cp -f $(SRCROOT)/tools/flascc-run.gdb $(SDK)/usr/share/
	cp -f $(SRCROOT)/tools/flascc-init.gdb $(SDK)/usr/share/

# TBD
pkgconfig:
	rm -rf $(BUILD)/pkgconfig
	mkdir -p $(BUILD)/pkgconfig
	cd $(BUILD)/pkgconfig && CFLAGS="-I$(SRCROOT)/avm2_env/misc" $(SRCROOT)/$(DEPENDENCY_PKG_CFG)/configure \
		--build=$(BUILD_TRIPLE) --host=$(HOST_TRIPLE) --target=$(TRIPLE) --prefix=$(SDK)/usr --disable-shared --disable-dependency-tracking
	cd $(BUILD)/pkgconfig && $(MAKE) -j$(THREADS) && $(MAKE) install

# TBD
libtool:
	rm -rf $(BUILD)/libtool
	mkdir -p $(BUILD)/libtool
	cd $(BUILD)/libtool && CC=gcc CXX=g++ $(SRCROOT)/$(DEPENDENCY_LIBTOOL)/configure \
		--build=$(BUILD_TRIPLE) --host=$(HOST_TRIPLE) --target=$(TRIPLE) \
		--prefix=$(SDK)/usr --enable-static --disable-shared --disable-ltdl-install
	cd $(BUILD)/libtool && $(MAKE) -j$(THREADS) && $(MAKE) install-exec

# ====================================================================================
# FINALCLEANUP
# ====================================================================================

# TBD
finalcleanup:
	rm -f $(SDK)/usr/lib/*.la
	rm -rf $(SDK)/usr/share/aclocal $(SDK)/usr/share/doc $(SDK)/usr/share/man $(SDK)/usr/share/info
ifneq (,$(findstring darwin,$(PLATFORM)))
	@$(LN) ../../share $(SDK)/usr/platform/darwin/share
endif
	$(RSYNC) $(SRCROOT)/tools/swf-info.py $(SDK)/usr/bin/
	$(RSYNC) $(SRCROOT)/tools/projector-dis.py $(SDK)/usr/bin/
	$(RSYNC) $(SRCROOT)/tools/swfdink.py $(SDK)/usr/bin/
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
# Submit tests
# ====================================================================================
    
# TBD
aliastest:
	mkdir -p $(BUILD)/aliastest
	cd $(BUILD)/aliastest && $(SDK)/usr/bin/llvm-as $(SRCROOT)/test/aliastest.ll -o aliastest.bc
	cd $(BUILD)/aliastest && $(SDK)/usr/bin/llc -jvm=$(JAVA) aliastest.bc -filetype=asm -o aliastest.s

# TBD
pthreadsubmittests_shell: pthreadsubmittests_shell_compile pthreadsubmittests_shell_run

# TBD
pthreadsubmittests_shell_compile:
	@rm -rf $(BUILD)/pthreadsubmit_shell
	@mkdir -p $(BUILD)/pthreadsubmit_shell
	cd $(BUILD)/pthreadsubmit_shell && $(SDK)/usr/bin/$(FLASCC_CC) -O4 -pthread -save-temps $(SRCROOT)/test/pthread_test.c -o pthread_test_optimized
	cd $(BUILD)/pthreadsubmit_shell && $(SDK)/usr/bin/$(FLASCC_CC) -O0 -pthread -save-temps $(SRCROOT)/test/pthread_test.c -o pthread_test

# TBD
pthreadsubmittests_shell_run:
	cd $(BUILD)/pthreadsubmit_shell && ./pthread_test_optimized
	cd $(BUILD)/pthreadsubmit_shell && ./pthread_test

# TBD
pthreadsubmittests_swf:
	@rm -rf $(BUILD)/pthreadsubmit_swf
	@mkdir -p $(BUILD)/pthreadsubmit_swf
	cd $(BUILD)/pthreadsubmit_swf && $(SDK)/usr/bin/$(FLASCC_CC) -O4 -emit-swf -pthread -save-temps $(SRCROOT)/test/pthread_test.c -o pthread_test_optimized.swf
	cd $(BUILD)/pthreadsubmit_swf && $(SDK)/usr/bin/$(FLASCC_CC) -O4 -pthread -save-temps $(SRCROOT)/test/pthread_test.c -emit-swc=com.adobe.flascc -o pthread_test_optimized.swc
	cd $(BUILD)/pthreadsubmit_swf && $(SDK)/usr/bin/$(FLASCC_CC) -O0 -emit-swf -pthread -save-temps $(SRCROOT)/test/pthread_test.c -o pthread_test.swf
	cp -f $(BUILD)/pthreadsubmit_swf/*.swf $(BUILDROOT)/extra/

# TBD
pthreadtests:
	@rm -rf $(BUILD)/pthread$(SWFDIR)
	@mkdir -p $(BUILD)/pthread$(SWFDIR)
	cd $(BUILD)/pthread$(SWFDIR) && $(SDK)/usr/bin/$(FLASCC_CC) -O4 -pthread $(EMITSWF) $(SWFVER) -save-temps $(SRCROOT)/test/pthread_cancel.c -o pthread_cancel$(SWFEXT)
	cd $(BUILD)/pthread$(SWFDIR) && $(SDK)/usr/bin/$(FLASCC_CC) -O4 -pthread $(EMITSWF) $(SWFVER) -save-temps $(SRCROOT)/test/pthread_async_cancel.c -o pthread_async_cancel$(SWFEXT)
	cd $(BUILD)/pthread$(SWFDIR) && $(SDK)/usr/bin/$(FLASCC_CC) -O4 -pthread $(EMITSWF) $(SWFVER) -save-temps $(SRCROOT)/test/pthread_create.c -o pthread_create$(SWFEXT)
	cd $(BUILD)/pthread$(SWFDIR) && $(SDK)/usr/bin/$(FLASCC_CC) -O4 -pthread $(EMITSWF) $(SWFVER) -save-temps $(SRCROOT)/test/pthread_create_test.c -o pthread_create_test$(SWFEXT)
	cd $(BUILD)/pthread$(SWFDIR) && $(SDK)/usr/bin/$(FLASCC_CC) -O4 -pthread $(EMITSWF) $(SWFVER) -save-temps $(SRCROOT)/test/pthread_mutex_test.c -o pthread_mutex_test$(SWFEXT)
	cd $(BUILD)/pthread$(SWFDIR) && $(SDK)/usr/bin/$(FLASCC_CC) -O4 -pthread $(EMITSWF) $(SWFVER) -save-temps $(SRCROOT)/test/pthread_mutex_test2.c -o pthread_mutex_test2$(SWFEXT)
	cd $(BUILD)/pthread$(SWFDIR) && $(SDK)/usr/bin/$(FLASCC_CC) -O4 -pthread $(EMITSWF) $(SWFVER) -save-temps $(SRCROOT)/test/pthread_malloc_test.c -o pthread_malloc_test$(SWFEXT)
	cd $(BUILD)/pthread$(SWFDIR) && $(SDK)/usr/bin/$(FLASCC_CC) -O4 -pthread $(EMITSWF) $(SWFVER) -save-temps $(SRCROOT)/test/pthread_specific.c -o pthread_specific$(SWFEXT)
	cd $(BUILD)/pthread$(SWFDIR) && $(SDK)/usr/bin/$(FLASCC_CC) -O4 -pthread $(EMITSWF) $(SWFVER) -save-temps $(SRCROOT)/test/pthread_suspend.c -o pthread_suspend$(SWFEXT)
	cd $(BUILD)/pthread$(SWFDIR) && $(SDK)/usr/bin/$(FLASCC_CC) -O4 -pthread $(EMITSWF) $(SWFVER) -save-temps $(SRCROOT)/test/thr_kill.c -o thr_kill$(SWFEXT)
	cd $(BUILD)/pthread$(SWFDIR) && $(SDK)/usr/bin/$(FLASCC_CC) -O4 -pthread $(EMITSWF) $(SWFVER) -save-temps $(SRCROOT)/test/peterson.c -o peterson$(SWFEXT)
	cd $(BUILD)/pthread$(SWFDIR) && $(SDK)/usr/bin/$(FLASCC_CC) -O4 -pthread $(EMITSWF) $(SWFVER) -save-temps -DORDER_STRENGTH=1 $(SRCROOT)/test/peterson.c -o peterson_nofence$(SWFEXT)
	$(MAKE) as3++tests

# TBD
conctests:
	mkdir -p $(BUILD)/conc$(SWFDIR)
	cd $(BUILD)/conc$(SWFDIR) && $(SDK)/usr/bin/$(FLASCC_CC) -O4 $(EMITSWF) $(SWFVER) -save-temps $(SRCROOT)/test/newThread.c -o newThread$(SWFEXT)
	cd $(BUILD)/conc$(SWFDIR) && $(SDK)/usr/bin/$(FLASCC_CC) -O4 -pthread $(EMITSWF) $(SWFVER) -save-temps $(SRCROOT)/test/avm2_conc.c -o avm2_conc$(SWFEXT)
	cd $(BUILD)/conc$(SWFDIR) && $(SDK)/usr/bin/$(FLASCC_CC) -O4 -pthread $(EMITSWF) $(SWFVER) -save-temps $(SRCROOT)/test/avm2_mutex.c -o avm2_mutex$(SWFEXT)
	cd $(BUILD)/conc$(SWFDIR) && $(SDK)/usr/bin/$(FLASCC_CC) -O4 -pthread $(EMITSWF) $(SWFVER) -save-temps $(SRCROOT)/test/avm2_mutex2.c -o avm2_mutex2$(SWFEXT)

# TBD
helloswf:
	@rm -rf $(BUILD)/helloswf
	@mkdir -p $(BUILD)/helloswf
	cd $(BUILD)/helloswf && $(SDK)/usr/bin/$(FLASCC_CC) -c -g -O0 $(SRCROOT)/test/hello.c -emit-llvm -o hello.bc
	cd $(BUILD)/helloswf && $(SDK)/usr/bin/llc -jvm="$(JAVA)" hello.bc -o hello.abc -filetype=obj
	cd $(BUILD)/helloswf && $(SDK)/usr/bin/llc -jvm="$(JAVA)" hello.bc -o hello.as -filetype=asm
	cd $(BUILD)/helloswf && $(SDK)/usr/bin/$(FLASCC_CC) -emit-swf -swf-size=200x200 -O0 -g hello.abc -o hello.swf

# TBD
helloswf_opt:
	@rm -rf $(BUILD)/helloswf_opt
	@mkdir -p $(BUILD)/helloswf_opt
	cd $(BUILD)/helloswf_opt && $(SDK)/usr/bin/$(FLASCC_CC) -emit-swf -swf-size=200x200 -O4 $(SRCROOT)/test/hello.c -o hello-opt.swf

# TBD
hellocpp_shell:
	@rm -rf $(BUILD)/hellocpp_shell
	@mkdir -p $(BUILD)/hellocpp_shell
	cd $(BUILD)/hellocpp_shell && $(SDK)/usr/bin/$(FLASCC_CXX) -g -O0 $(SRCROOT)/test/hello.cpp -o hello-cpp && ./hello-cpp

# TBD
hellocpp_swf:
	@rm -rf $(BUILD)/hellocpp_swf
	@mkdir -p $(BUILD)/hellocpp_swf
	cd $(BUILD)/hellocpp_swf && $(SDK)/usr/bin/$(FLASCC_CXX) -emit-swf -swf-size=200x200 -O0 $(SRCROOT)/test/hello.cpp -o hello-cpp.swf

# TBD
hellocpp_swf_opt:
	@rm -rf $(BUILD)/hellocpp_swf_opt
	@mkdir -p $(BUILD)/hellocpp_swf_opt
	cd $(BUILD)/hellocpp_swf_opt && $(SDK)/usr/bin/$(FLASCC_CXX) -emit-swf -swf-size=200x200 -O4 $(SRCROOT)/test/hello.cpp -o hello-cpp-opt.swf

# TBD
as3++tests:
	@rm -rf $(BUILD)/as3++_swf
	@mkdir -p $(BUILD)/as3++_swf
	cd $(BUILD)/as3++_swf && $(SDK)/usr/bin/$(FLASCC_CXX) -O4 -emit-swf -pthread -save-temps $(SRCROOT)/test/AS3++mt.cpp -lAS3++ -o AS3++mt.swf
	cd $(BUILD)/as3++_swf && $(SDK)/usr/bin/$(FLASCC_CXX) -O4 -emit-swf -pthread -save-temps $(SRCROOT)/test/AS3++mt1.cpp -lAS3++ -o AS3++mt1.swf
	cd $(BUILD)/as3++_swf && $(SDK)/usr/bin/$(FLASCC_CXX) -O4 -emit-swf -pthread -save-temps $(SRCROOT)/test/AS3++mt2.cpp -lAS3++ -o AS3++mt2.swf
	cd $(BUILD)/as3++_swf && $(SDK)/usr/bin/$(FLASCC_CXX) -O4 -emit-swf -pthread -save-temps $(SRCROOT)/test/AS3++mt3.cpp -lAS3++ -o AS3++mt3.swf

# TBD
posixtest:
	@rm -rf $(BUILD)/posixtest
	@mkdir -p $(BUILD)/posixtest
	$(SDK)/usr/bin/genfs --name my.test.BackingStore $(SRCROOT)/test/zipfsroot $(BUILD)/posixtest/alcfs
	cd $(BUILD)/posixtest && $(SCOMPFALCON) \
		-import $(call nativepath,$(SDK)/usr/lib/BinaryData.abc) \
		-import $(call nativepath,$(SDK)/usr/lib/playerglobal.abc) \
		-import $(call nativepath,$(SDK)/usr/lib/ISpecialFile.abc) \
		-import $(call nativepath,$(SDK)/usr/lib/IBackingStore.abc) \
		-import $(call nativepath,$(SDK)/usr/lib/IVFS.abc) \
		-import $(call nativepath,$(SDK)/usr/lib/AlcVFSZip.abc) \
		-import $(call nativepath,$(SDK)/usr/lib/InMemoryBackingStore.abc) \
		-import $(call nativepath,$(SDK)/usr/lib/PlayerKernel.abc) \
		$(call nativepath, $(BUILD)/posixtest/alcfsBackingStore.as) -outdir . -out alcfs
	cd $(BUILD)/posixtest && $(SDK)/usr/bin/$(FLASCC_CC) -emit-swf -O0 -swf-version=15 $(call nativepath,$(SDK)/usr/lib/AlcVFSZip.abc) alcfs.abc $(SRCROOT)/test/fileio.c -o posixtest.swf

# TBD
scimark:
	@mkdir -p $(BUILD)/scimark
	cd $(BUILD)/scimark && $(SDK)/usr/bin/$(FLASCC_CC) -O4 $(SRCROOT)/scimark2_1c/*.c -o scimark2 -save-temps
	$(BUILD)/scimark/scimark2 &> $(BUILD)/scimark/result.txt

# TBD
scimark_swf:
	@mkdir -p $(BUILD)/scimark_swf
	cd $(BUILD)/scimark_swf && $(SDK)/usr/bin/$(FLASCC_CC) -O4 -swf-version=17 $(SRCROOT)/scimark2_1c/*.c -emit-swf -swf-size=400x400 -o scimark2.swf
	cd $(BUILD)/scimark_swf && $(SDK)/usr/bin/$(FLASCC_CC) -O4 $(SRCROOT)/scimark2_1c/*.c -emit-swf -swf-size=400x400 -o scimark2v18.swf
	cp -f $(BUILD)/scimark_swf/*.swf $(BUILDROOT)/extra/

# TBD
scimark_asc:
	@mkdir -p $(BUILD)/scimark_asc
	cd $(BUILD)/scimark_asc && $(SDK)/usr/bin/$(FLASCC_CC) -O4 $(SRCROOT)/scimark2_1c/*.c -o scimark2 -save-temps
	cd $(BUILD)/scimark_asc && $(SDK)/usr/bin/$(FLASCC_CC) -O4 $(SRCROOT)/scimark2_1c/*.c -emit-swf -swf-size=400x400 -o scimark2.swf
	$(BUILD)/scimark_asc/scimark2 &> $(BUILD)/scimark_asc/result.txt

# TBD
parse_scimark_log:
	$(MAKE) scimark
	ant -f qa/performance/build.xml -Dbuild=$(FLASCC_VERSION_BUILD) \
		-DsendResults=true -Dbranch=mainline -DresultsFile=$(BUILD)/scimark/result.txt -DresultsFileFalcon=$(BUILD)/scimark/result.txt

# TBD
sjljtest:
	@mkdir -p $(BUILD)/sjljtest
	cd $(BUILD)/sjljtest && $(SDK)/usr/bin/$(FLASCC_CXX) -O0 $(SRCROOT)/test/sjljtest.c -o sjljtest -save-temps
	$(BUILD)/sjljtest/sjljtest &> $(BUILD)/sjljtest/result.txt
	diff --strip-trailing-cr $(BUILD)/sjljtest/result.txt $(SRCROOT)/test/sjljtest.expected.txt

# TBD
sjljtest_opt:
	@mkdir -p $(BUILD)/sjljtest_opt
	cd $(BUILD)/sjljtest_opt && $(SDK)/usr/bin/$(FLASCC_CC) -O4 $(SRCROOT)/test/sjljtest.c -o sjljtest -save-temps
	$(BUILD)/sjljtest_opt/sjljtest &> $(BUILD)/sjljtest_opt/result.txt
	diff --strip-trailing-cr $(BUILD)/sjljtest_opt/result.txt $(SRCROOT)/test/sjljtest.expected.txt

# TBD
ehtest:
	@mkdir -p $(BUILD)/ehtest
	cd $(BUILD)/ehtest && $(SDK)/usr/bin/$(FLASCC_CXX) -O0 $(SRCROOT)/test/ehtest.cpp -o ehtest -save-temps
	-$(BUILD)/ehtest/ehtest &> $(BUILD)/ehtest/result.txt
	diff --strip-trailing-cr $(BUILD)/ehtest/result.txt $(SRCROOT)/test/ehtest.expected.txt

# TBD
ehtest_opt:
	@mkdir -p $(BUILD)/ehtest_opt
	cd $(BUILD)/ehtest_opt && $(SDK)/usr/bin/$(FLASCC_CXX) -O4 $(SRCROOT)/test/ehtest.cpp -o ehtest -save-temps
	-$(BUILD)/ehtest_opt/ehtest &> $(BUILD)/ehtest_opt/result.txt
	diff --strip-trailing-cr $(BUILD)/ehtest_opt/result.txt $(SRCROOT)/test/ehtest.expected.txt

# TODO: Not in build
ehtest_asc:
	@mkdir -p $(BUILD)/ehtest_asc
	cd $(BUILD)/ehtest_asc && $(SDK)/usr/bin/$(FLASCC_CXX) -O0 $(SRCROOT)/test/ehtest.cpp -o ehtest -save-temps
	-$(BUILD)/ehtest_asc/ehtest &> $(BUILD)/ehtest_asc/result.txt
	diff --strip-trailing-cr $(BUILD)/ehtest_asc/result.txt $(SRCROOT)/test/ehtest.expected.txt

	cd $(BUILD)/ehtest_asc && $(SDK)/usr/bin/$(FLASCC_CXX) -O4 $(SRCROOT)/test/ehtest.cpp -o ehtest -save-temps
	-$(BUILD)/ehtest_asc/ehtest &> $(BUILD)/ehtest_asc/result.txt
	diff --strip-trailing-cr $(BUILD)/ehtest_asc/result.txt $(SRCROOT)/test/ehtest.expected.txt

# TBD
as3interoptest:
	@mkdir -p $(BUILD)/as3interoptest
	cd $(BUILD)/as3interoptest && $(SDK)/usr/bin/$(FLASCC_CXX) -O4 $(SRCROOT)/test/as3interoptest.c -o as3interoptest -save-temps
	$(BUILD)/as3interoptest/as3interoptest &> $(BUILD)/as3interoptest/result.txt

# TBD
symboltest:
	mkdir -p $(BUILD)/symboltest
	cd $(BUILD)/symboltest && $(SDK)/usr/bin/llvm-as $(SRCROOT)/test/symboltest.ll -o symboltest.bc
	cd $(BUILD)/symboltest && $(SDK)/usr/bin/llc -jvm=$(JAVA) symboltest.bc -filetype=asm -o symboltest.s
	cd $(BUILD)/symboltest && $(SDK)/usr/bin/llc -jvm=$(JAVA) symboltest.bc -filetype=obj -o symboltest.abc

	cd $(BUILD)/symboltest && $(SDK)/usr/bin/nm symboltest.abc | grep symbolTest > syms.abc.txt
	cd $(BUILD)/symboltest && $(SDK)/usr/bin/nm symboltest.bc | grep symbolTest > syms.bc.txt
	diff --strip-trailing-cr $(BUILD)/symboltest/*.txt

# TBD
samples:
	cd samples && PATH=$(SDK)/usr/bin:$(PATH) $(MAKE) FLASCC=$(SDK) FLEX=$(FLEX) -j$(THREADS)
	mkdir -p $(BUILDROOT)/extra
	find samples -iname "*.swf" -exec cp -f '{}' $(BUILDROOT)/extra/ \;

# TBD
# TODO: Not in build
gdbunit:
	ant $(MAKE) -f qa/gdbunit/build.xml -Dalchemy.dir=$(SDK)/../ -Ddebugplayer="$(PLAYER)" -Dflex.dir=$(SRCROOT)/tools/flex -Dgbdunit.halt.on.first.failure=false -Dgdbunit.excludes=**/quake.input -Dswfversion=17
	ant $(MAKE) -f qa/gdbunit/build.xml -Dalchemy.dir=$(SDK)/../ -Ddebugplayer="$(PLAYER)" -Dflex.dir=$(SRCROOT)/tools/flex -Dgbdunit.halt.on.first.failure=false -Dgdbunit.excludes=**/quake.input -Dswfversion=18

# TBD
# TODO: Not in build
vfstests:
	@cd qa/vfs/framework && $(MAKE) FLASCC=$(FLASCC)

# TBD
# TODO: Not in build
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

# TBD
# TODO: Undefined references in libthr (___sys_swapcontext, _check_suspend)
avm2_ui_thunk_test:
	@mkdir -p $(BUILD)/avm2_ui_thunk_test
	java -jar $(call nativepath,$(SDK)/usr/lib/asc2.jar) -merge -md -AS3 -strict -optimize \
		-import $(call nativepath,$(SDK)/usr/lib/builtin.abc) -import $(call nativepath,$(SDK)/usr/lib/playerglobal.abc) \
		-import $(call nativepath,$(SDK)/usr/lib/ISpecialFile.abc) -import $(call nativepath,$(SDK)/usr/lib/IBackingStore.abc) \
		-import $(call nativepath,$(SDK)/usr/lib/IVFS.abc) -import $(call nativepath,$(SDK)/usr/lib/InMemoryBackingStore.abc) \
		-import $(call nativepath,$(SDK)/usr/lib/AlcVFSZip.abc) -import $(call nativepath,$(SDK)/usr/lib/CModule.abc) \
		-import $(call nativepath,$(SDK)/usr/lib/C_Run.abc) -import $(call nativepath,$(SDK)/usr/lib/BinaryData.abc) \
		-import $(call nativepath,$(SDK)/usr/lib/PlayerKernel.abc) \
		test/avm2_ui_thunk.as -config CONFIG::BACKGROUND=false -config CONFIG::ASYNC=true -outdir $(call nativepath,$(BUILD)/avm2_ui_thunk_test) -out ConsoleAsync

	java -jar $(call nativepath,$(SDK)/usr/lib/asc2.jar) -merge -md -AS3 -strict -optimize \
		-import $(call nativepath,$(SDK)/usr/lib/builtin.abc) -import $(call nativepath,$(SDK)/usr/lib/playerglobal.abc) \
		-import $(call nativepath,$(SDK)/usr/lib/ISpecialFile.abc) -import $(call nativepath,$(SDK)/usr/lib/IBackingStore.abc) \
		-import $(call nativepath,$(SDK)/usr/lib/IVFS.abc) -import $(call nativepath,$(SDK)/usr/lib/InMemoryBackingStore.abc) \
		-import $(call nativepath,$(SDK)/usr/lib/AlcVFSZip.abc) -import $(call nativepath,$(SDK)/usr/lib/CModule.abc) \
		-import $(call nativepath,$(SDK)/usr/lib/C_Run.abc) -import $(call nativepath,$(SDK)/usr/lib/BinaryData.abc) \
		-import $(call nativepath,$(SDK)/usr/lib/PlayerKernel.abc) \
		test/avm2_ui_thunk.as -config CONFIG::BACKGROUND=true -config CONFIG::ASYNC=false -outdir $(call nativepath,$(BUILD)/avm2_ui_thunk_test) -out ConsoleBackground

	cd $(BUILD)/avm2_ui_thunk_test && $(SDK)/usr/bin/$(FLASCC_CC) -pthread $(SRCROOT)/test/avm2_ui_thunk.c -symbol-abc=ConsoleBackground.abc -emit-swf -o avm2_ui_thunk_background.swf
	cd $(BUILD)/avm2_ui_thunk_test && $(SDK)/usr/bin/$(FLASCC_CC) -pthread $(SRCROOT)/test/avm2_ui_thunk.c -symbol-abc=ConsoleAsync.abc -emit-swf -o avm2_ui_thunk_async.swf
	cp -f $(BUILD)/avm2_ui_thunk_test/*.swf $(BUILDROOT)/extra/

# TODO: Source not found
ieeetests_conversion:
	rm -rf $(BUILD)/ieeetests_conversion
	mkdir -p $(BUILD)/ieeetests_conversion
	$(RSYNC) $(SRCROOT)/test/IeeeCC754/ $(BUILD)/ieeetests_conversion
	echo "b\nb\na" > $(BUILD)/ieeetests_conversion/answers
	cd $(BUILD)/ieeetests_conversion && PATH=$(SDK)/usr/bin:$(PATH) ./dotests.sh < answers

# TODO: Source not found
ieeetests_basicops:
	rm -rf $(BUILD)/ieeetests_basicops
	mkdir -p $(BUILD)/ieeetests_basicops
	$(RSYNC) $(SRCROOT)/test/IeeeCC754/ $(BUILD)/ieeetests_basicops
	echo "a\nb\na" > $(BUILD)/ieeetests_basicops/answers
	cd $(BUILD)/ieeetests_basicops && PATH=$(SDK)/usr/bin:$(PATH) ./dotests.sh < answers

# ====================================================================================
# DEPLOY
# ====================================================================================

deliverables:
	rm -rf $(BUILDROOT)/staging
	mkdir -p $(BUILDROOT)/staging
	$(RSYNC) $(SDK) $(BUILDROOT)/staging/
	$(RSYNC) $(SRCROOT)/samples $(BUILDROOT)/staging/
	$(RSYNC) $(SRCROOT)/README.html $(BUILDROOT)/staging/
	$(RSYNC) $(SRCROOT)/docs $(BUILDROOT)/staging/
	$(RSYNC) $(BUILDROOT)/apidocs $(BUILDROOT)/staging/docs/
	rm -f $(BUILDROOT)/staging/sdk/usr/bin/gccbug*
	find $(BUILDROOT)/staging/ | grep "\.DS_Store$$" | xargs rm -f 
	echo $(FLASCC_VERSION_BUILD) > $(BUILDROOT)/staging/sdk/ver.txt
# Flatten symlinks
	find $(BUILDROOT)/staging/sdk -type l | xargs rm
	$(RSYNC) $(BUILDROOT)/staging/sdk/usr/platform/*/ $(BUILDROOT)/staging/sdk/usr
	rm -rf $(BUILDROOT)/staging/sdk/usr/platform
ifneq (,$(findstring cygwin,$(PLATFORM)))
	cd $(BUILDROOT)/staging/ && zip -qr $(BUILDROOT)/$(SDKNAME).zip *
	find $(BUILDROOT)/staging > $(BUILDROOT)/zipcontents.txt
else
	rm -f $(BUILDROOT)/$(SDKNAME).*.dmg 
	cp -f $(SRCROOT)/tools/Base.dmg $(BUILDROOT)/$(SDKNAME).tmp.dmg
	chmod u+rw $(BUILDROOT)/$(SDKNAME).tmp.dmg
	hdiutil resize -size 1G $(BUILDROOT)/$(SDKNAME).tmp.dmg
	hdiutil attach $(BUILDROOT)/$(SDKNAME).tmp.dmg -readwrite -mountpoint $(BUILDROOT)/dmgmount
	rm -f $(BUILDROOT)/staging/.DS_Store
	$(RSYNC) $(BUILDROOT)/staging/ $(BUILDROOT)/dmgmount/
	mv $(BUILDROOT)/dmgmount/.fseventsd $(BUILDROOT)/
	hdiutil detach $(BUILDROOT)/dmgmount
	hdiutil convert $(BUILDROOT)/$(SDKNAME).tmp.dmg -format UDZO -imagekey zlib-level=9 -o $(BUILDROOT)/$(SDKNAME).dmg
	rm -f $(BUILDROOT)/$(SDKNAME).tmp.dmg
	find $(BUILDROOT)/staging > $(BUILDROOT)/dmgcontents.txt
endif

# ====================================================================================
# Examples
# ====================================================================================

.PHONY: bmake posix binutils docs gcc samples libcxx libcxxrt libxxabi libunwind libgcceh
