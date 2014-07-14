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

all: check
	@echo "-------- MurmurHash --------"
	@echo "-> Generate SWIG wrappers around the functions in the library"
	"$(FLASCC)/usr/bin/swig" -as3 -module MurmurHash -outdir . -includeall -ignoremissing -o MurmurHash_wrapper.c swig.h

	@echo "-> Compile the SWIG wrapper to ABC"
	$(ASC2) -import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) -import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) MurmurHash.as
	# rename the output so the compiler doesn't accidentally use both this .as file along with the .abc file we just produced
	mv MurmurHash.as MurmurHash.as3

	@echo "-> Compile the library into a SWC"
	"$(FLASCC)/usr/bin/g++" $(BASE_CFLAGS) MurmurHash.abc MurmurHash_wrapper.c MurmurHash3.cpp main.cpp -emit-swc=sample.MurmurHash -o MurmurHash.swc

	@echo "-> Compile an example SWF that uses that SWC"
	"$(FLEX)/bin/mxmlc" -load-config+=./../../.flexConfig.xml -library-path+=MurmurHash.swc swigdemo.as -debug=$(MXMLC_DEBUG) -o swigdemo.swf

include ../../Makefile.common

clean:
	rm -f *.swf *.swc *.abc *.bc MurmurHash_wrapper.c MurmurHash.as3
