TOP        = ../..
SWIG       = $(TOP)/../preinst-swig
CXXSRCS    = 
AS_SRCS    = runme.as ExampleModule.as
TARGET     = example
INTERFACE  = example.i
LLVM_BCS   = example_wrap
LIBS	   = -lm

all::
	$(MAKE) -f $(TOP)/Makefile CXXSRCS='$(CXXSRCS)' AS_SRCS='$(AS_SRCS)' \
	SWIG='$(SWIG)' TARGET='$(TARGET)' INTERFACE='$(INTERFACE)' \
	LLVM_BCS='$(LLVM_BCS)' as3_cpp

clean::
	$(MAKE) -f $(TOP)/Makefile as3_clean

check: all
	./$(TARGET)

