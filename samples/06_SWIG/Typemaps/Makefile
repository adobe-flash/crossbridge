
all: check
	@echo "-------- Typemaps --------"
	@echo "-> Generate SWIG wrappers around the functions in the library"
	"$(FLASCC)/usr/bin/swig" -as3 -module IntArrayLib -outdir . -includeall -ignoremissing -o IntArrayLib_wrapper.c swig.i

	@echo "-> Compile the SWIG wrapper to ABC"
	$(AS3COMPILERARGS) -abcfuture -AS3 -import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) -import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) IntArrayLib.as
	# rename the output so the compiler doesn't accidentally use both this .as file along with the .abc file we just produced
	mv IntArrayLib.as IntArrayLib.as3

	@echo "-> Compile the library into a SWC"
	"$(FLASCC)/usr/bin/clang" -Werror IntArrayLib.abc IntArrayLib_wrapper.c intarraylibmain.c intarraylib.c -emit-swc=sample.IntArrayLib -o IntArrayLib.swc

	@echo "-> Compile an example SWF that uses that SWC"
	"$(FLEX)/bin/mxmlc" -static-link-runtime-shared-libraries -library-path+=IntArrayLib.swc Main.as -debug=true -o Main.swf

include ../../Makefile.common

clean:
	rm -f *_wrapper.c *.swf *.swc *.as3 *.abc

