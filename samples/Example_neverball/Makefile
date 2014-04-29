FLASCC=/path/to/FLASCC/sdk
ALCEXTRA=/path/to/alcextra
GLS3D=/path/to/GLS3D

clean:
	echo "Cleaning..."

all:
	mkdir -p $(BUILD)/neverball/
	rm -f $(BUILD)/data*.zip
	cd neverball-1.5.4/data1 && zip -9 -q -r $(BUILD)/neverball/data1.zip *
	cd neverball-1.5.4/data2 && zip -9 -q -r $(BUILD)/neverball/data2.zip *
	cd neverball-1.5.4/data3 && zip -9 -q -r $(BUILD)/neverball/data3.zip *

	java -jar $(FLASCC)/usr/lib/asc2.jar -merge -md -AS3 -strict -optimize \
		-import $(FLASCC)/usr/lib/builtin.abc \
		-import $(FLASCC)/usr/lib/playerglobal.abc \
		-import $(GLS3D)/install/usr/lib/libGL.abc \
		-import $(FLASCC)/usr/lib/ISpecialFile.abc \
		-import $(FLASCC)/usr/lib/IBackingStore.abc \
		-import $(FLASCC)/usr/lib/IVFS.abc \
		-import $(FLASCC)/usr/lib/InMemoryBackingStore.abc \
		-import $(FLASCC)/usr/lib/AlcVFSZip.abc \
		-import $(FLASCC)/usr/lib/CModule.abc \
		-import $(FLASCC)/usr/lib/C_Run.abc \
		-import $(FLASCC)/usr/lib/BinaryData.abc \
		-import $(FLASCC)/usr/lib/PlayerKernel.abc \
		neverball-1.5.4/Console.as -outdir $(BUILD)/neverball -out Console

	java -jar $(FLASCC)/usr/lib/asc2.jar -merge -md -AS3 -strict -optimize \
		-import $(FLASCC)/usr/lib/builtin.abc \
		-import $(FLASCC)/usr/lib/playerglobal.abc \
		-import $(GLS3D)/install/usr/lib/libGL.abc \
		-import $(FLASCC)/usr/lib/ISpecialFile.abc \
		-import $(FLASCC)/usr/lib/IBackingStore.abc \
		-import $(FLASCC)/usr/lib/IVFS.abc \
		-import $(FLASCC)/usr/lib/CModule.abc \
		-import $(FLASCC)/usr/lib/C_Run.abc \
		-import $(FLASCC)/usr/lib/BinaryData.abc \
		-import $(FLASCC)/usr/lib/PlayerKernel.abc \
		-import $(BUILD)/neverball/Console.abc \
		neverball-1.5.4/VFSPreLoader.as -swf com.adobe.flascc.preloader.VFSPreLoader,800,600,60 -outdir $(BUILD)/neverball -out VFSPreLoader
	
	cd neverball-1.5.4 && PATH=$(FLASCC)/usr/bin:$(ALCEXTRA)/usr/bin:$(PATH) make \
		DATADIR=data \
		LDFLAGS="-L$(ALCEXTRA)/install/usr/lib/ $(GLS3D)/install/usr/lib/libGL.abc -L$(GLS3D)/install/usr/lib/ $(FLASCC)/usr/lib/AlcVFSZip.abc -swf-preloader=$(BUILD)/neverball/VFSPreLoader.swf -swf-version=17 -symbol-abc=$(BUILD)/neverball/Console.abc -jvmopt=-Xmx4G -emit-swf -swf-size=800x600 " \
		CFLAGS="-O4 -flto-api=exports.txt" \
		CC="gcc" \
		SDL_CPPFLAGS="-I$(ALCEXTRA)/install/usr/include/ -I$(GLS3D)/install/usr/include -I$(ALCEXTRA)/install/usr/include -I$(FLASCC)/usr/include/libpng15 -I$(FLASCC)/usr/include/SDL -I$(ALCEXTRA)/install/usr/include/SDL" \
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
