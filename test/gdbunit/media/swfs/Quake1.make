# Include the common makefile
include $(SWFS_DIR)/Makefile.common

# This make file makes the Quake SWF from the tutorial then copies it over here

# Choose the right SWF to copy depending on what SWF version we have set
ifneq (18,$(SWF_VERSION))
    $?QUAKE_SWF_TO_COPY=Quake1.swf
else
    $?QUAKE_SWF_TO_COPY=Quake1MT.swf
endif


main:
    # Quake Makefile doesn't require FLEX, but the included tutorials/Makefile.common does, so we get around that by faking it
	cd $(FLASCC_DIR)/samples/Example_Quake1/ && make FLASCC=$(FLASCC_DIR)/sdk FLEX=$(FLEX_DIR) PAK0FILE=$(SWFS_DIR)/../../../../samples/Example_Quake1/sdlquake-1.0.9/ID1/PAK0.PAK DEBUG=TRUE
	cp $(FLASCC_DIR)/samples/Example_Quake1/$(QUAKE_SWF_TO_COPY) $(SWFS_DIR)/Quake1.swf
