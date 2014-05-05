TPF: check
	@echo "-------- PathFinder Example --------"

	-rm PathFinderWrapper.abc
	-rm PathFinder.swc
	-rm exports.txt
	-rm PathFinderAS3Project/libs/PathFinder.swc

	@echo "\n->Create LLVM bitcode of just our interface to get symbols"
	"$(FLASCC)/usr/bin/g++" -O4 -c as3api.cpp

	@echo "\n->Copy the base flascc symbols need to run"
	cp -f flascc_exports.txt exports.txt

	@echo "\n->Pull out symbols of the LLVM bitcode of our interface "
	"$(FLASCC)/usr/bin/nm" as3api.o | grep " T " | awk '{print $$3}' | sed 's/__/_/' >> exports.txt

	@echo "-> Compile the AS3 wrapper class to ABC"
	$(AS3COMPILERARGS) -abcfuture -AS3 -import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) -import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) PathFinderWrapper.as

	@echo "\n->Now compile a SWC and demo SWF"
	"$(FLASCC)/usr/bin/g++" -O4 -flto-api=exports.txt PathFinderWrapper.abc micropather.cpp pathfinder.cpp as3api.cpp main.cpp -emit-swc=com.renaun.flascc -o PathFinder.swc

	cp PathFinder.swc PathFinderAS3Project/libs/

include ../Makefile.common

clean:
	rm -f *.swf *.swc *.abc PathFinderAS3Project/libs/PathFinder.swc *.bc *.exe exports.txt *.o
