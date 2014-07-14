# Include the common makefile
include $(SWFS_DIR)/Makefile.common

main:
	java -jar $(ASC) -import $(NATIVE_FLASCC_DIR)/sdk/usr/lib/builtin.abc -import $(NATIVE_FLASCC_DIR)/sdk/usr/lib/playerglobal.abc -d $(NATIVE_SWFS_DIR)/as3Commands.as
	java -jar $(ASC) -import $(NATIVE_FLASCC_DIR)/sdk/usr/lib/builtin.abc -import $(NATIVE_FLASCC_DIR)/sdk/usr/lib/playerglobal.abc $(NATIVE_SWFS_DIR)/as3CommandsNonDebug.as
	$(FLASCC_DIR)/sdk/usr/bin/gcc $(SWFS_DIR)/as3Commands.abc $(SWFS_DIR)/as3CommandsNonDebug.abc $(SWFS_DIR)/as3Commands.c $(SWF_VERSION_FLAGS)  $(CFLAGS) -emit-swf -g -O0 -o $(SWFS_DIR)/as3Commands.swf
