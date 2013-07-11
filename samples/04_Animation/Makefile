$?EXTRACFLAGS=
OPT_CFLAGS:=-O4

T04: check
	@echo "-------- Sample 4 --------"
	$(AS3COMPILERARGS) -strict -AS3 \
		-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/ISpecialFile.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/CModule.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
		Console.as -outdir . -out Console

	"$(FLASCC)/usr/bin/clang++" $(BASE_CFLAGS) $(OPT_CFLAGS) GameOfLife.cpp -symbol-abc=Console.abc -emit-swf -swf-version=17 -swf-size=800x600 -o GameOfLife.swf $(EXTRACFLAGS)

T04Debug: check
	make T04 OPT_CFLAGS="-O0 -g"

include ../Makefile.common


clean:
	rm -f *.swf *.bc *.abc
