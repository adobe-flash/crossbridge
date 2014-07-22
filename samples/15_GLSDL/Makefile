include ../Makefile.common
all:
	$(AS3COMPILERARGS)	-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/BinaryData.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/ISpecialFile.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/IBackingStore.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/IVFS.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/InMemoryBackingStore.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/PlayerKernel.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/C_Run.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/libGL.abc) \
				Console.as -outdir . -out Console
	/sdk/gcc -O0 -D_GNU_SOURCE=1 -Wall -g test.c -symbol-abc=Console.abc -emit-swf -o test.swf -lGL -lSDL -lvgl -lm -swf-size=800x600