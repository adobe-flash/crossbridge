# Include the common makefile
include $(SWFS_DIR)/Makefile.common

main:
	cd $(SWFS_DIR) && $(FLASCC_DIR)/sdk/usr/bin/gcc -g -O0 $(SWF_VERSION_FLAGS) $(CFLAGS) loadMultipleSWF1.c -emit-swf -no-swf-preloader -symbol-class=0:my.a::Console -swf-ns=my.a -swf-size=100x100 -o loadMultipleSWF1.swf
	cd $(SWFS_DIR) && $(FLASCC_DIR)/sdk/usr/bin/gcc -g -O0 $(SWF_VERSION_FLAGS) $(CFLAGS) loadMultipleSWF2.c -emit-swf -no-swf-preloader -symbol-class=0:my.b::Console -swf-ns=my.b -swf-size=100x100 -o loadMultipleSWF2.swf
	cd $(SWFS_DIR) && $(FLEX_DIR)/bin/mxmlc -debug=true -use-network=false loadMultipleSWFsMain.as -o loadMultipleSWFsMain.swf
	
clean:
	rm $(SWFS_DIR)/loadMultipleSWF*.swf
