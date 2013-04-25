BULLETDIR:=bullet-2.80-rev2531
#EXTRA_CFLAGS:=-DUSE_PTHREADS
#EXTRA_OPTS:=-pthread
#EXTRA_LIBS:=-lBulletMultiThreaded
EXTRA_CFLAGS:=
EXTRA_OPTS:=
EXTRA_LIBS:=

T09: check
	@echo "-------- Example: Bullet --------"
	
	mkdir -p build
	cd build && PATH="$(call unixpath,$(FLASCC)/usr/bin):$(PATH)" cmake \
		-DCMAKE_CXX_FLAGS="-fno-exceptions -fno-rtti -O4" \
		-DUSE_DOUBLE_PRECISION:BOOL=ON -DBUILD_EXTRAS:BOOL=OFF -DBUILD_DEMOS:BOOL=OFF  \
		../$(BULLETDIR)/

	make recompile

recompile:

	cd build && PATH="$(call unixpath,$(FLASCC)/usr/bin):$(PATH)" make -j8

	make swc

swc:
	# Generate the SWIG wrappers
	PATH="$(call unixpath,$(FLASCC)/usr/bin):$(PATH)" swig -as3 -package org.bulletphysics -c++ -DBT_USE_DOUBLE_PRECISION $(EXTRA_CFLAGS) -I$(BULLETDIR)/src -module Bullet bullet.i &> build/swig.log
	mv bullet_wrap.cxx build
	mv Bullet.as build
	
	# Compile the SWIG AS3 wrappers
	java -jar $(call nativepath,$(FLASCC)/usr/lib/asc2.jar) -import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) -import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) build/Bullet.as

	# Compile the SWIG C++ wrappers
	cd build && PATH="$(call unixpath,$(FLASCC)/usr/bin):$(PATH)" g++ $(BASE_CFLAGS) -DBT_USE_DOUBLE_PRECISION -I../$(BULLETDIR)/src \
	-Lsrc/BulletCollision -Lsrc/BulletDynamics -Lsrc/LinearMath -Lsrc/BulletMultiThreaded \
	bullet_wrap.cxx -O4 -c

	# protect the required symbols in the LTO exports file
	cp -f exports.txt build/
	chmod u+rw build/exports.txt
	PATH="$(call unixpath,$(FLASCC)/usr/bin):$(PATH)" nm build/bullet_wrap.o | grep " T " | awk '{print $$3}' | sed 's/__/_/' >> build/exports.txt

	# Link the final library
	cd build && PATH="$(call unixpath,$(FLASCC)/usr/bin):$(PATH)" g++ $(BASE_CFLAGS) -DBT_USE_DOUBLE_PRECISION -I../$(BULLETDIR)/src ../main.cpp \
	-Lsrc/BulletCollision -Lsrc/BulletDynamics -Lsrc/LinearMath -Lsrc/BulletMultiThreaded \
	bullet_wrap.o Bullet.abc \
	-Wl,--start-group \
	$(EXTRA_LIBS) -lBulletDynamics -lBulletCollision -lLinearMath \
	-Wl,--end-group \
	-emit-swc=org.bulletphysics -O4 -flto-api=exports.txt $(EXTRA_OPTS) -o Bullet.swc

	make basicswf
	make away3dswf

basicswf:
	# Link the Bullet SWC with a basic sample app
	"$(FLEX)/bin/mxmlc" \
		-compiler.omit-trace-statements=false \
		-static-link-runtime-shared-libraries \
		-library-path=build/Bullet.swc \
		bulletdemo.as -debug=false -o build/bulletdemo.swf

away3dswf:
	# Link the Bullet SWC with a more advanced Away3D based example
	cd Away3DExample && "$(FLEX)/bin/mxmlc" \
		-debug=false -strict \
		-compiler.omit-trace-statements=false \
		-static-link-runtime-shared-libraries \
		-library-path+=../build/Bullet.swc \
		-library-path+=away3d-core-fp11_4_0_9_gold.swc \
		-swf-version=18 \
		BulletPhysicsTest.as -o BulletPhysicsTest.swf

include ../Makefile.common

clean:
	rm -rf build
	rm -f *.swf *.swc *.abc *.bc Away3dExample/BulletPhysicsTest.swf
