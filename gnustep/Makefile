SDK=$(PWD)/../sdk

all: make base

install: base_install

check: base_check

clean: make_clean base_clean

make: make_config make_compile

base: base_config base_compile

make_clean:
	cd core/make && PATH=$(SDK)/usr/bin:$(PATH) make clean

base_clean:
	cd core/base && PATH=$(SDK)/usr/bin:$(PATH) make clean

make_config:
	cd core/make && PATH=$(SDK)/usr/bin:$(PATH) ./configure --prefix=$(SDK)/usr/GNUstep --with-layout=gnustep --with-config-file=$(SDK)/GNUstep/Local/Configuration/GNUstep.conf CC='gcc -pthread -flto' shared=no

make_compile:
	cd core/make && PATH=$(SDK)/usr/bin:$(PATH) make CC='gcc -pthread -flto' shared=no
	cd core/make && PATH=$(SDK)/usr/bin:$(PATH) make CC='gcc -pthread -flto' install shared=no

base_config:
	cd core/base && $(SDK)/usr/bin/ar scru $(SDK)/usr/lib/libpthread.a
	cd core/base && . $(SDK)/usr/GNUstep/System/Library/Makefiles/GNUstep.sh && PATH=$(SDK)/usr/bin_shim:$(SDK)/usr/bin:$(PATH) ./configure --with-default-config=$(SDK)/GNUstep/Local/Configuration/GNUstep.conf --enable-static --disable-shared --disable-do --disable-openssl --disable-xml --disable-iconv --enable-libffi --with-ffi-include=$(SDK)/usr/lib/libffi-3.0.11/include --with-ffi-library=$(SDK)/usr/lib --with-sort-algorithm=shellsort --disable-tls --disable-icu LDFLAGS='-lBlocksRuntime -x c++ /Users/scottpetersen/dev/alchemy/posix/avm2_tramp.cpp -x objective-c -fgnu-runtime' CPPFLAGS='-DOVERRIDE__BUILTIN_APPLY_XXX=1 -include /Users/scottpetersen/dev/alchemy/avm2_env/usr/include/AS3/AVM2.h' --prefix=$(SDK)/usr CC='gcc -pthread -flto' shared=no

base_compile:
	cd core/base && . $(SDK)/usr/GNUstep/System/Library/Makefiles/GNUstep.sh && PATH=$(SDK)/usr/bin_shim:$(SDK)/usr/bin:$(PATH) make CC='gcc -pthread -flto' OBJ_MERGE_CMD='llvm-link -o $$(GNUSTEP_OBJ_DIR)/$$(SUBPROJECT_PRODUCT) $$^ ;' shared=no

base_install:
	cd core/base && . $(SDK)/usr/GNUstep/System/Library/Makefiles/GNUstep.sh && PATH=$(SDK)/usr/bin_shim:$(SDK)/usr/bin:$(PATH) make install CC='gcc -pthread -flto' shared=no

base_check:
	cd core/base && . $(SDK)/usr/GNUstep/System/Library/Makefiles/GNUstep.sh && PATH=$(SDK)/usr/bin_shim:$(SDK)/usr/bin:$(PATH) make check CC='gcc -pthread -flto -save-temps ' FND_LIBS='-Xlinker --whole-archive -lgnustep-base -Xlinker --no-whole-archive' OBJ_MERGE_CMD='llvm-link -o $$(GNUSTEP_OBJ_DIR)/$$(SUBPROJECT_PRODUCT) $$^ ;' shared=no

#TODO
#redundant CC=? (using it in make_config enough?)
#redundant shared=no?


