all: check
	make allsamples

.PHONY: Example_Lua Example_BulletPhysicsLibrary Example_libqrencode Example_Quake1 Example_Box2D

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
	@cd 12_Stage3D && make clean
	
	@cd Example_Lua && make clean
	@cd Example_BulletPhysicsLibrary && make clean
	@cd Example_libqrencode && make clean
	@cd Example_Quake1 && make clean
	@cd Example_Box2D && make clean

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
T12:
	cd 12_Stage3D && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"

Example_Lua:
	@cd Example_Lua && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"
Example_BulletPhysicsLibrary:
	@cd Example_BulletPhysicsLibrary && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"
Example_libqrencode:
	@cd Example_libqrencode && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"
Example_Quake1:
	@cd Example_Quake1 && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"
Example_Box2D:
	@cd Example_Box2D && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"


allsamples: T01 T02 T03 T04 T05 T06 T07 T08 T09 T10 T12
	echo "done!"

examples: Example_Lua Example_BulletPhysicsLibrary Example_libqrencode Example_Quake1 Example_Box2D
	echo "done!"

include Makefile.common
