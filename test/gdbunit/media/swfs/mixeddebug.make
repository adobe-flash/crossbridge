# Include the common makefile
include $(SWFS_DIR)/Makefile.common

main:
	$(FLASCC_DIR)/sdk/usr/bin/gcc $(SWF_VERSION_FLAGS) $(CFLAGS) -g -O0 -c $(SWFS_DIR)/mixeddebug-withdebug.c -o $(SWFS_DIR)/mixeddebug-withdebug.o
	$(FLASCC_DIR)/sdk/usr/bin/gcc $(SWF_VERSION_FLAGS) $(CFLAGS)   -O0 -c $(SWFS_DIR)/mixeddebug-withoutdebug.c -o $(SWFS_DIR)/mixeddebug-withoutdebug.o
	$(FLASCC_DIR)/sdk/usr/bin/gcc $(SWF_VERSION_FLAGS) $(CFLAGS) -g -O0 -c $(SWFS_DIR)/mixeddebug-main.c -o $(SWFS_DIR)/mixeddebug-main.o
	$(FLASCC_DIR)/sdk/usr/bin/gcc $(SWF_VERSION_FLAGS) $(CFLAGS) -g -O0 -emit-swf $(SWFS_DIR)/mixeddebug-main.o $(SWFS_DIR)/mixeddebug-withdebug.o $(SWFS_DIR)/mixeddebug-withoutdebug.o -o $(SWFS_DIR)/mixeddebug.swf
