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

.PHONY: Example_Lua Example_BulletPhysicsLibrary Example_libqrencode Example_Quake1 Example_Box2D

all: check
	make allsamples

T01:
	cd 01_HelloWorld && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"
T02:
	cd 02_Interop && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"
T03:
	cd 03_Drawing && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"
T04:
	cd 04_Animation && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"
T05:
	cd 05_SWC && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"
T06:
	cd 06_SWIG && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"
T07:
	cd 07_FileSystem && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"
T08:
	cd 08_GDB && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"
T09:
	cd 09_Pthreads && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"
T10:
	cd 10_MessageChannel && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"
T11:
	cd 11_OpenMP && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"
T12:
	cd 12_Stage3D && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"

E01:
	@cd Example_GLS3D && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"
E02:
	@cd Example_Box2D && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"
E03:
	@cd Example_BulletPhysicsLibrary && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"
E04:
	@cd Example_freeglut && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"
E05:
	@cd Example_Lua && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"
E06:
	@cd Example_pthreads && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"

allsamples: T01 T02 T03 T04 T05 T06 T07 T08 T09 T10 T11 T12
	echo "done!"

examples: E01 E02 E03 E04 E05 E06
	echo "done!"

include Makefile.common

clean:
	@cd 01_HelloWorld && make clean
	@cd 02_Interop && make clean
	@cd 03_Drawing && make clean
	@cd 04_Animation && make clean
	@cd 05_SWC && make clean
	@cd 06_SWIG && make clean
	@cd 07_FileSystem && make clean
	@cd 08_GDB && make clean
	@cd 09_Pthreads && make clean
	@cd 10_MessageChannel && make clean
	@cd 11_OpenMP && make clean
	@cd 12_Stage3D && make clean
	
	@cd Example_GLS3D && make clean
	@cd Example_GLSL2AGAL && make clean
	@cd Example_Box2D && make clean
	@cd Example_BulletPhysicsLibrary && make clean
	@cd Example_freeglut && make clean
	@cd Example_Lua && make clean
	@cd Example_dosbox && make clean
	@cd Example_libqrencode && make clean
	@cd Example_neverball && make clean
	@cd Example_pthreads && make clean
	@cd Example_Quake1 && make clean
	@cd Example_Quake3 && make clean
