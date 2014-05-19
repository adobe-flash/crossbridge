# Include the common makefile
include $(SWFS_DIR)/Makefile.common

main:
	$(FLASCC_DIR)/sdk/usr/bin/gcc $(SWF_VERSION_FLAGS) $(CFLAGS) -g -O0 -emit-swf $(SWFS_DIR)/helloWorld.c -o $(SWFS_DIR)/helloWorld.swf$

clean:
	rm $(SWFS_DIR)/helloWorld*.swf
