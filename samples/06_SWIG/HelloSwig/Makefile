$?JAVA_HOME=/cygdrive/c/Progra~1/Java/jdk
$?FLASCC=../../../sdk
$?CC=$(FLASCC)/usr/bin/gcc
$?LLC=$(FLASCC)/usr/bin/llc
$?TR=$(FLASCC)/../avmplus
$?ASC=java -jar $(FLASCC)/usr/lib/asc2.jar -abcfuture -AS3 -import $(FLASCC)/usr/lib/builtin.abc -import $(FLASCC)/usr/lib/shell_toplevel.abc -import $(FLASCC)/usr/lib/playerglobal.abc 
$?SWIG=$(FLASCC)/usr/bin/swig

.PHONY: clean all

all: simple

simple:
	$(SWIG) -as3 -module simpleModule -includeall -ignoremissing simple.swig
	$(ASC) -outdir . -out simpleModule simpleModule.as
	$(CC) -v -save-temps simpleModule.abc simplemain.c simple_wrap.c simple.c -emit-swc=sample.simpleModule -o simpleModule.swc

clean:
	rm -f simpleModule.as simple_wrap.* *.i *.abc *.bc *.o
