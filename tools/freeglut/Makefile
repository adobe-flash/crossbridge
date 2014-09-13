#
# =BEGIN MIT LICENSE
# 
# The MIT License (MIT)
#
# Copyright (c) 2014 The CrossBridge Team
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# 
# =END MIT LICENSE
#

# Detect host 
$?UNAME=$(shell uname -s)
#$(info $(UNAME))
ifneq (,$(findstring CYGWIN,$(UNAME)))
	$?nativepath=$(shell cygpath -at mixed $(1))
	$?unixpath=$(shell cygpath -at unix $(1))
else
	$?nativepath=$(abspath $(1))
	$?unixpath=$(abspath $(1))
endif

# CrossBridge SDK Home
ifneq "$(wildcard $(call unixpath,$(FLASCC_ROOT)/sdk))" ""
 $?FLASCC:=$(call unixpath,$(FLASCC_ROOT)/sdk)
else
 $?FLASCC:=/path/to/crossbridge-sdk/
endif
$?ASC2=java -jar $(call nativepath,$(FLASCC)/usr/lib/asc2.jar) -merge -md -parallel
 
# Auto Detect AIR/Flex SDKs
ifneq "$(wildcard $(AIR_HOME)/lib/compiler.jar)" ""
 $?FLEX=$(AIR_HOME)
else
 $?FLEX:=/path/to/adobe-air-sdk/
endif

.PHONY: clean all

SRC=$(wildcard src/*.c)
OBJS=$(subst .c,.o,$(SRC))

AS3COMP=$(ASC2) -AS3 -optimize \
	-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
	-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
	-import $(call nativepath,$(FLASCC)/usr/lib/BinaryData.abc) \
	-import $(call nativepath,$(FLASCC)/usr/lib/ISpecialFile.abc) \
	-import $(call nativepath,$(FLASCC)/usr/lib/IBackingStore.abc) \
	-import $(call nativepath,$(FLASCC)/usr/lib/IVFS.abc) \
	-import $(call nativepath,$(FLASCC)/usr/lib/InMemoryBackingStore.abc) \
	-import $(call nativepath,$(FLASCC)/usr/lib/PlayerKernel.abc) \
	-import $(call nativepath,$(FLASCC)/usr/lib/CModule.abc) \
	-import $(call nativepath,$(FLASCC)/usr/lib/libGL.abc)

all:
	@mkdir -p install/usr/lib
	
	@echo "Compiling libglut.o"
	make compile
	@$(FLASCC)/usr/bin/ar crus install/usr/lib/libglut.a src/*.o

compile: $(OBJS)

.c.o:
	@echo "Compiling $<"
	@$(FLASCC)/usr/bin/gcc -DHAVE_GETTIMEOFDAY -DHAVE_ERRNO_H -DHAVE_LIMITS_H -DHAVE_VFPRINTF -DHAVE_UNISTD_H -O4 -c \
		-I$(FLASCC)/usr/include/ -Iinstall/usr/include/ $< -o $@

clean:
	rm -f src/*.o
