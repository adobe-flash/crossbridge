# SingleSource/UnitTests/ObjC++/Makefile
LEVEL = ../../..
include $(LEVEL)/Makefile.config

DIRS :=

LDFLAGS += -lstdc++ -lobjc -framework Foundation

# This is a known gcc / llvm-gcc miscompilation fixed in clang.
ifdef CC_UNDER_TEST_IS_LLVM_GCC
PROGRAMS_TO_SKIP := property-reference-object
endif

include $(LEVEL)/SingleSource/Makefile.singlesrc
