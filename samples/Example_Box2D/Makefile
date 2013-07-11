$?EXTRACFLAGS=
$?OPT_CFLAGS=-O4
$?MXMLC_DEBUG=false

T12: check
	@echo "------- Example: Box2D --------"

	mkdir -p build
	cd build && PATH="$(call unixpath,$(FLASCC)/usr/bin):$(PATH)" CC=clang CXX=clang++ CFLAGS="$(OPT_CFLAGS) $(BASE_CFLAGS) $(EXTRACFLAGS)" CXXFLAGS="$(OPT_CFLAGS) $(BASE_CFLAGS) $(EXTRACFLAGS)" cmake ../Box2D_v2.2.1/

	make recompile

recompile:
	cd build && PATH="$(call unixpath,$(FLASCC)/usr/bin):$(PATH)" make -j8

	cp -f as3api.h build/
	cd build && "$(FLASCC)/usr/bin/swig" -as3 -c++ -I../Box2D_v2.2.1/ -DSWIGPP -module Box2D -outdir . -includeall -ignoremissing as3api.h
	cd build && java -jar $(call nativepath,$(FLASCC)/usr/lib/$(AS3COMPILER)) -import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) -import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) Box2D.as
	cd build && "$(FLASCC)/usr/bin/clang++" $(BASE_CFLAGS) $(OPT_CFLAGS) -I../Box2D_v2.2.1/ Box2D.abc as3api_wrap.cxx Box2D/libBox2D.a -emit-swc=sample.Box2D -o ../Box2D.swc $(EXTRACFLAGS)

	make swfs

swfs:
	"$(FLEX)/bin/mxmlc" -static-link-runtime-shared-libraries -library-path=Box2D.swc -debug=$(MXMLC_DEBUG) HelloWorld.as -o HelloWorld.swf
	"$(FLEX)/bin/mxmlc" -static-link-runtime-shared-libraries -library-path=Box2D.swc -debug=$(MXMLC_DEBUG) Boxes.as -o Boxes.swf

T12Debug:
	make T12 OPT_CFLAGS="-O0 -g" MXMLC_DEBUG=true

include ../Makefile.common

clean:
	rm -rf build *.swf *.swc 
