
all: check
	@echo "-------- PassingData --------"
	@echo "-> Generate SWIG wrappers around the functions in the library"
	"$(FLASCC)/usr/bin/swig" -as3 -module MyLib -outdir . -includeall -ignoremissing -o MyLib_wrapper.c swig.i

	@echo "-> Compile the SWIG wrapper to ABC"
	$(AS3COMPILERARGS) -abcfuture -AS3 -import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) -import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) MyLib.as
	# rename the output so the compiler doesn't accidentally use both this .as file along with the .abc file we just produced
	mv MyLib.as MyLib.as3

	@echo "-> Compile the library into a SWC"
	"$(FLASCC)/usr/bin/gcc" $(BASE_CFLAGS) MyLib.abc MyLib_wrapper.c mylibmain.c mylib.c -emit-swc=sample.MyLib -o MyLib.swc

	@echo "-> Compile an example SWF that uses that SWC"
	"$(FLEX)/bin/mxmlc" -static-link-runtime-shared-libraries -compiler.omit-trace-statements=false -library-path+=MyLib.swc PassData.as -debug=true -o PassData.swf

include ../../Makefile.common

clean:
	rm -f *_wrapper.c *.swf *.swc *.as3 *.abc
