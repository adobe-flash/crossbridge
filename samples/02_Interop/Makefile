T02: check
	@echo "-------- Sample 2 --------"
	@echo && echo "First let's compile it as a projector:"
	"$(FLASCC)/usr/bin/clang" $(BASE_CFLAGS) hellointerop.c -o hellointerop.exe

	@echo && echo "Now lets compile it as a SWF:"
	"$(FLASCC)/usr/bin/clang" $(BASE_CFLAGS) hellointerop.c -emit-swf -swf-size=200x200 -o hellointerop.swf

	@echo && echo "Some more advanced C++ interop:"
	"$(FLASCC)/usr/bin/clang++" $(BASE_CFLAGS) c++interop.cpp -emit-swf -swf-size=200x200 -lFlash++ -lAS3++ -o c++interop.swf

	@echo && echo "Some BitmapData C++ interop:"
	"$(FLASCC)/usr/bin/clang++" $(BASE_CFLAGS) bitmapdata.cpp -emit-swf -swf-version=17 -swf-size=200x200 -lFlash++ -lAS3++ -o bitmapdata.swf

include ../Makefile.common

clean:
	rm -f *.swf *.bc *.exe
