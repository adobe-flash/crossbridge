T13: check
	@echo "-------- Sample 13 --------"
	"$(FLASCC)/usr/bin/clang++" $(BASE_CFLAGS) -x objective-c test.m -lobjc -emit-swf -o test.swf

include ../Makefile.common

clean:
	rm -f test.swf
