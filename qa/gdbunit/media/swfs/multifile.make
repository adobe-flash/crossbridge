# Include the common makefile
include $(SWFS_DIR)/Makefile.common

main:
	$(FLASCC_DIR)/sdk/usr/bin/gcc $(CFLAGS) -g -O0 -c $(SWFS_DIR)/foo.c -o $(SWFS_DIR)/foo.o$
	$(FLASCC_DIR)/sdk/usr/bin/gcc $(CFLAGS) -g -O0 -c $(SWFS_DIR)/bar.c -o $(SWFS_DIR)/bar.o$
	$(FLASCC_DIR)/sdk/usr/bin/ar rcs $(SWFS_DIR)/libfoo.a $(SWFS_DIR)/foo.o
	$(FLASCC_DIR)/sdk/usr/bin/gcc $(SWF_VERSION_FLAGS) $(CFLAGS) -static -g -O0 -emit-swf $(SWFS_DIR)/multmain.c -L$(SWFS_DIR) $(SWFS_DIR)/bar.o -lfoo -o $(SWFS_DIR)/multifile.swf$

clean:
	rm -f $(SWFS_DIR)/multifile*.swf $(SWFS_DIR)/*.o $(SWFS_DIR)/libfoo.a
