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
	@echo "-------- Typemaps --------"
	@echo "-> Generate SWIG wrappers around the functions in the library"
	"$(FLASCC)/usr/bin/swig" -as3 -module IntArrayLib -outdir . -includeall -ignoremissing -o IntArrayLib_wrapper.c swig.i

	@echo "-> Compile the SWIG wrapper to ABC"
	$(AS3COMPILERARGS) -abcfuture -AS3 -import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) -import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) IntArrayLib.as
	# rename the output so the compiler doesn't accidentally use both this .as file along with the .abc file we just produced
	mv IntArrayLib.as IntArrayLib.as3

	@echo "-> Compile the library into a SWC"
	"$(FLASCC)/usr/bin/gcc" -Werror IntArrayLib.abc IntArrayLib_wrapper.c intarraylibmain.c intarraylib.c -emit-swc=sample.IntArrayLib -o IntArrayLib.swc

	@echo "-> Compile an example SWF that uses that SWC"
	"$(FLEX)/bin/mxmlc" -library-path+=IntArrayLib.swc Main.as -debug=$(MXMLC_DEBUG) -o Main.swf

include ../../Makefile.common

clean:
	rm -f *_wrapper.c *.swf *.swc *.as3 *.abc

