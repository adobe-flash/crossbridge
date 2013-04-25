# -*- python -*-
# ex: set syntax=python:
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.

from buildbot.process import factory
from buildbot.steps.source import Mercurial
from buildbot.steps.shell import *
from custom.buildbot_ext.steps.shellAddons import *
from buildbot.steps.trigger import Trigger

from commonsteps import *

class sandbox:
    
    BRANCHES = ["sandbox"]
    
    ####### SCHEDULERS
    from buildbot.scheduler import *
    # custom.buildbot_ext.scheduler import MUST happen after importing buildbot.scheduler
    from custom.buildbot_ext.scheduler import *
    
    #### SANDBOX
    compile = Scheduler(name="compile-sandbox", branch=BRANCHES, treeStableTimer=30, properties={'silent':'true'},
                     builderNames=["windows-compile-sandbox", "windows64-compile-sandbox",
                                   "mac-intel-10.5-compile-sandbox", "mac64-intel-compile-sandbox",
                                   "linux-compile-sandbox",
                                   "linux64-compile-sandbox",
                                   "android-compile-sandbox",
                                   "linux-arm-compile-sandbox",
                                   "linux-mips-compile-sandbox",
                                   ])

    smoke = BuilderDependent(name="smoke-sandbox",upstream=compile, callbackInterval=60, properties={'silent':'true'},
                    builderNames=["windows-smoke-sandbox", "windows64-smoke-sandbox",
                                   "mac-intel-10.5-smoke-sandbox", "mac64-intel-smoke-sandbox",
                                   "linux-smoke-sandbox",
                                   "linux64-smoke-sandbox",
                                   "android-smoke-sandbox",
                                   "linux-arm-smoke-sandbox",
                                   "linux-mips-smoke-sandbox",
                                   ],
                    builderDependencies=[
                                  ["windows-smoke-sandbox", "windows-compile-sandbox"], 
                                  ["windows64-smoke-sandbox", "windows64-compile-sandbox"], 
                                  ["mac-intel-10.5-smoke-sandbox", "mac-intel-10.5-compile-sandbox"],
                                  ["mac64-intel-smoke-sandbox", "mac64-intel-compile-sandbox"],
                                  ["linux-smoke-sandbox", "linux-compile-sandbox"],
                                  ["linux64-smoke-sandbox", "linux64-compile-sandbox"],
                                  ["android-smoke-sandbox","android-compile-sandbox"],
                                  ["linux-arm-smoke-sandbox","linux-arm-compile-sandbox"],
                                  ["linux-mips-smoke-sandbox","linux-mips-compile-sandbox"],
                                 ])

    test = BuilderDependent(name="test-sandbox",upstream=smoke, callbackInterval=60, properties={'silent':'true'},
                    builderNames=["windows-test-sandbox", "windows64-test-sandbox",
                                   "mac-intel-10.5-test-sandbox", "mac64-intel-test-sandbox",
                                   "linux-test-sandbox",
                                   "linux64-test-sandbox",
                                   "android-test-sandbox",
                                   "linux-arm-test-sandbox",
                                   "linux-mips-test-sandbox",
                                   ],
                    builderDependencies=[
                                  ["windows-test-sandbox", "windows-smoke-sandbox"], 
                                  ["windows64-test-sandbox", "windows64-smoke-sandbox"], 
                                  ["mac-intel-10.5-test-sandbox", "mac-intel-10.5-smoke-sandbox"],
                                  ["mac64-intel-test-sandbox", "mac64-intel-smoke-sandbox"],
                                  ["linux-test-sandbox", "linux-smoke-sandbox"],
                                  ["linux64-test-sandbox", "linux64-smoke-sandbox"],
                                  ["android-test-sandbox", "android-smoke-sandbox"],
                                  ["linux-arm-test-sandbox", "linux-arm-smoke-sandbox"],
                                  ["linux-mips-test-sandbox", "linux-mips-smoke-sandbox"],
                                 ])

    schedulers = [compile, smoke, test]


    ################################################################################
    ################################################################################
    ####                                                                        ####
    ####                    SANDBOX COMPILE BUILDERS                            ####
    ####                                                                        ####
    ################################################################################
    ################################################################################

    #############################################
    #### builder for windows-compile-sandbox ####
    #############################################
    sb_windows_compile_factory = factory.BuildFactory()
    sb_windows_compile_factory.addStep(sync_clean)
    sb_windows_compile_factory.addStep(sync_clone_sandbox)
    sb_windows_compile_factory.addStep(sync_update)
    sb_windows_compile_factory.addStep(bb_slaveupdate(slave="windows"))
    sb_windows_compile_factory.addStep(verify_builtinabc)        
    sb_windows_compile_factory.addStep(compile_generic(name="Release", shellname="avmshell", args=" ", upload="false"))
    sb_windows_compile_factory.addStep(compile_generic(name="Debug", shellname="avmshell_d", args="--enable-debug", upload="false"))
    sb_windows_compile_factory.addStep(compile_generic(name="ReleaseDebugger", shellname="avmshell_s", args="--enable-debugger", upload="false"))
    sb_windows_compile_factory.addStep(compile_generic(name="DebugDebugger", shellname="avmshell_sd", args="--enable-debug --enable-debugger", upload="false"))
    sb_windows_compile_factory.addStep(compile_buildcheck)
    sb_windows_compile_factory.addStep(verify_tracers)    
    sb_windows_compile_factory.addStep(util_upload_asteam)
    sb_windows_compile_factory.addStep(BuildShellCommand(
                command=['../all/file-check.py', '../../../../../repo'],
                env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
                description='running file-check against source...',
                descriptionDone='finished file-check.',
                name="FileCheck",
                workdir="../repo/build/buildbot/slaves/scripts")
    )

    sb_windows_compile_builder = {
                'name': "windows-compile-sandbox",
                'slavename': "windows",
                'factory': sb_windows_compile_factory,
                'builddir': './sandbox-windows-compile',
    }


    ###############################################
    #### builder for windows64-compile-sandbox ####
    ###############################################
    sb_windows_64_compile_factory = factory.BuildFactory()
    sb_windows_64_compile_factory.addStep(sync_clean)
    sb_windows_64_compile_factory.addStep(sync_clone_sandbox)
    sb_windows_64_compile_factory.addStep(sync_update)
    sb_windows_64_compile_factory.addStep(bb_slaveupdate(slave="windows64"))
    sb_windows_64_compile_factory.addStep(verify_builtinabc)        
    sb_windows_64_compile_factory.addStep(compile_generic(name="Release", shellname="avmshell_64", args="--target=x86_64-win", upload="false", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64"))
    sb_windows_64_compile_factory.addStep(compile_generic(name="Debug", shellname="avmshell_d_64", args="--enable-debug --target=x86_64-win", upload="false", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64"))
    sb_windows_64_compile_factory.addStep(compile_generic(name="ReleaseDebugger", shellname="avmshell_s_64", args="--enable-debugger --target=x86_64-win", upload="false", features="'+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64 +AVMFEATURE_DEBUGGER"))
    sb_windows_64_compile_factory.addStep(compile_generic(name="DebugDebugger", shellname="avmshell_sd_64", args="--enable-debug --enable-debugger --target=x86_64-win", upload="false", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64 +AVMFEATURE_DEBUGGER"))
    sb_windows_64_compile_factory.addStep(compile_buildcheck)
    sb_windows_64_compile_factory.addStep(verify_tracers)    
    sb_windows_64_compile_factory.addStep(util_upload_asteam)

    sb_windows_64_compile_builder = {
                'name': "windows64-compile-sandbox",
                'slavename': "windows64",
                'factory': sb_windows_64_compile_factory,
                'builddir': './sandbox-windows64-compile',
    }


    ####################################################
    #### builder for mac-intel-10_5-compile-sandbox ####
    ####################################################
    sb_mac_intel_105_compile_factory = factory.BuildFactory()
    sb_mac_intel_105_compile_factory.addStep(sync_clean)
    sb_mac_intel_105_compile_factory.addStep(sync_clone_sandbox)
    sb_mac_intel_105_compile_factory.addStep(sync_update)
    sb_mac_intel_105_compile_factory.addStep(bb_slaveupdate(slave="mac-intel-10_5"))
    sb_mac_intel_105_compile_factory.addStep(verify_builtinabc)    
    sb_mac_intel_105_compile_factory.addStep(compile_generic(name="Release", shellname="avmshell", args="--mac-sdk=105 --target=i686-darwin", upload="false", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32"))
    sb_mac_intel_105_compile_factory.addStep(compile_generic(name="Debug", shellname="avmshell_d", args="--enable-debug --mac-sdk=105 --target=i686-darwin", upload="false", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32"))
    sb_mac_intel_105_compile_factory.addStep(compile_generic(name="ReleaseDebugger", shellname="avmshell_s", args="--enable-debugger --mac-sdk=105 --target=i686-darwin", upload="false", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32 +AVMFEATURE_DEBUGGER"))
    sb_mac_intel_105_compile_factory.addStep(compile_generic(name="DebugDebugger", shellname="avmshell_sd", args="--enable-debug --enable-debugger --mac-sdk=105 --target=i686-darwin", upload="false", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32 +AVMFEATURE_DEBUGGER"))
    sb_mac_intel_105_compile_factory.addStep(compile_buildcheck)
    sb_mac_intel_105_compile_factory.addStep(verify_tracers)
    sb_mac_intel_105_compile_factory.addStep(util_upload_asteam)

    sb_mac_intel_105_compile_builder = {
                'name': "mac-intel-10.5-compile-sandbox",
                'slavename': "mac-intel-10_5",
                'factory': sb_mac_intel_105_compile_factory,
                'builddir': './sandbox-mac-intel-10_5-compile',
    }


    ##################################################
    #### builder for mac-intel-64-compile-sandbox ####
    ##################################################
    sb_mac_intel_64_compile_factory = factory.BuildFactory()
    sb_mac_intel_64_compile_factory.addStep(sync_clean)
    sb_mac_intel_64_compile_factory.addStep(sync_clone_sandbox)
    sb_mac_intel_64_compile_factory.addStep(sync_update)
    sb_mac_intel_64_compile_factory.addStep(bb_slaveupdate(slave="mac64-intel"))
    sb_mac_intel_64_compile_factory.addStep(verify_builtinabc)    
    sb_mac_intel_64_compile_factory.addStep(compile_generic(name="Release", shellname="avmshell_64", args="--target=x86_64-darwin --mac-sdk=105", upload="false", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64"))
    sb_mac_intel_64_compile_factory.addStep(compile_generic(name="Debug", shellname="avmshell_d_64", args="--enable-debug --target=x86_64-darwin --mac-sdk=105", upload="false", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64"))
    sb_mac_intel_64_compile_factory.addStep(compile_generic(name="ReleaseDebugger", shellname="avmshell_s_64", args="--enable-debugger --target=x86_64-darwin --mac-sdk=105", upload="false", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64 +AVMFEATURE_DEBUGGER"))
    sb_mac_intel_64_compile_factory.addStep(compile_generic(name="DebugDebugger", shellname="avmshell_sd_64", args="--enable-debug --enable-debugger --target=x86_64-darwin --mac-sdk=105", upload="false", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64 +AVMFEATURE_DEBUGGER"))
    sb_mac_intel_64_compile_factory.addStep(compile_buildcheck)
    sb_mac_intel_64_compile_factory.addStep(compile_testmedia)    
    sb_mac_intel_64_compile_factory.addStep(util_upload_asteam_local)
    sb_mac_intel_64_compile_factory.addStep(verify_tracers)

    sb_mac_intel_64_compile_builder = {
                'name': "mac64-intel-compile-sandbox",
                'slavename': "mac64-intel",
                'factory': sb_mac_intel_64_compile_factory,
                'builddir': './sandbox-mac64-intel-compile',
    }


    ###########################################
    #### builder for linux-compile-sandbox ####
    ###########################################
    sb_linux_compile_factory = factory.BuildFactory()
    sb_linux_compile_factory.addStep(sync_clean)
    sb_linux_compile_factory.addStep(sync_clone_sandbox)
    sb_linux_compile_factory.addStep(sync_update)
    sb_linux_compile_factory.addStep(bb_slaveupdate(slave="linux"))
    sb_linux_compile_factory.addStep(verify_builtinabc)    
    sb_linux_compile_factory.addStep(compile_generic(name="Release", shellname="avmshell", args="--target=i686-linux", upload="false", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32"))
    sb_linux_compile_factory.addStep(compile_generic(name="Debug", shellname="avmshell_d", args="--enable-debug --target=i686-linux", upload="false", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32"))
    sb_linux_compile_factory.addStep(compile_generic(name="ReleaseDebugger", shellname="avmshell_s", args="--enable-debugger --target=i686-linux", upload="false", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32 +AVMFEATURE_DEBUGGER"))
    sb_linux_compile_factory.addStep(compile_generic(name="DebugDebugger", shellname="avmshell_sd", args="--enable-debug --enable-debugger --target=i686-linux", upload="false", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32 +AVMFEATURE_DEBUGGER"))
    sb_linux_compile_factory.addStep(compile_buildcheck)
    sb_linux_compile_factory.addStep(verify_tracers)
    sb_linux_compile_factory.addStep(util_upload_asteam)

    sb_linux_compile_builder = {
                'name': "linux-compile-sandbox",
                'slavename': "linux",
                'factory': sb_linux_compile_factory,
                'builddir': './sandbox-linux-compile',
    }


    #############################################
    #### builder for linux64-compile-sandbox ####
    #############################################
    sb_linux_64_compile_factory = factory.BuildFactory()
    sb_linux_64_compile_factory.addStep(sync_clean)
    sb_linux_64_compile_factory.addStep(sync_clone_sandbox)
    sb_linux_64_compile_factory.addStep(sync_update)
    sb_linux_64_compile_factory.addStep(bb_slaveupdate(slave="linux64"))
    sb_linux_64_compile_factory.addStep(verify_builtinabc)    
    sb_linux_64_compile_factory.addStep(compile_generic(name="Release", shellname="avmshell_64", args=" ", upload="false"))
    sb_linux_64_compile_factory.addStep(compile_generic(name="Debug", shellname="avmshell_d_64", args="--enable-debug", upload="false"))
    sb_linux_64_compile_factory.addStep(compile_generic(name="ReleaseDebugger", shellname="avmshell_s_64", args="--enable-debugger", upload="false"))
    sb_linux_64_compile_factory.addStep(compile_generic(name="DebugDebugger", shellname="avmshell_sd_64", args="--enable-debug --enable-debugger", upload="false"))
    sb_linux_64_compile_factory.addStep(verify_tracers)
    sb_linux_64_compile_factory.addStep(compile_buildcheck)
    sb_linux_64_compile_factory.addStep(util_upload_asteam)

    sb_linux_64_compile_builder = {
                'name': "linux64-compile-sandbox",
                'slavename': "linux64",
                'factory': sb_linux_64_compile_factory,
                'builddir': './sandbox-linux64-compile',
    }


    ###########################################
    #### builder for android on mac        ####
    ###########################################

    sb_android_compile_factory = factory.BuildFactory()
    sb_android_compile_factory.addStep(sync_clean)
    sb_android_compile_factory.addStep(sync_clone_sandbox)
    sb_android_compile_factory.addStep(sync_update)
    sb_android_compile_factory.addStep(bb_slaveupdate(slave="android"))
    sb_android_compile_factory.addStep(verify_builtinabc)    
    sb_android_compile_factory.addStep(compile_generic(name="Release", shellname="avmshell", args="--arm-arch=armv7-a --target=arm-android", upload="false"))
    sb_android_compile_factory.addStep(compile_generic(name="Debug", shellname="avmshell_d", args="--enable-debug --arm-arch=armv7-a --target=arm-android", upload="false"))
    sb_android_compile_factory.addStep(verify_tracers)
    sb_android_compile_factory.addStep(compile_buildcheck_local)
    sb_android_compile_factory.addStep(util_upload_asteam_local)
    
    sb_android_compile_builder = {
                'name': "android-compile-sandbox",
                'slavename': "android",
                'factory': sb_android_compile_factory,
                'builddir': './sandbox-android-compile',
    }
    
    ###############################
    #### builder for linux-arm ####
    ###############################
    sb_linux_arm_compile_factory = factory.BuildFactory()
    sb_linux_arm_compile_factory.addStep(sync_clean)
    sb_linux_arm_compile_factory.addStep(sync_clone_sandbox)
    sb_linux_arm_compile_factory.addStep(sync_update)
    sb_linux_arm_compile_factory.addStep(bb_slaveupdate(slave="linux-arm"))
    sb_linux_arm_compile_factory.addStep(verify_builtinabc)    
    sb_linux_arm_compile_factory.addStep(compile_generic(name="Release", shellname="avmshell_neon_arm", args="--enable-arm-neon --arm-arch=armv7-a --target=arm-linux --enable-sys-root-dir=/usr/local/arm-linux/debian5", upload="false", features=""))
    sb_linux_arm_compile_factory.addStep(compile_generic(name="Debug", shellname="avmshell_neon_arm_d", args="--enable-debug --enable-arm-neon --arm-arch=armv7-a --target=arm-linux --enable-sys-root-dir=/usr/local/arm-linux/debian5", upload="false", features=""))
    sb_linux_arm_compile_factory.addStep(verify_tracers)
    sb_linux_arm_compile_factory.addStep(compile_buildcheck_local)
    sb_linux_arm_compile_factory.addStep(util_upload_asteam_local)

    sb_linux_arm_compile_builder = {
                'name': "linux-arm-compile-sandbox",
                'slavename': "linux-arm",
                'factory': sb_linux_arm_compile_factory,
                'builddir': './sandbox-linux-arm-compile',
    }
    
    
    ################################
    #### builder for linux-mips ####
    ################################
    sb_linux_mips_compile_factory = factory.BuildFactory()
    sb_linux_mips_compile_factory.addStep(sync_clean)
    sb_linux_mips_compile_factory.addStep(sync_clone_sandbox)
    sb_linux_mips_compile_factory.addStep(sync_update)
    sb_linux_mips_compile_factory.addStep(bb_slaveupdate(slave="linux-mips"))
    sb_linux_mips_compile_factory.addStep(verify_builtinabc)    
    sb_linux_mips_compile_factory.addStep(compile_generic(name="Release", shellname="avmshell_mips", args="--target=mips-linux", upload="false", features=""))
    sb_linux_mips_compile_factory.addStep(compile_generic(name="Debug", shellname="avmshell_mips_d", args="--enable-debug --target=mips-linux", upload="false", features=""))
    sb_linux_mips_compile_factory.addStep(verify_tracers)
    sb_linux_mips_compile_factory.addStep(compile_buildcheck_local)
    sb_linux_mips_compile_factory.addStep(util_upload_asteam_local)
    
    sb_linux_mips_compile_builder = {
                'name': "linux-mips-compile-sandbox",
                'slavename': "linux-mips",
                'factory': sb_linux_mips_compile_factory,
                'builddir': './sandbox-linux-mips-compile',
    }


    ################################################################################
    ################################################################################
    ####                                                                        ####
    ####                     SANDBOX SMOKE BUILDERS                             ####
    ####                                                                        ####
    ################################################################################
    ################################################################################




    ###########################################
    #### builder for windows-smoke-sandbox ####
    ###########################################
    sb_windows_smoke_factory = factory.BuildFactory()
    sb_windows_smoke_factory.addStep(download_testmedia)
    sb_windows_smoke_factory.addStep(test_smoke)
    sb_windows_smoke_factory.addStep(util_process_clean)

    sb_windows_smoke_builder = {
                'name': "windows-smoke-sandbox",
                'slavename': "windows",
                'factory': sb_windows_smoke_factory,
                'builddir': './sandbox-windows-smoke',
    }


    #############################################
    #### builder for windows64-smoke-sandbox ####
    #############################################
    sb_windows_64_smoke_factory = factory.BuildFactory()
    sb_windows_64_smoke_factory.addStep(download_testmedia)
    sb_windows_64_smoke_factory.addStep(test_smoke)
    sb_windows_64_smoke_factory.addStep(util_process_clean)

    sb_windows_64_smoke_builder = {
                'name': "windows64-smoke-sandbox",
                'slavename': "windows64",
                'factory': sb_windows_64_smoke_factory,
                'builddir': './sandbox-windows64-smoke',
    }


    ##################################################
    #### builder for mac-intel-10_5-smoke-sandbox ####
    ##################################################
    sb_mac_intel_105_smoke_factory = factory.BuildFactory()
    sb_mac_intel_105_smoke_factory.addStep(download_testmedia)
    sb_mac_intel_105_smoke_factory.addStep(test_smoke)
    sb_mac_intel_105_smoke_factory.addStep(util_process_clean)

    sb_mac_intel_105_smoke_builder = {
                'name': "mac-intel-10.5-smoke-sandbox",
                'slavename': "mac-intel-10_5",
                'factory': sb_mac_intel_105_smoke_factory,
                'builddir': './sandbox-mac-intel-10_5-smoke',
    }


    ###############################################
    #### builder for mac64-intel-smoke-sandbox ####
    ###############################################
    sb_mac_intel_64_smoke_factory = factory.BuildFactory()
    sb_mac_intel_64_smoke_factory.addStep(download_testmedia)
    sb_mac_intel_64_smoke_factory.addStep(test_smoke)
    sb_mac_intel_64_smoke_factory.addStep(util_process_clean)

    sb_mac_intel_64_smoke_builder = {
                'name': "mac64-intel-smoke-sandbox",
                'slavename': "mac64-intel",
                'factory': sb_mac_intel_64_smoke_factory,
                'builddir': './sandbox-mac64-intel-smoke',
    }


    #########################################
    #### builder for linux-smoke-sandbox ####
    #########################################
    sb_linux_smoke_factory = factory.BuildFactory()
    sb_linux_smoke_factory.addStep(download_testmedia)
    sb_linux_smoke_factory.addStep(test_smoke)
    sb_linux_smoke_factory.addStep(util_process_clean)

    sb_linux_smoke_builder = {
                'name': "linux-smoke-sandbox",
                'slavename': "linux",
                'factory': sb_linux_smoke_factory,
                'builddir': './sandbox-linux-smoke',
    }


    ###########################################
    #### builder for linux64-smoke-sandbox ####
    ###########################################
    sb_linux_64_smoke_factory = factory.BuildFactory()
    sb_linux_64_smoke_factory.addStep(download_testmedia)
    sb_linux_64_smoke_factory.addStep(test_smoke)
    sb_linux_64_smoke_factory.addStep(util_process_clean)

    sb_linux_64_smoke_builder = {
                'name': "linux64-smoke-sandbox",
                'slavename': "linux64",
                'factory': sb_linux_64_smoke_factory,
                'builddir': './sandbox-linux64-smoke',
    }


    #########################################
    #### builder for android-smoke       ####
    #########################################
    sb_android_smoke_factory = factory.BuildFactory()
    sb_android_smoke_factory.addStep(download_testmedia)
    sb_android_smoke_factory.addStep(test_smoke_local)
    sb_android_smoke_factory.addStep(util_process_clean)

    sb_android_smoke_builder = {
                'name': "android-smoke-sandbox",
                'slavename': "android",
                'factory': sb_android_smoke_factory,
                'builddir': './sanbox-android-smoke',
    }
    
    ###########################################
    #### builder for linxu-arm-smoke       ####
    ###########################################
    sb_linux_arm_smoke_factory = factory.BuildFactory()
    sb_linux_arm_smoke_factory.addStep(download_testmedia)
    sb_linux_arm_smoke_factory.addStep(TestSuiteShellCommand(
                command=['../all/run-smoketests-ssh.sh', WithProperties('%s','revision'), './runsmokes-ssh.txt'],
                env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
                description='starting to run smoke tests...',
                descriptionDone='finished smoke tests.',
                name="SmokeTest",
                workdir="../repo/build/buildbot/slaves/scripts")
    )
    sb_linux_arm_smoke_factory.addStep(util_process_clean)

    sb_linux_arm_smoke_builder = {
                'name': "linux-arm-smoke-sandbox",
                'slavename': "linux-arm",
                'factory': sb_linux_arm_smoke_factory,
                'builddir': './sandbox-linux-arm-smoke',
    }
    
    
    #########################################
    #### builder for linux-mips-smoke    ####
    #########################################
    sb_linux_mips_smoke_factory = factory.BuildFactory()
    sb_linux_mips_smoke_factory.addStep(download_testmedia)
    sb_linux_mips_smoke_factory.addStep(test_smoke_ssh)
    sb_linux_mips_smoke_factory.addStep(util_process_clean_ssh)

    sb_linux_mips_smoke_builder = {
                'name': "linux-mips-smoke-sandbox",
                'slavename': "linux-mips",
                'factory': sb_linux_mips_smoke_factory,
                'builddir': './sandbox-linux-mips-smoke',
    }


    ################################################################################
    ################################################################################
    ####                                                                        ####
    ####                    SANDBOX TEST BUILDERS                               ####
    ####                                                                        ####
    ################################################################################
    ################################################################################




    ##########################################
    #### builder for windows-test-sandbox ####
    ##########################################
    sb_windows_test_factory = factory.BuildFactory()
    sb_windows_test_factory.addStep(test_commandline)
    sb_windows_test_factory.addStep(test_selftest(name="Release", shellname="avmshell"))
    sb_windows_test_factory.addStep(test_generic(name="Release", shellname="avmshell", vmargs="", config="", scriptargs=""))
    sb_windows_test_factory.addStep(test_generic(name="Release-interp", shellname="avmshell", vmargs="-Dinterp", config="", scriptargs=""))
    sb_windows_test_factory.addStep(test_generic(name="Release-jit", shellname="avmshell", vmargs="-Ojit", config="", scriptargs=""))
    sb_windows_test_factory.addStep(test_generic(name="ReleaseDebugger", shellname="avmshell_s", vmargs="", config="", scriptargs=""))
    sb_windows_test_factory.addStep(test_generic(name="Debug", shellname="avmshell_d", vmargs="", config="", scriptargs=""))
    sb_windows_test_factory.addStep(test_generic(name="DebugDebugger", shellname="avmshell_sd", vmargs="", config="", scriptargs=""))
    sb_windows_test_factory.addStep(test_differential)
    sb_windows_test_factory.addStep(util_process_clean)
    sb_windows_test_factory.addStep(util_clean_buildsdir)
    sb_windows_test_factory.addStep(sync_clean)

    sb_windows_test_builder = {
                'name': "windows-test-sandbox",
                'slavename': "windows",
                'factory': sb_windows_test_factory,
                'builddir': './sandbox-windows-test',
    }

    ############################################
    #### builder for windows64-test-sandbox ####
    ############################################
    sb_windows_64_test_factory = factory.BuildFactory()
    sb_windows_64_test_factory.addStep(test_commandline)
    sb_windows_64_test_factory.addStep(test_selftest(name="Release", shellname="avmshell_64"))
    sb_windows_64_test_factory.addStep(test_generic(name="Release", shellname="avmshell_64", vmargs="", config="", scriptargs=""))
    sb_windows_64_test_factory.addStep(test_generic(name="Release-interp", shellname="avmshell_64", vmargs="-Dinterp", config="", scriptargs=""))
    sb_windows_64_test_factory.addStep(test_generic(name="Release-jit", shellname="avmshell_64", vmargs="-Ojit", config="", scriptargs=""))
    sb_windows_64_test_factory.addStep(test_generic(name="ReleaseDebugger", shellname="avmshell_s_64", vmargs="", config="", scriptargs=""))
    sb_windows_64_test_factory.addStep(test_generic(name="Debug", shellname="avmshell_d_64", vmargs="", config="", scriptargs=""))
    sb_windows_64_test_factory.addStep(test_generic(name="DebugDebugger", shellname="avmshell_sd_64", vmargs="", config="", scriptargs=""))
    sb_windows_64_test_factory.addStep(util_process_clean)
    sb_windows_64_test_factory.addStep(util_clean_buildsdir)
    sb_windows_64_test_factory.addStep(sync_clean)

    sb_windows_64_test_builder = {
                'name': "windows64-test-sandbox",
                'slavename': "windows64",
                'factory': sb_windows_64_test_factory,
                'builddir': './sandbox-windows64-test',
    }


    #################################################
    #### builder for mac-intel-10_5-test-sandbox ####
    #################################################
    sb_mac_intel_105_test_factory = factory.BuildFactory()
    sb_mac_intel_105_test_factory.addStep(test_commandline)
    sb_mac_intel_105_test_factory.addStep(test_selftest(name="Release", shellname="avmshell"))
    sb_mac_intel_105_test_factory.addStep(test_generic(name="Release", shellname="avmshell", vmargs="", config="", scriptargs=""))
    sb_mac_intel_105_test_factory.addStep(test_generic(name="Release-interp", shellname="avmshell", vmargs="-Dinterp", config="", scriptargs=""))
    sb_mac_intel_105_test_factory.addStep(test_generic(name="Release-jit", shellname="avmshell", vmargs="-Ojit", config="", scriptargs=""))
    sb_mac_intel_105_test_factory.addStep(test_generic(name="ReleaseDebugger", shellname="avmshell_s", vmargs="", config="", scriptargs=""))
    sb_mac_intel_105_test_factory.addStep(test_generic(name="Debug", shellname="avmshell_d", vmargs="", config="", scriptargs=""))
    sb_mac_intel_105_test_factory.addStep(test_generic(name="DebugDebugger", shellname="avmshell_sd", vmargs="", config="", scriptargs=""))
    sb_mac_intel_105_test_factory.addStep(test_differential)
    sb_mac_intel_105_test_factory.addStep(util_process_clean)
    sb_mac_intel_105_test_factory.addStep(util_clean_buildsdir)
    sb_mac_intel_105_test_factory.addStep(sync_clean)

    sb_mac_intel_105_test_builder = {
                'name': "mac-intel-10.5-test-sandbox",
                'slavename': "mac-intel-10_5",
                'factory': sb_mac_intel_105_test_factory,
                'builddir': './sandbox-mac-intel-10_5-test',
    }

    ##############################################
    #### builder for mac64-intel-test-sandbox ####
    ##############################################
    sb_mac_intel_64_test_factory = factory.BuildFactory()
    sb_mac_intel_64_test_factory.addStep(test_commandline)
    sb_mac_intel_64_test_factory.addStep(test_selftest(name="Release", shellname="avmshell_64"))
    sb_mac_intel_64_test_factory.addStep(test_generic(name="Release", shellname="avmshell_64", vmargs="", config="", scriptargs=""))
    sb_mac_intel_64_test_factory.addStep(test_generic(name="Release-interp", shellname="avmshell_64", vmargs="-Dinterp", config="", scriptargs=""))
    sb_mac_intel_64_test_factory.addStep(test_generic(name="Release-jit", shellname="avmshell_64", vmargs="-Ojit", config="", scriptargs=""))
    sb_mac_intel_64_test_factory.addStep(test_generic(name="ReleaseDebugger", shellname="avmshell_s_64", vmargs="", config="", scriptargs=""))
    sb_mac_intel_64_test_factory.addStep(test_generic(name="Debug", shellname="avmshell_d_64", vmargs="", config="", scriptargs=""))
    sb_mac_intel_64_test_factory.addStep(test_generic(name="DebugDebugger", shellname="avmshell_sd_64", vmargs="", config="", scriptargs=""))
    sb_mac_intel_64_test_factory.addStep(util_process_clean)
    sb_mac_intel_64_test_factory.addStep(util_clean_buildsdir)
    sb_mac_intel_64_test_factory.addStep(sync_clean)

    sb_mac_intel_64_test_builder = {
                'name': "mac64-intel-test-sandbox",
                'slavename': "mac64-intel",
                'factory': sb_mac_intel_64_test_factory,
                'builddir': './sandbox-mac64-intel-test',
    }


    ########################################
    #### builder for linux-test-sandbox ####
    ########################################
    sb_linux_test_factory = factory.BuildFactory()
    sb_linux_test_factory.addStep(test_commandline)
    sb_linux_test_factory.addStep(test_selftest(name="Release", shellname="avmshell"))
    sb_linux_test_factory.addStep(test_generic(name="Release", shellname="avmshell", vmargs="", config="", scriptargs=""))
    sb_linux_test_factory.addStep(test_generic(name="Release-interp", shellname="avmshell", vmargs="-Dinterp", config="", scriptargs=""))
    sb_linux_test_factory.addStep(test_generic(name="Release-jit", shellname="avmshell", vmargs="-Ojit", config="", scriptargs=""))
    sb_linux_test_factory.addStep(test_generic(name="ReleaseDebugger", shellname="avmshell_s", vmargs="", config="", scriptargs=""))
    sb_linux_test_factory.addStep(test_generic(name="Debug", shellname="avmshell_d", vmargs="", config="", scriptargs=""))
    sb_linux_test_factory.addStep(test_generic(name="DebugDebugger", shellname="avmshell_sd", vmargs="", config="", scriptargs=""))
    sb_linux_test_factory.addStep(test_differential)
    sb_linux_test_factory.addStep(util_process_clean)
    sb_linux_test_factory.addStep(util_clean_buildsdir)
    sb_linux_test_factory.addStep(sync_clean)

    sb_linux_test_builder = {
                'name': "linux-test-sandbox",
                'slavename': "linux",
                'factory': sb_linux_test_factory,
                'builddir': './sandbox-linux-test',
    }


    ##########################################
    #### builder for linux64-test-sandbox ####
    ##########################################
    sb_linux_64_test_factory = factory.BuildFactory()
    sb_linux_64_test_factory.addStep(test_commandline)
    sb_linux_64_test_factory.addStep(test_selftest(name="Release", shellname="avmshell_64"))
    sb_linux_64_test_factory.addStep(test_generic(name="Release", shellname="avmshell_64", vmargs="", config="", scriptargs=""))
    sb_linux_64_test_factory.addStep(test_generic(name="Release-interp", shellname="avmshell_64", vmargs="-Dinterp", config="", scriptargs=""))
    sb_linux_64_test_factory.addStep(test_generic(name="Release-jit", shellname="avmshell_64", vmargs="-Ojit", config="", scriptargs=""))
    sb_linux_64_test_factory.addStep(test_generic(name="ReleaseDebugger", shellname="avmshell_s_64", vmargs="", config="", scriptargs=""))
    sb_linux_64_test_factory.addStep(test_generic(name="Debug", shellname="avmshell_d_64", vmargs="", config="", scriptargs=""))
    sb_linux_64_test_factory.addStep(test_generic(name="DebugDebugger", shellname="avmshell_sd_64", vmargs="", config="", scriptargs=""))
    sb_linux_64_test_factory.addStep(util_process_clean)
    sb_linux_64_test_factory.addStep(util_clean_buildsdir)
    sb_linux_64_test_factory.addStep(sync_clean)

    sb_linux_64_test_builder = {
                'name': "linux64-test-sandbox",
                'slavename': "linux64",
                'factory': sb_linux_64_test_factory,
                'builddir': './sandbox-linux64-test',
    }


    ########################################
    #### builder for android-test       ####
    ########################################
    sb_android_test_factory = factory.BuildFactory()
    sb_android_test_factory.addStep(test_generic_adb(name="Release", shellname="avmshell", vmargs="", config="", scriptargs=""))
    sb_android_test_factory.addStep(test_generic_adb(name="Release-interp", shellname="avmshell", vmargs="-Dinterp", config="", scriptargs=""))
    sb_android_test_factory.addStep(test_generic_adb(name="Release-jit", shellname="avmshell", vmargs="-Ojit", config="", scriptargs=""))
    sb_android_test_factory.addStep(test_generic_adb(name="Debug", shellname="avmshell_d", vmargs="", config="", scriptargs=""))
    sb_android_test_factory.addStep(util_process_clean)
    sb_android_test_factory.addStep(util_clean_buildsdir)
    sb_android_test_factory.addStep(sync_clean)

    sb_android_test_builder = {
                'name': "android-test-sandbox",
                'slavename': "android",
                'factory': sb_android_test_factory,
                'builddir': './sandbox-android-test',
    }
    
    ##########################################
    #### builder for linux-arm-test       ####
    ##########################################
    sb_linux_arm_test_factory = factory.BuildFactory()
    sb_linux_arm_test_factory.addStep(test_selftest_ssh(name="Release", shellname="avmshell_neon_arm"))
    sb_linux_arm_test_factory.addStep(test_generic_ssh(name="Release-vfp", shellname="avmshell_neon_arm", vmargs="-Darm_arch 7 -Darm_vfp", config="arm-lnx-tvm-release", scriptargs=""))
    sb_linux_arm_test_factory.addStep(test_generic_ssh(name="Release-interp", shellname="avmshell_neon_arm", vmargs="-Dinterp", config="arm-lnx-tvm-release-Dinterp", scriptargs=""))
    sb_linux_arm_test_factory.addStep(test_generic_ssh(name="Release-jit-vfp", shellname="avmshell_neon_arm", vmargs="-Darm_arch 7 -Darm_vfp -Ojit", config="arm-lnx-tvm-release-Ojit", scriptargs=""))
    sb_linux_arm_test_factory.addStep(util_acceptance_clean_ssh)
    sb_linux_arm_test_factory.addStep(util_clean_buildsdir)
    sb_linux_arm_test_factory.addStep(sync_clean)

    sb_linux_arm_test_builder = {
                'name': "linux-arm-test-sandbox",
                'slavename': "linux-arm",
                'factory': sb_linux_arm_test_factory,
                'builddir': './sandbox-linux-arm-test',
    }
    
    
    ##########################################
    #### builder for linux-mips-test      ####
    ##########################################
    sb_linux_mips_test_factory = factory.BuildFactory()
    sb_linux_mips_test_factory.addStep(test_generic_ssh(name="Release", shellname="avmshell_mips", vmargs="", config="mips-lnx-tvm-release", scriptargs=""))
    sb_linux_mips_test_factory.addStep(test_generic_ssh(name="Debug", shellname="avmshell_mips_d", vmargs="", config="mips-lnx-tvm-debug", scriptargs=""))
    sb_linux_mips_test_factory.addStep(util_process_clean_ssh)
    sb_linux_mips_test_factory.addStep(util_clean_buildsdir)
    sb_linux_mips_test_factory.addStep(sync_clean)

    sb_linux_mips_test_builder = {
                'name': "linux-mips-test-sandbox",
                'slavename': "linux-mips",
                'factory': sb_linux_mips_test_factory,
                'builddir': './sandbox-linux-mips-test',
    }


    builders = [
                sb_windows_compile_builder,
                sb_windows_64_compile_builder,
                sb_mac_intel_105_compile_builder,
                sb_mac_intel_64_compile_builder,
                sb_linux_compile_builder,
                sb_linux_64_compile_builder,
                sb_android_compile_builder,
                sb_linux_arm_compile_builder,
                sb_linux_mips_compile_builder,
                
                sb_windows_smoke_builder,
                sb_windows_64_smoke_builder,
                sb_mac_intel_105_smoke_builder,
                sb_mac_intel_64_smoke_builder,
                sb_linux_smoke_builder,
                sb_linux_64_smoke_builder,
                sb_android_smoke_builder,
                sb_linux_arm_smoke_builder,
                sb_linux_mips_smoke_builder,
                
                sb_windows_test_builder,
                sb_windows_64_test_builder,
                sb_mac_intel_105_test_builder,
                sb_mac_intel_64_test_builder,
                sb_linux_test_builder,
                sb_linux_64_test_builder,
                sb_android_test_builder,
                sb_linux_arm_test_builder,
                sb_linux_mips_test_builder,

                ]

