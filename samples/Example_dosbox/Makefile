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

BUILD=$(PWD)/build
SRCROOT=$(PWD)

DOSBOX_OPTS:=-O4 -flto-api=$(SRCROOT)/dosbox-0.74/exports.txt -fno-exceptions -DDISABLE_JOYSTICK=1
#DOSBOX_OPTS:=-O0 -fno-exceptions -DDISABLE_JOYSTICK=1

BOCHS_OPTS:=-O4 -fno-exceptions

BOCHS_CFG:=--disable-show-ips --enable-clgd54xx --enable-static --disable-plugins --enable-fpu --without-x11 --with-sdl

bochsnative:
	mkdir -p $(BUILD)/bochsnative
	
	cd $(BUILD)/bochsnative/ && CFLAGS="-O3" CXXFLAGS="-O3" \
			$(SRCROOT)/bochs-2.6/configure $(BOCHS_CFG)
	cd $(BUILD)/bochsnative/ && FLASCC=$(FLASCC) make

bochs:
	mkdir -p $(BUILD)/bochs
	
	cd $(BUILD)/bochs/ && CFLAGS="-O3" CXXFLAGS="-O3" \
			PATH=$(FLASCC)/usr/bin:$(FLASCC)/usr/bin:$(PATH) CFLAGS="$(BOCHS_OPTS) " CXXFLAGS="$(BOCHS_OPTS) -I$(FLASCC)/usr/include" $(SRCROOT)/bochs-2.6/configure $(BOCHS_CFG)
	cd $(BUILD)/bochs/ && PATH=$(FLASCC)/usr/bin:$(FLASCC)/usr/bin:$(PATH) SWF_LINK_OPTS="-Wl,--warn-unresolved-symbols -pthread -emit-swf -swf-preloader=VFSPreLoader.swf -swf-size=1024x768 -symbol-abc=Console.abc $(FLASCC)/usr/lib/AlcVFSZip.abc " make
	mv $(BUILD)/bochs/bochs $(BUILD)/bochs/bochs.swf


dbnative:
	mkdir -p $(BUILD)/dbnative
	
	cd $(BUILD)/dbnative/ && CFLAGS="-O3" CXXFLAGS="-O3" \
			$(SRCROOT)/dosbox-0.74/configure --disable-debug --disable-sdltest --disable-alsa-midi \
		--disable-alsatest --disable-dynamic-core --disable-dynrec --disable-fpu-x86 --disable-opengl
	cd $(BUILD)/dbnative/ && make

all:
	mkdir -p $(BUILD)/dosbox

	cd $(BUILD)/dosbox/ && PATH=$(FLASCC)/usr/bin:$(FLASCC)/usr/bin:$(PATH) CFLAGS="$(DOSBOX_OPTS) " CXXFLAGS="$(DOSBOX_OPTS) -I$(FLASCC)/usr/include" \
			$(SRCROOT)/dosbox-0.74/configure --disable-debug --disable-sdltest --disable-alsa-midi \
		--disable-alsatest --disable-dynamic-core --disable-dynrec --disable-fpu-x86 --disable-opengl
	cd $(BUILD)/dosbox/ && PATH=$(FLASCC)/usr/bin:$(FLASCC)/usr/bin:$(PATH) make

	mkdir -p $(SRCROOT)/dosbox-0.74/fs
	cd $(SRCROOT)/dosbox-0.74/fs && zip -i -9 -q -r $(BUILD)/dosbox/dosboxvfs.zip *

	cd $(BUILD)/dosbox && java -jar $(FLASCC)/usr/lib/asc2.jar -merge -md \
		-AS3 -strict -optimize \
		-import $(FLASCC)/usr/lib/builtin.abc \
		-import $(FLASCC)/usr/lib/playerglobal.abc \
		-import $(FLASCC)/usr/lib/ISpecialFile.abc \
		-import $(FLASCC)/usr/lib/IBackingStore.abc \
		-import $(FLASCC)/usr/lib/InMemoryBackingStore.abc \
		-import $(FLASCC)/usr/lib/IVFS.abc \
		-import $(FLASCC)/usr/lib/CModule.abc \
		-import $(FLASCC)/usr/lib/C_Run.abc \
		-import $(FLASCC)/usr/lib/BinaryData.abc \
		-import $(FLASCC)/usr/lib/PlayerKernel.abc \
		-import $(FLASCC)/usr/lib/AlcVFSZip.abc \
		-import dosboxvfs.abc \
		$(SRCROOT)/dosbox-0.74/Console.as -outdir . -out Console

	cd $(BUILD)/dosbox && java -jar $(FLASCC)/usr/lib/asc2.jar -merge -md \
		-AS3 -strict -optimize \
		-import $(FLASCC)/usr/lib/builtin.abc \
		-import $(FLASCC)/usr/lib/playerglobal.abc \
		-import $(FLASCC)/usr/lib/ISpecialFile.abc \
		-import $(FLASCC)/usr/lib/IBackingStore.abc \
		-import $(FLASCC)/usr/lib/IVFS.abc \
		-import $(FLASCC)/usr/lib/CModule.abc \
		-import $(FLASCC)/usr/lib/C_Run.abc \
		-import $(FLASCC)/usr/lib/BinaryData.abc \
		-import $(FLASCC)/usr/lib/PlayerKernel.abc \
		-import Console.abc \
		$(SRCROOT)/dosbox-0.74/VFSPreLoader.as -swf VFSPreLoader,1024,768,60 -outdir . -out VFSPreLoader

	make dbfinal

dbfinal:
	cd $(BUILD)/dosbox/ && $(FLASCC)/usr/bin/g++ $(DOSBOX_OPTS) -pthread -save-temps \
		src/dosbox.o \
		$(FLASCC)/usr/lib/AlcVFSZip.abc \
		src/cpu/libcpu.a src/debug/libdebug.a src/dos/libdos.a src/fpu/libfpu.a  \
		src/hardware/libhardware.a src/gui/libgui.a src/ints/libints.a \
		src/misc/libmisc.a src/shell/libshell.a src/hardware/serialport/libserial.a src/libs/gui_tk/libgui_tk.a \
		-lSDL -lm -lvgl -lpng -lz \
		-swf-size=1024x768 \
		-symbol-abc=Console.abc \
		-emit-swf -swf-version=18 -swf-preloader=VFSPreLoader.swf -o dosbox.swf

include ../Makefile.common

clean:
	rm -rf $(BUILD)