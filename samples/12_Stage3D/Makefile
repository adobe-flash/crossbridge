.PHONY: SpaceTriangle

T12: check

	@echo "-------- Sample 12 --------"

	$(AS3COMPILERARGS) -AS3 -strict \
		-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
		AGALAssembler/*.as AGALAssembler/macro/*.as -outdir . -out AGAL

	$(AS3COMPILERARGS) -AS3 -strict \
		-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/ISpecialFile.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/CModule.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
		Console.as -outdir . -out Console

	java -jar $(call nativepath, $(FLASCC)/usr/lib/as3wig.jar) -i AGAL.abc -o AGAL
	"$(FLASCC)/usr/bin/g++" AGAL.cpp -c -emit-llvm &> AGAL.txt

	"$(FLASCC)/usr/bin/g++" $(BASE_CFLAGS) -fllvm-llc-opt=-ascopt=-import -fllvm-llc-opt=-ascopt=$(call nativepath,AGAL.abc) -I. AGAL.o stage3d.cpp AGAL.abc -lAS3++ -lFlash++ \
		-O4 -flto-api=exports.txt -symbol-abc=Console.abc -swf-size=320x480 -emit-swf -o stage3d.swf

	make SpaceTriangle

SpaceTriangle: check

	$(AS3COMPILERARGS) -AS3 -strict \
		-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/ISpecialFile.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/CModule.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
		SpaceTriangle/Console.as -outdir SpaceTriangle -out Console

	"$(FLASCC)/usr/bin/g++" $(BASE_CFLAGS) \
		-fllvm-llc-opt=-ascopt=-import \
		-fllvm-llc-opt=-ascopt=$(call nativepath,AGAL.abc) \
		-I. -ISpaceTriangle SpaceTriangle/*.cpp \
		AGAL.o AGAL.abc -lAS3++ -lFlash++ \
		-O4 -flto-api=exports.txt -symbol-abc=SpaceTriangle/Console.abc -swf-size=320x480 -emit-swf -o SpaceTriangle.swf

clean:
	rm -f stage3d.swf SpaceTriangle.swf rm AGAL.* AGAL_* *.abc *.o SpaceTriangle/*.abc

include ../Makefile.common
