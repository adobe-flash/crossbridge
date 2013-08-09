$?EXTRACFLAGS=
$?OPT_CFLAGS=-O4
$?MXMLC_DEBUG=false

T10: check
	@echo "-------- Example: libQREncode -------"
	cd libqrencode && "$(FLASCC)/usr/bin/swig" -as3  -I. -module QREncode -outdir . -includeall -ignoremissing as3api.h
	cd libqrencode && java -jar $(call nativepath,$(FLASCC)/usr/lib/$(AS3COMPILER)) -import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) -import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) QREncode.as

	cd libqrencode && "$(FLASCC)/usr/bin/clang" $(BASE_CFLAGS) $(OPT_CFLAGS) -DHAVE_CONFIG_H as3api_wrap.c bitstream.c mask.c mmask.c mqrspec.c qrencode.c qrinput.c qrspec.c rscode.c split.c QREncode.abc -emit-swc=sample.qrencode -o ../qrencode.swc $(EXTRACFLAGS)

	@echo && echo "Compiling test app using SWC:"
	"$(FLEX)/bin/mxmlc" -static-link-runtime-shared-libraries -library-path+=qrencode.swc qrencoder.mxml -debug=$(MXMLC_DEBUG) -o qrencoder.swf

T10Debug:
	make T10 OPT_CFLAGS="-O0 -g" MXMLC_DEBUG=true

include ../Makefile.common

clean:
	rm -f qrencoder.swf qrencode.swc
