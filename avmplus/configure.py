#!/usr/bin/env python
# -*- Mode: Python; indent-tabs-mode: nil -*-
# vi: set ts=4 sw=4 expandtab:
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

#
# This script runs just like a traditional configure script, to do configuration
# testing and makefile generation.

#****************************************************************************
# If you're building android the android public sdk/ndk must be set up on your
# build machine. # See the wiki page here for instructions on how to create
# the android public sdk/ndk:
# https://zerowing.corp.adobe.com/display/FlashPlayer/android+tamarin+shell+support
#
# Before building edit the /android-public/android-vars.sh script
# and check that the ANDROIDTOP variable is set correctly. Then run the script 
# before invoking configure.py:
#        . /android-public/android-vars.sh
#
#****************************************************************************

import os
import os.path
import sys
import build.process
import re
import string
import subprocess

thisdir = os.path.dirname(os.path.abspath(__file__))

# Look for additional modules in our build/ directory.
sys.path.append(thisdir)

from build.configuration import *
import build.getopt
import build.avmfeatures


# Used to set the mac SDK parameters

def _setSDKParams(sdk_version, os_ver, xcode_version):
    if sdk_version is None:
        # Infer SDK version from the current OS version
        if os_ver == '10.4':
            sdk_version = '104u'
        else:
            sdk_version = os_ver.translate(None, string.punctuation)
    # On 10.5/6 systems, and only if "--mac-sdk=104u" is passed in, compile for the 10.4u SDK and override CC/CXX (set in configuration.py) to use gcc/gxx 4.0.x
    # Infer xcode version from the SDK version if not directly specified
    sdk_version = sdk_version.strip()
    if sdk_version == '104u':
        os_ver,sdk_number = '10.4','10.4u'
        config._acvars['CXX'] = 'g++-4.0'
        config._acvars['CC']  = 'gcc-4.0'
        if xcode_version is None:
            xcode_version = '3'
    elif sdk_version == '105':
        os_ver,sdk_number = '10.5','10.5'
        if xcode_version is None:
            xcode_version = '3'
    elif sdk_version == '106':
        os_ver,sdk_number = '10.6','10.6'
        if xcode_version is None:
            xcode_version = '3'
    elif sdk_version == '107' or sdk_version == '108':
        os_ver,sdk_number = '10.7','10.7'
        if xcode_version is None:
            xcode_version = '4'
    # Current XBridge Versions require XCode 5
    # OSX version support for Yosemite and El Captain
    elif sdk_version == '109' or sdk_version == '1010' or sdk_version == '1011':
        os_ver,sdk_number = '10.9','10.9'
        if xcode_version is None:
            xcode_version = '5'
    else:
        print'Unknown SDK version -> %s. Expected values are xxx 104u, 105, 106 or 107.' % sdk_version
        sys.exit(2)

    sdk_prefix = None
    if xcode_version is not None:
       xcode_major_version = xcode_version.split(".")[0]
       if int(xcode_major_version) >= 4:
          sdk_prefix = "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX"
    if sdk_prefix is None:
       sdk_prefix = "/Developer/SDKs/MacOSX"

    sdk_path = sdk_prefix + sdk_number + ".sdk"
    if not os.path.exists(sdk_path):
        print'Could not find %s' % sdk_path
        sys.exit(2)
    else:
        return os_ver,sdk_path

def _setGCCVersionedFlags(FLAGS, MAJOR_VERSION, MINOR_VERSION, current_cpu):
    # warnings have been updated to try to include all those enabled by current Flash/AIR builds -- disable with caution, or risk integration pain
    if MAJOR_VERSION >= 5:
        return FLAGS

    if MAJOR_VERSION >= 4:
        FLAGS += "-Wstrict-null-sentinel "
        if current_cpu == 'mips':
            FLAGS += "-Wstrict-aliasing=0 "
        elif (MAJOR_VERSION == 4 and MINOR_VERSION <= 2): # 4.0 - 4.2
            # Bugzilla 654996: -Werror for gcc prior to 4.3 can _usually_ be
            # turned on; see core/manifest.mk for Interpreter.cpp workaround.
            FLAGS += "-Wstrict-aliasing=0 -Werror "
        elif (MAJOR_VERSION == 4 and MINOR_VERSION == 4): # 4.4
            FLAGS += "-Werror -Wempty-body -Wno-logical-op -Wmissing-field-initializers -Wstrict-aliasing=0 -Wno-array-bounds -Wno-clobbered -Wstrict-overflow=0 -funit-at-a-time  "
        else: # gcc 4.5 or later
            FLAGS += "-Werror -Wempty-body -Wno-logical-op -Wmissing-field-initializers -Wstrict-aliasing=3 -Wno-array-bounds -Wno-clobbered -Wstrict-overflow=0 -funit-at-a-time  "
            if (MAJOR_VERSION == 4 and MINOR_VERSION == 6): # 4.6
                FLAGS += "-Wno-psabi -Wno-unused-variable -Wno-unused-but-set-variable "

    return FLAGS

o = build.getopt.Options()

config = Configuration(thisdir, options = o,
                       sourcefile = 'core/avmplus.h')

the_os, cpu = config.getTarget()

arm_fpu = o.getBoolArg("arm-fpu", False)
arm_neon = o.getBoolArg("arm-neon", False)
arm_thumb = False
if cpu == "thumb2":
    arm_thumb = True
else:
    arm_thumb = o.getBoolArg("arm-thumb",False)
arm_hard_float = o.getBoolArg("arm-hard-float", False)
arm_arch = o.arm_arch
if arm_arch == None and cpu == "thumb2":
    arm_arch = "armv7-a"

buildTamarin = o.getBoolArg('tamarin', True)
if buildTamarin:
    config.subst("ENABLE_TAMARIN", 1)

buildShell = o.getBoolArg("shell", True)
if (buildShell):
    config.subst("ENABLE_SHELL", 1)

buildAot = o.peekBoolArg("aot", False)
if buildAot:
    config.subst("ENABLE_AOT", 1)

APP_CPPFLAGS = "-DAVMSHELL_BUILD "
APP_CXXFLAGS = ""
APP_CFLAGS = ""
OPT_CXXFLAGS = "-O3 "
OPT_CPPFLAGS = ""
DEBUG_CPPFLAGS = "-DDEBUG -D_DEBUG "
DEBUG_CXXFLAGS = ""
DEBUG_CFLAGS = ""
DEBUG_LDFLAGS = ""
OS_LIBS = []
OS_LDFLAGS = ""
LDFLAGS = config._acvars['LDFLAGS']
MMGC_CPPFLAGS = "-DAVMSHELL_BUILD "
AVMSHELL_CPPFLAGS = ""
AVMSHELL_LDFLAGS = ""
MMGC_DEFINES = {}
NSPR_INCLUDES = ""
NSPR_LDOPTS = ""
DISABLE_RTMPE = None
ANDROIDPLATFORMVER = "android-9"
ARM_EABI_VER = '4.4.3'
ARM_EABI = 'arm-linux-androideabi'

if 'APP_CPPFLAGS' in os.environ:
    APP_CPPFLAGS += os.environ['APP_CPPFLAGS'] + " "
if 'APP_CXXFLAGS' in os.environ:
    APP_CXXFLAGS += os.environ['APP_CXXFLAGS'] + " "
if 'APP_CFLAGS' in os.environ:
    APP_CFLAGS += os.environ['APP_CFLAGS'] + " "
if 'OPT_CXXFLAGS' in os.environ:
    OPT_CXXFLAGS += os.environ['OPT_CXXFLAGS'] + " "
if 'OPT_CPPFLAGS' in os.environ:
    OPT_CPPFLAGS += os.environ['OPT_CPPFLAGS'] + " "
if 'DEBUG_CPPFLAGS' in os.environ:
    DEBUG_CPPFLAGS += os.environ['DEBUG_CPPFLAGS'] + " "
if 'DEBUG_CXXFLAGS' in os.environ:
    DEBUG_CXXFLAGS += os.environ['DEBUG_CXXFLAGS'] + " "
if 'DEBUG_CFLAGS' in os.environ:
    DEBUG_CFLAGS += os.environ['DEBUG_CFLAGS'] + " "
if 'DEBUG_LDFLAGS' in os.environ:
    DEBUG_LDFLAGS += os.environ['DEBUG_LDFLAGS'] + " "
if 'OS_LDFLAGS' in os.environ:
    OS_LDFLAGS += os.environ['OS_LDFLAGS'] + " "
if 'MMGC_CPPFLAGS' in os.environ:
    MMGC_CPPFLAGS += os.environ['MMGC_CPPFLAGS'] + " "
if 'AVMSHELL_CPPFLAGS' in os.environ:
    AVMSHELL_CPPFLAGS += os.environ['AVMSHELL_CPPFLAGS'] + " "
if 'ARM_EABI' in os.environ:
    ARM_EABI = os.environ['ARM_EABI']
if 'ARM_EABI_VER' in os.environ:
    ARM_EABI_VER = os.environ['ARM_EABI_VER']
if 'AVMSHELL_LDFLAGS' in os.environ:
    AVMSHELL_LDFLAGS += os.environ['AVMSHELL_LDFLAGS'] + " "
if 'NSPR_INCLUDES' in os.environ:
    NSPR_INCLUDES += os.environ['NSPR_INCLUDES'] + " "
if 'NSPR_LDOPTS' in os.environ:
    NSPR_LDOPTS += os.environ['NSPR_LDOPTS'] + " "
if 'DISABLE_RTMPE' in os.environ:
    DISABLE_RTMPE += os.environ['DISABLE_RTMPE'] + " "    
if o.getBoolArg('valgrind', False, False):
    OPT_CXXFLAGS = "-O1 -g "


valinc = '$(topsrcdir)/other-licenses'
if 'VALGRIND_HOME' in os.environ:
    valinc = os.environ['VALGRIND_HOME'] + '/include'
APP_CPPFLAGS += '-I' + valinc + ' '

# builtinBuildFlags() must be called first, featureSettings() will clear the features!
config.subst("BUILTIN_BUILDFLAGS",build.avmfeatures.builtinBuildFlags(o));
# See build/avmfeatures.py for the code that processes switches for
# standard feature names.
APP_CPPFLAGS += build.avmfeatures.featureSettings(o)

if not o.getBoolArg("methodenv-impl32", True):
    APP_CPPFLAGS += "-DVMCFG_METHODENV_IMPL32=0 "

memoryProfiler = o.getBoolArg("memory-profiler", False)
if memoryProfiler:
    APP_CPPFLAGS += "-DMMGC_MEMORY_PROFILER "

MMGC_INTERIOR_PTRS = o.getBoolArg('mmgc-interior-pointers', False)
if MMGC_INTERIOR_PTRS:
    MMGC_DEFINES['MMGC_INTERIOR_PTRS'] = None

MMGC_DYNAMIC = o.getBoolArg('mmgc-shared', False)
if MMGC_DYNAMIC:
    MMGC_DEFINES['MMGC_DLL'] = None
    MMGC_CPPFLAGS += "-DMMGC_IMPL "
    
# For -Wreorder, see https://bugzilla.mozilla.org/show_bug.cgi?id=475750
if config.getCompiler() == 'GCC':
    if 'CXX' in os.environ:
        rawver = build.process.run_for_output(['$CXX', '--version'])
    else:
        rawver = build.process.run_for_output(['gcc', '--version'])
    vre = re.compile(".* ([3-9]\.[0-9]+\.[0-9]+)[ \n]")

    if vre.match(rawver) != None:
        ver = vre.match(rawver).group(1)
        ver_arr = ver.split('.')
        GCC_MAJOR_VERSION = int(ver_arr[0])
        GCC_MINOR_VERSION = int(ver_arr[1])
    else:
        GCC_MAJOR_VERSION = 10
        GCC_MINOR_VERSION = 0

    

    if the_os == 'android':
        try:
            ANDROID_TOOLCHAIN = os.environ['ANDROID_TOOLCHAIN']
            ANDROID_NDK = os.environ['ANDROID_NDK']
            ANDROID_NDK_BIN = os.environ['ANDROID_NDK_BIN']
            ANDROID_SDK = os.environ['ANDROID_SDK']
        except:
            print('\nANDROID_ variables not found in environment\nPlease run /android-public/android-vars.sh')
            exit(0)

        ANDROID_INCLUDES = "-I$(topsrcdir)/other-licenses/zlib "\
                           "-I$(ANDROID_NDK)/platforms/%s/arch-arm/usr/include "\
                           "-I$(ANDROID_NDK_BIN) "\
                           "-I$(ANDROID_SDK) "\
                           "-I$(ANDROID_NDK)/sources/cxx-stl/stlport/stlport "\
                           "-I$(ANDROID_TOOLCHAIN)/openssl/include "\
                           "-I$(ANDROID_TOOLCHAIN)/frameworks/base/opengl/include " % (ANDROIDPLATFORMVER)

        # These flags are shared with some of the other builds such as ARM, but better to keep them separate here for flexibility
        COMMON_CXX_FLAGS = "-Wall -Wdisabled-optimization -Wextra -Wformat=2 -Winit-self -Winvalid-pch -Wno-invalid-offsetof " \
                           "-Wno-switch -Wpointer-arith -Wwrite-strings -Woverloaded-virtual -Wsign-promo " \
                           "-fmessage-length=0 -fno-exceptions -fno-rtti -fsigned-char -fno-inline-functions-called-once -ffunction-sections -fdata-sections -Wno-ctor-dtor-privacy "

        # Additional flags used by android
        APP_CXX_FLAGS = "%s -Qunused-arguments -Wno-ctor-dtor-privacy -Wlogical-op -Wstrict-overflow=1 " \
                    "-Wmissing-include-dirs -Wno-missing-field-initializers -Wno-type-limits -Wno-unused-parameter " \
                    "-Wnon-virtual-dtor -Wstrict-null-sentinel -Wno-missing-braces -Wno-multichar -Wno-psabi -Wno-reorder " \
                    "-fno-short-enums -fno-strict-aliasing -fpic -funwind-tables -fstack-protector -finline-limit=200 -ftree-vectorize " \
                    "-feliminate-unused-debug-symbols -feliminate-unused-debug-types -MD -fwrapv " % COMMON_CXX_FLAGS
        APP_CXXFLAGS += _setGCCVersionedFlags(APP_CXX_FLAGS, GCC_MAJOR_VERSION, GCC_MINOR_VERSION, cpu)

        # LFLAGS_HEADLESS gets picked up in configuration.py by MKPROGRAM
        LFLAGS_HEADLESS = "-nostdlib -Bdynamic -Wl,-T,"\
                          "$(ANDROID_NDK_BIN)/../%s/lib/ldscripts/armelf_linux_eabi.x "\
                          "-Wl,-dynamic-linker,/system/bin/linker "\
                          "-Wl,-z,nocopyreloc "\
                          "-L$(ANDROID_NDK)/platforms/%s/arch-arm/usr/lib "\
                          "-L$(ANDROID_NDK)/sources/cxx-stl/stlport/libs/armeabi "\
                          "-Wl,-rpath-link=$(ANDROID_NDK)/platforms/%s/arch-arm/usr/lib "\
                          "$(ANDROID_NDK)/platforms/%s/arch-arm/usr/lib/crtbegin_dynamic.o "\
                          "$(ANDROID_NDK)/platforms/%s/arch-arm/usr/lib/crtend_android.o " % (ARM_EABI,ANDROIDPLATFORMVER,ANDROIDPLATFORMVER,ANDROIDPLATFORMVER,ANDROIDPLATFORMVER)

        LDFLAGS += "$(ANDROID_TOOLCHAIN)/openssl/libcrypto.a $(ANDROID_TOOLCHAIN)/openssl/libssl.a"
        
        # SEARCH_DIRS gets picked up in configuration.py by MKPROGRAM
        SEARCH_DIRS = "-L."

        BASE_M_FLAGS = "-mlong-calls -mthumb-interwork "

        if arm_arch == "armv7-a" or arm_arch == None:
            BASE_CXX_FLAGS = "%s -march=armv7-a -mtune=cortex-a8 -mfloat-abi=softfp -mno-thumb -fno-section-anchors -D__ARM_ARCH__=7 " \
                        "-DARMV6_ASSEMBLY " % BASE_M_FLAGS
            APP_CXXFLAGS += BASE_CXX_FLAGS

        elif arm_arch == "armv6":
            BASE_CXX_FLAGS = "%s -march=armv6 -mfloat-abi=soft -D__ARM_ARCH__=6 -DARMV5_ASSEMBLY -DARMV6_ASSEMBLY " % BASE_M_FLAGS
            APP_CXXFLAGS += BASE_CXX_FLAGS
            LFLAGS_HEADLESS += "-Wl,--no-enum-size-warning"

        elif arm_arch == "armv5":
            BASE_CXX_FLAGS = "%s -march=armv5te -mfloat-abi=soft -mtune=xscale -D__ARM_ARCH__=5 -DARMV5_ASSEMBLY " % BASE_M_FLAGS
            APP_CXXFLAGS += BASE_CXX_FLAGS
            LFLAGS_HEADLESS += "-Wl,--no-enum-size-warning"

        else:
            raise Exception('Unrecognized architecture: %s' % arm_arch)

        APP_CPPFLAGS += "-DAVMPLUS_UNIX -DUNIX -Dlinux -DUSE_PTHREAD_MUTEX -DGTEST_USE_OWN_TR1_TUPLE=1 -DHAVE_STDARG -DAVMPLUS_ARM %s" % ANDROID_INCLUDES

    else:
        APP_CXXFLAGS += "-Wall -Wcast-align -Wdisabled-optimization -Wextra -Wformat=2 -Winit-self -Winvalid-pch -Wno-invalid-offsetof -Wno-switch "\
                       "-Wparentheses -Wpointer-arith -Wreorder -Wsign-compare -Wunused-parameter -Wwrite-strings -Wno-ctor-dtor-privacy -Woverloaded-virtual "\
                       "-Wsign-promo -Wno-char-subscripts -fmessage-length=0 -fno-exceptions -fno-rtti -fno-check-new -fstrict-aliasing -fsigned-char  "
        APP_CXXFLAGS += _setGCCVersionedFlags(APP_CXXFLAGS, GCC_MAJOR_VERSION, GCC_MINOR_VERSION, cpu)

    if cpu == 'sh4':
        APP_CXXFLAGS += "-mieee -Wno-cast-align "

    if cpu == 'arm' or cpu == 'thumb2':
        APP_CXXFLAGS += "-Wno-cast-align "
        APP_CFLAGS += "-Wno-cast-align "

    FLOAT_ABI = None;
    EXTRA_CFLAGS = "";
    if arm_fpu:
        FLOAT_ABI = "-mfloat-abi=softfp "
        EXTRA_CFLAGS = "-mfpu=vfp -march=%s " % arm_arch # compile to use hardware fpu
    if arm_hard_float:
        FLOAT_ABI = "-mfloat-abi=hard -march=%s " % arm_arch # compile to use neon vfp
        AVMSHELL_LDFLAGS += "-static "
    if arm_neon:
        if FLOAT_ABI == None:
            FLOAT_ABI = "-mfloat-abi=softfp "
        EXTRA_CFLAGS = "-mfpu=neon -march=%s -DTARGET_NEON " % arm_arch # compile to use neon vfp
    if arm_thumb:
        EXTRA_CFLAGS += "-mthumb -DTARGET_THUMB2 "
    if arm_thumb != False and arm_arch == "armv7-a":
        EXTRA_CFLAGS += "-mtune=cortex-a8 "
    #if arm_arch:
        #OPT_CXXFLAGS += "-march=%s " % arm_arch
        #DEBUG_CXXFLAGS += "-march=%s " % arm_arch
    if EXTRA_CFLAGS != None:
        APP_CXXFLAGS += EXTRA_CFLAGS
        APP_CFLAGS += EXTRA_CFLAGS
    if FLOAT_ABI != None:
        APP_CXXFLAGS += FLOAT_ABI
        APP_CFLAGS += FLOAT_ABI
        AVMSHELL_LDFLAGS += FLOAT_ABI

    if config.getDebug():
        APP_CXXFLAGS += ""
    else:
        APP_CXXFLAGS += "-Wuninitialized "
    DEBUG_CXXFLAGS += "-g "
    DEBUG_LDFLAGS += "-g "
elif config.getCompiler() == 'VS':
    if cpu == "arm":
        APP_CXXFLAGS = "-W4 -WX -wd4291 -wd4201 -wd4189 -wd4740 -wd4127 -fp:fast -GF -GS- -Zc:wchar_t- "
        OS_LDFLAGS += "-MAP "
        if config.getDebug():
            DEBUG_CXXFLAGS = "-Od "
            DEBUG_CFLAGS = "-Od "
            APP_CXXFLAGS += "-GR- -fp:fast -GS- -Zc:wchar_t- -Zc:forScope "
        else:
            OPT_CXXFLAGS = "-O2 -GR- "
        if arm_arch:
            OPT_CXXFLAGS += "-QR%s " % arm_arch
        if arm_fpu:
            OPT_CXXFLAGS += "-QRfpe- " # compile to use hardware fpu
    else:
        APP_CXXFLAGS = "-W4 -WX -wd4291 -GF -GS- -Zc:wchar_t- "
        APP_CFLAGS = "-W3 -WX -wd4291 -GF -GS- -Zc:wchar_t- "

        if cpu == 'x86_64':
            pass # 64 bit VC does NaN comparisons incorrectly with fp:fast
        else:
            APP_CXXFLAGS += "-fp:fast "
            APP_CFLAGS += "-fp:fast "

        OS_LDFLAGS += "-MAP "
        if config.getDebug():
            DEBUG_CXXFLAGS = "-Od "
            DEBUG_CFLAGS = "-Od "
        else:
            OPT_CXXFLAGS = "-O2 -Ob1 -GR- "
            OPT_CFLAGS = "-O2 -Ob1 -GR- "

        if memoryProfiler:
            OPT_CXXFLAGS += "-Oy- -Zi "

    DEBUG_CXXFLAGS += "-Zi "
    DEBUG_CFLAGS += "-Zi "
    DEBUG_LDFLAGS += "-DEBUG "
elif config.getCompiler() == 'SunStudio':
    APP_CXXFLAGS = "-template=no%extdef -erroff"
    OPT_CXXFLAGS = "-xO2 "
    DEBUG_CXXFLAGS += "-g "
else:
    raise Exception('Unrecognized compiler: ' + config.getCompiler())

zlib_include_dir = o.getStringArg('zlib-include-dir')
if zlib_include_dir is not None:
    AVMSHELL_CPPFLAGS += "-I%s " % zlib_include_dir

zlib_lib = o.getStringArg('zlib-lib')
if zlib_lib is None:
    zlib_lib = '$(call EXPAND_LIBNAME,zlib)'

lzma_lib = o.getStringArg('lzma-lib')
if lzma_lib is None:
    lzma_lib = '$(call EXPAND_LIBNAME,lzma)'

AVMSHELL_LDFLAGS += zlib_lib + ' ' + lzma_lib

sys_root_dir = o.getStringArg('sys-root-dir')
if sys_root_dir is not None:
    OS_LDFLAGS += " --sysroot=%s " % sys_root_dir
    OPT_CXXFLAGS += " --sysroot=%s " % sys_root_dir

if the_os == "darwin":
    # Get machine's OS version number and trim off anything after '10.x'
    p = subprocess.Popen('sw_vers -productVersion', shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    os_ver = p.stdout.read()
    parts = os_ver.split('.')
    os_ver = parts[0] + '.' + parts[1]

    AVMSHELL_LDFLAGS += " -exported_symbols_list $(topsrcdir)/platform/mac/avmshell/exports.exp"
    MMGC_DEFINES.update({'TARGET_API_MAC_CARBON': 1,
                         'DARWIN': 1,
                         '_MAC': None,
                         'AVMPLUS_MAC': None,
                         'TARGET_RT_MAC_MACHO': 1})
    APP_CXXFLAGS += "-fpascal-strings -fasm-blocks "

    if os_ver != '10.9' :
        APP_CXXFLAGS += "-faltiveca"

    # If an sdk is selected align OS and gcc/g++ versions to it
    os_ver,sdk_path = _setSDKParams(o.mac_sdk, os_ver, o.mac_xcode)
    APP_CXXFLAGS += "-mmacosx-version-min=%s -isysroot %s " % (os_ver,sdk_path)
    config.subst("MACOSX_DEPLOYMENT_TARGET",os_ver)

    if cpu == 'ppc64':
        APP_CXXFLAGS += "-arch ppc64 "
        APP_CFLAGS += "-arch ppc64 "
        OS_LDFLAGS += "-arch ppc64 "
    elif cpu == 'x86_64':
        APP_CXXFLAGS += "-arch x86_64 "
        APP_CFLAGS += "-arch x86_64 "
        OS_LDFLAGS += "-arch x86_64 "

elif the_os == "windows" or the_os == "cygwin":
    MMGC_DEFINES.update({'WIN32': None,
                         '_CRT_SECURE_NO_DEPRECATE': None})
    OS_LDFLAGS += "-MAP "
    if cpu == "arm":
        APP_CPPFLAGS += "-DARM -D_ARM_ -DUNICODE -DUNDER_CE=1 -DMMGC_ARM "
        if arm_fpu:
            APP_CPPFLAGS += "-DARMV6 -QRarch6 "
        else:
            APP_CPPFLAGS += "-DARMV5 -QRarch5t "
        OS_LIBS.append('mmtimer corelibc coredll')
    elif cpu == "thumb2": 
        APP_CPPFLAGS += "-DARMv7 -D_ARM_ -DTARGET_THUMB2 -DUNICODE -DUNDER_RT=1 -DWIN32_LEAN_AND_MEAN -D_CONSOLE -D_ARM_WINAPI_PARTITION_DESKTOP_SDK_AVAILABLE "
        OS_LIBS.append('winmm')
        OS_LIBS.append('shlwapi')
        OS_LIBS.append('AdvAPI32')
    else:
        APP_CPPFLAGS += "-DWIN32_LEAN_AND_MEAN -D_CONSOLE "
        OS_LIBS.append('winmm')
        OS_LIBS.append('shlwapi')
        OS_LIBS.append('AdvAPI32')
elif the_os == "linux":
    MMGC_DEFINES.update({'UNIX': None,
                         'AVMPLUS_UNIX': None})
    OS_LIBS.append('pthread')
    if cpu == "i686":
        APP_CPPFLAGS += "-m32 -march=i686 "
        OS_LDFLAGS += "-m32 "
#    if cpu == "x86_64":
#        # workaround https://bugzilla.mozilla.org/show_bug.cgi?id=467776
#        OPT_CXXFLAGS += '-fno-schedule-insns2 '
    if config.getDebug():
        OS_LIBS.append("dl")
elif the_os == "android":
    BASE_D_FLAGS = "-DANDROID -DHAVE_SYS_UIO_H -Dlinux -DUNIX -Dcompress=zlib_compress "

    APP_CXXFLAGS += BASE_D_FLAGS

    if config.getDebug():
        DEBUG_CXXFLAGS += "-DDEBUG -D_DEBUG -DASYNC_DEBUG -O0 -ggdb3 "
        DEBUG_CPPFLAGS = ""
    else:
        APP_CXXFLAGS += "-DNDEBUG -O3 -fomit-frame-pointer -fvisibility=hidden -finline-functions -fgcse-after-reload -frerun-cse-after-loop -frename-registers -fvisibility-inlines-hidden "
        DEBUG_CPPFLAGS = ""
elif the_os == "sunos":
    if config.getCompiler() != 'GCC':
        APP_CXXFLAGS = "-template=no%extdef -erroff"
        OPT_CXXFLAGS = "-xO2 "
        DEBUG_CXXFLAGS = "-g "
    MMGC_DEFINES.update({'UNIX': None,
                         'AVMPLUS_UNIX': None,
                         'SOLARIS': None})
    OS_LIBS.append('pthread')
    OS_LIBS.append('rt')
    OS_LIBS.append('Cstd')
    APP_CPPFLAGS += '-DAVMPLUS_CDECL '
    if config.getDebug():
        OS_LIBS.append("dl")
else:
    raise Exception("Unsupported OS")

if cpu == "i686":
    if config.getCompiler() == 'GCC' :
        # we require sse2
        APP_CPPFLAGS += "-msse2 "
elif cpu == "powerpc":
    # we detect this in core/avmbuild.h and MMgc/*build.h
    None
elif cpu == "ppc64":
    # we detect this in core/avmbuild.h and MMgc/*build.h
    None
elif cpu == "sparc":
    APP_CPPFLAGS += "-DAVMPLUS_SPARC "
elif cpu == "x86_64":
    # we detect this in core/avmbuild.h and MMgc/*build.h
    None
elif cpu == "thumb2":
    # we detect this in core/avmbuild.h and MMgc/*build.h
    None
elif cpu == "arm":
    # we detect this in core/avmbuild.h and MMgc/*build.h
    None
elif cpu == "mips":
    # we detect this in core/avmbuild.h and MMgc/*build.h
    None
elif cpu == "sh4":
    # work around for a problem with tas.b instruction on some sh4 boards
    APP_CPPFLAGS += "-DUSE_PTHREAD_MUTEX "
else:
    raise Exception("Unsupported CPU")

if o.getBoolArg('perfm'):
    APP_CPPFLAGS += "-DPERFM "

if o.help:
    sys.stdout.write(o.getHelp())
    sys.exit(1)

# Get the optional avm description string
# This is NOT supported on windows/cygwin due to cygwin-wrapper.sh
# not passing the string correctly to cl.exe
AVMPLUS_DESC = o.getStringArg('desc') or ''
if the_os == "windows" or the_os == "cygwin":
    if AVMPLUS_DESC:
        print('AVMPLUS_DESC is not supported on windows via cygwin make.'
              '  Ignoring description.')
else: # all other platforms
    # place in Makefile even if the value is empty so
    # it can be updated by hand if desired
    APP_CPPFLAGS += '-DAVMPLUS_DESC="${AVMPLUS_DESC}" '
    config.subst("AVMPLUS_DESC", AVMPLUS_DESC)

# Append MMGC_DEFINES to APP_CPPFLAGS
APP_CPPFLAGS += ''.join(val is None and ('-D%s ' % var) or ('-D%s=%s ' % (var, val))
                        for (var, val) in MMGC_DEFINES.iteritems())

config.subst("APP_CPPFLAGS", APP_CPPFLAGS)
config.subst("APP_CXXFLAGS", APP_CXXFLAGS)
config.subst("APP_CFLAGS", APP_CFLAGS)
config.subst("OPT_CPPFLAGS", OPT_CPPFLAGS)
config.subst("OPT_CXXFLAGS", OPT_CXXFLAGS)
config.subst("DEBUG_CPPFLAGS", DEBUG_CPPFLAGS)
config.subst("DEBUG_CXXFLAGS", DEBUG_CXXFLAGS)
config.subst("DEBUG_LDFLAGS", DEBUG_LDFLAGS)
config.subst("OS_LIBS", " ".join(OS_LIBS))
config.subst("OS_LDFLAGS", OS_LDFLAGS)
config.subst("MMGC_CPPFLAGS", MMGC_CPPFLAGS)
config.subst("AVMSHELL_CPPFLAGS", AVMSHELL_CPPFLAGS)
config.subst("AVMSHELL_LDFLAGS", AVMSHELL_LDFLAGS)
config.subst("MMGC_DYNAMIC", MMGC_DYNAMIC and 1 or '')
if the_os == "android":
    config.subst("LFLAGS_HEADLESS", LFLAGS_HEADLESS)
    config.subst("LDFLAGS", LDFLAGS)    
    config.subst("SEARCH_DIRS", SEARCH_DIRS)

config.generate("Makefile")

o.finish()
