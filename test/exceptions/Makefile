$?EXTRACFLAGS=

$?UNAME=$(shell uname -s)
ifneq (,$(findstring CYGWIN,$(UNAME)))
	$?nativepath=$(shell cygpath -at mixed $(1))
	$?unixpath=$(shell cygpath -at unix $(1))
else
	$?nativepath=$(abspath $(1))
	$?unixpath=$(abspath $(1))
endif

everything: clean console exceptionWithinFunction exceptionAcrossFunctions

exceptionWithinFunction: clean console
	$(FLASCC)/usr/bin/g++ ExceptionWithinFunction.c -symbol-abc=Console.abc -emit-swf -O4 -o ExceptionWithinFunction.swf $(EXTRACFLAGS)

exceptionAcrossFunctions: clean console
	$(FLASCC)/usr/bin/g++ ExceptionAcrossFunctions.c -symbol-abc=Console.abc -emit-swf -O4 -o ExceptionAcrossFunctions.swf $(EXTRACFLAGS) 

clean:
	rm -f *.abc ExceptionWithinFunction.swf ExceptionAcrossFunctions.swf

console:
	java $(JVMARGS) -jar $(call nativepath,$(FLASCC)/usr/lib/asc2.jar) -merge -md -abcfuture -AS3 \
	-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
	-import $(call nativepath,$(FLASCC)/usr/lib/ISpecialFile.abc) \
	-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
	Console.as -outdir . -out Console

