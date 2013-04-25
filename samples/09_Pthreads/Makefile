T09: check
	@echo "-------- Sample 9 --------"
	
	"$(FLASCC)/usr/bin/g++" $(BASE_CFLAGS) pthreads.cpp -lAS3++ -lFlash++ -pthread -O4 -flto-api=exports.txt -emit-swf -o pthreads.swf

	"$(FLASCC)/usr/bin/g++" $(BASE_CFLAGS) -O4 -flto-api=exports.txt sound.cpp \
		-lAS3++ -lFlash++ -pthread -emit-swf -o sound.swf

swctest:
	"$(FLASCC)/usr/bin/g++" $(BASE_CFLAGS) -pthread pthread_swc.cpp -emit-swf -o pthread_swc1.swf
	"$(FLASCC)/usr/bin/g++" $(BASE_CFLAGS) -pthread pthread_swc.cpp -emit-swc=sample.pthread -o pthread.swc
	"$(FLEX)/bin/mxmlc" -library-path=pthread.swc -debug=true -swf-version=18 pthread_swc.as -o pthread_swc2.swf

include ../Makefile.common

clean:
	rm -f *.swf
