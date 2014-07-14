# Include the common makefile
include $(SWFS_DIR)/Makefile.common

# This make file pulls the hellointerop.c source file directly from the flascc build's samples folder
# and creates a SWF in this folder.

main:
	$(FLASCC_DIR)/sdk/usr/bin/gcc $(SWF_VERSION_FLAGS) $(CXXFLAGS) -g -O0 -emit-swf $(SWFS_DIR)/hellointerop.c -o $(SWFS_DIR)/hellointerop.swf
