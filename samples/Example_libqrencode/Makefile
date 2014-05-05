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

T10: check
	@echo "-------- Example: libQREncode -------"
	cd libqrencode && "$(FLASCC)/usr/bin/swig" -as3  -I. -module QREncode -outdir . -includeall -ignoremissing as3api.h
	cd libqrencode && $(AS3COMPILERARGS) -import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) -import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) QREncode.as

	cd libqrencode && "$(FLASCC)/usr/bin/gcc" $(BASE_CFLAGS) $(OPT_CFLAGS) -DHAVE_CONFIG_H as3api_wrap.c bitstream.c mask.c mmask.c mqrspec.c qrencode.c qrinput.c qrspec.c rscode.c split.c QREncode.abc -emit-swc=sample.qrencode -o ../qrencode.swc $(EXTRACFLAGS)

	@echo && echo "Compiling test app using SWC:"
	"$(FLEX)/bin/mxmlc" -library-path+=qrencode.swc qrencoder.mxml -debug=$(MXMLC_DEBUG) -o qrencoder.swf

T10Debug:
	make T10 OPT_CFLAGS="-O0 -g" MXMLC_DEBUG=true

include ../Makefile.common

clean:
	rm -f qrencoder.swf qrencode.swc
