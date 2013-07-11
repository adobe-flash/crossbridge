
all: check
	@echo "-------- MurmurHash --------"
	@echo "-> Generate SWIG wrappers around the functions in the library"
	"$(FLASCC)/usr/bin/swig" -as3 -module MurmurHash -outdir . -includeall -ignoremissing -o MurmurHash_wrapper.c swig.h

	@echo "-> Compile the SWIG wrapper to ABC"
	$(AS3COMPILERARGS) -import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) -import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) MurmurHash.as
	# rename the output so the compiler doesn't accidentally use both this .as file along with the .abc file we just produced
	mv MurmurHash.as MurmurHash.as3

	@echo "-> Compile the library into a SWC"
	"$(FLASCC)/usr/bin/clang++" $(BASE_CFLAGS) MurmurHash.abc MurmurHash_wrapper.c MurmurHash3.cpp main.cpp -emit-swc=sample.MurmurHash -o MurmurHash.swc

	@echo "-> Compile an example SWF that uses that SWC"
	"$(FLEX)/bin/mxmlc" -static-link-runtime-shared-libraries -compiler.omit-trace-statements=false -library-path+=MurmurHash.swc swigdemo.as -debug=false -o swigdemo.swf

include ../../Makefile.common

clean:
	rm -f *.swf *.swc *.abc *.bc MurmurHash_wrapper.c MurmurHash.as3
