SRCROOT:=$(PWD)
TRIPLE:=i686-mingw32
HOST:=x86_64-apple-darwin10
GCCVER=4.7.0
BUILD:=$(SRCROOT)/build
SDK:=$(SRCROOT)/sdk
THREADS:=$(shell sysctl -n hw.ncpu)

COPTS:="-march=core2 -mfpmath=sse"

all:
	make base
	make binutils
	make gcc
	make w32api
	make mingwrt
	make libcpp

clean:
	rm -rf $(SDK) $(BUILD)

base:
	mkdir -p $(SDK)/usr/bin
	mkdir -p $(SDK)/usr/lib
	mkdir -p $(BUILD)
	find $(SDK) | grep "\.exe$$" | xargs rm -f
	
binutils:
	rm -rf $(BUILD)/binutils-2.22-1-mingw32-src

	cd $(BUILD) && lzmadec $(SRCROOT)/packages/binutils-2.22-1-mingw32-src.tar.lzma | tar -xf -
	cd $(BUILD)/binutils-2.22-1-mingw32-src && tar -zxf binutils-2.22.tar.bz2

	cd $(BUILD)/binutils-2.22-1-mingw32-src/binutils-2.22 && ./configure \
		--disable-doc --with-gcc --with-gnu-as --with-gnu-ld --disable-shared --disable-nls \
		--target=$(TRIPLE) --with-sysroot=$(SDK)/usr \
		--program-prefix="$(TRIPLE)-" --prefix=$(SDK)/usr --disable-werror \
		--enable-targets=$(TRIPLE)

	cd $(BUILD)/binutils-2.22-1-mingw32-src/binutils-2.22 && make && make install


mingwrt:
	cd $(BUILD) && lzmadec $(SRCROOT)/packages/mingwrt-3.20-2-mingw32-src.tar.lzma | tar -xf -
	cd $(BUILD) && lzmadec $(SRCROOT)/packages/w32api-3.17-2-mingw32-src.tar.lzma | tar -xf -
	mv $(BUILD)/w32api-3.17-2-mingw32 $(BUILD)/w32api

	cd $(BUILD)/mingwrt-3.20-2-mingw32 && CFLAGS=$(COPTS) CXXFLAGS=$(COPTS) ./configure --target=$(TRIPLE) --prefix=$(SDK)/usr
	cd $(BUILD)/mingwrt-3.20-2-mingw32 && PATH=$(SDK)/usr/bin:$(PATH) CFLAGS=$(COPTS) CXXFLAGS=$(COPTS) make && make install

w32api:
	cd $(BUILD) && lzmadec $(SRCROOT)/packages/w32api-3.17-2-mingw32-src.tar.lzma | tar -xf -

	cd $(BUILD)/w32api-3.17-2-mingw32 && PATH=$(SDK)/usr/bin:$(PATH) CFLAGS=$(COPTS) CXXFLAGS=$(COPTS) ./configure \
		--target=$(TRIPLE) --prefix=$(SDK)/usr
	cd $(BUILD)/w32api-3.17-2-mingw32 && PATH=$(SDK)/usr/bin:$(PATH) CFLAGS=$(COPTS) CXXFLAGS=$(COPTS) make && make install


msyscore:
	cd $(BUILD) && lzmadec $(SRCROOT)/packages/msysCORE-1.0.17-1-msys-1.0.17-src.tar.lzma | tar -xf -

	mkdir -p $(BUILD)/msysbuild
	cd $(BUILD)/msysbuild && PATH=$(SDK)/usr/bin:$(PATH) ../source/configure --target=$(TRIPLE) --host=i686-apple-darwin10 --prefix=$(SDK)/usr
	cd $(BUILD)/msysbuild && PATH=$(SDK)/usr/bin:$(PATH) make && make install

blah:
	cd $(BUILD)/gcc-$(GCCVER) && CC=gcc CPP=cpp make -j$(THREADS) all-target-libgcc

gcc:
	rm -rf $(BUILD)/gcc*

	cd $(BUILD) && lzmadec $(SRCROOT)/packages/gcc-$(GCCVER)-1-mingw32-src.tar.lzma | tar -xf -
	cd $(BUILD) && lzmadec $(SRCROOT)/packages/mingwrt-3.20-2-mingw32-src.tar.lzma | tar -xf -

	cd $(BUILD) && tar -zxf gcc-4.7.0-1-mingw32-src/gcc-$(GCCVER).tar.bz2
	mkdir -p $(SDK)/mingw/include

	# If this fails, try "sudo port install libmpc mpfr gmp libiconv ossp-uuid"

	# --enable-bootstrap --with-dwarf2 --disable-sjlj-exceptions --disable-multilib  --enable-threads=posix --enable-shared --disable-__cxa_atexit --disable-symvers  --disable-multilib --enable-version-specific-runtime-libs
	mv $(BUILD)/gcc-$(GCCVER) $(BUILD)/gcc-$(GCCVER)-src
	mkdir -p $(BUILD)/gcc-$(GCCVER)

	# --with-gnu-ld --with-gnu-as --disable-shared --disable-multilib --disable-libgomp --disable-libada
	cd $(BUILD)/gcc-$(GCCVER) && CFLAGS_FOR_TARGET=$(COPTS) CXXFLAGS_FOR_TARGET=$(COPTS) CC=gcc CPP=cpp CFLAGS="-L/opt/local/lib" $(BUILD)/gcc-$(GCCVER)-src/configure --enable-languages=c,c++ \
		--datadir=$(SDK)/usr/share --infodir=$(SDK)/usr/share/info \
		--mandir=$(SDK)/usr/share/man -v --with-gmp=/opt/local --with-mpfr=/opt/local --with-mpc=/opt/local \
		--libexecdir=$(SDK)/usr/lib --disable-multilib --disable-libada --disable-shared \
		--program-prefix="$(TRIPLE)-" \
		--prefix=$(SDK)/usr --with-build-sysroot=$(SDK)/ --target=$(TRIPLE) --host=$(HOST) --build=$(HOST)

	cd $(BUILD)/gcc-$(GCCVER) && CC=gcc CPP=cpp make -j$(THREADS) all-gcc
	cd $(BUILD)/gcc-$(GCCVER) && make install-gcc

	cd $(BUILD) && lzmadec $(SRCROOT)/packages/w32api-3.17-2-mingw32-src.tar.lzma | tar -xf -
	cd $(BUILD) && lzmadec $(SRCROOT)/packages/mingwrt-3.20-2-mingw32-src.tar.lzma | tar -xf -
	cp -R $(BUILD)/w32api-3.17-2-mingw32/include/ $(SDK)/usr/include/
	cp -R $(BUILD)/mingwrt-3.20-2-mingw32/include/ $(SDK)/usr/include/

	mkdir -p $(SDK)/usr/i686-mingw32/include/c++
	#cd $(SDK)/usr/i686-mingw32/include/c++ && ln -sfh ../../../lib/gcc/i686-mingw32/$(GCCVER)/include/c++ $(GCCVER)
	cd $(SDK)/usr/i686-mingw32 && ln -sfh ../include sys-include

	cd $(BUILD)/gcc-$(GCCVER) && CC=gcc CPP=cpp make -j$(THREADS) all-target-libgcc
	cd $(BUILD)/gcc-$(GCCVER) && make install-target-libgcc

	cd $(SDK)/usr/bin && ln -sf i686-mingw32-gcc gcc
	cd $(SDK)/usr/bin && ln -sf i686-mingw32-ar ar
	cd $(SDK)/usr/bin && ln -sf i686-mingw32-as as
	cd $(SDK)/usr/bin && ln -sf i686-mingw32-nm nm
	cd $(SDK)/usr/bin && ln -sf i686-mingw32-ranlib ranlib
	cd $(SDK)/usr/bin && ln -sf i686-mingw32-objcopy objcopy
	cd $(SDK)/usr/bin && ln -sf i686-mingw32-g++ g++
	cd $(SDK)/usr/bin && ln -sf i686-mingw32-ld ld
	cd $(SDK)/usr/bin && ln -sf i686-mingw32-dlltool dlltool

libcpp:
	cp $(SDK)/usr/lib/crt2.o $(SDK)/usr/lib/gcc/$(TRIPLE)/$(GCCVER)/
	cp $(SDK)/usr/lib/crt2.o $(BUILD)/gcc-$(GCCVER)/gcc/
	cp $(SDK)/usr/lib/crt2.o $(BUILD)/gcc-$(GCCVER)/$(TRIPLE)/libgcc/

	cp $(SDK)/usr/lib/dllcrt2.o $(SDK)/usr/lib/gcc/$(TRIPLE)/$(GCCVER)/
	cp $(SDK)/usr/lib/dllcrt2.o $(BUILD)/gcc-$(GCCVER)/gcc/
	cp $(SDK)/usr/lib/dllcrt2.o $(BUILD)/gcc-$(GCCVER)/$(TRIPLE)/libgcc/

	cd $(BUILD)/gcc-$(GCCVER) && CC=gcc CPP=cpp make -j$(THREADS) all-target-libstdc++-v3
	cd $(BUILD)/gcc-$(GCCVER) && make install-target-libstdc++-v3

fixup:
	#mv -f $(SDK)/usr/lib/*.* $(SDK)/usr/lib/gcc/$(TRIPLE)/$(GCCVER)/
	#mv -f $(SDK)/usr/lib/w32api/* $(SDK)/usr/lib/gcc/$(TRIPLE)/$(GCCVER)/
	rm -f $(SDK)/usr/lib/gcc/$(TRIPLE)/$(GCCVER)/*.la

	#cp $(SDK)/usr/include/ncurses/*.h $(SDK)/usr/include/
	mkdir -p $(SDK)/usr/$(TRIPLE)/include/c++
	cd $(SDK)/usr/$(TRIPLE)/include/c++ && ln -sfh ../../../lib/gcc/$(TRIPLE)/$(GCCVER)/include/c++ $(GCCVER)
	cd $(SDK)/usr/$(TRIPLE) && ln -sfh ../include sys-include
	mkdir -p $(SDK)/usr/altbin
	cd $(SDK)/usr/altbin && ln -sfh ../bin/$(TRIPLE)-gcc gcc
	cd $(SDK)/usr/altbin && ln -sfh ../bin/$(TRIPLE)-g++ g++
	cd $(SDK)/usr/altbin && ln -sfh ../bin/$(TRIPLE)-gcc gcc-4.2
	cd $(SDK)/usr/altbin && ln -sfh ../bin/$(TRIPLE)-g++ g++-4.2
	cd $(SDK)/usr/altbin && ln -sfh ../bin/$(TRIPLE)-ar ar
	cd $(SDK)/usr/altbin && ln -sfh ../bin/$(TRIPLE)-nm nm
	cd $(SDK)/usr/altbin && ln -sfh ../bin/$(TRIPLE)-ranlib ranlib

	cp libintl-hack.h $(SDK)/usr/include/libintl.h
