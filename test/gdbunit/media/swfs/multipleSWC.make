# Include the common makefile
include $(SWFS_DIR)/Makefile.common

main:
	cd $(SWFS_DIR) && "$(FLASCC_DIR)/sdk/usr/bin/g++" -g -O0 $(SWF_VERSION_FLAGS) multipleSWC1.cpp multipleSWCmain.cpp -emit-swc=sample.MultipleSWC1 -o MultipleSWC1.swc
	cd $(SWFS_DIR) && "$(FLASCC_DIR)/sdk/usr/bin/g++" -g -O0 $(SWF_VERSION_FLAGS) multipleSWC2.cpp multipleSWCmain.cpp -emit-swc=sample.MultipleSWC2 -o MultipleSWC2.swc
	cd $(SWFS_DIR) && "$(FLASCC_DIR)/sdk/usr/bin/g++" -g -O0 $(SWF_VERSION_FLAGS) multipleSWC3.cpp multipleSWCmain.cpp -emit-swc=sample.MultipleSWC3 -o MultipleSWC3.swc
	cd $(SWFS_DIR) && "$(FLEX_DIR)/bin/mxmlc" $(SWF_VERSION_FLAGS) -compiler.omit-trace-statements=false -library-path=./ -debug=true multipleSWC.as -o multipleSWC.swf

clean:
	rm $(SWFS_DIR)/multipleSWC*.swf
	rm $(SWFS_DIR)/MultipleSWC*.swc
