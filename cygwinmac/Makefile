SRCROOT:=$(PWD)
TRIPLE:=i686-pc-cygwin
BUILD:=$(SRCROOT)/build
SDK:=$(SRCROOT)/sdk
THREADS:=$(shell sysctl -n hw.ncpu)

all:
	make base
	make binutils
	make gcc
	make fixup

clean:
	rm -rf $(SDK) $(BUILD)

base:
	mkdir -p $(SDK)/usr/bin
	mkdir -p $(SDK)/usr/lib
	mkdir -p $(BUILD)

	cd $(SDK) && tar -zxf $(SRCROOT)/packages/cygwin-1.7.9-1.tar.bz2
	cd $(SDK) && tar -zxf $(SRCROOT)/packages/w32api-3.17-2.tar.bz2
	cd $(SDK) && tar -zxf $(SRCROOT)/packages/libuuid-devel-2.17.2-1.tar.bz2
	cd $(SDK) && tar -zxf $(SRCROOT)/packages/libuuid1-2.17.2-1.tar.bz2
	cd $(SDK) && tar -zxf $(SRCROOT)/packages/libstdc++6-devel-4.5.3-3.tar.bz2
	cd $(SDK) && tar -zxf $(SRCROOT)/packages/gcc4-core-4.5.3-3.tar.bz2
	cd $(SDK) && tar -zxf $(SRCROOT)/packages/gcc4-g++-4.5.3-3.tar.bz2
	cd $(SDK) && tar -zxf $(SRCROOT)/packages/libgmp-devel-4.3.2-1.tar.bz2
	cd $(SDK) && tar -zxf $(SRCROOT)/packages/libgmp3-4.3.2-1.tar.bz2
	cd $(SDK) && tar -zxf $(SRCROOT)/packages/libmpfr-devel-3.0.1-1.tar.bz2
	cd $(SDK) && tar -zxf $(SRCROOT)/packages/libmpfr4-3.0.1-1.tar.bz2
	cd $(SDK) && tar -zxf $(SRCROOT)/packages/libmpfr1-2.4.1-4.tar.bz2
	cd $(SDK) && tar -zxf $(SRCROOT)/packages/libmpfr0-4.1.4-4.tar.bz2
	cd $(SDK) && tar -zxf $(SRCROOT)/packages/libncurses-devel-5.7-18.tar.bz2
	cd $(SDK) && tar -zxf $(SRCROOT)/packages/zlib-devel-1.2.5-1.tar.bz2
	cd $(SDK) && tar -zxf $(SRCROOT)/packages/gettext-devel-0.18.1.1-2.tar.bz2
	cd $(SDK) && tar -zxf $(SRCROOT)/packages/gettext-0.18.1.1-2.tar.bz2
	cd $(SDK) && tar -zxf $(SRCROOT)/packages/libintl8-0.18.1.1-2.tar.bz2
	cd $(SDK) && tar -zxf $(SRCROOT)/packages/libiconv-1.14-2.tar.bz2
	cd $(SDK) && tar -zxf $(SRCROOT)/packages/libiconv2-1.14-2.tar.bz2
	cd $(SDK) && tar -zxf $(SRCROOT)/packages/libglib2.0-devel-2.30.2-1.tar.bz2

	find $(SDK) | grep "\.exe$$" | xargs rm
	
binutils:
	rm -rf $(BUILD)/binutils-2.22.51-1

	cd $(BUILD) && tar -zxf $(SRCROOT)/packages/binutils-2.22.51-1-src.tar.bz2
	cd $(BUILD)/binutils-2.22.51-1 && ./configure \
		--disable-doc \
		--target=$(TRIPLE) --with-sysroot=$(SDK)/usr \
		--program-prefix="i686-pc-cygwin-" --prefix=$(SDK)/usr --disable-werror \
		--enable-targets=$(TRIPLE)

	cd $(BUILD)/binutils-2.22.51-1 && make && make install

gcc:
	rm -rf $(BUILD)/gcc*
	mkdir $(BUILD)/gcc
	cd $(BUILD)/gcc && tar -zxf $(SRCROOT)/packages/gcc4-4.5.3-3-src.tar.bz2
	cd $(BUILD) && tar -zxf gcc/gcc-4.5.3.tar.bz2

	# If this fails, try "sudo port install libmpc mpfr gmp libiconv ossp-uuid"

	cd $(BUILD)/gcc-4.5.3 && CFLAGS="-L/opt/local/lib" ./configure --enable-languages=c,c++ \
		--datadir=$(SDK)/usr/share --infodir=$(SDK)/usr/share/info \
		--mandir=$(SDK)/usr/share/man -v --with-gmp=/opt/local --with-mpfr=/opt/local --with-mpc=/opt/local \
		--enable-bootstrap --enable-version-specific-runtime-libs \
		--libexecdir=$(SDK)/usr/lib --enable-static \
		--enable-shared --enable-shared-libgcc --disable-__cxa_atexit \
		--with-gnu-ld --with-gnu-as --with-dwarf2 --disable-sjlj-exceptions \
		--disable-symvers \
		--disable-libgomp --disable-multilib --disable-libada --enable-threads=posix \
		--with-arch=i686 --with-tune=generic --program-prefix="$(TRIPLE)-" \
		--prefix=$(SDK)/usr --with-build-sysroot=$(SDK)/ --target=$(TRIPLE)

	cd $(BUILD)/gcc-4.5.3 && make -j$(THREADS) all-gcc
	cd $(BUILD)/gcc-4.5.3 && make install-gcc

fixup:
	mv -f $(SDK)/usr/lib/*.* $(SDK)/usr/lib/gcc/i686-pc-cygwin/4.5.3/
	mv -f $(SDK)/usr/lib/w32api/* $(SDK)/usr/lib/gcc/i686-pc-cygwin/4.5.3/
	rm $(SDK)/usr/lib/gcc/i686-pc-cygwin/4.5.3/*.la

	cp $(SDK)/usr/include/ncurses/*.h $(SDK)/usr/include/
	mkdir -p $(SDK)/usr/i686-pc-cygwin/include/c++
	cd $(SDK)/usr/i686-pc-cygwin/include/c++ && ln -sfh ../../../lib/gcc/i686-pc-cygwin/4.5.3/include/c++ 4.5.3
	cd $(SDK)/usr/i686-pc-cygwin && ln -sfh ../include sys-include
	mkdir -p $(SDK)/usr/altbin
	cd $(SDK)/usr/altbin && ln -sfh ../bin/$(TRIPLE)-gcc gcc
	cd $(SDK)/usr/altbin && ln -sfh ../bin/$(TRIPLE)-g++ g++
	cd $(SDK)/usr/altbin && ln -sfh ../bin/$(TRIPLE)-gcc gcc-4.2
	cd $(SDK)/usr/altbin && ln -sfh ../bin/$(TRIPLE)-g++ g++-4.2
	cd $(SDK)/usr/altbin && ln -sfh ../bin/$(TRIPLE)-ar ar
	cd $(SDK)/usr/altbin && ln -sfh ../bin/$(TRIPLE)-nm nm
	cd $(SDK)/usr/altbin && ln -sfh ../bin/$(TRIPLE)-ranlib ranlib

	cp libintl-hack.h $(SDK)/usr/include/libintl.h
