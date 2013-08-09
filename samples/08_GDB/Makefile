T8: check
	@echo "-------- Sample 8 --------"
	@echo && echo "Compile a debug SWF:"
	"$(FLASCC)/usr/bin/clang" $(BASE_CFLAGS) debuggingexample.c -g -O0 -emit-swf -swf-size=200x200 -o debuggingexample.swf

include ../Makefile.common

clean:
	rm -f debuggingexample.swf
