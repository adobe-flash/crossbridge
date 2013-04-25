T05: check
	@echo "-------- Sample 5 --------"

	@echo && echo "Now compile a SWC and demo SWF"
	"$(FLASCC)/usr/bin/g++" $(BASE_CFLAGS) -O4 MurmurHash3.cpp as3api.cpp main.cpp -emit-swc=sample.MurmurHash -o MurmurHash.swc
	"$(FLEX)/bin/mxmlc" -static-link-runtime-shared-libraries -compiler.omit-trace-statements=false -library-path=MurmurHash.swc -debug=false swcdemo.as -o swcdemo.swf

include ../Makefile.common

clean:
	rm -f *.swf *.swc *.bc *.exe
