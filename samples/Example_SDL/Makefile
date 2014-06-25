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

.PHONY: init clean all 

$?SDL_TARGETS=	lesson01 lesson02 lesson03 lesson04 lesson05 lesson06 lesson07 lesson08 lesson09 lesson10 \
	lesson11 lesson12 lesson13 lesson14 lesson15 lesson16 lesson17 lesson18 lesson19 lesson20 \
	lesson21 lesson22 lesson23 lesson24 lesson25 lesson26 lesson27 lesson28 lesson29 lesson30 \
	lesson31 lesson32 lesson33 lesson34 lesson35 lesson36

all: clean init check $(SDL_TARGETS)

#@curl -L -o $@.zip http://lazyfoo.net/downloads/index.php?file=SDLTut_$@ && mv $@.zip temp/ && unzip -qq temp/$@.zip && mv $@ lessons
%:
	"$(FLASCC)/usr/bin/g++" $(BASE_CFLAGS) lessons/$@/$@.cpp $(GLS3D)/install/usr/lib/libGL.abc -symbol-abc=Console.abc \
	-I$(GLS3D)/install/usr/include/ -L$(GLS3D)/install/usr/lib/ \
	-lSDL -lvgl -emit-swf -swf-version=$(SWF_VERSION) -swf-size=$(SWF_SIZE) -o $@.swf 

include ../Makefile.common

init:
	$(ASC2) -AS3 -optimize \
		-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
		-import $(call nativepath,$(GLS3D)/install/usr/lib/libGL.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/ISpecialFile.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/IBackingStore.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/IVFS.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/InMemoryBackingStore.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/AlcVFSZip.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/CModule.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/C_Run.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/BinaryData.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/PlayerKernel.abc) \
		Console.as -outdir . -out Console 
  
clean:
	@rm -f *.swf *.swc *.bc *.abc *.exe *.zip
