$?JAVA_HOME=/cygdrive/c/Progra~1/Java/jdk
$?FLASCC=../../../sdk
$?CC=$(FLASCC)/usr/bin/gcc
$?LLC=$(FLASCC)/usr/bin/llc
$?TR=$(FLASCC)/../avmplus
$?ASC=java -jar $(FLASCC)/usr/lib/asc2.jar -abcfuture -AS3 -import $(FLASCC)/usr/lib/builtin.abc -import $(FLASCC)/usr/lib/shell_toplevel.abc
$?SWIG=$(FLASCC)/usr/bin/swig

.PHONY: clean all

all: simple

simple: simple.c simple.h simple.i Main.as
	$(SWIG) -as3 -module simpleModule simple.i
	$(ASC) -outdir . -out Main Main.as
	$(ASC) -outdir . -out simpleModule simpleModule.as
	$(CC) -c -o simple.bc simple.c
	$(CC) -c -o simple_wrap.bc simple_wrap.c
	$(LLC) -jvm=$(JAVA_HOME) -o simple.abc -filetype=obj simple.bc
	$(LLC) -jvm=$(JAVA_HOME) -o simple_wrap.abc -filetype=obj simple_wrap.bc
	$(CC) -enable-debugger -o simple simple.abc simple_wrap.abc \
		simpleModule.abc Main.abc 

clean:
	rm -f simpleModule.as simpleModule.abc simple_wrap.c Main.abc simple.bc \
	simple.abc simple_wrap.bc simple_wrap.abc simple
