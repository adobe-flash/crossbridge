$?SDK=$(FLASCC_ROOT)/sdk
$?CC=$(SDK)/usr/bin/gcc
$?LLC=$(SDK)/usr/bin/llc
$?TR=$(FLASCC_ROOT)/tamarin-redux
$?ASC=java -classpath $(TR)/utils/asc.jar \
	macromedia.asc.embedding.ScriptCompiler -abcfuture -AS3 -import \
	$(TR)/generated/builtin.abc -import $(TR)/generated/shell_toplevel.abc
$?SWIG=$(SDK)/usr/bin/swig

.PHONY: clean all

all: simple

simple: simple.c simple.h simple.i Main.as
	$(SWIG) -as3 simple.i
	$(ASC) -outdir . -out Main Main.as
	$(ASC) -outdir . -out simpleModule simpleModule.as
	$(CC) -c -o simple.bc simple.c
	$(CC) -c -o simple_wrap.bc simple_wrap.c
	$(LLC) -o simple.abc -filetype=obj simple.bc
	$(LLC) -o simple_wrap.abc -filetype=obj simple_wrap.bc
	$(CC) -enable-debugger -o simple simple.abc simple_wrap.abc \
		simpleModule.abc Main.abc 

clean:
	rm -f simpleModule.as simpleModule.abc simple_wrap.c Main.abc simple.bc \
	simple.abc simple_wrap.bc simple_wrap.abc simple
