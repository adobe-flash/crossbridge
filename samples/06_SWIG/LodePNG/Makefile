
all: check
	@echo "-------- LodePNG --------"
	@echo "-> Generate SWIG wrappers around the functions in the library"
	"$(FLASCC)/usr/bin/swig" -as3 -module LodePNGLib -outdir . -includeall -ignoremissing -o LodePNGLib_wrapper.c swig.i

	@echo "-> Compile the SWIG wrapper to ABC"
	$(AS3COMPILERARGS) -abcfuture -AS3 -import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) -import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) LodePNGLib.as
	# rename the output so the compiler doesn't accidentally use both this .as file along with the .abc file we just produced
	mv LodePNGLib.as LodePNGLib.as3

	@echo "-> Compile the library into a SWC"
	"$(FLASCC)/usr/bin/clang" $(BASE_CFLAGS) LodePNGLib.abc LodePNGLib_wrapper.c lodepngmain.c lodepng.c -emit-swc=sample.LodePNG -o LodePNGLib.swc

	@echo "-> Compile an example SWF that uses that SWC"
	"$(FLEX)/bin/mxmlc" -static-link-runtime-shared-libraries -library-path+=LodePNGLib.swc LodePNGExample.as -debug=true -o LodePNG.swf

include ../../Makefile.common

clean:
	rm -f *_wrapper.c *.swf *.swc *.as3 *.abc

