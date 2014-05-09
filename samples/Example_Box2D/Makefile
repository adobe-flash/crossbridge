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

T12: check
	@echo "------- Example: Box2D --------"

	mkdir -p build
	cd build && PATH="$(call unixpath,$(FLASCC)/usr/bin):$(PATH)" CC=gcc CXX=g++ CFLAGS="$(OPT_CFLAGS) $(BASE_CFLAGS) $(EXTRACFLAGS)" CXXFLAGS="$(OPT_CFLAGS) $(BASE_CFLAGS) $(EXTRACFLAGS)" cmake ../Box2D_v2.2.1/

	make recompile

recompile:
	cd build && PATH="$(call unixpath,$(FLASCC)/usr/bin):$(PATH)" make -j8

	cp -f as3api.h build/
	cd build && "$(FLASCC)/usr/bin/swig" -as3 -c++ -I../Box2D_v2.2.1/ -DSWIGPP -module Box2D -outdir . -includeall -ignoremissing as3api.h
	cd build && $(ASC2) -import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) -import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) Box2D.as
	cd build && "$(FLASCC)/usr/bin/g++" $(BASE_CFLAGS) $(OPT_CFLAGS) -I../Box2D_v2.2.1/ Box2D.abc as3api_wrap.cxx Box2D/libBox2D.a -emit-swc=sample.Box2D -o ../Box2D.swc $(EXTRACFLAGS)

	make swfs

swfs:
	"$(FLEX)/bin/mxmlc" -library-path=Box2D.swc -debug=$(MXMLC_DEBUG) HelloWorld.as -o HelloWorld.swf
	"$(FLEX)/bin/mxmlc" -library-path=Box2D.swc -debug=$(MXMLC_DEBUG) Boxes.as -o Boxes.swf

T12Debug:
	make T12 OPT_CFLAGS="-O0 -g" MXMLC_DEBUG=true

include ../Makefile.common

clean:
	rm -rf build *.swf *.swc 
