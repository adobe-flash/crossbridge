T01: check
	@echo "-------- Sample 1 --------"
	@echo && echo "First let's compile it as a projector:"
	"$(FLASCC)/usr/bin/clang" $(BASE_CFLAGS) hello.c -o hello.exe

	@echo && echo "Now lets compile it as a SWF:"
	"$(FLASCC)/usr/bin/clang" $(BASE_CFLAGS) hello.c -emit-swf -swf-size=200x200 -o hello.swf

include ../Makefile.common

clean:
	rm -f hello.swf hello *.bc *.exe
