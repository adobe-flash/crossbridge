# ====================================================================================
# CrossBridge Makefile
# ====================================================================================

$?UNAME=$(shell uname -s)

$?SRCROOT=$(FLASCC_ROOT)
$?SDK=$(SRCROOT)/sdk-msys
$?BUILD=$(SRCROOT)/build-msys

$?PLATFORM=mingw
$?TRIPLE=avm2-unknown-freebsd8
$?EXEEXT=.exe
$?SOEXT=.dll
$?SDLFLAGS=
$?TAMARIN_CONFIG_FLAGS=--target=i686-linux
$?TAMARINLDFLAGS=" -Wl,--stack,16000000"
$?SDKEXT=.zip
$?PLATFORM_NAME=win
$?HOST_TRIPLE=i686-mingw32
$?BUILD_TRIPLE=i686-mingw32

$?CC=mingw32-gcc
$?CXX=mingw32-g++
$?NATIVE_AR=mingw32-ar
export CC:=$(CC)
export CXX:=$(CXX)

$?FLASCC_VERSION_MAJOR:=1
$?FLASCC_VERSION_MINOR:=0
$?FLASCC_VERSION_PATCH:=2
$?FLASCC_VERSION_BUILD:=devbuild
$?SDKNAME=Crossbridge_$(FLASCC_VERSION_MAJOR).$(FLASCC_VERSION_MINOR).$(FLASCC_VERSION_PATCH).$(FLASCC_VERSION_BUILD)
BUILD_VER_DEFS"-DFLASCC_VERSION_MAJOR=$(FLASCC_VERSION_MAJOR) -DFLASCC_VERSION_MINOR=$(FLASCC_VERSION_MINOR) -DFLASCC_VERSION_PATCH=$(FLASCC_VERSION_PATCH) -DFLASCC_VERSION_BUILD=$(FLASCC_VERSION_BUILD)"

all:
	@echo "~~~ Crossbridge $(SDKNAME) ~~~"
	@echo "User: $(UNAME)"
	@echo "Platform: $(PLATFORM)"
	@echo "Build: $(BUILD)"
	@echo "Triple: $(TRIPLE)"
	@echo "Host Triple: $(HOST_TRIPLE)"
	@echo "Build Triple: $(BUILD_TRIPLE)"
	# Clean
	@rm -rf $(BUILD)
	@rm -rf $(SDK)
	# Init
	@mkdir -p $(BUILD)/logs
	@mkdir -p $(SDK)/usr/bin
	@mkdir -p $(SDK)/usr/lib
	# Make
	@mkdir -p $(BUILD)/make/
	cp -r $(SRCROOT)/make-3.82/* $(BUILD)/make/
	cd $(BUILD)/make && CC=$(CC) CXX=$(CXX) ./configure --prefix=$(SDK)/usr --program-prefix="" \
		--build=$(BUILD_TRIPLE) --host=$(HOST_TRIPLE) --target=$(TRIPLE)
	cd $(BUILD)/make && CC=$(CC) CXX=$(CXX) $(MAKE) -j$(THREADS)
	cd $(BUILD)/make && CC=$(CC) CXX=$(CXX) $(MAKE) install

diagnostics:
	@$(CC) --version
	@$(CXX) --version
	@$(NATIVE_AR) --version
	@$(MAKE) --version