#Set these to the paths to your FlasCC and Flex installs
FLASCC=~/flascc/sdk/
FLEX=~/flex

all:
	$(FLASCC)/usr/bin/swig -as3 gme.i
	$(FLASCC)/usr/bin/genfs --type=embed testfiles testfs
	java -jar $(FLASCC)/usr/lib/asc2.jar -merge -md -import \
		$(FLASCC)/usr/lib/InMemoryBackingStore.abc \
		-import $(FLASCC)/usr/lib/builtin.abc \
		-import $(FLASCC)/usr/lib/playerglobal.abc \
		-import $(FLASCC)/usr/lib/BinaryData.abc \
		testfs*.as
	$(FLASCC)/usr/bin/g++ gme_wrap.c gme/*.cpp libgme.as testfs*.abc \
		main.cpp demo/Wave_Writer.cpp -emit-swc=sample.libgme -o libgme.swc
	rm -f libgme.as
	$(FLEX)/bin/mxmlc -static-link-runtime-shared-libraries \
		-compiler.omit-trace-statements=false -library-path=libgme.swc \
		-debug=false demo.as -o demo.swf

include ../Makefile.common 

clean:
	rm -f libgme.swc gme_wrap.c gme_wrap.o libgme.as demo.swf testfs* \

