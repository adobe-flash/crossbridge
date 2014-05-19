# Include the common makefile
include $(SWFS_DIR)/Makefile.common

main:
	$(FLASCC_DIR)/sdk/usr/bin/g++ $(SWF_VERSION_FLAGS) $(CXXFLAGS) -g -O0 -emit-swf $(SWFS_DIR)/helloWorldCPP.cpp -o $(SWFS_DIR)/helloWorldCPP.swf

clean:
	rm $(SWFS_DIR)/helloWorldCPP*.swf
