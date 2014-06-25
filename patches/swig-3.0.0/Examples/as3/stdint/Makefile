TOP        = ../..
SWIG       = $(TOP)/../../sdk/usr/bin/swig
SWIGOPT    = -v
SRCS       = example.c 
AS_SRCS    = runme.as ExampleModule.as
TARGET     = example
INTERFACE  = example.i
LLVM_BCS   = example example_wrap

all::
	$(MAKE) -f $(TOP)/Makefile SRCS='$(SRCS)' AS_SRCS='$(AS_SRCS)' \
	SWIG='$(SWIG)' SWIGOPT='$(SWIGOPT)' TARGET='$(TARGET)' \
	INTERFACE='$(INTERFACE)' LLVM_BCS='$(LLVM_BCS)' as3

clean::
	$(MAKE) -f $(TOP)/Makefile as3_clean
	rm -f $(TARGET).py

check: all
	./$(TARGET)
