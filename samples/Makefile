all: check T01 T02 T03 T04 T05 T06 T07 T08 T09 T10 T12

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

include Makefile.common
