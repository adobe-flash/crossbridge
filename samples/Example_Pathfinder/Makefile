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

.PHONY: clean all 

all: check
	@echo "-------- PathFinder Example --------"

	@echo "\n->Create LLVM bitcode of just our interface to get symbols"
	"$(FLASCC)/usr/bin/g++" -O4 -c as3api.cpp

	@echo "\n->Copy the base flascc symbols need to run"
	cp -f ../exports.txt exports.txt

	@echo "\n->Pull out symbols of the LLVM bitcode of our interface "
	"$(FLASCC)/usr/bin/nm" as3api.o | grep " T " | awk '{print $$3}' | sed 's/__/_/' >> exports.txt

	@echo "-> Compile the AS3 wrapper class to ABC"
	$(ASC2) -abcfuture -AS3 -import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) -import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) PathFinderWrapper.as

	@echo "\n->Now compile a SWC and demo SWF"
	"$(FLASCC)/usr/bin/g++" -O4 -flto-api=exports.txt PathFinderWrapper.abc micropather.cpp pathfinder.cpp as3api.cpp main.cpp -emit-swc=com.renaun.flascc -o PathFinder.swc

	cp PathFinder.swc PathFinderAS3Project/libs/

include ../Makefile.common

clean:
	rm -f *.swf *.swc *.abc PathFinderAS3Project/libs/PathFinder.swc *.bc *.exe exports.txt *.o
