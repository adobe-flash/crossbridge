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

T01: check
	@echo "-------- Sample 1 --------"
	@echo "Compiling C EXE ..."
	"$(FLASCC)/usr/bin/gcc" $(BASE_CFLAGS) hello.c -o hello.exe
	@echo "Compiling C SWF (Debug) ..."
	"$(FLASCC)/usr/bin/gcc" $(BASE_CFLAGS) hello.c -g -emit-swf -swf-version=$(SWF_VERSION) -swf-size=$(SWF_SIZE) -o hello-debug.swf
	@echo "Generating C Exports.txt ..."
	"$(FLASCC)/usr/bin/gcc" $(BASE_CFLAGS) -c hello.c -o hello.o
	cp -f ../exports.txt exports.txt
	"$(FLASCC)/usr/bin/nm" hello.o | grep ' T ' | sed 's/.*__/_/' | sed 's/.* T //' >> exports.txt
	@echo "Compiling C SWF (LTO) with save-temporary-files ..."
	"$(FLASCC)/usr/bin/gcc" $(BASE_CFLAGS) hello.c -flto-api=exports.txt -save-temps -emit-swf -swf-version=$(SWF_VERSION) -swf-size=$(SWF_SIZE) -o hello-lto.swf
	@echo "Compiling CPP EXE ..."
	"$(FLASCC)/usr/bin/g++" $(BASE_CFLAGS) hello.cpp -o hello-cpp.exe
	@echo "Compiling CPP SWF (Debug) ..."
	"$(FLASCC)/usr/bin/g++" $(BASE_CFLAGS) hello.cpp -g -emit-swf -swf-version=$(SWF_VERSION) -swf-size=$(SWF_SIZE) -o hello-cpp-debug.swf
	@echo "Generating CPP Exports.txt ..."
	"$(FLASCC)/usr/bin/g++" $(BASE_CFLAGS) -c hello.cpp -o hello-cpp.o
	cp -f ../exports.txt exports-cpp.txt
	"$(FLASCC)/usr/bin/nm" hello-cpp.o | grep ' T ' | sed 's/.*__/_/' | sed 's/.* T //' >> exports-cpp.txt
	@echo "Compiling CPP SWF (LTO) with save-temporary-files ..."
	"$(FLASCC)/usr/bin/g++" $(BASE_CFLAGS) hello.cpp -flto-api=exports-cpp.txt -save-temps -emit-swf -swf-version=$(SWF_VERSION) -swf-size=$(SWF_SIZE) -o hello-cpp-lto.swf

include ../Makefile.common

clean:
	rm -f hello *.swf *.bc *.o *.exe *.txt *.i *.ii
