$?AVMSHELL=../build/trd/shell/avmshell
$?TARGET=print_stat_info
$?BUILD=../build/posix

.PHONY: stat tests

stat: $(TARGET).c
	../sdk/usr/bin/gcc -c $(TARGET).c 
	../sdk/usr/bin/llvm-ld -internalize-public-api-file=../sdk/public-api.txt \
	$(TARGET).o ../sdk/usr/lib/crt1_c.o ../sdk/usr/lib/libgcc.a \
	../sdk/usr/lib/libc.a ../sdk/usr/lib/libm.a -o $(TARGET)-linked
	perl ../llvm-2.9/lib/Target/AVM2/build.pl ../sdk/usr $(TARGET)-linked.bc \
	../tamarin-redux/utils/asc.jar ../llvm-2.9/lib/Target/AVM2 $(TARGET)
	$(AVMSHELL) ../build/swfmake.abc -- -o $(TARGET).swf \
	../sdk/usr/lib/C_Run.abc \
	../sdk/usr/lib/Exit.abc ../sdk/usr/lib/LongJmp.abc \
	../sdk/usr/lib/CModule.abc $(TARGET).abc ../sdk/usr/lib/startHack.abc 
	$(AVMSHELL) ../build/projectormake.abc -- -o $(TARGET) $(AVMSHELL) \
	$(TARGET).swf -- -Djitordie
	chmod u+x $(TARGET)
	rm $(TARGET)-linked $(TARGET)-linked.bc* $(TARGET).abc $(TARGET).cpp \
	$(TARGET).h $(TARGET).o $(TARGET).swf

tests:
	python genfs.py tests/test-root $(BUILD)/alcfs.as
	java -ea -DAS3 -Xmx200m -DAVMPLUS -classpath \
	../tamarin-redux/utils/asc.jar macromedia.asc.embedding.ScriptCompiler \
	-import ../asc/abc/builtin.abc \
	-import ../asc/abc/playerglobal.abc \
	-swf posix.test.Main,800,600 -out PosixTester \
	-outdir $(BUILD) -d \
	`find nochump -name "*.as"` `find asunit/as3/src -name "*.as"` \
	$(BUILD)/alcfs.as tests/TestCModule.as PlayerKernel.as tests/PosixTester.as
