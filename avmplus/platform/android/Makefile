# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Makefile for building AVM shell for Android.

#****************************************************************************
# The following Android SDK commands must be run before invoking this
# makefile content:
#        the android sdk volume should be mounted to /Volumes/android
#            cd /Volumes/android/device
#            . build/envsetup.sh
#            choosecombo 0 1 2 3
#  
#            make MAKECMDGOALS=release
#            make MAKECMDGOALS=debug
#
#****************************************************************************

#****************************************************************************
# Targets of the build
#****************************************************************************

FINAL_TARGET := avmshell

# make sure this is the first (and so default) target
.PHONY: default_target
default_target:

#****************************************************************************
# Variables and includes
#****************************************************************************

ENABLE_NANOJIT := true

ANDROID_MAKEFILE_DIR := .

# from make.common
ifndef TARGET_ARCH
	TARGET_ARCH := ARMV5
endif

ifndef BUILD_CONFIG
	ifeq (${MAKECMDGOALS},release)
		BUILD_CONFIG := Release
	else
		BUILD_CONFIG := Debug
	endif
endif

# some libraries that we link with are not in the official NDK, so we need to tell the compiler line
# where to pick those libraries up from.  We don't wan't to use this path more generally because we
# want to keep a clear understanding of what libraries we are using that are not part of the NDK.
# We do this by setting the ANDR_OUT_SYS_LIB_DIR path and then explicitly linking to non-NDK libraries
# using this path.  This has the effect of making us require that we pick a "product" to link against
# even though our targets are, in spirit, supposed to represent more generic configurations.

ifeq (${TARGET_ARCH},ARMV7-A)
	ARCH_CFLAGS := -march=armv7-a -mtune=cortex-a8 -mfloat-abi=softfp -mfpu=neon -D__ARM_ARCH__=7 -DARMV6_ASSEMBLY -DTARGET_NEON -DSDK_ON2_OPT -DSDK_ON2_OPT_ARM11 -DFP_ON2_USE_C_FILTERING_FUNCTIONS
	PLATFORM_LDFLAGS := $(PLATFORM_LDFLAGS) -Wl,--fix-cortex-a8
	INCLUDE_ARM_ASM_FILES := 1
	HAS_OPENGL_ES2_SUPPORT := 0
	ANDR_OUT_SYS_LIB_DIR := $(ANDROID_BUILD_TOP)/out/target/product/sholes/system/lib
	YS_ARCH := arm7
else
	ifeq (${TARGET_ARCH},ARMV6)
		ARCH_CFLAGS := -march=armv6 -mfloat-abi=soft -D__ARM_ARCH__=6 -DARMV5_ASSEMBLY -DARMV6_ASSEMBLY
		INCLUDE_ARM_ASM_FILES := 1
		HAS_OPENGL_ES2_SUPPORT := 0
		ANDR_OUT_SYS_LIB_DIR := $(ANDROID_BUILD_TOP)/out/target/product/generic/system/lib
		YS_ARCH := arm5
	else
		ifeq (${TARGET_ARCH},ARMV5)
			ARCH_CFLAGS := -march=armv5te -mfloat-abi=soft -mtune=xscale -D__ARM_ARCH__=5 -DARMV5_ASSEMBLY
			INCLUDE_ARM_ASM_FILES := 0
			HAS_OPENGL_ES2_SUPPORT := 0
			ANDR_OUT_SYS_LIB_DIR := $(ANDROID_BUILD_TOP)/out/target/product/generic/system/lib
			YS_ARCH := arm5
		else
			$(warning is not a supported target architecture)
		endif
	endif
endif

BUILD_ROOT			:= ./build/android
BUILD_RESULTS_DIR	:= ${BUILD_ROOT}/results/${FINAL_TARGET}/${TARGET_ARCH}/${BUILD_CONFIG}
BUILD_INT_ROOT		:= ${BUILD_ROOT}/int
BUILD_INT_DIR		:= ${BUILD_INT_ROOT}/${FINAL_TARGET}/${TARGET_ARCH}/${BUILD_CONFIG}
OBJDIR				:= ${BUILD_INT_DIR}/obj
LIBDIR				:= ${BUILD_INT_DIR}/lib

# warnings for both c and c++
# disable -Werror 2/1/2010 dschaffe
FLASH_C_WARNINGS := -Wall -Wdisabled-optimization -Wextra -Wformat=2 -Winit-self -Winvalid-pch -Wlogical-op -Wmissing-include-dirs -Wpointer-arith -Wstrict-aliasing=3 -Wstrict-overflow=1 -Wwrite-strings -Wno-missing-field-initializers -Wno-type-limits -Wno-unused-parameter -Wno-clobbered 

# additional warnings that are only for c++
FLASH_CXX_WARNINGS := -Wctor-dtor-privacy -Wnon-virtual-dtor -Woverloaded-virtual -Wsign-promo -Wstrict-null-sentinel -Wno-missing-braces -Wno-multichar -Wno-psabi -Wno-reorder -Wno-switch -Wno-invalid-offsetof

# re-#define ZLIB's "compress()" to "zlib_compress()" to avoid libys.a link errors
MMGC_DEFINES := -DAVMFEATURE_OVERRIDE_GLOBAL_NEW=0 -DRTMFPUTIL_OVERRIDE_OPERATOR_NEW
FLASH_DEFINES := -DANDROID -DNETSCAPE -DDISABLE_DRM -DGENERIC_PLATFORM -DHAVE_SYS_UIO_H -Dlinux -DNEEDS_IN6_H -DUNIX -Dcompress=zlib_compress $(MMGC_DEFINES)
ifeq (${HAS_OPENGL_ES2_SUPPORT},1)
FLASH_DEFINES := $(FLASH_DEFINES) -DFEATURE_GPU_ACCEL_OPENGLES2 -DFEATURE_GPU_VECTOR -DGL_GLEXT_PROTOTYPES
endif

# some cflags that are used by all Android builds as defined by Google
GOOGLE_COMMON_CFLAGS:= -fmessage-length=0 -fpic -fno-strict-aliasing -mlong-calls -mthumb-interwork -fsigned-char -funwind-tables -fstack-protector

# cflags that we use for both c and c++, and for both debug-like and release-like targets
FLASH_COMMON_CFLAGS := $(GOOGLE_COMMON_CFLAGS) $(FLASH_C_WARNINGS) $(FLASH_DEFINES) $(ARCH_CFLAGS) -finline-limit=200 -MD -fwrapv

# additional cflags that we use only for c++, but for both debug-like and release-like targets
FLASH_COMMON_CXXFLAGS := $(FLASH_COMMON_CFLAGS) -fno-exceptions -fno-rtti $(FLASH_CXX_WARNINGS)

FLASH_DEBUG_CFLAGS := -DDEBUG -D_DEBUG -DASYNC_DEBUG -O0 -ggdb3
FLASH_DEBUG_CXXFLAGS := $(FLASH_DEBUG_CFLAGS)

FLASH_RELEASE_CFLAGS := -DNDEBUG -O3 -fomit-frame-pointer -fvisibility=hidden -finline-functions -fgcse-after-reload -frerun-cse-after-loop -frename-registers
FLASH_RELEASE_CXXFLAGS := $(FLASH_RELEASE_CFLAGS) -fvisibility-inlines-hidden

# Enable RTMPE?
ifeq ($(DISABLE_RTMPE),1)
	FLASH_COMMON_CXXFLAGS := $(FLASH_COMMON_CXXFLAGS) -DDISABLE_RTMPE
	YS_LIB :=
else
	# link libys.a and silence its linker warnings
	YS_LIB := $(FLASHRUNTIME_CODE_ROOT)/third_party/yellowstone/libs/GCC-arm/$(YS_ARCH)/libys.a
	PLATFORM_LDFLAGS := $(PLATFORM_LDFLAGS) -Wl,--no-enum-size-warning
endif # DISABLE_RTMPE

#
# ^^^ If any CXXFLAGS need CFLAGS flags, then combine them in the previous CXXFLAGS definitions. ^^^
# To simplify this code, CONFIG FLAGS should strictly use CFLAGS or CXXFLAGS, as appropriate.
#
ifeq (${BUILD_CONFIG},Debug)
	CONFIG_CFLAGS   := $(FLASH_COMMON_CFLAGS)   $(FLASH_DEBUG_CFLAGS)
	CONFIG_CXXFLAGS := $(FLASH_COMMON_CXXFLAGS) $(FLASH_DEBUG_CXXFLAGS)
else
	CONFIG_CFLAGS   := $(FLASH_COMMON_CFLAGS)   $(FLASH_RELEASE_CFLAGS)
	CONFIG_CXXFLAGS := $(FLASH_COMMON_CXXFLAGS) $(FLASH_RELEASE_CXXFLAGS)
endif

FLASH_CFLAGS_ARM   := $(CONFIG_CFLAGS)
FLASH_CXXFLAGS_ARM := $(CONFIG_CXXFLAGS) 
FLASH_CFLAGS_THUMB := $(FLASH_CFLAGS_ARM) -mthumb
FLASH_CXXFLAGS_THUMB := $(FLASH_CXXFLAGS_ARM) -mthumb

CC		:= $(ANDROID_EABI_TOOLCHAIN)/arm-eabi-gcc
AS		:= $(ANDROID_EABI_TOOLCHAIN)/arm-eabi-as
AR		:= $(ANDROID_EABI_TOOLCHAIN)/arm-eabi-ar
LD		:= $(ANDROID_EABI_TOOLCHAIN)/arm-eabi-ld
STRIP	:= $(ANDROID_EABI_TOOLCHAIN)/arm-eabi-strip

INC :=	-I$(ANDROID_BUILD_TOP)/bionic/libc/arch-arm/include \
	-I$(ANDROID_BUILD_TOP)/bionic/libc/kernel/arch-arm \
	-I$(ANDROID_BUILD_TOP)/bionic/libc/kernel/common \
	-I$(ANDROID_BUILD_TOP)/bionic/libm/include \
	-I$(ANDROID_BUILD_TOP)/bionic/libstdc++/include \
	-I$(ANDROID_BUILD_TOP)/bionic/libc/include \
	-I$(ANDROID_BUILD_TOP)/external/webkit/WebKit/android/stl \
	-I$(ANDROID_BUILD_TOP)/external/openssl/include \
	-I$(ANDROID_BUILD_TOP)/frameworks/base/opengl/include

# Link flags for a headless (command-line) application:
LFLAGS_HEADLESS := -nostdlib -Bdynamic \
	-Wl,-T,$(ANDROID_BUILD_TOP)/build/core/armelf.x \
	-Wl,-dynamic-linker,/system/bin/linker \
	-Wl,-z,nocopyreloc \
	-L$(ANDROID_PRODUCT_OUT)/system/lib \
	-Wl,-rpath-link=$(ANDROID_PRODUCT_OUT)/system/lib \
	$(ANDROID_PRODUCT_OUT)/obj/lib/crtbegin_dynamic.o \
	$(ANDROID_PRODUCT_OUT)/obj/lib/crtend_android.o
	
# Define these to use the ctuils atomics API
#LFLAGS_HEADLESS := $(LFLAGS_HEADLESS) -lcutils
#INC := $(INC) -I$(ANDROID_BUILD_TOP)/system/core/include

ifndef VERBOSE
	VERBOSE := 
endif

ifeq (${VERBOSE},)

# compile-files-arm function:
# ${1} = obj directory name
define compile-files-arm
	@mkdir -p ${OBJDIR}/${1}
	@echo C++ arm   $<
	@$(CC) $(FLASH_CXXFLAGS_ARM) ${AVMSHELL_DEFS} $(INC) -o $@ -c $< 
endef

# compile-files function:
# ${1} = obj directory name
define compile-files
	@mkdir -p ${OBJDIR}/${1}
	@echo C++ thumb $<
	@$(CC) $(FLASH_CXXFLAGS_THUMB) ${AVMSHELL_DEFS} $(INC) -o $@ -c $< 
endef

# compile-c-files-arm function:
# ${1} = obj directory name
define compile-c-files-arm
	@mkdir -p ${OBJDIR}/${1}
	@echo C arm $<
	@$(CC) $(FLASH_CFLAGS_ARM) ${AVMSHELL_DEFS} $(INC) -o $@ -c $< 
endef

# compile-c-files function:
# ${1} = obj directory name
define compile-c-files
	@mkdir -p ${OBJDIR}/${1}
	@echo C thumb $<
	@$(CC) $(FLASH_CFLAGS_THUMB) ${AVMSHELL_DEFS} $(INC) -o $@ -c $< 
endef

# compile-asm-files function:
# ${1} = obj directory name
define compile-asm-files
	@mkdir -p ${OBJDIR}/${1}
	@echo asm   $<
	@$(AS) -o $@ $< 
endef

else

define compile-files-arm
	@echo compiling arm
	@mkdir -p ${OBJDIR}/${1}
	$(CC) $(FLASH_CXXFLAGS_ARM) ${AVMSHELL_DEFS} $(INC) -o $@ -c $< 
endef

define compile-files
	@mkdir -p ${OBJDIR}/${1}
	$(CC) $(FLASH_CXXFLAGS_THUMB) ${AVMSHELL_DEFS} $(INC) -o $@ -c $< 
endef

# compile-c-files-arm function:
# ${1} = obj directory name
# ${2} = *_DEFS defines
define compile-c-files-arm
	@mkdir -p ${OBJDIR}/${1}
	$(CC) $(FLASH_CFLAGS_ARM) ${AVMSHELL_DEFS} $(INC) -o $@ -c $< 
endef

# compile-c-files function:
# ${1} = obj directory name
# ${2} = *_DEFS defines
define compile-c-files
	@mkdir -p ${OBJDIR}/${1}
	$(CC) $(FLASH_CFLAGS_THUMB) ${AVMSHELL_DEFS} $(INC) -o $@ -c $< 
endef

# compile-asm-files function:
# ${1} = obj directory name
define compile-asm-files
	@mkdir -p ${OBJDIR}/${1}
	$(AS) -o $@ $< 
endef

endif

# make-lib function:
# ${1} = *_OBJS files
define make-lib
	@mkdir -p ${LIBDIR}
	${AR} -rcs $@ ${1}
endef # make-lib

# Display the state of some Makefile variables:
.PHONY: variables
variables:
ifeq ($(ENABLE_NANOJIT),true)
	@echo nanojit ENabled
else
	@echo nanojit DISabled
endif
ifdef SPEAK_STATUS
	@echo Speak status ENabled
else
	@echo Speak status DISabled
endif

# end make.common

# Modify variables set by make.common before including other makefiles:
INC := -I. -I.. -I../.. -I../../shell -I../../core -I../../eval -I../../MMgc -I../../pcre -I../../extensions -I../../VMPI -I../../vmbase -I../../generated -I../../other-licenses/zlib  -I../../other-licenses $(INC)

AVMSHELL_DEFS := -DUNIX -Dlinux -DUSE_PTHREAD_MUTEX -DNO_SYS_SIGNAL -DHAVE_STDARG -DNO_CONSOLE_FWRITE -DAVMPLUS_ARM -DAVMPLUS_UNIX -DAVMSHELL_BUILD 

ifneq ($(ENABLE_NANOJIT),true)
	AVMSHELL_DEFS := ${AVMSHELL_DEFS} -DAVMPLUS_DISABLE_NJ
endif

include $(ANDROID_MAKEFILE_DIR)/make.avm
include $(ANDROID_MAKEFILE_DIR)/make.mmgc
include $(ANDROID_MAKEFILE_DIR)/make.zlib

LDFLAGS := -lc -lm -lstdc++ -lssl -lgcc

#****************************************************************************
# Shell Source files
#****************************************************************************

SHELL_SRCS := ../../shell/ConsoleOutputStream.cpp \
  ../../shell/DebugCLI.cpp \
  ../../shell/DomainClass.cpp \
  ../../shell/FileClass.cpp \
  ../../shell/FileInputStream.cpp \
  ../../shell/PosixFile.cpp \
  ../../shell/PosixPartialPlatform.cpp \
  ../../shell/ShellCore.cpp \
  ../../shell/SystemClass.cpp \
  ../../shell/avmshell.cpp \
  ../../shell/avmshellUnix.cpp \
  ../../shell/swf.cpp \
  ../../shell/../extensions/SamplerScript.cpp \
  ../../shell/../extensions/Selftest.cpp \
  ../../shell/../extensions/SelftestInit.cpp \
  ../../shell/../extensions/ST_avmplus_basics.cpp \
  ../../shell/../extensions/ST_avmplus_peephole.cpp \
  ../../shell/../extensions/ST_mmgc_basics.cpp \
  ../../shell/../extensions/ST_mmgc_threads.cpp \
  ../../shell/../extensions/ST_mmgc_weakref.cpp \
  ../../shell/../extensions/ST_mmgc_dependent.cpp \
  ../../shell/../extensions/ST_mmgc_gcheap.cpp \
  ../../shell/../extensions/ST_mmgc_finalize_uninit.cpp \
  ../../shell/../extensions/ST_vmpi_threads.cpp \
  ../../shell/../extensions/ST_mmgc_575631.cpp \
	android_cpuid.cpp

#****************************************************************************
# Dependencies
#****************************************************************************

ALL_SRCS := ${SHELL_SRCS} ${AVMPLUS_SRCS} ${MMGC_SRCS} ${ZLIB_SRC}
SHELL_OBJS := $(addprefix ${OBJDIR}/avmshell/, $(notdir $(addsuffix .o, $(basename ${SHELL_SRCS}))))
ALL_OBJS := ${SHELL_OBJS} ${AVMPLUS_OBJS} ${MMGC_OBJS} ${ZLIB_OBJS}

#****************************************************************************
# Output
#****************************************************************************

${FINAL_TARGET}: $(ALL_OBJS) 
	@echo "=== linking:" $@ "==="
ifeq ($(ENABLE_NANOJIT),true)
	@echo Note: nanojit ENabled
else
	@echo Note: nanojit DISabled
endif
	${CC} $(LFLAGS_HEADLESS)  $(ALL_OBJS) ${LDFLAGS}  -o $@
	@if [ ${SPEAK_STATUS}XXX != XXX ]; then say "Build succeeded"; fi
	@echo "=== done:" $@ "==="

$(OBJDIR)/avmshell/%.o: ../../shell/%.cpp
	${call compile-files-arm,avmshell}

$(OBJDIR)/avmshell/%.o: ../../extensions/%.cpp
	${call compile-files-arm,avmshell}

$(OBJDIR)/avmshell/%.o: %.cpp
	${call compile-files-arm,avmshell}

default_target:
	@$(MAKE) -j4 ${FINAL_TARGET}

#****************************************************************************
# Additional targets
#****************************************************************************

clean:
	-rm -f core* ${ALL_OBJS} ${FINAL_TARGET} ${AVMPLUS_LIB} ${MMGC_LIB} ${ZLIB_LIB} log outputdebug.txt

install:
	-adb push ${FINAL_TARGET} /data/app
	@if [ ${SPEAK_STATUS}XXX != XXX ]; then say "Installation complete"; fi

uninstall:
	-adb shell rm /data/app/${FINAL_TARGET}
