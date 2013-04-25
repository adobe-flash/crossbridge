T11: check
	@echo "-------- Sample 11 --------"
	@echo && echo "Lets compile it as a SWF:"
	"$(FLASCC)/usr/bin/g++" $(BASE_CFLAGS) mandel.cpp -lAS3++ -lFlash++ \
		-pthread -fopenmp -O4 -flto-api=exports.txt -fno-stack-protector -swf-size=1024x768 -emit-swf -o mandel.swf

include ../Makefile.common

clean:
	rm -f mandel.swf
