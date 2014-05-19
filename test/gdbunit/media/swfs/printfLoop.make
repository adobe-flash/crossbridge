# Include the common makefile
include $(SWFS_DIR)/Makefile.common

main:
	$(FLASCC_DIR)/sdk/usr/bin/gcc $(SWF_VERSION_FLAGS) $(CFLAGS) -g -O0 -emit-swf $(SWFS_DIR)/printfLoop.c -o $(SWFS_DIR)/printfLoop.swf$

clean:
	rm $(SWFS_DIR)/printfLoop*.swf
