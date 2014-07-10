# Include the common makefile
include $(SWFS_DIR)/Makefile.common

main:
	$(FLASCC_DIR)/sdk/usr/bin/g++ $(SWF_VERSION_FLAGS) $(CXXFLAGS) -g -O0 -emit-swf $(SWFS_DIR)/flashPlusPlus.cpp -lFlash++ -lAS3++ -o $(SWFS_DIR)/flashPlusPlus.swf

clean:
	rm $(SWFS_DIR)/flashPlusPlus*.swf
