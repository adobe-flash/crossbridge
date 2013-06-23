# SingleSource/UnitTests/ObjC/Makefile
LEVEL = ../../..
include $(LEVEL)/Makefile.config

DISABLE_JIT := 1
DIRS :=

LDFLAGS += -lobjc -framework Foundation

# This is a known gcc / llvm-gcc miscompilation fixed in clang.
ifdef CC_UNDER_TEST_IS_LLVM_GCC
EXEC_XFAILS = dot-syntax-2
endif

include $(LEVEL)/SingleSource/Makefile.singlesrc
