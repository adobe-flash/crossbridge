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

BUILD=$(PWD)/build

# CFlag -flto-api=exports.txt removes used symbols, generate..
all:
	mkdir -p $(BUILD)/neverball/
	rm -f $(BUILD)/data*.zip
	cd neverball-1.5.4/data1 && zip -9 -q -r $(BUILD)/neverball/data1.zip *
	cd neverball-1.5.4/data2 && zip -9 -q -r $(BUILD)/neverball/data2.zip *
	cd neverball-1.5.4/data3 && zip -9 -q -r $(BUILD)/neverball/data3.zip *

	$(ASC2) -AS3 -strict -optimize \
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
		neverball-1.5.4/Console.as -outdir $(call nativepath,$(BUILD)/neverball) -out Console

	$(ASC2) -AS3 -strict -optimize \
		-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
		-import $(call nativepath,$(GLS3D)/install/usr/lib/libGL.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/ISpecialFile.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/IBackingStore.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/IVFS.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/CModule.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/C_Run.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/BinaryData.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/PlayerKernel.abc) \
		-import $(call nativepath,$(BUILD)/neverball/Console.abc) \
		neverball-1.5.4/VFSPreLoader.as -swf com.adobe.flascc.preloader.VFSPreLoader,800,600,60 -outdir $(call nativepath,$(BUILD)/neverball) -out VFSPreLoader
	
	cd neverball-1.5.4 && PATH=$(FLASCC)/usr/bin:$(PATH) make \
		DATADIR=data \
		LDFLAGS="-L$(FLASCC)/install/usr/lib/ $(GLS3D)/install/usr/lib/libGL.abc -L$(GLS3D)/install/usr/lib/ $(FLASCC)/usr/lib/AlcVFSZip.abc -swf-preloader=$(BUILD)/neverball/VFSPreLoader.swf -swf-version=17 -symbol-abc=$(BUILD)/neverball/Console.abc -jvmopt=-Xmx4G -emit-swf -swf-size=800x600 " \
		CFLAGS="-O3 " \
		CC="gcc" \
		SDL_CPPFLAGS="-I$(FLASCC)/usr/include/ -I$(GLS3D)/install/usr/include " \
		PNG_CPPFLAGS="$(shell $(FLASCC)/usr/bin/libpng-config --cflags)" \
		SDL_LIBS="$(shell $(FLASCC)/usr/bin/sdl-config --libs) -lvgl" \
		PNG_LIBS="$(shell $(FLASCC)/usr/bin/libpng-config --libs)" \
		OGL_LIBS="-lGL -lz -lfreetype -lvorbis -logg" \
		EXT=".swf" \
		DEBUG=0 ENABLE_NLS=0 \
		neverball.swf neverputt.swf \
		-j8

	mv neverball-1.5.4/*.swf $(BUILD)/neverball/

include ../Makefile.common

clean:
	echo "Cleaning..."