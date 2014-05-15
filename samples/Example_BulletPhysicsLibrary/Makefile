#
# =BEGIN MIT LICENSE
# 
# The MIT License (MIT)
#
# Copyright (c) 2014 The CrossBridge Team
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# 
# =END MIT LICENSE
#

.PHONY: debug clean all 

BULLETDIR:=bullet-2.80-rev2531
#EXTRA_CFLAGS:=-DUSE_PTHREADS
#EXTRA_OPTS:=-pthread
#EXTRA_LIBS:=-lBulletMultiThreaded
EXTRA_CFLAGS:=
EXTRA_OPTS:=
EXTRA_LIBS:=

all: check
	@echo "-------- Example: Bullet --------"
	
	mkdir -p build
	cd build && PATH="$(call unixpath,$(FLASCC)/usr/bin):$(PATH)" CC=gcc CXX=g++ CFLAGS="$(OPT_CFLAGS) $(BASE_CFLAGS) $(EXTRACFLAGS)" CXXFLAGS="$(OPT_CFLAGS) $(BASE_CFLAGS) $(EXTRACFLAGS)" cmake \
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
	$(ASC2) -import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) -import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) build/Bullet.as

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
	#make away3dswf

basicswf:
	# Link the Bullet SWC with a basic sample app
	"$(FLEX)/bin/mxmlc" -library-path=build/Bullet.swc bulletdemo.as -debug=$(MXMLC_DEBUG) -o build/bulletdemo.swf

# Fails with undefined method createThreadedDispatcher and createThreadedSolver
away3dswf:
	# Link the Bullet SWC with a more advanced Away3D based example
	cd Away3DExample && "$(FLEX)/bin/mxmlc" \
		-debug=$(MXMLC_DEBUG) -strict \
		-library-path+=../build/Bullet.swc \
		-library-path+=away3d-core-fp11_4_0_9_gold.swc \
		-swf-version=18 \
		BulletPhysicsTest.as -o BulletPhysicsTest.swf

include ../Makefile.common

clean:
	rm -rf build
	rm -f *.swf *.swc *.abc *.bc Away3dExample/BulletPhysicsTest.swf
