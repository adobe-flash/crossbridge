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

class argo:
    
    HG_URL = "http://asteam.macromedia.com/hg/tamarin-redux-argo/"
    BRANCH = "tamarin-argo"
    
    ####### SCHEDULERS
    from buildbot.scheduler import *
    # custom.buildbot_ext.scheduler import MUST happen after importing buildbot.scheduler
    from custom.buildbot_ext.scheduler import *

    compile = Scheduler(name="compile-argo", branch=BRANCH, treeStableTimer=30, fileIsImportant=startCompile, properties={'silent':'false'},
                     builderNames=["windows-compile-argo", "windows64-compile-argo",
                                   "mac-intel-10.4-compile-argo", "mac-intel-10.5-compile-argo", "mac64-intel-compile-argo",
                                   "mac-ppc-10.4a-compile-argo", "mac-ppc-10.4b-compile-argo", 
                                   "mac-ppc-10.5a-compile-argo", "mac-ppc-10.5b-compile-argo", 
                                   "mac64-ppc-compile-argo", "mac64b-ppc-compile-argo",
                                   "linux-compile-argo", "linux64-compile-argo",
                                   "winmobile-emulator-compile-argo",
                                   "solaris-sparc-compile-argo", "solaris-sparc2-compile-argo",
                                   "android-compile-argo",
                                   "linux-arm-compile-argo", "linux-arm2-compile-argo"])

    smoke = BuilderDependent(name="smoke-argo",upstream=compile, callbackInterval=60, properties={'silent':'false'},
                    builderNames=["windows-smoke-argo", "windows64-smoke-argo",
                                   "mac-intel-10.4-smoke-argo", "mac-intel-10.5-smoke-argo", "mac64-intel-smoke-argo",
                                   "mac-ppc-10.4a-smoke-argo", "mac-ppc-10.4b-smoke-argo", 
                                   "mac-ppc-10.5a-smoke-argo", "mac-ppc-10.5b-smoke-argo", 
                                   "mac64-ppc-smoke-argo", "mac64b-ppc-smoke-argo",
                                   "linux-smoke-argo", "linux64-smoke-argo",
                                   "winmobile-emulator-smoke-argo",
                                   "solaris-sparc-smoke-argo", "solaris-sparc2-smoke-argo",
                                   "android-smoke-argo",
                                   "linux-arm-smoke-argo", "linux-arm2-smoke-argo"],
                    builderDependencies=[
                                  ["windows-smoke-argo", "windows-compile-argo"], 
                                  ["windows64-smoke-argo", "windows64-compile-argo"], 
                                  ["mac-intel-10.4-smoke-argo", "mac-intel-10.4-compile-argo"], 
                                  ["mac-intel-10.5-smoke-argo", "mac-intel-10.5-compile-argo"],
                                  ["mac64-intel-smoke-argo", "mac64-intel-compile-argo"],
                                  ["mac-ppc-10.4a-smoke-argo", "mac-intel-10.4-compile-argo"],
                                  ["mac-ppc-10.4b-smoke-argo", "mac-intel-10.4-compile-argo"],
                                  ["mac-ppc-10.5a-smoke-argo", "mac-intel-10.5-compile-argo"],
                                  ["mac-ppc-10.5b-smoke-argo", "mac-intel-10.5-compile-argo"],
                                  ["mac64-ppc-smoke-argo", "mac64-intel-compile-argo"],
                                  ["mac64b-ppc-smoke-argo", "mac64-intel-compile-argo"],
                                  ["linux-smoke-argo", "linux-compile-argo"],
                                  ["linux64-smoke-argo", "linux64-compile-argo"],
                                  ["winmobile-emulator-smoke-argo", "winmobile-emulator-compile-argo"],
                                  ["solaris-sparc-smoke-argo", "solaris-sparc-compile-argo"],
                                  ["solaris-sparc2-smoke-argo", "solaris-sparc-compile-argo"],
                                  ["android-smoke-argo","android-compile-argo"],
                                  ["linux-arm-smoke-argo","linux-compile-argo"],
                                  ["linux-arm2-smoke-argo","linux-compile-argo"],
                                 ])

    test = BuilderDependent(name="test-argo",upstream=smoke, callbackInterval=60, properties={'silent':'false'},
                    builderNames=["windows-test-argo", "windows64-test-argo",
                                   "mac-intel-10.4-test-argo", "mac-intel-10.5-test-argo", "mac64-intel-test-argo",
                                   "mac-ppc-10.4a-test-argo", "mac-ppc-10.4b-test-argo", 
                                   "mac-ppc-10.5a-test-argo", "mac-ppc-10.5b-test-argo", 
                                   "mac64-ppc-test-argo", "mac64b-ppc-test-argo",
                                   "linux-test-argo", "linux64-test-argo",
                                   "winmobile-emulator-test-argo",
                                   "solaris-sparc-test-argo", "solaris-sparc2-test-argo",
                                   "android-test-argo",
                                   "linux-arm-test-argo", "linux-arm2-test-argo"],
                    builderDependencies=[
                                  ["windows-test-argo", "windows-smoke-argo"], 
                                  ["windows64-test-argo", "windows64-smoke-argo"], 
                                  ["mac-intel-10.4-test-argo", "mac-intel-10.4-smoke-argo"], 
                                  ["mac-intel-10.5-test-argo", "mac-intel-10.5-smoke-argo"],
                                  ["mac64-intel-test-argo", "mac64-intel-smoke-argo"],
                                  ["mac-ppc-10.4a-test-argo", "mac-ppc-10.4a-smoke-argo"],
                                  ["mac-ppc-10.4b-test-argo", "mac-ppc-10.4b-smoke-argo"],
                                  ["mac-ppc-10.5a-test-argo", "mac-ppc-10.5a-smoke-argo"],
                                  ["mac-ppc-10.5b-test-argo", "mac-ppc-10.5b-smoke-argo"],
                                  ["mac64-ppc-test-argo", "mac64-ppc-smoke-argo"],
                                  ["mac64b-ppc-test-argo", "mac64b-ppc-smoke-argo"],
                                  ["linux-test-argo", "linux-smoke-argo"],
                                  ["linux64-test-argo", "linux64-smoke-argo"],
                                  ["winmobile-emulator-test-argo", "winmobile-emulator-smoke-argo"],
                                  ["solaris-sparc-test-argo", "solaris-sparc-smoke-argo"],
                                  ["solaris-sparc2-test-argo", "solaris-sparc2-smoke-argo"],
                                  ["android-test-argo", "android-smoke-argo"],
                                  ["linux-arm-test-argo", "linux-arm-smoke-argo"],
                                  ["linux-arm2-test-argo", "linux-arm2-smoke-argo"],
                                 ])

    performance = PhaseTwoScheduler(name="performance-argo", branch="%s-performance" % BRANCH, treeStableTimer=30, properties={'silent':'false'},
                    fileIsImportant=startPerformanceRun, priority=1, changeDir="changes/perf/processed",
                    builderNames=["windows-performance-argo",
                                   "mac-performance-argo", "mac64-performance-argo",
                                   "mac-ppc-performance-argo",
                                   "linux-performance-argo",
                                   "winmobile-performance-argo",
                                   "android-performance-argo"],
                    builderDependencies=[
                                  ["windows-performance-argo", "windows-test-argo"], 
                                  ["mac-performance-argo", "mac-intel-10.5-test-argo"],
                                  ["mac64-performance-argo", "mac64-intel-test-argo"],
                                  ["mac-ppc-performance-argo", "mac-ppc-10.5a-test-argo"],
                                  ["linux-performance-argo", "linux-test-argo"],
                                  ["winmobile-performance-argo", "winmobile-emulator-test-argo"],
                                  ["android-performance-argo", "android-test-argo"], 
                                 ])
    
    deep = PhaseTwoScheduler(name="deep-argo", branch="%s-deep" % BRANCH, treeStableTimer=30, properties={'silent':'false'},
                    fileIsImportant=startCompile, priority=1, changeDir="changes/deep/processed",
                    builderNames=[
                                    "windows-deep-argo",
                                    "windows-p3-deep-argo",
                                    "windows-frr-argo",
                                    "mac-ppc-deep-argo",
                                    "solaris-sparc-deep-argo",
                                    "windows64-deep-argo",
                                    "linux-deep-argo",
                                    "linux-arm-deep-argo",
                                    "winmobile-emulator-deep-argo",
                                 ],
                    builderDependencies=[
                                  ["linux-deep-argo", "linux-test-argo"],
                                  ["linux-arm-deep-argo", "linux-arm-test-argo"],
                                  ["windows-deep-argo", "windows-test-argo"],
                                  ["windows-p3-deep-argo", "windows-test-argo"],
                                  ["windows-frr-argo", "windows-test-argo"], 
                                  ["mac-ppc-deep-argo", "mac-ppc-10.5a-test-argo"], 
                                  ["solaris-sparc-deep-argo", "solaris-sparc-test-argo"], 
                                  ["windows64-deep-argo", "windows64-test-argo"], 
                                  ["winmobile-emulator-deep-argo", "winmobile-emulator-test-argo"],
                                 ])

    schedulers = [compile, smoke, test, performance, deep]
    
    

    ################################################################################
    ################################################################################
    ####                                                                        ####
    ####                          COMPILE BUILDERS                              ####
    ####                                                                        ####
    ################################################################################
    ################################################################################

    #####################################
    #### builder for windows-compile ####
    #####################################

    windows_compile_factory = factory.BuildFactory()
    windows_compile_factory.addStep(sync_clean)
    windows_compile_factory.addStep(sync_clone(url=HG_URL))
    windows_compile_factory.addStep(sync_update)
    windows_compile_factory.addStep(bb_slaveupdate(slave="windows"))
    windows_compile_factory.addStep(compile_builtin)
    windows_compile_factory.addStep(compile_generic(name="Release", shellname="avmshell", args="--enable-shell", upload="false"))
    windows_compile_factory.addStep(compile_generic(name="Release-wordcode", shellname="avmshell_wordcode", args="--enable-shell --enable-wordcode-interp", upload="false"))
    windows_compile_factory.addStep(compile_generic(name="Debug", shellname="avmshell_d", args="--enable-shell --enable-debug", upload="false"))
    windows_compile_factory.addStep(compile_generic(name="ReleaseDebugger", shellname="avmshell_s", args="--enable-shell --enable-debugger", upload="false"))
    windows_compile_factory.addStep(compile_generic(name="DebugDebugger", shellname="avmshell_sd", args="--enable-shell --enable-debug --enable-debugger", upload="false"))
    windows_compile_factory.addStep(BuildShellCommand(
                command=['../all/file-check.py', '../../../../../repo'],
                env={'branch': WithProperties('%s','branch')},
                description='running file-check against source...',
                descriptionDone='finished file-check.',
                name="FileCheck",
                workdir="../repo/build/buildbot/slaves/scripts")
    )
    windows_compile_factory.addStep(compile_buildcheck)
    windows_compile_factory.addStep(util_upload_asteam)
    windows_compile_factory.addStep(BuildShellCommand(
                command=['./build-release-sizereport.sh',WithProperties('%s','revision')],
                env={'branch': WithProperties('%s','branch')},
                description='starting win release-sizereport build...',
                descriptionDone='finished win release-sizereport build.',
                name='Build_Release_sizereport',
                workdir="../repo/build/buildbot/slaves/scripts")
    )

    windows_compile_builder = {
                'name': "windows-compile-argo",
                'slavename': "windows",
                'factory': windows_compile_factory,
                'builddir': './argo-windows-compile',
    }




    #######################################
    #### builder for windows64-compile ####
    #######################################
    windows_64_compile_factory = factory.BuildFactory()
    windows_64_compile_factory.addStep(sync_clean)
    windows_64_compile_factory.addStep(sync_clone(url=HG_URL))
    windows_64_compile_factory.addStep(sync_update)
    windows_64_compile_factory.addStep(bb_slaveupdate(slave="windows64"))

    windows_64_compile_builder = {
                'name': "windows64-compile-argo",
                'slavename': "windows64",
                'factory': windows_64_compile_factory,
                'builddir': './argo-windows64-compile',
    }


    ############################################
    #### builder for mac-intel-10_4-compile ####
    ############################################
    mac_intel_104_compile_factory = factory.BuildFactory()
    mac_intel_104_compile_factory.addStep(sync_clean)
    mac_intel_104_compile_factory.addStep(sync_clone(url=HG_URL))
    mac_intel_104_compile_factory.addStep(sync_update)
    mac_intel_104_compile_factory.addStep(bb_slaveupdate(slave="mac-intel-10_4"))
    mac_intel_104_compile_factory.addStep(compile_builtin)
    mac_intel_104_compile_factory.addStep(compile_generic(name="Release", shellname="avmshell_104", args="--enable-shell --mac-sdk=104u", upload="false"))
    mac_intel_104_compile_factory.addStep(compile_generic(name="Release-wordcode", shellname="avmshell_wordcode_104", args="--enable-shell --enable-wordcode-interp --mac-sdk=104u", upload="false"))
    mac_intel_104_compile_factory.addStep(compile_generic(name="Debug", shellname="avmshell_d_104", args="--enable-shell --enable-debug --mac-sdk=104u", upload="false"))
    mac_intel_104_compile_factory.addStep(compile_generic(name="ReleaseDebugger", shellname="avmshell_s_104", args="--enable-shell --enable-debugger --mac-sdk=104u", upload="false"))
    mac_intel_104_compile_factory.addStep(compile_generic(name="DebugDebugger", shellname="avmshell_sd_104", args="--enable-shell --enable-debug --enable-debugger --mac-sdk=104u", upload="false"))
    mac_intel_104_compile_factory.addStep(compile_generic(name="Release_PPC", shellname="avmshell_104_ppc", args="--enable-shell --target=ppc-darwin --mac-sdk=104u", upload="false"))
    mac_intel_104_compile_factory.addStep(compile_generic(name="Release-wordcode_PPC", shellname="avmshell_wordcode_104_ppc", args="--enable-shell --enable-wordcode-interp --target=ppc-darwin --mac-sdk=104u", upload="false"))
    mac_intel_104_compile_factory.addStep(compile_generic(name="Debug_PPC", shellname="avmshell_d_104_ppc", args="--enable-shell --enable-debug --target=ppc-darwin --mac-sdk=104u", upload="false"))
    mac_intel_104_compile_factory.addStep(compile_generic(name="ReleaseDebugger_PPC", shellname="avmshell_s_104_ppc", args="--enable-shell --enable-debugger --target=ppc-darwin --mac-sdk=104u", upload="false"))
    mac_intel_104_compile_factory.addStep(compile_generic(name="DebugDebugger_PPC", shellname="avmshell_sd_104_ppc", args="--enable-shell --enable-debug --enable-debugger --target=ppc-darwin --mac-sdk=104u", upload="false"))
    mac_intel_104_compile_factory.addStep(compile_buildcheck_local)
    mac_intel_104_compile_factory.addStep(util_upload_asteam_local)

    mac_intel_104_compile_builder = {
                'name': "mac-intel-10.4-compile-argo",
                'slavename': "mac-intel-10_4",
                'factory': mac_intel_104_compile_factory,
                'builddir': './argo-mac-intel-10_4-compile',
    }



    ############################################
    #### builder for mac-intel-10_5-compile ####
    ############################################
    mac_intel_105_compile_factory = factory.BuildFactory()
    mac_intel_105_compile_factory.addStep(sync_clean)
    mac_intel_105_compile_factory.addStep(sync_clone(url=HG_URL))
    mac_intel_105_compile_factory.addStep(sync_update)
    mac_intel_105_compile_factory.addStep(bb_slaveupdate(slave="mac-intel-10_5"))
    mac_intel_105_compile_factory.addStep(compile_builtin)
    mac_intel_105_compile_factory.addStep(compile_generic(name="Release", shellname="avmshell", args="--enable-shell --mac-sdk=104u", upload="false"))
    mac_intel_105_compile_factory.addStep(compile_generic(name="Release-wordcode", shellname="avmshell_wordcode", args="--enable-shell --enable-wordcode-interp --mac-sdk=104u", upload="false"))
    mac_intel_105_compile_factory.addStep(compile_generic(name="Debug", shellname="avmshell_d", args="--enable-shell --enable-debug --mac-sdk=104u", upload="false"))
    mac_intel_105_compile_factory.addStep(compile_generic(name="ReleaseDebugger", shellname="avmshell_s", args="--enable-shell --enable-debugger --mac-sdk=104u", upload="false"))
    mac_intel_105_compile_factory.addStep(compile_generic(name="DebugDebugger", shellname="avmshell_sd", args="--enable-shell --enable-debug --enable-debugger --mac-sdk=104u", upload="false"))
    mac_intel_105_compile_factory.addStep(compile_generic(name="Release_PPC", shellname="avmshell_ppc", args="--enable-shell --target=ppc-darwin --mac-sdk=104u", upload="false"))
    mac_intel_105_compile_factory.addStep(compile_generic(name="Release-wordcode_PPC", shellname="avmshell_wordcode_ppc", args="--enable-shell --enable-wordcode-interp --target=ppc-darwin --mac-sdk=104u", upload="false"))
    mac_intel_105_compile_factory.addStep(compile_generic(name="Debug_PPC", shellname="avmshell_d_ppc", args="--enable-shell --enable-debug --target=ppc-darwin --mac-sdk=104u", upload="false"))
    mac_intel_105_compile_factory.addStep(compile_generic(name="ReleaseDebugger_PPC", shellname="avmshell_s_ppc", args="--enable-shell --enable-debugger --target=ppc-darwin --mac-sdk=104u", upload="false"))
    mac_intel_105_compile_factory.addStep(compile_generic(name="DebugDebugger_PPC", shellname="avmshell_sd_ppc", args="--enable-shell --enable-debug --enable-debugger --target=ppc-darwin --mac-sdk=104u", upload="false"))
    mac_intel_105_compile_factory.addStep(compile_buildcheck_local)
    mac_intel_105_compile_factory.addStep(util_upload_asteam_local)

    mac_intel_105_compile_builder = {
                'name': "mac-intel-10.5-compile-argo",
                'slavename': "mac-intel-10_5",
                'factory': mac_intel_105_compile_factory,
                'builddir': './argo-mac-intel-10_5-compile',
    }


    ##########################################
    #### builder for mac-intel-64-compile ####
    ##########################################
    mac_intel_64_compile_factory = factory.BuildFactory()
    mac_intel_64_compile_factory.addStep(sync_clean)
    mac_intel_64_compile_factory.addStep(sync_clone(url=HG_URL))
    mac_intel_64_compile_factory.addStep(sync_update)
    mac_intel_64_compile_factory.addStep(bb_slaveupdate(slave="mac64-intel"))
    mac_intel_64_compile_factory.addStep(compile_builtin)
    mac_intel_64_compile_factory.addStep(compile_generic(name="Release", shellname="avmshell_64", args="--enable-shell --target=x86_64-darwin --mac-sdk=105", upload="false"))
    mac_intel_64_compile_factory.addStep(compile_generic(name="Release-wordcode", shellname="avmshell_wordcode_64", args="--enable-shell --enable-wordcode-interp --target=x86_64-darwin --mac-sdk=105", upload="false"))
    mac_intel_64_compile_factory.addStep(compile_generic(name="Debug", shellname="avmshell_d_64", args="--enable-shell --enable-debug --target=x86_64-darwin --mac-sdk=105", upload="false"))
    mac_intel_64_compile_factory.addStep(compile_generic(name="ReleaseDebugger", shellname="avmshell_s_64", args="--enable-shell --enable-debugger --target=x86_64-darwin --mac-sdk=105", upload="false"))
    mac_intel_64_compile_factory.addStep(compile_generic(name="DebugDebugger", shellname="avmshell_sd_64", args="--enable-shell --enable-debug --enable-debugger --target=x86_64-darwin --mac-sdk=105", upload="false"))
    mac_intel_64_compile_factory.addStep(compile_generic(name="Release_PPC", shellname="avmshell_64_ppc", args="--enable-shell --target=ppc64-darwin --mac-sdk=105", upload="false"))
    mac_intel_64_compile_factory.addStep(compile_generic(name="Release-wordcode_PPC", shellname="avmshell_wordcode_64_ppc", args="--enable-shell --enable-wordcode-interp --target=ppc64-darwin --mac-sdk=105", upload="false"))
    mac_intel_64_compile_factory.addStep(compile_generic(name="Debug_PPC", shellname="avmshell_d_64_ppc", args="--enable-shell --enable-debug --target=ppc64-darwin --mac-sdk=105", upload="false"))
    mac_intel_64_compile_factory.addStep(compile_generic(name="ReleaseDebugger_PPC", shellname="avmshell_s_64_ppc", args="--enable-shell --enable-debugger --target=ppc64-darwin --mac-sdk=105", upload="false"))
    mac_intel_64_compile_factory.addStep(compile_generic(name="DebugDebugger_PPC", shellname="avmshell_sd_64_ppc", args="--enable-shell --enable-debug --enable-debugger --target=ppc64-darwin --mac-sdk=105", upload="false"))
    mac_intel_64_compile_factory.addStep(compile_buildcheck_local)
    mac_intel_64_compile_factory.addStep(util_upload_asteam_local)

    mac_intel_64_compile_builder = {
                'name': "mac64-intel-compile-argo",
                'slavename': "mac64-intel",
                'factory': mac_intel_64_compile_factory,
                'builddir': './argo-mac64-intel-compile',
    }


    ############################################
    #### builder for mac-ppc-10_4a-compile ####
    ############################################
    mac_ppc_104a_compile_factory = factory.BuildFactory()
    mac_ppc_104a_compile_factory.addStep(sync_clean)
    mac_ppc_104a_compile_factory.addStep(sync_clone(url=HG_URL))
    mac_ppc_104a_compile_factory.addStep(sync_update)
    mac_ppc_104a_compile_factory.addStep(bb_slaveupdate(slave="mac-ppc-10_4"))

    mac_ppc_104a_compile_builder = {
                'name': "mac-ppc-10.4a-compile-argo",
                'slavename': "mac-ppc-10_4a",
                'factory': mac_ppc_104a_compile_factory,
                'builddir': './argo-mac-ppc-10_4a-compile',
    }


    ###########################################
    #### builder for mac-ppc-10_4b-compile ####
    ###########################################
    mac_ppc_104b_compile_factory = factory.BuildFactory()
    mac_ppc_104b_compile_factory.addStep(sync_clean)
    mac_ppc_104b_compile_factory.addStep(sync_clone(url=HG_URL))
    mac_ppc_104b_compile_factory.addStep(sync_update)
    mac_ppc_104b_compile_factory.addStep(bb_slaveupdate(slave="mac-ppc-10_4"))

    mac_ppc_104b_compile_builder = {
                'name': "mac-ppc-10.4b-compile-argo",
                'slavename': "mac-ppc-10_4b",
                'factory': mac_ppc_104b_compile_factory,
                'builddir': './argo-mac-ppc-10_4b-compile',
    }



    ###########################################
    #### builder for mac-ppc-10_5a-compile ####
    ###########################################
    mac_ppc_105a_compile_factory = factory.BuildFactory()
    mac_ppc_105a_compile_factory.addStep(sync_clean)
    mac_ppc_105a_compile_factory.addStep(sync_clone(url=HG_URL))
    mac_ppc_105a_compile_factory.addStep(sync_update)
    mac_ppc_105a_compile_factory.addStep(bb_slaveupdate(slave="mac-ppc-10_5"))

    mac_ppc_105a_compile_builder = {
                'name': "mac-ppc-10.5a-compile-argo",
                'slavename': "mac-ppc-10_5a",
                'factory': mac_ppc_105a_compile_factory,
                'builddir': './argo-mac-ppc-10_5a-compile',
    }

    ###########################################
    #### builder for mac-ppc-10_5b-compile ####
    ###########################################
    mac_ppc_105b_compile_factory = factory.BuildFactory()
    mac_ppc_105b_compile_factory.addStep(sync_clean)
    mac_ppc_105b_compile_factory.addStep(sync_clone(url=HG_URL))
    mac_ppc_105b_compile_factory.addStep(sync_update)
    mac_ppc_105b_compile_factory.addStep(bb_slaveupdate(slave="mac-ppc-10_5"))

    mac_ppc_105b_compile_builder = {
                'name': "mac-ppc-10.5b-compile-argo",
                'slavename': "mac-ppc-10_5b",
                'factory': mac_ppc_105b_compile_factory,
                'builddir': './argo-mac-ppc-10_5b-compile',
    }


    ########################################
    #### builder for mac-ppc-64-compile ####
    ########################################
    mac_ppc_64_compile_factory = factory.BuildFactory()
    mac_ppc_64_compile_factory.addStep(sync_clean)
    mac_ppc_64_compile_factory.addStep(sync_clone(url=HG_URL))
    mac_ppc_64_compile_factory.addStep(sync_update)
    mac_ppc_64_compile_factory.addStep(bb_slaveupdate(slave="mac64-ppc"))

    mac_ppc_64_compile_builder = {
                'name': "mac64-ppc-compile-argo",
                'slavename': "mac64a-ppc",
                'factory': mac_ppc_64_compile_factory,
                'builddir': './argo-mac64-ppc-compile',
    }
    

    #########################################
    #### builder for mac-ppc-64b-compile ####
    #########################################
    mac_ppc_64b_compile_factory = factory.BuildFactory()
    mac_ppc_64b_compile_factory.addStep(sync_clean)
    mac_ppc_64b_compile_factory.addStep(sync_clone(url=HG_URL))
    mac_ppc_64b_compile_factory.addStep(sync_update)
    mac_ppc_64b_compile_factory.addStep(bb_slaveupdate(slave="mac64-ppc"))

    mac_ppc_64b_compile_builder = {
                'name': "mac64b-ppc-compile-argo",
                'slavename': "mac64b-ppc",
                'factory': mac_ppc_64b_compile_factory,
                'builddir': './argo-mac64b-ppc-compile',
    }


    ###################################
    #### builder for linux-compile ####
    ###################################
    linux_compile_factory = factory.BuildFactory()
    linux_compile_factory.addStep(sync_clean)
    linux_compile_factory.addStep(sync_clone(url=HG_URL))
    linux_compile_factory.addStep(sync_update)
    linux_compile_factory.addStep(bb_slaveupdate(slave="linux"))
    linux_compile_factory.addStep(compile_builtin)
    linux_compile_factory.addStep(compile_generic(name="Release", shellname="avmshell", args="--enable-shell", upload="false"))
    linux_compile_factory.addStep(compile_generic(name="Release-wordcode", shellname="avmshell_wordcode", args="--enable-shell --enable-wordcode-interp", upload="false"))
    linux_compile_factory.addStep(compile_generic(name="Debug", shellname="avmshell_d", args="--enable-shell --enable-debug", upload="false"))
    linux_compile_factory.addStep(compile_generic(name="ReleaseDebugger", shellname="avmshell_s", args="--enable-shell --enable-debugger", upload="false"))
    linux_compile_factory.addStep(compile_generic(name="DebugDebugger", shellname="avmshell_sd", args="--enable-shell --enable-debug --enable-debugger", upload="false"))
    linux_compile_factory.addStep(BuildShellCommand(
                command=['../all/compile-generic.sh', WithProperties('%s','revision'), '--enable-shell --enable-arm-neon --target=arm-linux --enable-sys-root-dir=/usr/local/arm-linux/debian5', 'avmshell_neon_arm', 'false'],
                env={
                    'branch': WithProperties('%s','branch'),
                    'CXX': 'arm-none-linux-gnueabi-g++',
                    'CC' : 'arm-none-linux-gnueabi-gcc',
                    'LD' : 'arm-none-linux-gnueabi-ld',
                    'AR' : 'arm-none-linux-gnueabi-ar',
                },
                description='starting Release_arm-linux build...',
                descriptionDone='finished Release_arm-linux build.',
                name="Release_arm-linux",
                workdir="../repo/build/buildbot/slaves/scripts")
    )
    linux_compile_factory.addStep(BuildShellCommand(
                command=['../all/compile-generic.sh', WithProperties('%s','revision'), '--enable-shell --enable-debug --enable-arm-neon --target=arm-linux --enable-sys-root-dir=/usr/local/arm-linux/debian5', 'avmshell_neon_arm_d', 'false'],
                env={
                    'branch': WithProperties('%s','branch'),
                    'CXX': 'arm-none-linux-gnueabi-g++',
                    'CC' : 'arm-none-linux-gnueabi-gcc',
                    'LD' : 'arm-none-linux-gnueabi-ld',
                    'AR' : 'arm-none-linux-gnueabi-ar',
                },
                description='starting Debug_arm-linux build...',
                descriptionDone='finished Debug_arm-linux build.',
                name="Debug_arm-linux",
                workdir="../repo/build/buildbot/slaves/scripts")
    )
    linux_compile_factory.addStep(compile_buildcheck_local)
    linux_compile_factory.addStep(util_upload_asteam_local)

    linux_compile_builder = {
                'name': "linux-compile-argo",
                'slavename': "linux",
                'factory': linux_compile_factory,
                'builddir': './argo-linux-compile',
    }


    #####################################
    #### builder for linux64-compile ####
    #####################################
    linux_64_compile_factory = factory.BuildFactory()
    linux_64_compile_factory.addStep(sync_clean)
    linux_64_compile_factory.addStep(sync_clone(url=HG_URL))
    linux_64_compile_factory.addStep(sync_update)
    linux_64_compile_factory.addStep(bb_slaveupdate(slave="linux64"))
    linux_64_compile_factory.addStep(compile_builtin)
    linux_64_compile_factory.addStep(compile_generic(name="Release", shellname="avmshell_64", args="--enable-shell", upload="false"))
    linux_64_compile_factory.addStep(compile_generic(name="Release-wordcode", shellname="avmshell_wordcode_64", args="--enable-shell --enable-wordcode-interp", upload="false"))
    linux_64_compile_factory.addStep(compile_generic(name="Debug", shellname="avmshell_d_64", args="--enable-shell --enable-debug", upload="false"))
    linux_64_compile_factory.addStep(compile_generic(name="ReleaseDebugger", shellname="avmshell_s_64", args="--enable-shell --enable-debugger", upload="false"))
    linux_64_compile_factory.addStep(compile_generic(name="DebugDebugger", shellname="avmshell_sd_64", args="--enable-shell --enable-debug --enable-debugger", upload="false"))
    linux_64_compile_factory.addStep(compile_testmedia)
    linux_64_compile_factory.addStep(compile_buildcheck_local)
    linux_64_compile_factory.addStep(util_upload_asteam_local)

    linux_64_compile_builder = {
                'name': "linux64-compile-argo",
                'slavename': "linux64",
                'factory': linux_64_compile_factory,
                'builddir': './argo-linux64-compile',
    }



    ################################################
    #### builder for winmobile-emulator-compile ####
    ################################################
    winmobile_emulator_compile_factory = factory.BuildFactory()
    winmobile_emulator_compile_factory.addStep(sync_clean)
    winmobile_emulator_compile_factory.addStep(sync_clone(url=HG_URL))
    winmobile_emulator_compile_factory.addStep(sync_update)
    winmobile_emulator_compile_factory.addStep(bb_slaveupdate(slave="winmobile-arm"))
    winmobile_emulator_compile_factory.addStep(compile_builtin)
    winmobile_emulator_compile_factory.addStep(compile_generic(name="ReleaseARM", shellname="avmshell_arm", args="--enable-shell --target=arm-windows", upload="false"))
    winmobile_emulator_compile_factory.addStep(compile_generic(name="Release-wordcode-ARM", shellname="avmshell_wordcode_arm", args="--enable-shell --enable-wordcode-interp --target=arm-windows", upload="false"))
    winmobile_emulator_compile_factory.addStep(compile_generic(name="Release-fpu-ARM", shellname="avmshell_fpu_arm", args="--enable-shell --enable-arm-fpu --target=arm-windows", upload="false"))
    winmobile_emulator_compile_factory.addStep(compile_generic(name="DebugARM", shellname="avmshell_arm_d", args="--enable-shell --enable-debug --target=arm-windows", upload="false"))
    winmobile_emulator_compile_factory.addStep(compile_generic(name="Debug-fpu-ARM", shellname="avmshell_fpu_arm_d", args="--enable-shell --enable-debug --enable-arm-fpu --target=arm-windows", upload="false"))
    winmobile_emulator_compile_factory.addStep(BuildShellCommand(
                command=['../all/compile-generic.sh', WithProperties('%s','revision'), '--enable-shell --target=x86_64-win', 'avmshell_64', 'false'],
                env={
                    'branch': WithProperties('%s','branch'),
                    'compile64':'true'
                },
                description='starting Release64 build...',
                descriptionDone='finished Release64 build.',
                name="Release64",
                workdir="../repo/build/buildbot/slaves/scripts")
    )
    winmobile_emulator_compile_factory.addStep(BuildShellCommand(
                command=['../all/compile-generic.sh', WithProperties('%s','revision'), '--enable-shell --enable-wordcode-interp --target=x86_64-win', 'avmshell_wordcode_64', 'false'],
                env={
                    'branch': WithProperties('%s','branch'),
                    'compile64':'true'
                },
                description='starting Release-wordcode64 build...',
                descriptionDone='finished Release-wordcode64 build.',
                name="Release-wordcode64",
                workdir="../repo/build/buildbot/slaves/scripts")
    )
    winmobile_emulator_compile_factory.addStep(BuildShellCommand(
                command=['../all/compile-generic.sh', WithProperties('%s','revision'), '--enable-shell --enable-debug --target=x86_64-win', 'avmshell_d_64', 'false'],
                env={
                    'branch': WithProperties('%s','branch'),
                    'compile64':'true'
                },
                description='starting Debug64 build...',
                descriptionDone='finished Debug64 build.',
                name="Debug64",
                workdir="../repo/build/buildbot/slaves/scripts")
    )
    winmobile_emulator_compile_factory.addStep(BuildShellCommand(
                command=['../all/compile-generic.sh', WithProperties('%s','revision'), '--enable-shell --enable-debugger --target=x86_64-win', 'avmshell_s_64', 'false'],
                env={
                    'branch': WithProperties('%s','branch'),
                    'compile64':'true'
                },
                description='starting ReleaseDebugger64 build...',
                descriptionDone='finished ReleaseDebugger64 build.',
                name="ReleaseDebugger64",
                workdir="../repo/build/buildbot/slaves/scripts")
    )
    winmobile_emulator_compile_factory.addStep(BuildShellCommand(
                command=['../all/compile-generic.sh', WithProperties('%s','revision'), '--enable-shell --enable-debug --enable-debugger --target=x86_64-win', 'avmshell_sd_64', 'false'],
                env={
                    'branch': WithProperties('%s','branch'),
                    'compile64':'true'
                },
                description='starting DebugDebugger64 build...',
                descriptionDone='finished DebugDebugger64 build.',
                name="DebugDebugger64",
                workdir="../repo/build/buildbot/slaves/scripts")
    )
    winmobile_emulator_compile_factory.addStep(compile_buildcheck_local)
    winmobile_emulator_compile_factory.addStep(util_upload_asteam_local)
    winmobile_emulator_compile_factory.addStep(BuildShellCommand(
                command=['./build-release-mobile-pocketpc-arm-sizereport.sh', WithProperties('%s','revision')],
                env={'branch': WithProperties('%s','branch')},
                description='starting to run sizereport...',
                descriptionDone='finished sizereport.',
                name="Build_Release_sizereport",
                workdir="../repo/build/buildbot/slaves/scripts")
    )

    winmobile_emulator_compile_builder = {
                'name': "winmobile-emulator-compile-argo",
                'slavename': "winmobile-emulator",
                'factory': winmobile_emulator_compile_factory,
                'builddir': './argo-winmobile-emulator-compile',
    }


    ###########################################
    #### builder for solaris-sparc-compile ####
    ###########################################
    solaris_sparc_compile_factory = factory.BuildFactory()
    solaris_sparc_compile_factory.addStep(sync_clean)
    solaris_sparc_compile_factory.addStep(sync_clone(url=HG_URL))
    solaris_sparc_compile_factory.addStep(sync_update)
    solaris_sparc_compile_factory.addStep(bb_slaveupdate(slave="solaris-sparc"))
    solaris_sparc_compile_factory.addStep(compile_builtin)
    solaris_sparc_compile_factory.addStep(compile_generic(name="Release", shellname="avmshell", args="--enable-shell", upload="false"))
    solaris_sparc_compile_factory.addStep(compile_generic(name="Release-wordcode", shellname="avmshell_wordcode", args="--enable-shell --enable-wordcode-interp", upload="false"))
    solaris_sparc_compile_factory.addStep(compile_generic(name="Debug", shellname="avmshell_d", args="--enable-shell --enable-debug", upload="false"))
    solaris_sparc_compile_factory.addStep(compile_generic(name="ReleaseDebugger", shellname="avmshell_s", args="--enable-shell --enable-debugger", upload="false"))
    solaris_sparc_compile_factory.addStep(compile_generic(name="DebugDebugger", shellname="avmshell_sd", args="--enable-shell --enable-debug --enable-debugger", upload="false"))
    solaris_sparc_compile_factory.addStep(compile_buildcheck_local)
    solaris_sparc_compile_factory.addStep(util_upload_asteam)

    solaris_sparc_compile_builder = {
                'name': "solaris-sparc-compile-argo",
                'slavename': "solaris-sparc",
                'factory': solaris_sparc_compile_factory,
                'builddir': './argo-solaris-sparc-compile',
    }
    
    
    ############################################
    #### builder for solaris-sparc2-compile ####
    ############################################
    solaris_sparc2_compile_factory = factory.BuildFactory()
    solaris_sparc2_compile_factory.addStep(sync_clean)
    solaris_sparc2_compile_factory.addStep(sync_clone(url=HG_URL))
    solaris_sparc2_compile_factory.addStep(sync_update)
    solaris_sparc2_compile_factory.addStep(bb_slaveupdate(slave="solaris-sparc"))
    
    solaris_sparc2_compile_builder = {
                'name': "solaris-sparc2-compile-argo",
                'slavename': "solaris-sparc2",
                'factory': solaris_sparc2_compile_factory,
                'builddir': './argo-solaris-sparc2-compile',
    }

    ###########################################
    #### builder for android on mac        ####
    ###########################################

    android_compile_factory = factory.BuildFactory();
    android_compile_factory.addStep(sync_clean)
    android_compile_factory.addStep(sync_clone(url=HG_URL))
    android_compile_factory.addStep(sync_update)
    android_compile_factory.addStep(bb_slaveupdate(slave="mac-intel-server"))
    android_compile_factory.addStep(compile_builtin)
    android_compile_factory.addStep(BuildShellCommand(
                command=['./build-debug-shell-android.sh', WithProperties('%s','revision')],
                env={'branch': WithProperties('%s','branch')},
                description='building debug shell...',
                descriptionDone='finished building debug shell.',
                name="Build_Debug",
                workdir="../repo/build/buildbot/slaves/scripts",
                timeout=3600)
    )
    android_compile_factory.addStep(BuildShellCommand(
                command=['./build-release-shell-android.sh', WithProperties('%s','revision')],
                env={'branch': WithProperties('%s','branch')},
                description='building release shell...',
                descriptionDone='finished building release shell.',
                name="Build_Release",
                workdir="../repo/build/buildbot/slaves/scripts",
                timeout=3600)
    )
    android_compile_factory.addStep(BuildShellCommand(
                command=['./build-check-android.sh', WithProperties('%s','revision')],
                env={'branch': WithProperties('%s','branch')},
                description='running build check...',
                descriptionDone='finished build check.',
                name="Build_Check",
                workdir="../repo/build/buildbot/slaves/scripts",
                timeout=3600)
    )
    android_compile_factory.addStep(BuildShellCommand(
                command=['./upload-asteam-android.sh', WithProperties('%s','revision')],
                env={'branch': WithProperties('%s','branch')},
                description='running upload to asteam...',
                descriptionDone='finished upload to asteam.',
                name="Upload_ASTEAM",
                workdir="../repo/build/buildbot/slaves/scripts",
                timeout=3600)
    )

    android_compile_builder = {
                'name': "android-compile-argo",
                'slavename': "android",
                'factory': android_compile_factory,
                'builddir': './argo-android-compile',
    }


    ###############################
    #### builder for linux-arm ####
    ###############################
    linux_arm_compile_factory = factory.BuildFactory()
    linux_arm_compile_factory.addStep(sync_clean)
    linux_arm_compile_factory.addStep(sync_clone(url=HG_URL))
    linux_arm_compile_factory.addStep(sync_update)
    linux_arm_compile_factory.addStep(bb_slaveupdate(slave="linux-arm"))

    linux_arm_compile_builder = {
                'name': "linux-arm-compile-argo",
                'slavename': "linux-arm",
                'factory': linux_arm_compile_factory,
                'builddir': './argo-linux-arm-compile',
    }
    
    
    ################################
    #### builder for linux-arm2 ####
    ################################
    linux_arm2_compile_factory = factory.BuildFactory()
    linux_arm2_compile_factory.addStep(sync_clean)
    linux_arm2_compile_factory.addStep(sync_clone(url=HG_URL))
    linux_arm2_compile_factory.addStep(sync_update)
    linux_arm2_compile_factory.addStep(bb_slaveupdate(slave="linux-arm"))

    linux_arm2_compile_builder = {
                'name': "linux-arm2-compile-argo",
                'slavename': "linux-arm2",
                'factory': linux_arm2_compile_factory,
                'builddir': './argo-linux-arm2-compile',
    }

    ################################################################################
    ################################################################################
    ####                                                                        ####
    ####                            SMOKE BUILDERS                              ####
    ####                                                                        ####
    ################################################################################
    ################################################################################




    ###################################
    #### builder for windows-smoke ####
    ###################################
    windows_smoke_factory = factory.BuildFactory()
    windows_smoke_factory.addStep(download_testmedia)
    windows_smoke_factory.addStep(test_smoke)
    windows_smoke_factory.addStep(util_process_clean)

    windows_smoke_builder = {
                'name': "windows-smoke-argo",
                'slavename': "windows",
                'factory': windows_smoke_factory,
                'builddir': './argo-windows-smoke',
    }


    #####################################
    #### builder for windows64-smoke ####
    #####################################
    windows_64_smoke_factory = factory.BuildFactory()
    windows_64_smoke_factory.addStep(download_testmedia)
    windows_64_smoke_factory.addStep(test_smoke)
    windows_64_smoke_factory.addStep(util_process_clean)

    windows_64_smoke_builder = {
                'name': "windows64-smoke-argo",
                'slavename': "windows64",
                'factory': windows_64_smoke_factory,
                'builddir': './argo-windows64-smoke',
    }


    ##########################################
    #### builder for mac-intel-10_4-smoke ####
    ##########################################
    mac_intel_104_smoke_factory = factory.BuildFactory()
    mac_intel_104_smoke_factory.addStep(download_testmedia)
    mac_intel_104_smoke_factory.addStep(test_smoke)
    mac_intel_104_smoke_factory.addStep(util_process_clean)

    mac_intel_104_smoke_builder = {
                'name': "mac-intel-10.4-smoke-argo",
                'slavename': "mac-intel-10_4",
                'factory': mac_intel_104_smoke_factory,
                'builddir': './argo-mac-intel-10_4-smoke',
    }


    ##########################################
    #### builder for mac-intel-10_5-smoke ####
    ##########################################
    mac_intel_105_smoke_factory = factory.BuildFactory()
    mac_intel_105_smoke_factory.addStep(download_testmedia)
    mac_intel_105_smoke_factory.addStep(test_smoke)
    mac_intel_105_smoke_factory.addStep(util_process_clean)

    mac_intel_105_smoke_builder = {
                'name': "mac-intel-10.5-smoke-argo",
                'slavename': "mac-intel-10_5",
                'factory': mac_intel_105_smoke_factory,
                'builddir': './argo-mac-intel-10_5-smoke',
    }


    #######################################
    #### builder for mac64-intel-smoke ####
    #######################################
    mac_intel_64_smoke_factory = factory.BuildFactory()
    mac_intel_64_smoke_factory.addStep(download_testmedia)
    mac_intel_64_smoke_factory.addStep(test_smoke)
    mac_intel_64_smoke_factory.addStep(util_process_clean)

    mac_intel_64_smoke_builder = {
                'name': "mac64-intel-smoke-argo",
                'slavename': "mac64-intel",
                'factory': mac_intel_64_smoke_factory,
                'builddir': './argo-mac64-intel-smoke',
    }

    #########################################
    #### builder for mac-ppc-10_4a-smoke ####
    #########################################
    mac_ppc_104a_smoke_factory = factory.BuildFactory()
    mac_ppc_104a_smoke_factory.addStep(download_testmedia)
    mac_ppc_104a_smoke_factory.addStep(test_smoke)
    mac_ppc_104a_smoke_factory.addStep(util_process_clean)

    mac_ppc_104a_smoke_builder = {
                'name': "mac-ppc-10.4a-smoke-argo",
                'slavename': "mac-ppc-10_4a",
                'factory': mac_ppc_104a_smoke_factory,
                'builddir': './argo-mac-ppc-10_4a-smoke',
    }


    #########################################
    #### builder for mac-ppc-10_4b-smoke ####
    #########################################
    mac_ppc_104b_smoke_factory = factory.BuildFactory()
    mac_ppc_104b_smoke_factory.addStep(download_testmedia)
    mac_ppc_104b_smoke_factory.addStep(test_smoke)
    mac_ppc_104b_smoke_factory.addStep(util_process_clean)

    mac_ppc_104b_smoke_builder = {
                'name': "mac-ppc-10.4b-smoke-argo",
                'slavename': "mac-ppc-10_4b",
                'factory': mac_ppc_104b_smoke_factory,
                'builddir': './argo-mac-ppc-10_4b-smoke',
    }


    #########################################
    #### builder for mac-ppc-10_5a-smoke ####
    #########################################
    mac_ppc_105a_smoke_factory = factory.BuildFactory()
    mac_ppc_105a_smoke_factory.addStep(download_testmedia)
    mac_ppc_105a_smoke_factory.addStep(test_smoke)
    mac_ppc_105a_smoke_factory.addStep(util_process_clean)

    mac_ppc_105a_smoke_builder = {
                'name': "mac-ppc-10.5a-smoke-argo",
                'slavename': "mac-ppc-10_5a",
                'factory': mac_ppc_105a_smoke_factory,
                'builddir': './argo-mac-ppc-10_5a-smoke',
    }

    #########################################
    #### builder for mac-ppc-10_5b-smoke ####
    #########################################
    mac_ppc_105b_smoke_factory = factory.BuildFactory()
    mac_ppc_105b_smoke_factory.addStep(download_testmedia)
    mac_ppc_105b_smoke_factory.addStep(test_smoke)
    mac_ppc_105b_smoke_factory.addStep(util_process_clean)

    mac_ppc_105b_smoke_builder = {
                'name': "mac-ppc-10.5b-smoke-argo",
                'slavename': "mac-ppc-10_5b",
                'factory': mac_ppc_105b_smoke_factory,
                'builddir': './argo-mac-ppc-10_5b-smoke',
    }


    #####################################
    #### builder for mac64-ppc-smoke ####
    #####################################
    mac_ppc_64_smoke_factory = factory.BuildFactory()
    mac_ppc_64_smoke_factory.addStep(download_testmedia)
    mac_ppc_64_smoke_factory.addStep(test_smoke)
    mac_ppc_64_smoke_factory.addStep(util_process_clean)

    mac_ppc_64_smoke_builder = {
                'name': "mac64-ppc-smoke-argo",
                'slavename': "mac64a-ppc",
                'factory': mac_ppc_64_smoke_factory,
                'builddir': './argo-mac64-ppc-smoke',
    }


    ######################################
    #### builder for mac64b-ppc-smoke ####
    ######################################
    mac_ppc_64b_smoke_factory = factory.BuildFactory()
    mac_ppc_64b_smoke_factory.addStep(download_testmedia)
    mac_ppc_64b_smoke_factory.addStep(test_smoke)
    mac_ppc_64b_smoke_factory.addStep(util_process_clean)

    mac_ppc_64b_smoke_builder = {
                'name': "mac64b-ppc-smoke-argo",
                'slavename': "mac64b-ppc",
                'factory': mac_ppc_64b_smoke_factory,
                'builddir': './argo-mac64b-ppc-smoke',
    }

    #################################
    #### builder for linux-smoke ####
    #################################
    linux_smoke_factory = factory.BuildFactory()
    linux_smoke_factory.addStep(download_testmedia)
    linux_smoke_factory.addStep(test_smoke)
    linux_smoke_factory.addStep(util_process_clean)

    linux_smoke_builder = {
                'name': "linux-smoke-argo",
                'slavename': "linux",
                'factory': linux_smoke_factory,
                'builddir': './argo-linux-smoke',
    }


    ###################################
    #### builder for linux64-smoke ####
    ###################################
    linux_64_smoke_factory = factory.BuildFactory()
    linux_64_smoke_factory.addStep(download_testmedia)
    linux_64_smoke_factory.addStep(test_smoke)
    linux_64_smoke_factory.addStep(util_process_clean)

    linux_64_smoke_builder = {
                'name': "linux64-smoke-argo",
                'slavename': "linux64",
                'factory': linux_64_smoke_factory,
                'builddir': './argo-linux64-smoke',
    }


    ##############################################
    #### builder for winmobile-emulator-smoke ####
    ##############################################
    winmobile_emulator_smoke_factory = factory.BuildFactory()
    winmobile_emulator_smoke_factory.addStep(download_testmedia)
    winmobile_emulator_smoke_factory.addStep(test_emulator_smoke_mobile)
    winmobile_emulator_smoke_factory.addStep(util_process_clean)

    winmobile_emulator_smoke_builder = {
                'name': "winmobile-emulator-smoke-argo",
                'slavename': "winmobile-emulator",
                'factory': winmobile_emulator_smoke_factory,
                'builddir': './argo-winmobile-emulator-smoke',
    }


    #########################################
    #### builder for solaris-sparc-smoke ####
    #########################################
    solaris_sparc_smoke_factory = factory.BuildFactory()
    solaris_sparc_smoke_factory.addStep(download_testmedia)
    solaris_sparc_smoke_factory.addStep(test_smoke)
    solaris_sparc_smoke_factory.addStep(util_process_clean)

    solaris_sparc_smoke_builder = {
                'name': "solaris-sparc-smoke-argo",
                'slavename': "solaris-sparc",
                'factory': solaris_sparc_smoke_factory,
                'builddir': './argo-solaris-sparc-smoke',
    }
    
    
    ##########################################
    #### builder for solaris-sparc2-smoke ####
    ##########################################
    solaris_sparc2_smoke_factory = factory.BuildFactory()
    solaris_sparc2_smoke_factory.addStep(download_testmedia)
    solaris_sparc2_smoke_factory.addStep(test_smoke)
    solaris_sparc2_smoke_factory.addStep(util_process_clean)

    solaris_sparc2_smoke_builder = {
                'name': "solaris-sparc2-smoke-argo",
                'slavename': "solaris-sparc2",
                'factory': solaris_sparc2_smoke_factory,
                'builddir': './argo-solaris-sparc2-smoke',
    }


    #########################################
    #### builder for android-smoke       ####
    #########################################
    android_smoke_factory = factory.BuildFactory()
    android_smoke_factory.addStep(download_testmedia)
    android_smoke_factory.addStep(test_smoke_local)
    android_smoke_factory.addStep(util_process_clean)

    android_smoke_builder = {
                'name': "android-smoke-argo",
                'slavename': "android",
                'factory': android_smoke_factory,
                'builddir': './argo-android-smoke',
    }


    ###########################################
    #### builder for linxu-arm-smoke       ####
    ###########################################
    linux_arm_smoke_factory = factory.BuildFactory()
    linux_arm_smoke_factory.addStep(download_testmedia)
    linux_arm_smoke_factory.addStep(TestSuiteShellCommand(
                command=['../all/run-smoketests.sh', WithProperties('%s','revision'), './runsmokes-arm.txt'],
                env={'branch': WithProperties('%s','branch')},
                description='starting to run smoke tests...',
                descriptionDone='finished smoke tests.',
                name="SmokeTest",
                workdir="../repo/build/buildbot/slaves/scripts")
    )
    linux_arm_smoke_factory.addStep(util_process_clean)

    linux_arm_smoke_builder = {
                'name': "linux-arm-smoke-argo",
                'slavename': "linux-arm",
                'factory': linux_arm_smoke_factory,
                'builddir': './argo-linux-arm-smoke',
    }
    
    
    ###########################################
    #### builder for linxu-arm2-smoke      ####
    ###########################################
    linux_arm2_smoke_factory = factory.BuildFactory()
    linux_arm2_smoke_factory.addStep(download_testmedia)
    linux_arm2_smoke_factory.addStep(TestSuiteShellCommand(
                command=['../all/run-smoketests.sh', WithProperties('%s','revision'), './runsmokes-arm.txt'],
                env={'branch': WithProperties('%s','branch')},
                description='starting to run smoke tests...',
                descriptionDone='finished smoke tests.',
                name="SmokeTest",
                workdir="../repo/build/buildbot/slaves/scripts")
    )
    linux_arm2_smoke_factory.addStep(util_process_clean)

    linux_arm2_smoke_builder = {
                'name': "linux-arm2-smoke-argo",
                'slavename': "linux-arm2",
                'factory': linux_arm2_smoke_factory,
                'builddir': './argo-linux-arm2-smoke',
    }


    ################################################################################
    ################################################################################
    ####                                                                        ####
    ####                            TEST BUILDERS                               ####
    ####                                                                        ####
    ################################################################################
    ################################################################################




    ##################################
    #### builder for windows-test ####
    ##################################
    windows_test_factory = factory.BuildFactory()
    windows_test_factory.addStep(test_commandline)
    windows_test_factory.addStep(test_selftest(name="Release", shellname="avmshell"))
    windows_test_factory.addStep(test_generic(name="Release", shellname="avmshell", vmargs="", config="", scriptargs=""))
    windows_test_factory.addStep(test_generic(name="Release-interp", shellname="avmshell", vmargs="-Dinterp", config="", scriptargs=""))
    windows_test_factory.addStep(test_generic(name="Release-wordcode-interp", shellname="avmshell_wordcode", vmargs="-Dinterp", config="", scriptargs=""))
    windows_test_factory.addStep(test_generic(name="Release-jit", shellname="avmshell", vmargs="-Ojit", config="", scriptargs=""))
    windows_test_factory.addStep(test_generic(name="ReleaseDebugger", shellname="avmshell_s", vmargs="", config="", scriptargs=""))
    windows_test_factory.addStep(test_generic(name="Debug", shellname="avmshell_d", vmargs="", config="", scriptargs=""))
    windows_test_factory.addStep(test_generic(name="DebugDebugger", shellname="avmshell_sd", vmargs="", config="", scriptargs=""))
    windows_test_factory.addStep(test_differential)
    windows_test_factory.addStep(util_process_clean)
    windows_test_factory.addStep(util_clean_buildsdir)

    windows_test_builder = {
                'name': "windows-test-argo",
                'slavename': "windows",
                'factory': windows_test_factory,
                'builddir': './argo-windows-test',
    }


    ####################################
    #### builder for windows64-test ####
    ####################################
    windows_64_test_factory = factory.BuildFactory()
    windows_64_test_factory.addStep(test_commandline)
    windows_64_test_factory.addStep(test_selftest(name="Release", shellname="avmshell_64"))
    windows_64_test_factory.addStep(test_generic(name="Release", shellname="avmshell_64", vmargs="", config="", scriptargs=""))
    windows_64_test_factory.addStep(test_generic(name="Release-interp", shellname="avmshell_64", vmargs="-Dinterp", config="", scriptargs=""))
    windows_64_test_factory.addStep(test_generic(name="Release-wordcode-interp", shellname="avmshell_wordcode_64", vmargs="-Dinterp", config="", scriptargs=""))
    windows_64_test_factory.addStep(test_generic(name="Release-jit", shellname="avmshell_64", vmargs="-Ojit", config="", scriptargs=""))
    windows_64_test_factory.addStep(test_generic(name="ReleaseDebugger", shellname="avmshell_s_64", vmargs="", config="", scriptargs=""))
    windows_64_test_factory.addStep(test_generic(name="Debug", shellname="avmshell_d_64", vmargs="", config="", scriptargs=""))
    windows_64_test_factory.addStep(test_generic(name="DebugDebugger", shellname="avmshell_sd_64", vmargs="", config="", scriptargs=""))
    windows_64_test_factory.addStep(util_process_clean)
    windows_64_test_factory.addStep(util_clean_buildsdir)

    windows_64_test_builder = {
                'name': "windows64-test-argo",
                'slavename': "windows64",
                'factory': windows_64_test_factory,
                'builddir': './argo-windows64-test',
    }


    #########################################
    #### builder for mac-intel-10_4-test ####
    #########################################
    mac_intel_104_test_factory = factory.BuildFactory()
    mac_intel_104_test_factory.addStep(test_commandline)
    mac_intel_104_test_factory.addStep(test_selftest(name="Release", shellname="avmshell_104"))
    mac_intel_104_test_factory.addStep(test_generic(name="Release", shellname="avmshell_104", vmargs="", config="", scriptargs=""))
    mac_intel_104_test_factory.addStep(test_generic(name="Release-interp", shellname="avmshell_104", vmargs="-Dinterp", config="", scriptargs=""))
    mac_intel_104_test_factory.addStep(test_generic(name="Release-wordcode-interp", shellname="avmshell_wordcode_104", vmargs="-Dinterp", config="", scriptargs=""))
    mac_intel_104_test_factory.addStep(test_generic(name="Release-jit", shellname="avmshell_104", vmargs="-Ojit", config="", scriptargs=""))
    mac_intel_104_test_factory.addStep(test_generic(name="ReleaseDebugger", shellname="avmshell_s_104", vmargs="", config="", scriptargs=""))
    mac_intel_104_test_factory.addStep(test_generic(name="Debug", shellname="avmshell_d_104", vmargs="", config="", scriptargs=""))
    mac_intel_104_test_factory.addStep(test_generic(name="DebugDebugger", shellname="avmshell_sd_104", vmargs="", config="", scriptargs=""))
    mac_intel_104_test_factory.addStep(test_differential)
    mac_intel_104_test_factory.addStep(util_process_clean)
    mac_intel_104_test_factory.addStep(util_clean_buildsdir)

    mac_intel_104_test_builder = {
                'name': "mac-intel-10.4-test-argo",
                'slavename': "mac-intel-10_4",
                'factory': mac_intel_104_test_factory,
                'builddir': './argo-mac-intel-10_4-test',
    }


    #########################################
    #### builder for mac-intel-10_5-test ####
    #########################################
    mac_intel_105_test_factory = factory.BuildFactory()
    mac_intel_105_test_factory.addStep(test_commandline)
    mac_intel_105_test_factory.addStep(test_selftest(name="Release", shellname="avmshell"))
    mac_intel_105_test_factory.addStep(test_generic(name="Release", shellname="avmshell", vmargs="", config="", scriptargs=""))
    mac_intel_105_test_factory.addStep(test_generic(name="Release-interp", shellname="avmshell", vmargs="-Dinterp", config="", scriptargs=""))
    mac_intel_105_test_factory.addStep(test_generic(name="Release-wordcode-interp", shellname="avmshell_wordcode", vmargs="-Dinterp", config="", scriptargs=""))
    mac_intel_105_test_factory.addStep(test_generic(name="Release-jit", shellname="avmshell", vmargs="-Ojit", config="", scriptargs=""))
    mac_intel_105_test_factory.addStep(test_generic(name="ReleaseDebugger", shellname="avmshell_s", vmargs="", config="", scriptargs=""))
    mac_intel_105_test_factory.addStep(test_generic(name="Debug", shellname="avmshell_d", vmargs="", config="", scriptargs=""))
    mac_intel_105_test_factory.addStep(test_generic(name="DebugDebugger", shellname="avmshell_sd", vmargs="", config="", scriptargs=""))
    mac_intel_105_test_factory.addStep(test_differential)
    mac_intel_105_test_factory.addStep(util_process_clean)
    mac_intel_105_test_factory.addStep(util_clean_buildsdir)

    mac_intel_105_test_builder = {
                'name': "mac-intel-10.5-test-argo",
                'slavename': "mac-intel-10_5",
                'factory': mac_intel_105_test_factory,
                'builddir': './argo-mac-intel-10_5-test',
    }


    ######################################
    #### builder for mac64-intel-test ####
    ######################################
    mac_intel_64_test_factory = factory.BuildFactory()
    mac_intel_64_test_factory.addStep(test_commandline)
    mac_intel_64_test_factory.addStep(test_selftest(name="Release", shellname="avmshell_64"))
    mac_intel_64_test_factory.addStep(test_generic(name="Release", shellname="avmshell_64", vmargs="", config="", scriptargs=""))
    mac_intel_64_test_factory.addStep(test_generic(name="Release-interp", shellname="avmshell_64", vmargs="-Dinterp", config="", scriptargs=""))
    mac_intel_64_test_factory.addStep(test_generic(name="Release-wordcode-interp", shellname="avmshell_wordcode_64", vmargs="-Dinterp", config="", scriptargs=""))
    mac_intel_64_test_factory.addStep(test_generic(name="Release-jit", shellname="avmshell_64", vmargs="-Ojit", config="", scriptargs=""))
    mac_intel_64_test_factory.addStep(test_generic(name="ReleaseDebugger", shellname="avmshell_s_64", vmargs="", config="", scriptargs=""))
    mac_intel_64_test_factory.addStep(test_generic(name="Debug", shellname="avmshell_d_64", vmargs="", config="", scriptargs=""))
    mac_intel_64_test_factory.addStep(test_generic(name="DebugDebugger", shellname="avmshell_sd_64", vmargs="", config="", scriptargs=""))
    mac_intel_64_test_factory.addStep(util_process_clean)
    mac_intel_64_test_factory.addStep(util_clean_buildsdir)

    mac_intel_64_test_builder = {
                'name': "mac64-intel-test-argo",
                'slavename': "mac64-intel",
                'factory': mac_intel_64_test_factory,
                'builddir': './argo-mac64-intel-test',
    }

    ########################################
    #### builder for mac-ppc-10_4a-test ####
    ########################################
    mac_ppc_104a_test_factory = factory.BuildFactory()
    mac_ppc_104a_test_factory.addStep(test_commandline)
    mac_ppc_104a_test_factory.addStep(test_selftest(name="Release", shellname="avmshell_104_ppc"))
    mac_ppc_104a_test_factory.addStep(test_generic(name="Release", shellname="avmshell_104_ppc", vmargs="", config="", scriptargs=""))
    mac_ppc_104a_test_factory.addStep(test_generic(name="Release-interp", shellname="avmshell_104_ppc", vmargs="-Dinterp", config="", scriptargs=""))
    mac_ppc_104a_test_factory.addStep(test_generic(name="Release-wordcode-interp", shellname="avmshell_wordcode_104_ppc", vmargs="-Dinterp", config="", scriptargs=""))
    mac_ppc_104a_test_factory.addStep(test_generic(name="Release-jit", shellname="avmshell_104_ppc", vmargs="-Ojit", config="", scriptargs=""))
    mac_ppc_104a_test_factory.addStep(test_generic(name="Debug", shellname="avmshell_d_104_ppc", vmargs="", config="", scriptargs=""))
    mac_ppc_104a_test_factory.addStep(util_process_clean)
    mac_ppc_104a_test_factory.addStep(util_clean_buildsdir)

    mac_ppc_104a_test_builder = {
                'name': "mac-ppc-10.4a-test-argo",
                'slavename': "mac-ppc-10_4a",
                'factory': mac_ppc_104a_test_factory,
                'builddir': './argo-mac-ppc-10_4a-test',
    }


    ########################################
    #### builder for mac-ppc-10_4b-test ####
    ########################################
    mac_ppc_104b_test_factory = factory.BuildFactory()
    mac_ppc_104b_test_factory.addStep(test_generic(name="ReleaseDebugger", shellname="avmshell_s_104_ppc", vmargs="", config="", scriptargs=""))
    mac_ppc_104b_test_factory.addStep(test_generic(name="DebugDebugger", shellname="avmshell_sd_104_ppc", vmargs="", config="", scriptargs=""))
    mac_ppc_104b_test_factory.addStep(util_process_clean)
    mac_ppc_104b_test_factory.addStep(util_clean_buildsdir)

    mac_ppc_104b_test_builder = {
                'name': "mac-ppc-10.4b-test-argo",
                'slavename': "mac-ppc-10_4b",
                'factory': mac_ppc_104b_test_factory,
                'builddir': './argo-mac-ppc-10_4b-test',
    }


    ########################################
    #### builder for mac-ppc-10_5a-test ####
    ########################################
    mac_ppc_105a_test_factory = factory.BuildFactory()
    mac_ppc_105a_test_factory.addStep(test_commandline)
    mac_ppc_105a_test_factory.addStep(test_selftest(name="Release", shellname="avmshell_ppc"))
    mac_ppc_105a_test_factory.addStep(test_generic(name="Release", shellname="avmshell_ppc", vmargs="", config="", scriptargs=""))
    mac_ppc_105a_test_factory.addStep(test_generic(name="Release-interp", shellname="avmshell_ppc", vmargs="-Dinterp", config="", scriptargs=""))
    mac_ppc_105a_test_factory.addStep(test_generic(name="Release-wordcode-interp", shellname="avmshell_wordcode_ppc", vmargs="-Dinterp", config="", scriptargs=""))
    mac_ppc_105a_test_factory.addStep(test_generic(name="Release-jit", shellname="avmshell_ppc", vmargs="-Ojit", config="", scriptargs=""))
    mac_ppc_105a_test_factory.addStep(test_generic(name="Debug", shellname="avmshell_d_ppc", vmargs="", config="", scriptargs=""))
    mac_ppc_105a_test_factory.addStep(util_process_clean)
    mac_ppc_105a_test_factory.addStep(util_clean_buildsdir)

    mac_ppc_105a_test_builder = {
                'name': "mac-ppc-10.5a-test-argo",
                'slavename': "mac-ppc-10_5a",
                'factory': mac_ppc_105a_test_factory,
                'builddir': './argo-mac-ppc-10_5a-test',
    }


    ########################################
    #### builder for mac-ppc-10_5b-test ####
    ########################################
    mac_ppc_105b_test_factory = factory.BuildFactory()
    mac_ppc_105b_test_factory.addStep(test_generic(name="ReleaseDebugger", shellname="avmshell_s_ppc", vmargs="", config="", scriptargs=""))
    mac_ppc_105b_test_factory.addStep(test_generic(name="DebugDebugger", shellname="avmshell_sd_ppc", vmargs="", config="", scriptargs=""))
    mac_ppc_105b_test_factory.addStep(util_process_clean)
    mac_ppc_105b_test_factory.addStep(util_clean_buildsdir)

    mac_ppc_105b_test_builder = {
                'name': "mac-ppc-10.5b-test-argo",
                'slavename': "mac-ppc-10_5b",
                'factory': mac_ppc_105b_test_factory,
                'builddir': './argo-mac-ppc-10_5b-test',
    }


    ####################################
    #### builder for mac64-ppc-test ####
    ####################################
    mac_ppc_64_test_factory = factory.BuildFactory()
    mac_ppc_64_test_factory.addStep(test_commandline)
    mac_ppc_64_test_factory.addStep(test_selftest(name="Release", shellname="avmshell_64_ppc"))
    mac_ppc_64_test_factory.addStep(test_generic(name="Release", shellname="avmshell_64_ppc", vmargs="", config="", scriptargs=""))
    mac_ppc_64_test_factory.addStep(test_generic(name="Release-interp", shellname="avmshell_64_ppc", vmargs="-Dinterp", config="", scriptargs=""))
    mac_ppc_64_test_factory.addStep(test_generic(name="Release-jit", shellname="avmshell_64_ppc", vmargs="-Ojit", config="", scriptargs=""))
    mac_ppc_64_test_factory.addStep(test_generic(name="Debug", shellname="avmshell_d_64_ppc", vmargs="", config="", scriptargs=""))
    mac_ppc_64_test_factory.addStep(util_process_clean)
    mac_ppc_64_test_factory.addStep(util_clean_buildsdir)

    mac_ppc_64_test_builder = {
                'name': "mac64-ppc-test-argo",
                'slavename': "mac64a-ppc",
                'factory': mac_ppc_64_test_factory,
                'builddir': './argo-mac64-ppc-test',
    }


    #####################################
    #### builder for mac64b-ppc-test ####
    #####################################
    mac_ppc_64b_test_factory = factory.BuildFactory()
    mac_ppc_64b_test_factory.addStep(test_generic(name="Release-wordcode-interp", shellname="avmshell_wordcode_64_ppc", vmargs="-Dinterp", config="", scriptargs=""))
    mac_ppc_64b_test_factory.addStep(test_generic(name="ReleaseDebugger", shellname="avmshell_s_64_ppc", vmargs="", config="", scriptargs=""))
    mac_ppc_64b_test_factory.addStep(test_generic(name="DebugDebugger", shellname="avmshell_sd_64_ppc", vmargs="", config="", scriptargs=""))
    mac_ppc_64b_test_factory.addStep(util_process_clean)
    mac_ppc_64b_test_factory.addStep(util_clean_buildsdir)

    mac_ppc_64b_test_builder = {
                'name': "mac64b-ppc-test-argo",
                'slavename': "mac64b-ppc",
                'factory': mac_ppc_64b_test_factory,
                'builddir': './argo-mac64b-ppc-test',
    }


    ################################
    #### builder for linux-test ####
    ################################
    linux_test_factory = factory.BuildFactory()
    linux_test_factory.addStep(test_commandline)
    linux_test_factory.addStep(test_selftest(name="Release", shellname="avmshell"))
    linux_test_factory.addStep(test_generic(name="Release", shellname="avmshell", vmargs="", config="", scriptargs=""))
    linux_test_factory.addStep(test_generic(name="Release-interp", shellname="avmshell", vmargs="-Dinterp", config="", scriptargs=""))
    linux_test_factory.addStep(test_generic(name="Release-wordcode-interp", shellname="avmshell_wordcode", vmargs="-Dinterp", config="", scriptargs=""))
    linux_test_factory.addStep(test_generic(name="Release-jit", shellname="avmshell", vmargs="-Ojit", config="", scriptargs=""))
    linux_test_factory.addStep(test_generic(name="ReleaseDebugger", shellname="avmshell_s", vmargs="", config="", scriptargs=""))
    linux_test_factory.addStep(test_generic(name="Debug", shellname="avmshell_d", vmargs="", config="", scriptargs=""))
    linux_test_factory.addStep(test_generic(name="DebugDebugger", shellname="avmshell_sd", vmargs="", config="", scriptargs=""))
    linux_test_factory.addStep(test_differential)
    linux_test_factory.addStep(util_process_clean)
    linux_test_factory.addStep(util_clean_buildsdir)

    linux_test_builder = {
                'name': "linux-test-argo",
                'slavename': "linux",
                'factory': linux_test_factory,
                'builddir': './argo-linux-test',
    }


    ##################################
    #### builder for linux64-test ####
    ##################################
    linux_64_test_factory = factory.BuildFactory()
    linux_64_test_factory.addStep(test_commandline)
    linux_64_test_factory.addStep(test_selftest(name="Release", shellname="avmshell_64"))
    linux_64_test_factory.addStep(test_generic(name="Release", shellname="avmshell_64", vmargs="", config="", scriptargs=""))
    linux_64_test_factory.addStep(test_generic(name="Release-interp", shellname="avmshell_64", vmargs="-Dinterp", config="", scriptargs=""))
    linux_64_test_factory.addStep(test_generic(name="Release-wordcode-interp", shellname="avmshell_wordcode_64", vmargs="-Dinterp", config="", scriptargs=""))
    linux_64_test_factory.addStep(test_generic(name="Release-jit", shellname="avmshell_64", vmargs="-Ojit", config="", scriptargs=""))
    linux_64_test_factory.addStep(test_generic(name="ReleaseDebugger", shellname="avmshell_s_64", vmargs="", config="", scriptargs=""))
    linux_64_test_factory.addStep(test_generic(name="Debug", shellname="avmshell_d_64", vmargs="", config="", scriptargs=""))
    linux_64_test_factory.addStep(test_generic(name="DebugDebugger", shellname="avmshell_sd_64", vmargs="", config="", scriptargs=""))
    linux_64_test_factory.addStep(util_process_clean)
    linux_64_test_factory.addStep(util_clean_buildsdir)

    linux_64_test_builder = {
                'name': "linux64-test-argo",
                'slavename': "linux64",
                'factory': linux_64_test_factory,
                'builddir': './argo-linux64-test',
    }


    #############################################
    #### builder for winmobile-emulator-test ####
    #############################################
    winmobile_emulator_test_factory = factory.BuildFactory()
    winmobile_emulator_test_factory.addStep(test_emulator_generic(name="Release", shellname="avmshell_arm", vmargs="", config="", scriptargs=""))
    winmobile_emulator_test_factory.addStep(test_emulator_generic(name="Release-interp", shellname="avmshell_arm", vmargs="-Dinterp", config="", scriptargs=""))
    winmobile_emulator_test_factory.addStep(test_emulator_generic(name="Release-wordcode-interp", shellname="avmshell_wordcode_arm", vmargs="-Dinterp", config="", scriptargs=""))
    winmobile_emulator_test_factory.addStep(test_emulator_generic(name="Release-jit", shellname="avmshell_arm", vmargs="-Ojit", config="", scriptargs=""))
    winmobile_emulator_test_factory.addStep(util_process_clean)
    winmobile_emulator_test_factory.addStep(util_clean_buildsdir)

    winmobile_emulator_test_builder = {
                'name': "winmobile-emulator-test-argo",
                'slavename': "winmobile-emulator",
                'factory': winmobile_emulator_test_factory,
                'builddir': './argo-winmobile-emulator-test',
    }


    ########################################
    #### builder for solaris-sparc-test ####
    ########################################
    solaris_sparc_test_factory = factory.BuildFactory()
    solaris_sparc_test_factory.addStep(test_commandline)
    solaris_sparc_test_factory.addStep(test_selftest(name="Release", shellname="avmshell"))
    solaris_sparc_test_factory.addStep(test_generic(name="Debug", shellname="avmshell_d", vmargs="", config="", scriptargs=""))
    solaris_sparc_test_factory.addStep(test_generic(name="DebugDebugger", shellname="avmshell_sd", vmargs="", config="", scriptargs=""))
    solaris_sparc_test_factory.addStep(util_process_clean)
    solaris_sparc_test_factory.addStep(util_clean_buildsdir)

    solaris_sparc_test_builder = {
                'name': "solaris-sparc-test-argo",
                'slavename': "solaris-sparc",
                'factory': solaris_sparc_test_factory,
                'builddir': './argo-solaris-sparc-test',
    }
    
    
    #########################################
    #### builder for solaris-sparc2-test ####
    #########################################
    solaris_sparc2_test_factory = factory.BuildFactory()
    solaris_sparc2_test_factory.addStep(test_generic(name="Release", shellname="avmshell", vmargs="", config="", scriptargs=""))
    solaris_sparc2_test_factory.addStep(test_generic(name="Release-interp", shellname="avmshell", vmargs="-Dinterp", config="", scriptargs=""))
    solaris_sparc2_test_factory.addStep(test_generic(name="Release-wordcode-interp", shellname="avmshell_wordcode", vmargs="-Dinterp", config="", scriptargs=""))
    solaris_sparc2_test_factory.addStep(test_generic(name="Release-jit", shellname="avmshell", vmargs="-Ojit", config="", scriptargs=""))
    solaris_sparc2_test_factory.addStep(test_generic(name="ReleaseDebugger", shellname="avmshell_s", vmargs="", config="", scriptargs=""))
    solaris_sparc2_test_factory.addStep(util_process_clean)
    solaris_sparc2_test_factory.addStep(util_clean_buildsdir)

    solaris_sparc2_test_builder = {
                'name': "solaris-sparc2-test-argo",
                'slavename': "solaris-sparc2",
                'factory': solaris_sparc2_test_factory,
                'builddir': './argo-solaris-sparc2-test',
    }
    
    ########################################
    #### builder for android-test       ####
    ########################################
    android_test_factory = factory.BuildFactory()
    android_test_factory.addStep(test_generic_adb(name="Release", shellname="avmshell", vmargs="", config="", scriptargs=""))
    android_test_factory.addStep(test_generic_adb(name="Release-interp", shellname="avmshell", vmargs="-Dinterp", config="", scriptargs=""))
    android_test_factory.addStep(test_generic_adb(name="Release-jit", shellname="avmshell", vmargs="-Ojit", config="", scriptargs=""))
    android_test_factory.addStep(test_generic_adb(name="Debug", shellname="avmshell_d", vmargs="", config="", scriptargs=""))
    android_test_factory.addStep(util_process_clean)
    android_test_factory.addStep(util_clean_buildsdir)

    android_test_builder = {
                'name': "android-test-argo",
                'slavename': "android",
                'factory': android_test_factory,
                'builddir': './argo-android-test',
    }
    
    
    ##########################################
    #### builder for linux-arm-test       ####
    ##########################################
    linux_arm_test_factory = factory.BuildFactory()
    linux_arm_test_factory.addStep(test_selftest(name="Release", shellname="avmshell_neon_arm"))
    linux_arm_test_factory.addStep(test_generic(name="Release-softfloat", shellname="avmshell_neon_arm", vmargs="", config="", scriptargs=""))
    linux_arm_test_factory.addStep(test_generic(name="Release-vfp", shellname="avmshell_neon_arm", vmargs="-Darm_arch 7 -Darm_vfp", config="", scriptargs=""))
    linux_arm_test_factory.addStep(test_generic(name="Release-jit-vfp", shellname="avmshell_neon_arm", vmargs="-Darm_arch 7 -Darm_vfp -Ojit", config="", scriptargs=""))
    linux_arm_test_factory.addStep(util_process_clean)
    linux_arm_test_factory.addStep(util_clean_buildsdir)

    linux_arm_test_builder = {
                'name': "linux-arm-test-argo",
                'slavename': "linux-arm",
                'factory': linux_arm_test_factory,
                'builddir': './argo-linux-arm-test',
    }
    
    
    ##########################################
    #### builder for linux-arm2-test      ####
    ##########################################
    linux_arm2_test_factory = factory.BuildFactory()
    linux_arm2_test_factory.addStep(test_generic(name="Release-interp", shellname="avmshell_neon_arm", vmargs="-Dinterp", config="", scriptargs=""))
    linux_arm2_test_factory.addStep(test_generic(name="Debug-vfp", shellname="avmshell_neon_arm_d", vmargs="-Darm_arch 7 -Darm_vfp", config="", scriptargs=""))
    linux_arm2_test_factory.addStep(util_process_clean)
    linux_arm2_test_factory.addStep(util_clean_buildsdir)

    linux_arm2_test_builder = {
                'name': "linux-arm2-test-argo",
                'slavename': "linux-arm2",
                'factory': linux_arm2_test_factory,
                'builddir': './argo-linux-arm2-test',
    }



    ################################################################################
    ################################################################################
    ####                                                                        ####
    ####                     PERFORMANCE BUILDERS                               ####
    ####                                                                        ####
    ################################################################################
    ################################################################################


    #########################################
    #### builder for windows-performance ####
    #########################################
    windows_performance_factory = factory.BuildFactory()
    windows_performance_factory.addStep(sync_clean)
    windows_performance_factory.addStep(sync_clone(url=HG_URL))
    windows_performance_factory.addStep(sync_update)
    windows_performance_factory.addStep(bb_slaveupdate(slave="windows-performance"))
    windows_performance_factory.addStep(bb_lockacquire)
    windows_performance_factory.addStep(perf_prepare)
    windows_performance_factory.addStep(perf_release)
    windows_performance_factory.addStep(perf_release_interp)
    windows_performance_factory.addStep(perf_release_jit)
    windows_performance_factory.addStep(util_process_clean)
    windows_performance_factory.addStep(util_clean_buildsdir)
    windows_performance_factory.addStep(bb_lockrelease)

    windows_performance_builder = {
                'name': "windows-performance-argo",
                'slavename': "windows-performance",
                'factory': windows_performance_factory,
                'builddir': './argo-windows-performance',
    }


    #####################################
    #### builder for mac-performance ####
    #####################################
    mac_performance_factory = factory.BuildFactory()
    mac_performance_factory.addStep(sync_clean)
    mac_performance_factory.addStep(sync_clone(url=HG_URL))
    mac_performance_factory.addStep(sync_update)
    mac_performance_factory.addStep(bb_slaveupdate(slave="mac-performance"))
    mac_performance_factory.addStep(bb_lockacquire)
    mac_performance_factory.addStep(perf_prepare)
    mac_performance_factory.addStep(perf_release)
    mac_performance_factory.addStep(perf_release_interp)
    mac_performance_factory.addStep(perf_release_jit)
    mac_performance_factory.addStep(util_process_clean)
    mac_performance_factory.addStep(util_clean_buildsdir)
    mac_performance_factory.addStep(bb_lockrelease)

    mac_performance_builder = {
                'name': "mac-performance-argo",
                'slavename': "mac-performance",
                'factory': mac_performance_factory,
                'builddir': './argo-mac-performance',
    }


    #######################################
    #### builder for mac64-performance ####
    #######################################
    mac_64_performance_factory = factory.BuildFactory()
    mac_64_performance_factory.addStep(sync_clean)
    mac_64_performance_factory.addStep(sync_clone(url=HG_URL))
    mac_64_performance_factory.addStep(sync_update)
    mac_64_performance_factory.addStep(bb_slaveupdate(slave="mac64-performance"))
    mac_64_performance_factory.addStep(bb_lockacquire)
    mac_64_performance_factory.addStep(perf_prepare)
    mac_64_performance_factory.addStep(perf_release)
    mac_64_performance_factory.addStep(perf_release_interp)
    mac_64_performance_factory.addStep(perf_release_jit)
    mac_64_performance_factory.addStep(util_process_clean)
    mac_64_performance_factory.addStep(util_clean_buildsdir)
    mac_64_performance_factory.addStep(bb_lockrelease)

    mac_64_performance_builder = {
                'name': "mac64-performance-argo",
                'slavename': "mac64-performance",
                'factory': mac_64_performance_factory,
                'builddir': './argo-mac64-performance',
    }


    #########################################
    #### builder for mac-ppc-performance ####
    #########################################
    mac_ppc_performance_factory = factory.BuildFactory()
    mac_ppc_performance_factory.addStep(sync_clean)
    mac_ppc_performance_factory.addStep(sync_clone(url=HG_URL))
    mac_ppc_performance_factory.addStep(sync_update)
    mac_ppc_performance_factory.addStep(bb_slaveupdate(slave="mac-ppc-performance"))
    mac_ppc_performance_factory.addStep(bb_lockacquire)
    mac_ppc_performance_factory.addStep(perf_prepare)
    mac_ppc_performance_factory.addStep(perf_release)
    mac_ppc_performance_factory.addStep(perf_release_interp)
    mac_ppc_performance_factory.addStep(perf_release_jit)
    mac_ppc_performance_factory.addStep(util_process_clean)
    mac_ppc_performance_factory.addStep(util_clean_buildsdir)
    mac_ppc_performance_factory.addStep(bb_lockrelease)

    mac_ppc_performance_builder = {
                'name': "mac-ppc-performance-argo",
                'slavename': "mac-ppc-performance",
                'factory': mac_ppc_performance_factory,
                'builddir': './argo-mac-ppc-performance',
    }


    #######################################
    #### builder for linux-performance ####
    #######################################
    linux_performance_factory = factory.BuildFactory()
    linux_performance_factory.addStep(sync_clean)
    linux_performance_factory.addStep(sync_clone(url=HG_URL))
    linux_performance_factory.addStep(sync_update)
    linux_performance_factory.addStep(bb_slaveupdate(slave="linux-performance"))
    linux_performance_factory.addStep(bb_lockacquire)
    linux_performance_factory.addStep(perf_prepare)
    linux_performance_factory.addStep(perf_release)
    linux_performance_factory.addStep(perf_release_interp)
    linux_performance_factory.addStep(perf_release_jit)
    linux_performance_factory.addStep(util_process_clean)
    linux_performance_factory.addStep(util_clean_buildsdir)
    linux_performance_factory.addStep(bb_lockrelease)

    linux_performance_builder = {
                'name': "linux-performance-argo",
                'slavename': "linux-performance",
                'factory': linux_performance_factory,
                'builddir': './argo-linux-performance',
    }


    ###########################################
    #### builder for android-performance ######
    ###########################################
    android_performance_factory = factory.BuildFactory()
    android_performance_factory.addStep(sync_clean)
    android_performance_factory.addStep(sync_clone(url=HG_URL))
    android_performance_factory.addStep(sync_update)
    android_performance_factory.addStep(bb_slaveupdate(slave="android-performance"))
    android_performance_factory.addStep(perf_prepare)
    android_performance_factory.addStep(BuildShellCommand(
                command=['./run-performance-release-android.sh',WithProperties('%s','revision')],
                env={'branch':WithProperties('%s','branch')},
                description='running android performance...',
                descriptionDone='finished running android performance.',
                name="Release",
                workdir="../repo/build/buildbot/slaves/scripts",
                timeout=3600)
    )
    android_performance_factory.addStep(BuildShellCommand(
                command=['./run-performance-release-android-interp.sh',WithProperties('%s','revision')],
                env={'branch':WithProperties('%s','branch')},
                description='running android interp performance...',
                descriptionDone='finished running android interp performance.',
                name="ReleaseInterp",
                workdir="../repo/build/buildbot/slaves/scripts",
                timeout=3600)
    )
    android_performance_factory.addStep(BuildShellCommand(
                command=['./run-performance-release-android-jit.sh',WithProperties('%s','revision')],
                env={'branch':WithProperties('%s','branch')},
                description='running android jit performance...',
                descriptionDone='finished running android jit performance.',
                name="ReleaseJIT",
                workdir="../repo/build/buildbot/slaves/scripts",
                timeout=3600)
    )
    android_performance_factory.addStep(util_clean_buildsdir)

    android_performance_builder = {
                'name': "android-performance-argo",
                'slavename': "android-performance",
                'factory': android_performance_factory,
                'builddir': './argo-android-performance',
    }

    ###########################################
    #### builder for winmobile-performance ####
    ###########################################
    winmobile_performance_factory = factory.BuildFactory()
    winmobile_performance_factory.addStep(sync_clean)
    winmobile_performance_factory.addStep(sync_clone(url=HG_URL))
    winmobile_performance_factory.addStep(sync_update)
    winmobile_performance_factory.addStep(bb_slaveupdate(slave="winmobile-performance"))
    winmobile_performance_factory.addStep(bb_lockacquire)
    winmobile_performance_factory.addStep(perf_prepare)
    winmobile_performance_factory.addStep(perf_release_arm)
    winmobile_performance_factory.addStep(perf_release_arm_interp)
    winmobile_performance_factory.addStep(perf_release_arm_jit)
    winmobile_performance_factory.addStep(util_clean_buildsdir)
    winmobile_performance_factory.addStep(bb_lockrelease)

    winmobile_performance_builder = {
                'name': "winmobile-performance-argo",
                'slavename': "winmobile-performance",
                'factory': winmobile_performance_factory,
                'builddir': './argo-winmobile-performance',
    }

    ################################################################################
    ################################################################################
    ####                                                                        ####
    ####                       DEEP TEST BUILDERS                               ####
    ####                                                                        ####
    ################################################################################
    ################################################################################


    ##################################
    #### builder for windows-deep ####
    ##################################
    windows_deep_factory = factory.BuildFactory()
    windows_deep_factory.addStep(sync_clean)
    windows_deep_factory.addStep(sync_clone(url=HG_URL))
    windows_deep_factory.addStep(sync_update)
    windows_deep_factory.addStep(bb_slaveupdate(slave="windows-deep"))
    windows_deep_factory.addStep(download_testmedia)
    windows_deep_factory.addStep(deep_release_esc)
    windows_deep_factory.addStep(deep_codecoverage)
    windows_deep_factory.addStep(test_misc)
    windows_deep_factory.addStep(compile_generic(name="ReleaseDebugger-air", shellname="avmshell_air", args="--enable-shell --enable-override-global-new --enable-use-system-malloc --enable-debugger", upload="true"))
    windows_deep_factory.addStep(test_generic(name="ReleaseDebugger-air", shellname="avmshell_air", vmargs="", config="", scriptargs=""))
    windows_deep_factory.addStep(test_generic(name="Release-Dgreedy", shellname="avmshell", vmargs="-Dgreedy", config="", scriptargs="--timeout=180 --random"))
    windows_deep_factory.addStep(test_generic(name="Release_Verify", shellname="avmshell_sd", vmargs="", config="", scriptargs="--verify --timeout=300 --random"))
    windows_deep_factory.addStep(util_process_clean)
    windows_deep_factory.addStep(util_clean_buildsdir)

    windows_deep_builder = {
                'name': "windows-deep-argo",
                'slavename': "windows-deep",
                'factory': windows_deep_factory,
                'builddir': './argo-windows-deep',
    }

    ##################################
    #### builder for windows-p3-deep ####
    ##################################
    windows_p3_deep_factory = factory.BuildFactory()
    windows_p3_deep_factory.addStep(sync_clean)
    windows_p3_deep_factory.addStep(sync_clone(url=HG_URL))
    windows_p3_deep_factory.addStep(sync_update)
    windows_p3_deep_factory.addStep(bb_slaveupdate(slave="windows-p3-deep"))
    windows_p3_deep_factory.addStep(download_testmedia)
    windows_p3_deep_factory.addStep(test_smoke)
    windows_p3_deep_factory.addStep(test_commandline)
    windows_p3_deep_factory.addStep(test_selftest(name="Release", shellname="avmshell"))
    windows_p3_deep_factory.addStep(test_generic(name="Release", shellname="avmshell", vmargs="", config="", scriptargs=""))
    windows_p3_deep_factory.addStep(test_generic(name="Release-interp", shellname="avmshell", vmargs="-Dinterp", config="", scriptargs=""))
    windows_p3_deep_factory.addStep(test_generic(name="Release-wordcode-interp", shellname="avmshell_wordcode", vmargs="-Dinterp", config="", scriptargs=""))
    windows_p3_deep_factory.addStep(test_generic(name="Release-jit", shellname="avmshell", vmargs="-Ojit", config="", scriptargs=""))
    windows_p3_deep_factory.addStep(test_generic(name="ReleaseDebugger", shellname="avmshell_s", vmargs="", config="", scriptargs=""))
    windows_p3_deep_factory.addStep(test_generic(name="Debug", shellname="avmshell_d", vmargs="", config="", scriptargs=""))
    windows_p3_deep_factory.addStep(test_generic(name="DebugDebugger", shellname="avmshell_sd", vmargs="", config="", scriptargs=""))
    #windows_p3_deep_factory.addStep(test_differential)
    windows_p3_deep_factory.addStep(util_process_clean)
    windows_p3_deep_factory.addStep(util_clean_buildsdir)

    windows_p3_deep_builder = {
                'name': "windows-p3-deep-argo",
                'slavename': "windows-p3-deep",
                'factory': windows_p3_deep_factory,
                'builddir': './argo-windows-p3-deep',
    }


    ##################################
    #### builder for windows-frr ####
    ##################################
    windows_frr_factory = factory.BuildFactory()
    windows_frr_factory.addStep(BuildShellCommand(
                command=['./build-frr.sh', WithProperties('%s','revision')],
                env={'branch': WithProperties('%s','branch')},
                description='building frr...',
                descriptionDone='finished building frr.',
                name="BuildFRR",
                workdir="../scripts",
                timeout=3600)
    )
    windows_frr_factory.addStep(BuildShellCommand(
                command=['./run-frunit.sh', WithProperties('%s','revision')],
                env={'branch': WithProperties('%s','branch')},
                description='running frunit...',
                descriptionDone='finished running frunit.',
                name="RunFrunit",
                workdir="../scripts",
                timeout=3600)
    )

    windows_frr_builder = {
                'name': "windows-frr-argo",
                'slavename': "windows-frr",
                'factory': windows_frr_factory,
                'builddir': './argo-windows-frr',
    }

    ##################################
    #### builder for mac-ppc-deep ####
    ##################################
    mac_ppc_deep_factory = factory.BuildFactory()
    mac_ppc_deep_factory.addStep(sync_clean)
    mac_ppc_deep_factory.addStep(sync_clone(url=HG_URL))
    mac_ppc_deep_factory.addStep(sync_update)
    mac_ppc_deep_factory.addStep(bb_slaveupdate(slave="mac-ppc-deep"))
    mac_ppc_deep_factory.addStep(download_testmedia)
    mac_ppc_deep_factory.addStep(test_generic(name="Debug", shellname="avmshell_d_ppc", vmargs="", config="ppc-mac-tvm-debug-deep", scriptargs=""))
    mac_ppc_deep_factory.addStep(test_generic(name="DebugDebugger", shellname="avmshell_sd_ppc", vmargs="", config="ppc-mac-tvm-debugdebugger-deep", scriptargs=""))
    mac_ppc_deep_factory.addStep(test_generic(name="Debug64", shellname="avmshell_d_64_ppc", vmargs="", config="ppc-mac-tvm-debug-deep", scriptargs=""))
    mac_ppc_deep_factory.addStep(test_generic(name="DebugDebugger64", shellname="avmshell_sd_64_ppc", vmargs="", config="ppc-mac-tvm-debugdebugger-deep", scriptargs=""))
    mac_ppc_deep_factory.addStep(deep_release_esc)
    mac_ppc_deep_factory.addStep(test_misc)
    mac_ppc_deep_factory.addStep(compile_generic(name="ReleaseDebugger-air", shellname="avmshell_air_ppc", args="--enable-shell --enable-override-global-new --enable-use-system-malloc --enable-debugger --mac-sdk=104u", upload="true"))
    mac_ppc_deep_factory.addStep(test_generic(name="ReleaseDebugger-air", shellname="avmshell_air_ppc", vmargs="", config="", scriptargs=""))
    mac_ppc_deep_factory.addStep(test_generic(name="Release-Dgreedy", shellname="avmshell_ppc", vmargs="-Dgreedy", config="", scriptargs="--timeout=180 --random"))
    mac_ppc_deep_factory.addStep(test_generic(name="Release_Verify", shellname="avmshell_sd_ppc", vmargs="", config="", scriptargs="--verify --timeout=300 --random"))
    mac_ppc_deep_factory.addStep(util_process_clean)
    mac_ppc_deep_factory.addStep(util_clean_buildsdir)

    mac_ppc_deep_builder = {
                'name': "mac-ppc-deep-argo",
                'slavename': "mac-ppc-deep",
                'factory': mac_ppc_deep_factory,
                'builddir': './argo-mac-ppc-deep',
    }

    ########################################
    #### builder for solaris-sparc-deep ####
    ########################################
    solaris_sparc_deep_factory = factory.BuildFactory()
    solaris_sparc_deep_factory.addStep(sync_clean)
    solaris_sparc_deep_factory.addStep(sync_clone(url=HG_URL))
    solaris_sparc_deep_factory.addStep(sync_update)
    solaris_sparc_deep_factory.addStep(bb_slaveupdate(slave="solaris-sparc-deep"))
    solaris_sparc_deep_factory.addStep(download_testmedia)
    solaris_sparc_deep_factory.addStep(test_generic(name="Debug", shellname="avmshell_d", vmargs="", config="sparc-sol-tvm-debug-deep", scriptargs=""))
    solaris_sparc_deep_factory.addStep(test_generic(name="DebugDebugger", shellname="avmshell_sd", vmargs="", config="sparc-sol-tvm-debugdebugger-deep", scriptargs=""))
    #solaris_deep_factory.addStep(deep_release_esc)
    solaris_sparc_deep_factory.addStep(test_misc)
    solaris_sparc_deep_factory.addStep(test_generic(name="Release-Dgreedy", shellname="avmshell", vmargs="-Dgreedy", config="", scriptargs="--timeout=180 --random"))
    solaris_sparc_deep_factory.addStep(test_generic(name="Release_Verify", shellname="avmshell_sd", vmargs="", config="", scriptargs="--verify --timeout=300 --random"))
    solaris_sparc_deep_factory.addStep(util_process_clean)
    solaris_sparc_deep_factory.addStep(util_clean_buildsdir)

    solaris_sparc_deep_builder = {
                'name': "solaris-sparc-deep-argo",
                'slavename': "solaris-sparc",
                'factory': solaris_sparc_deep_factory,
                'builddir': './argo-solaris-sparc-deep',
    }

    ##################################
    #### builder for windows64-deep ####
    ##################################
    windows_64_deep_factory = factory.BuildFactory()
    windows_64_deep_factory.addStep(sync_clean)
    windows_64_deep_factory.addStep(sync_clone(url=HG_URL))
    windows_64_deep_factory.addStep(sync_update)
    windows_64_deep_factory.addStep(bb_slaveupdate(slave="windows64-deep"))
    windows_64_deep_factory.addStep(download_testmedia)
    windows_64_deep_factory.addStep(compile_generic(name="ReleaseDebugger-air", shellname="avmshell_air_64", args="--enable-shell --enable-override-global-new --enable-use-system-malloc --enable-debugger --target=x86_64-win", upload="true"))
    windows_64_deep_factory.addStep(test_generic(name="ReleaseDebugger-air", shellname="avmshell_air_64", vmargs="", config="", scriptargs=""))
    windows_64_deep_factory.addStep(test_generic(name="Debug", shellname="avmshell_d_64", vmargs="", config="x64-win-tvm-debug-deep", scriptargs=""))
    windows_64_deep_factory.addStep(test_generic(name="DebugDebugger", shellname="avmshell_sd_64", vmargs="", config="x64-win-tvm-debugdebugger-deep", scriptargs=""))
    windows_64_deep_factory.addStep(deep_release_esc)
    windows_64_deep_factory.addStep(test_generic(name="ReleaseDebugger-Dverifyall", shellname="avmshell_s_64", vmargs="-Dverifyall", config="", scriptargs=""))
    windows_64_deep_factory.addStep(test_generic(name="DebugDebugger-Dverifyall", shellname="avmshell_sd_64", vmargs="-Dverifyall", config="", scriptargs=""))
    windows_64_deep_factory.addStep( TestSuiteShellCommand(
                command=['../all/run-acceptance-avmdiff-3264.sh', WithProperties('%s','revision')],
                env={'branch': WithProperties('%s','branch')},
                description='starting to run 32-64 differential vmtests...',
                descriptionDone='finished 32-64 differential vmtests.',
                name="Testsuite_Differential3264",
                workdir="../repo/build/buildbot/slaves/scripts"
                )
    )
    windows_64_deep_factory.addStep(test_misc)
    windows_64_deep_factory.addStep(test_generic(name="Release-Dgreedy", shellname="avmshell_64", vmargs="-Dgreedy", config="", scriptargs="--timeout=180 --random"))
    windows_64_deep_factory.addStep(test_generic(name="Release_Verify", shellname="avmshell_sd_64", vmargs="", config="", scriptargs="--verify --timeout=300 --random"))
    windows_64_deep_factory.addStep(util_process_clean)
    windows_64_deep_factory.addStep(util_clean_buildsdir)

    windows_64_deep_builder = {
                'name': "windows64-deep-argo",
                'slavename': "windows64-deep",
                'factory': windows_64_deep_factory,
                'builddir': './argo-windows64-deep',
    }

    ##################################
    #### builder for winmobile-emulator-deep ####
    ##################################
    winmobile_emulator_deep_factory = factory.BuildFactory()
    winmobile_emulator_deep_factory.addStep(sync_clean)
    winmobile_emulator_deep_factory.addStep(sync_clone(url=HG_URL))
    winmobile_emulator_deep_factory.addStep(sync_update)
    winmobile_emulator_deep_factory.addStep(bb_slaveupdate(slave="winmobile-arm-deep"))
    winmobile_emulator_deep_factory.addStep(download_testmedia)
    winmobile_emulator_deep_factory.addStep(test_emulator_generic(name="Release", shellname="avmshell_arm", vmargs="", config="arm-winmobile-emulator-tvm-release-deep", scriptargs=""))
    winmobile_emulator_deep_factory.addStep(test_emulator_generic(name="Release-interp", shellname="avmshell_arm", vmargs="-Dinterp", config="arm-winmobile-emulator-tvm-release-Dinterp-deep", scriptargs=""))
    winmobile_emulator_deep_factory.addStep(test_emulator_generic(name="Release-wordcode-interp", shellname="avmshell_wordcode_arm", vmargs="-Dinterp", config="arm-winmobile-emulator-tvm-release-Dinterp-deep", scriptargs=""))
    winmobile_emulator_deep_factory.addStep(test_emulator_generic(name="Release-jit", shellname="avmshell_arm", vmargs="-Ojit", config="arm-winmobile-emulator-tvm-release-Ojit-deep", scriptargs=""))
    winmobile_emulator_deep_factory.addStep(util_process_clean)
    winmobile_emulator_deep_factory.addStep(util_clean_buildsdir)

    winmobile_emulator_deep_builder = {
                'name': "winmobile-emulator-deep-argo",
                'slavename': "winmobile-emulator-deep",
                'factory': winmobile_emulator_deep_factory,
                'builddir': './argo-winmobile-emulator-deep',
    }

    ##################################
    #### builder for linux-deep   ####
    ##################################
    linux_deep_factory = factory.BuildFactory()
    linux_deep_factory.addStep(sync_clean)
    linux_deep_factory.addStep(sync_clone(url=HG_URL))
    linux_deep_factory.addStep(sync_update)
    linux_deep_factory.addStep(bb_slaveupdate(slave="linux-deep"))
    linux_deep_factory.addStep(download_testmedia)
    linux_deep_factory.addStep(test_generic(name="Release-Djitordie", shellname="avmshell", vmargs="-Djitordie", config="", scriptargs=""))
    linux_deep_factory.addStep(test_generic(name="ReleaseDebugger-nodebugger", shellname="avmshell_s", vmargs="-Dnodebugger", config="", scriptargs=""))
    linux_deep_factory.addStep(test_generic(name="DebugDebugger-nodebugger", shellname="avmshell_sd", vmargs="-Dnodebugger", config="", scriptargs=""))
    linux_deep_factory.addStep(compile_generic(name="Release-nojit", shellname="avmshell_nojit", args="--enable-shell --disable-jit", upload="true"))
    linux_deep_factory.addStep(test_generic(name="Release-nojit", shellname="avmshell_nojit", vmargs="", config="x86-lnx-tvm-release-nojit", scriptargs=""))
    linux_deep_factory.addStep(compile_generic(name="Release-system-malloc", shellname="avmshell_sysmalloc", args="--enable-shell --enable-use-system-malloc", upload="true"))
    linux_deep_factory.addStep(test_generic(name="Release-system-malloc", shellname="avmshell_sysmalloc", vmargs="", config="x86-lnx-tvm-release-use-system-malloc", scriptargs=""))
    linux_deep_factory.addStep(test_misc)
    linux_deep_factory.addStep(deep_codecoverage)
    linux_deep_factory.addStep(compile_generic(name="ReleaseDebugger-air", shellname="avmshell_air", args="--enable-shell --enable-override-global-new --enable-use-system-malloc --enable-debugger", upload="true"))
    linux_deep_factory.addStep(test_generic(name="ReleaseDebugger-air", shellname="avmshell_air", vmargs="", config="", scriptargs=""))
    linux_deep_factory.addStep(test_generic(name="Release-Dgreedy", shellname="avmshell", vmargs="-Dgreedy", config="", scriptargs="--timeout=180 --random"))
    linux_deep_factory.addStep(test_generic(name="Release_Verify", shellname="avmshell_sd", vmargs="", config="", scriptargs="--verify --timeout=300 --random"))
    linux_deep_factory.addStep(util_process_clean)
    linux_deep_factory.addStep(util_clean_buildsdir)

    linux_deep_builder = {
                'name': "linux-deep-argo",
                'slavename': "linux-deep",
                'factory': linux_deep_factory,
                'builddir': './argo-linux-deep',
    }
    
    
    ######################################
    #### builder for linux-arm-deep   ####
    ######################################
    linux_arm_deep_factory = factory.BuildFactory()
    linux_arm_deep_factory.addStep(sync_clean)
    linux_arm_deep_factory.addStep(sync_clone(url=HG_URL))
    linux_arm_deep_factory.addStep(sync_update)
    linux_arm_deep_factory.addStep(bb_slaveupdate(slave="linux-arm-deep"))
    linux_arm_deep_factory.addStep(download_testmedia)
    linux_arm_deep_factory.addStep(test_generic(name="Debug-softfloat", shellname="avmshell_neon_arm_d", vmargs="", config="", scriptargs=""))
    linux_arm_deep_factory.addStep(test_generic(name="Release-softfloat-deep", shellname="avmshell_neon_arm", vmargs="", config="arm-lnx-tvm-release-deep", scriptargs=""))
    linux_arm_deep_factory.addStep(test_generic(name="Release-deep", shellname="avmshell_neon_arm", vmargs="-Darm_arch 7 -Darm_vfp", config="arm-lnx-tvm-release-deep", scriptargs=""))
    linux_arm_deep_factory.addStep(test_generic(name="Release-Dinterp-deep", shellname="avmshell_neon_arm", vmargs="-Dinterp", config="arm-lnx-tvm-release-Dinterp-deep", scriptargs=""))
    linux_arm_deep_factory.addStep(util_process_clean)
    linux_arm_deep_factory.addStep(util_clean_buildsdir)

    linux_arm_deep_builder = {
                'name': "linux-arm-deep-argo",
                'slavename': "linux-arm-deep",
                'factory': linux_arm_deep_factory,
                'builddir': './argo-linux-arm-deep',
    }

    
    builders = [
                windows_compile_builder,
                windows_64_compile_builder,
                mac_intel_104_compile_builder,
                mac_intel_105_compile_builder,
                mac_intel_64_compile_builder,
                mac_ppc_104a_compile_builder,
                mac_ppc_104b_compile_builder,
                mac_ppc_105a_compile_builder,
                mac_ppc_105b_compile_builder,
                mac_ppc_64_compile_builder,
                mac_ppc_64b_compile_builder,
                linux_compile_builder,
                linux_64_compile_builder,
                winmobile_emulator_compile_builder,
                solaris_sparc_compile_builder,
                solaris_sparc2_compile_builder,
                android_compile_builder,
                linux_arm_compile_builder,
                linux_arm2_compile_builder,
                
                windows_smoke_builder,
                windows_64_smoke_builder,
                mac_intel_104_smoke_builder,
                mac_intel_105_smoke_builder,
                mac_intel_64_smoke_builder,
                mac_ppc_104a_smoke_builder,
                mac_ppc_104b_smoke_builder,
                mac_ppc_105a_smoke_builder,
                mac_ppc_105b_smoke_builder,
                mac_ppc_64_smoke_builder,
                mac_ppc_64b_smoke_builder,
                linux_smoke_builder,
                linux_64_smoke_builder,
                winmobile_emulator_smoke_builder,
                solaris_sparc_smoke_builder,
                solaris_sparc2_smoke_builder,
                android_smoke_builder,
                linux_arm_smoke_builder,
                linux_arm2_smoke_builder,
                
                windows_test_builder,
                windows_64_test_builder,
                mac_intel_104_test_builder,
                mac_intel_105_test_builder,
                mac_intel_64_test_builder,
                mac_ppc_104a_test_builder,
                mac_ppc_104b_test_builder,
                mac_ppc_105a_test_builder,
                mac_ppc_105b_test_builder,
                mac_ppc_64_test_builder,
                mac_ppc_64b_test_builder,
                linux_test_builder,
                linux_64_test_builder,
                winmobile_emulator_test_builder,
                solaris_sparc_test_builder,
                solaris_sparc2_test_builder,
                android_test_builder,
                linux_arm_test_builder,
                linux_arm2_test_builder,

                windows_performance_builder,
                mac_performance_builder,
                mac_64_performance_builder,
                mac_ppc_performance_builder,
                linux_performance_builder,
                winmobile_performance_builder,
                android_performance_builder,

                windows_deep_builder,
                windows_p3_deep_builder,
                mac_ppc_deep_builder,
                windows_64_deep_builder,
                solaris_sparc_deep_builder,
                winmobile_emulator_deep_builder,
                linux_deep_builder,
                linux_arm_deep_builder,
                windows_frr_builder

                ]


