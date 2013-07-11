T10: check
	@echo "-------- Sample 10 --------"
	@echo && echo "Lets compile it as a SWF:"
	"$(FLASCC)/usr/bin/clang++" $(BASE_CFLAGS) mc.cpp -lFlash++ -lAS3++ -pthread -emit-swf -o mc.swf

include ../Makefile.common


clean:
	rm -f mc.swf
