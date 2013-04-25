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

class tamarinredux:
    
    HG_URL = "http://asteam.macromedia.com/hg/mirror-tamarin-redux/"
    BRANCHES_DEEP = ["tamarin-redux-deep"]
    branch_deep_priorities = [('tamarin-redux-deep', 2)]

    
    ####### SCHEDULERS
    from buildbot.scheduler import *
    # custom.buildbot_ext.scheduler import MUST happen after importing buildbot.scheduler
    from custom.buildbot_ext.scheduler import *

    deep = PhaseTwoScheduler(name="deep", branch=BRANCHES_DEEP, treeStableTimer=30, properties={'silent':'false'},
                    fileIsImportant=startCompile, priorities=branch_deep_priorities, changeDir="changes/deep/processed",
                    builderNames=["windows-deep",
                                    "windows64-deep",                                    
                                    "mac-deep",
                                    "mac64-deep",
                                    "linux-deep",
                                    "linux-arm-deep",
                                    "linux-mips-deep",
                                                                    ],
                    builderDependencies=[
                                  ["windows-deep", "windows-test"],
                                  ["windows64-deep", "windows64-test"],
                                  ["mac-deep","mac-intel-10.5-test"],
                                  ["mac64-deep","mac64-intel-test"],                                  
                                  ["linux-deep", "linux-test"],
                                  ["linux-arm-deep", "linux-arm-test"],
                                  ["linux-mips-deep", "linux-mips-test"],                                  
                                 ])
    
    # The promote_build phase only runs when deep passes all tests.
    promote_build = Dependent(name="promote_build", upstream=deep, properties={'silent':'false'},
                        builderNames=["promote-build"],
                    )
    

    
    schedulers = [deep, promote_build]

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
    windows_deep_factory.addStep(sync_clone_sandbox)
    windows_deep_factory.addStep(sync_update)
    windows_deep_factory.addStep(bb_slaveupdate(slave="windows-deep"))
    windows_deep_factory.addStep(compile_generic(name="Release-wordcode", shellname="avmshell_wordcode", args="--enable-wordcode-interp", upload="false", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32 +AVMFEATURE_WORDCODE_INTERP"))
    windows_deep_factory.addStep(compile_generic(name="DebugDebugger-wordcode", shellname="avmshell_sd_wordcode", args="--enable-debug --enable-debugger --enable-wordcode-interp", upload="false", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32 +AVMFEATURE_WORDCODE_INTERP"))    
    windows_deep_factory.addStep(download_testmedia)
    windows_deep_factory.addStep(deep_release_esc)
    windows_deep_factory.addStep(test_misc)
    windows_deep_factory.addStep(test_selftest(name="Debug", shellname="avmshell_d"))
    windows_deep_factory.addStep(test_selftest(name="ReleaseDebugger", shellname="avmshell_s"))
    windows_deep_factory.addStep(test_selftest(name="DebugDebugger", shellname="avmshell_sd"))
    windows_deep_factory.addStep(test_generic(name="Release-wordcode-interp", shellname="avmshell_wordcode", vmargs="-Dinterp", config="", scriptargs=""))
    windows_deep_factory.addStep(test_generic(name="DebugDebugger-wordcode-interp", shellname="avmshell_sd_wordcode", vmargs="-Dinterp", config="", scriptargs=""))
    windows_deep_factory.addStep(compile_generic(name="ReleaseDebugger-air", shellname="avmshell_air", args="--enable-override-global-new --enable-use-system-malloc --enable-debugger", upload="true", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32 +AVMFEATURE_DEBUGGER +AVMFEATURE_OVERRIDE_GLOBAL_NEW +AVMFEATURE_USE_SYSTEM_MALLOC"))
    windows_deep_factory.addStep(test_generic(name="ReleaseDebugger-air", shellname="avmshell_air", vmargs="", config="", scriptargs=""))
    windows_deep_factory.addStep(compile_generic(name="Release-osr", shellname="avmshell_osr", args="--enable-osr", upload="false", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32 +AVMFEATURE_OSR"))
    windows_deep_factory.addStep(compile_generic(name="Debug-osr", shellname="avmshell_d_osr", args="--enable-debug --enable-osr", upload="false", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32 +AVMFEATURE_OSR"))    
    windows_deep_factory.addStep(test_generic(name="Release-osr-17", shellname="avmshell_osr", vmargs="-osr=17", config="", scriptargs=""))
    windows_deep_factory.addStep(test_generic(name="Release-osr-0", shellname="avmshell_osr", vmargs="-osr=0", config="", scriptargs=""))
    windows_deep_factory.addStep(test_generic(name="Debug-osr-17", shellname="avmshell_d_osr", vmargs="-osr=17", config="", scriptargs=""))
    windows_deep_factory.addStep(test_generic(name="Debug-osr-0", shellname="avmshell_d_osr", vmargs="-osr=0", config="", scriptargs=""))
    windows_deep_factory.addStep(test_generic(name="Release-Dgreedy", shellname="avmshell", vmargs="-Dgreedy", config="", scriptargs="--timeout=180 --random --threads=1"))
    windows_deep_factory.addStep(test_generic(name="DebugDebugger_VerifyAll", shellname="avmshell_sd", vmargs="", config="", scriptargs="--verify --timeout=300 --random --threads=1"))
    windows_deep_factory.addStep(test_generic(name="DebugDebugger_VerifyOnly", shellname="avmshell_sd", vmargs="", config="", scriptargs="--verifyonly --timeout=300 --random --threads=1"))
    windows_deep_factory.addStep(test_generic(name="Release-GCthreshold", shellname="avmshell", vmargs="-Dgcthreshold 128 -load 1.05,1,1.05,5,1.05,20", config="", scriptargs=""))
    windows_deep_factory.addStep(test_generic(name="DebugDebugger-GCthreshold", shellname="avmshell_sd", vmargs="-Dgcthreshold 128 -load 1.05,1,1.05,5,1.05,20", config="", scriptargs=""))
    windows_deep_factory.addStep(test_generic(name="Debug-jit", shellname="avmshell_d", vmargs="-Ojit", config="", scriptargs=""))
    windows_deep_factory.addStep(compile_generic(name="ReleaseHeapGraph", shellname="avmshell_heapgraph", args="--enable-heap-graph", upload="false", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32 +AVMTWEAK_HEAP_GRAPH"))
    windows_deep_factory.addStep(test_selftest(name="ReleaseHeapGraph", shellname="avmshell_heapgraph"))
    windows_deep_factory.addStep(BuildShellCommand(
                command=['./build-vtune.sh', WithProperties('%s','revision')],
                env={'branch': WithProperties('%s','branch')},
                description='starting VTune build...',
                descriptionDone='finished VTune build.',
                name="VTune",
                workdir="../repo/build/buildbot/slaves/scripts")
    )

    #####################
    # Start float testing
    windows_deep_factory.addStep(BuildShellCommand(
                command=['../all/float-pre.sh', WithProperties('%s','revision')],
                env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
                description='prepare for float...',
                descriptionDone='finished preparing for float',
                name="Float-pre",
                workdir="../repo/build/buildbot/slaves/scripts")
    )
    windows_deep_factory.addStep(compile_generic(name="Release-Float", shellname="avmshell_float", args="--target=i686-windows --enable-float", upload="false", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32 +AVMFEATURE_FLOAT"))
    windows_deep_factory.addStep(compile_generic(name="Debug-Float", shellname="avmshell_d_float", args="--target=i686-windows --enable-float --enable-debug", upload="false", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32 +AVMFEATURE_FLOAT"))
    # Force a recompilation of the test media on the first run of acceptance since media needs to be recompiled
    windows_deep_factory.addStep(test_generic(name="Release-float", shellname="avmshell_float", vmargs="", config="",
                                              scriptargs="--ascargs=-abcfuture --addtoconfig=-float -f"))
    windows_deep_factory.addStep(test_generic(name="Release-float-Ojit", shellname="avmshell_float", vmargs="-Ojit", config="",
                                              scriptargs="--ascargs=-abcfuture --addtoconfig=-float"))
    windows_deep_factory.addStep(test_generic(name="Debug-float", shellname="avmshell_d_float", vmargs="", config="",
                                              scriptargs="--ascargs=-abcfuture --addtoconfig=-float"))
    windows_deep_factory.addStep(test_generic(name="Debug-float-Ojit", shellname="avmshell_d_float", vmargs="-Ojit", config="",
                                              scriptargs="--ascargs=-abcfuture --addtoconfig=-float"))
    windows_deep_factory.addStep(BuildShellCommand(
                command=['../all/float-post.sh', WithProperties('%s','revision')],
                env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
                description='revert from  float...',
                descriptionDone='finished reverting from float',
                name="Float-post",
                workdir="../repo/build/buildbot/slaves/scripts")
    )
    # End float testing
    ###################

    # Do a test run where we compile with -ES. MUST be the last step of the build as it recompiles the .abc files used by all the other steps
    windows_deep_factory.addStep(test_generic(name="Release_ES", shellname="avmshell", vmargs="", config="",
                                              scriptargs="--ascargs=-no-AS3 --addtoconfig=-ES -f -x abcasm,ecma3,spidermonkey"))
    windows_deep_factory.addStep(download_testmedia) # grab test media again, to protect against previous -ES run
    windows_deep_factory.addStep(util_process_clean)
    windows_deep_factory.addStep(util_clean_buildsdir)
    windows_deep_factory.addStep(sync_clean)

    windows_deep_builder = {
                'name': "windows-deep",
                'slavename': "windows-deep",
                'factory': windows_deep_factory,
                'builddir': './windows-deep',
    }

    ##################################
    #### builder for windows64-deep ####
    ##################################
    windows_64_deep_factory = factory.BuildFactory()
    windows_64_deep_factory.addStep(sync_clean)
    windows_64_deep_factory.addStep(sync_clone_sandbox)
    windows_64_deep_factory.addStep(sync_update)
    windows_64_deep_factory.addStep(bb_slaveupdate(slave="windows64-deep"))
    windows_64_deep_factory.addStep(compile_generic(name="Release-wordcode", shellname="avmshell_wordcode_64", args="--enable-wordcode-interp --target=x86_64-win", upload="true", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64 +AVMFEATURE_WORDCODE_INTERP"))
    windows_64_deep_factory.addStep(compile_generic(name="DebugDebugger-wordcode", shellname="avmshell_sd_wordcode_64", args="--enable-debug --enable-debugger --enable-wordcode-interp --target=x86_64-win", upload="false", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64 +AVMFEATURE_WORDCODE_INTERP"))    
    windows_64_deep_factory.addStep(download_testmedia)
    windows_64_deep_factory.addStep(test_selftest(name="Debug", shellname="avmshell_d_64"))
    windows_64_deep_factory.addStep(test_selftest(name="ReleaseDebugger", shellname="avmshell_s_64"))
    windows_64_deep_factory.addStep(test_selftest(name="DebugDebugger", shellname="avmshell_sd_64"))
    windows_64_deep_factory.addStep(test_generic(name="Release-wordcode-interp", shellname="avmshell_wordcode_64", vmargs="-Dinterp", config="", scriptargs=""))
    windows_64_deep_factory.addStep(test_generic(name="DebugDebugger-wordcode-interp", shellname="avmshell_sd_wordcode_64", vmargs="-Dinterp", config="", scriptargs=""))
    windows_64_deep_factory.addStep(compile_generic(name="ReleaseDebugger-air", shellname="avmshell_air_64", args="--enable-override-global-new --enable-use-system-malloc --enable-debugger --target=x86_64-win", upload="true", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64 +AVMFEATURE_DEBUGGER +AVMFEATURE_OVERRIDE_GLOBAL_NEW +AVMFEATURE_USE_SYSTEM_MALLOC"))
    windows_64_deep_factory.addStep(test_generic(name="ReleaseDebugger-air", shellname="avmshell_air_64", vmargs="", config="", scriptargs=""))
    windows_64_deep_factory.addStep(compile_generic(name="Release-osr", shellname="avmshell_osr_64", args="--enable-osr --target=x86_64-win", upload="false", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64 +AVMFEATURE_OSR"))
    windows_64_deep_factory.addStep(compile_generic(name="Debug-osr", shellname="avmshell_d_osr_64", args="--enable-debug --enable-osr --target=x86_64-win", upload="false", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64 +AVMFEATURE_OSR"))
    windows_64_deep_factory.addStep(test_generic(name="Release-osr-17", shellname="avmshell_osr_64", vmargs="-osr=17", config="", scriptargs=""))
    windows_64_deep_factory.addStep(test_generic(name="Release-osr-0", shellname="avmshell_osr_64", vmargs="-osr=0", config="", scriptargs=""))
    windows_64_deep_factory.addStep(test_generic(name="Debug-osr-17", shellname="avmshell_d_osr_64", vmargs="-osr=17", config="", scriptargs=""))
    windows_64_deep_factory.addStep(test_generic(name="Debug-osr-0", shellname="avmshell_d_osr_64", vmargs="-osr=0", config="", scriptargs=""))
    windows_64_deep_factory.addStep(test_generic(name="Debug", shellname="avmshell_d_64", vmargs="", config="x64-win-tvm-debug-deep", scriptargs=""))
    windows_64_deep_factory.addStep(test_generic(name="DebugDebugger", shellname="avmshell_sd_64", vmargs="", config="x64-win-tvm-debugdebugger-deep", scriptargs=""))
    windows_64_deep_factory.addStep(deep_release_esc)
    windows_64_deep_factory.addStep(test_generic(name="ReleaseDebugger-Dverifyall", shellname="avmshell_s_64", vmargs="-Dverifyall", config="", scriptargs=""))
    windows_64_deep_factory.addStep(test_generic(name="DebugDebugger-Dverifyall", shellname="avmshell_sd_64", vmargs="-Dverifyall", config="", scriptargs=""))
    windows_64_deep_factory.addStep(test_generic(name="Release-GCthreshold", shellname="avmshell_64", vmargs="-Dgcthreshold 128 -load 1.05,1,1.05,5,1.05,20", config="", scriptargs=""))
    windows_64_deep_factory.addStep(test_generic(name="DebugDebugger-GCthreshold", shellname="avmshell_sd_64", vmargs="-Dgcthreshold 128 -load 1.05,1,1.05,5,1.05,20", config="", scriptargs=""))
    windows_64_deep_factory.addStep(test_generic(name="Debug-jit", shellname="avmshell_d_64", vmargs="-Ojit", config="", scriptargs=""))
    windows_64_deep_factory.addStep(compile_generic(name="ReleaseHeapGraph", shellname="avmshell_heapgraph_64", args="--enable-heap-graph --target=x86_64-win", upload="false", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64 +AVMTWEAK_HEAP_GRAPH"))
    windows_64_deep_factory.addStep(test_selftest(name="ReleaseHeapGraph", shellname="avmshell_heapgraph_64"))
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
    windows_64_deep_factory.addStep(test_generic(name="Release-Dgreedy", shellname="avmshell_64", vmargs="-Dgreedy", config="", scriptargs="--timeout=180 --random --threads=1"))
    windows_64_deep_factory.addStep(test_generic(name="DebugDebugger_VerifyAll", shellname="avmshell_sd_64", vmargs="", config="", scriptargs="--verify --timeout=300 --random --threads=1"))
    windows_64_deep_factory.addStep(test_generic(name="DebugDebugger_VerifyOnly", shellname="avmshell_sd_64", vmargs="", config="", scriptargs="--verifyonly --timeout=300 --random --threads=1"))
    windows_64_deep_factory.addStep(deep_codecoverage(compilecsv="../all/codecoverage-compile.csv", testcsv="../all/codecoverage-test.csv"))
    windows_64_deep_factory.addStep(deep_codecoverage_process)
    windows_64_deep_factory.addStep(download_testmedia) # grab test media again, to protect against previous -ES run in code coverage
    # Do a test run where we compile with -ES. MUST be the last step of the build as it recompiles the .abc files used by all the other steps
    windows_64_deep_factory.addStep(test_generic(name="Release_ES", shellname="avmshell", vmargs="", config="",
                                              scriptargs="--ascargs=-no-AS3 --addtoconfig=-ES -f -x abcasm,ecma3,spidermonkey"))
    windows_64_deep_factory.addStep(download_testmedia) # grab test media again, to protect against previous -ES run
    windows_64_deep_factory.addStep(util_process_clean)
    windows_64_deep_factory.addStep(util_clean_buildsdir)
    windows_64_deep_factory.addStep(sync_clean)

    windows_64_deep_builder = {
                'name': "windows64-deep",
                'slavename': "windows64-deep",
                'factory': windows_64_deep_factory,
                'builddir': './windows64-deep',
    }


    ##################################
    #### builder for mac-deep ####
    ##################################
    mac_deep_factory = factory.BuildFactory()
    mac_deep_factory.addStep(sync_clean)
    mac_deep_factory.addStep(sync_clone_sandbox)
    mac_deep_factory.addStep(sync_update)
    mac_deep_factory.addStep(bb_slaveupdate(slave="mac-deep"))
    mac_deep_factory.addStep(verify_builtinabc)
    mac_deep_factory.addStep(compile_generic(name="Release-wordcode", shellname="avmshell_wordcode", args="--enable-wordcode-interp --mac-sdk=105", upload="false", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32 +AVMFEATURE_WORDCODE_INTERP"))
    mac_deep_factory.addStep(compile_generic(name="DebugDebugger-wordcode", shellname="avmshell_sd_wordcode", args="--enable-debug --enable-debugger --enable-wordcode-interp --mac-sdk=105", upload="false", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32 +AVMFEATURE_WORDCODE_INTERP"))    
    mac_deep_factory.addStep(download_testmedia)
    mac_deep_factory.addStep(deep_release_esc)
    mac_deep_factory.addStep(test_misc)
    mac_deep_factory.addStep(test_selftest(name="Debug", shellname="avmshell_d"))
    mac_deep_factory.addStep(test_selftest(name="ReleaseDebugger", shellname="avmshell_s"))
    mac_deep_factory.addStep(test_selftest(name="DebugDebugger", shellname="avmshell_sd"))
    mac_deep_factory.addStep(test_generic(name="Release-wordcode-interp", shellname="avmshell_wordcode", vmargs="-Dinterp", config="", scriptargs=""))
    mac_deep_factory.addStep(test_generic(name="DebugDebugger-wordcode-interp", shellname="avmshell_sd_wordcode", vmargs="-Dinterp", config="", scriptargs=""))
    mac_deep_factory.addStep(compile_generic(name="ReleaseDebugger-air", shellname="avmshell_air", args="--enable-override-global-new --enable-use-system-malloc --enable-debugger", upload="true", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32 +AVMFEATURE_DEBUGGER +AVMFEATURE_OVERRIDE_GLOBAL_NEW +AVMFEATURE_USE_SYSTEM_MALLOC"))
    mac_deep_factory.addStep(test_generic(name="ReleaseDebugger-air", shellname="avmshell_air", vmargs="", config="x86-mac-tvm-releasedebugger-air", scriptargs=""))
    mac_deep_factory.addStep(compile_generic(name="Release-osr", shellname="avmshell_osr", args="--enable-osr --target=i686-darwin --mac-sdk=105", upload="false", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32 +AVMFEATURE_OSR"))
    mac_deep_factory.addStep(compile_generic(name="Debug-osr", shellname="avmshell_d_osr", args="--enable-debug --enable-osr --target=i686-darwin --mac-sdk=105", upload="false", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32 +AVMFEATURE_OSR"))
    mac_deep_factory.addStep(test_generic(name="Release-osr-17", shellname="avmshell_osr", vmargs="-osr=17", config="", scriptargs=""))
    mac_deep_factory.addStep(test_generic(name="Release-osr-0", shellname="avmshell_osr", vmargs="-osr=0", config="", scriptargs=""))
    mac_deep_factory.addStep(test_generic(name="Debug-osr-17", shellname="avmshell_d_osr", vmargs="-osr=17", config="", scriptargs=""))
    mac_deep_factory.addStep(test_generic(name="Debug-osr-0", shellname="avmshell_d_osr", vmargs="-osr=0", config="", scriptargs=""))
    mac_deep_factory.addStep(test_generic(name="Release-Dgreedy", shellname="avmshell", vmargs="-Dgreedy", config="", scriptargs="--timeout=180 --random --threads=1"))
    mac_deep_factory.addStep(test_generic(name="DebugDebugger_VerifyAll", shellname="avmshell_sd", vmargs="", config="", scriptargs="--verify --timeout=300 --random --threads=1"))
    mac_deep_factory.addStep(test_generic(name="DebugDebugger_VerifyOnly", shellname="avmshell_sd", vmargs="", config="", scriptargs="--verifyonly --timeout=300 --random --threads=1"))
    mac_deep_factory.addStep(test_generic(name="Release-GCthreshold", shellname="avmshell", vmargs="-Dgcthreshold 128 -load 1.05,1,1.05,5,1.05,20", config="", scriptargs=""))
    mac_deep_factory.addStep(test_generic(name="DebugDebugger-GCthreshold", shellname="avmshell_sd", vmargs="-Dgcthreshold 128 -load 1.05,1,1.05,5,1.05,20", config="", scriptargs=""))
    mac_deep_factory.addStep(test_generic(name="Debug-jit", shellname="avmshell_d", vmargs="-Ojit", config="", scriptargs=""))
    mac_deep_factory.addStep(compile_generic(name="Release-aot", shellname="avmshell_aot", args="--enable-aot --disable-eval --target=i686-darwin --mac-sdk=105", upload="false", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32 +AVMFEATURE_AOT"))
    # Commenting because of bug 696275 - mac_deep_factory.addStep(compile_generic(name="Debug-aot", shellname="avmshell_d_aot", args="--enable-debug --enable-aot --target=i686-darwin --mac-sdk=105", upload="false", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32 +AVMFEATURE_AOT"))
    mac_deep_factory.addStep(compile_generic(name="ReleaseHeapGraph", shellname="avmshell_heapgraph", args="--enable-heap-graph --target=i686-darwin --mac-sdk=105", upload="false", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32 +AVMTWEAK_HEAP_GRAPH"))
    mac_deep_factory.addStep(test_selftest(name="ReleaseHeapGraph", shellname="avmshell_heapgraph"))
    mac_deep_factory.addStep(deep_run_brightspot(name='ReleaseDebugger', shell='avmshell_s', testargs='--quiet'))
    mac_deep_factory.addStep(deep_codecoverage(compilecsv="../all/codecoverage-compile.csv", testcsv="../all/codecoverage-test.csv"))
    mac_deep_factory.addStep(deep_codecoverage_process)
    mac_deep_factory.addStep(download_testmedia) # grab test media again, to protect against previous -ES run in code coverage
    mac_deep_factory.addStep(BuildShellCommand(
                command=['../all/build-doxygen.sh', WithProperties('%s','revision')],
                env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
                description='starting doxygen build...',
                descriptionDone='finished doxygen build.',
                name="Doxygen",
                workdir="../repo/build/buildbot/slaves/scripts")
    )
    mac_deep_factory.addStep(compile_generic(name="ReleaseDebugger-Valgrind", shellname="avmshell_s_valgrind", args="--enable-debugger --enable-valgrind", upload="false", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32 +AVMFEATURE_DEBUGGER +AVMFEATURE_VALGRIND"))
    mac_deep_factory.addStep(test_generic(name="ReleaseDebugger-Valgrind", shellname="avmshell_s_valgrind", vmargs="", config="", scriptargs="--valgrind"))

    #####################
    # Start float testing
    mac_deep_factory.addStep(BuildShellCommand(
                command=['../all/float-pre.sh', WithProperties('%s','revision')],
                env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
                description='prepare for float...',
                descriptionDone='finished preparing for float',
                name="Float-pre",
                workdir="../repo/build/buildbot/slaves/scripts")
    )
    mac_deep_factory.addStep(compile_generic(name="Release-Float", shellname="avmshell_float", args="--target=i686-darwin --enable-float", upload="false", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32 +AVMFEATURE_FLOAT"))
    mac_deep_factory.addStep(compile_generic(name="Debug-Float", shellname="avmshell_d_float", args="--target=i686-darwin --enable-float --enable-debug", upload="false", features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32 +AVMFEATURE_FLOAT"))
    # Force a recompilation of the test media on the first run of acceptance since media needs to be recompiled
    mac_deep_factory.addStep(test_generic(name="Release-float", shellname="avmshell_float", vmargs="", config="",
                                              scriptargs="--ascargs=-abcfuture --addtoconfig=-float -f"))
    mac_deep_factory.addStep(test_generic(name="Release-float-Ojit", shellname="avmshell_float", vmargs="-Ojit", config="",
                                              scriptargs="--ascargs=-abcfuture --addtoconfig=-float"))
    mac_deep_factory.addStep(test_generic(name="Debug-float", shellname="avmshell_d_float", vmargs="", config="",
                                              scriptargs="--ascargs=-abcfuture --addtoconfig=-float"))
    mac_deep_factory.addStep(test_generic(name="Debug-float-Ojit", shellname="avmshell_d_float", vmargs="-Ojit", config="",
                                              scriptargs="--ascargs=-abcfuture --addtoconfig=-float"))
    mac_deep_factory.addStep(BuildShellCommand(
                command=['../all/float-post.sh', WithProperties('%s','revision')],
                env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
                description='revert from  float...',
                descriptionDone='finished reverting from float',
                name="Float-post",
                workdir="../repo/build/buildbot/slaves/scripts")
    )
    # End float testing
    ###################

    # Do a test run where we compile with -ES. MUST be the last step of the build as it recompiles the .abc files used by all the other steps
    mac_deep_factory.addStep(test_generic(name="Release_ES", shellname="avmshell", vmargs="", config="",
                                              scriptargs="--ascargs=-no-AS3 --addtoconfig=-ES -f -x abcasm,ecma3,spidermonkey"))
    mac_deep_factory.addStep(download_testmedia) # grab test media again, to protect against previous -ES run
    mac_deep_factory.addStep(util_process_clean)
    mac_deep_factory.addStep(util_clean_buildsdir)
    mac_deep_factory.addStep(sync_clean)

    mac_deep_builder = {
                'name': "mac-deep",
                'slavename': "mac-deep",
                'factory': mac_deep_factory,
                'builddir': './mac-deep',
    }


    ################################
    #### builder for mac64-deep ####
    ################################
    mac64_deep_factory = factory.BuildFactory()
    mac64_deep_factory.addStep(sync_clean)
    mac64_deep_factory.addStep(sync_clone_sandbox)
    mac64_deep_factory.addStep(sync_update)
    mac64_deep_factory.addStep(bb_slaveupdate(slave="mac64-deep"))
    mac64_deep_factory.addStep(verify_builtinabc)
    mac64_deep_factory.addStep(compile_generic(name="Release-wordcode", shellname="avmshell_wordcode_64", args="--enable-wordcode-interp --target=x86_64-darwin --mac-sdk=105", upload="false", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64 +AVMFEATURE_WORDCODE_INTERP"))
    mac64_deep_factory.addStep(compile_generic(name="DebugDebugger-wordcode", shellname="avmshell_sd_wordcode_64", args="--enable-debug --enable-debugger --enable-wordcode-interp --target=x86_64-darwin --mac-sdk=105", upload="false", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64 +AVMFEATURE_WORDCODE_INTERP"))    
    mac64_deep_factory.addStep(download_testmedia)
    mac64_deep_factory.addStep(deep_release_esc)
    mac64_deep_factory.addStep(test_misc)
    mac64_deep_factory.addStep(test_selftest(name="Debug", shellname="avmshell_d_64"))
    mac64_deep_factory.addStep(test_selftest(name="ReleaseDebugger", shellname="avmshell_s_64"))
    mac64_deep_factory.addStep(test_selftest(name="DebugDebugger", shellname="avmshell_sd_64"))
    mac64_deep_factory.addStep(test_generic(name="Release-wordcode-interp", shellname="avmshell_wordcode_64", vmargs="-Dinterp", config="", scriptargs=""))
    mac64_deep_factory.addStep(test_generic(name="DebugDebugger-wordcode-interp", shellname="avmshell_sd_wordcode_64", vmargs="-Dinterp", config="", scriptargs=""))
    mac64_deep_factory.addStep(compile_generic(name="ReleaseDebugger-air", shellname="avmshell_air_64", args="--target=x86_64-darwin --enable-override-global-new --enable-use-system-malloc --enable-debugger", upload="true", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64 +AVMFEATURE_DEBUGGER +AVMFEATURE_OVERRIDE_GLOBAL_NEW +AVMFEATURE_USE_SYSTEM_MALLOC"))
    mac64_deep_factory.addStep(test_generic(name="ReleaseDebugger-air", shellname="avmshell_air_64", vmargs="", config="", scriptargs=""))
    mac64_deep_factory.addStep(compile_generic(name="Release-osr", shellname="avmshell_osr_64", args="--enable-osr --target=x86_64-darwin --mac-sdk=105", upload="false", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64 +AVMFEATURE_OSR"))
    mac64_deep_factory.addStep(compile_generic(name="Debug-osr", shellname="avmshell_d_osr_64", args="--enable-debug --enable-osr --target=x86_64-darwin --mac-sdk=105", upload="false", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64 +AVMFEATURE_OSR"))
    mac64_deep_factory.addStep(test_generic(name="Release-osr-17", shellname="avmshell_osr_64", vmargs="-osr=17", config="", scriptargs=""))
    mac64_deep_factory.addStep(test_generic(name="Release-osr-0", shellname="avmshell_osr_64", vmargs="-osr=0", config="", scriptargs=""))
    mac64_deep_factory.addStep(test_generic(name="Debug-osr-17", shellname="avmshell_d_osr_64", vmargs="-osr=17", config="", scriptargs=""))
    mac64_deep_factory.addStep(test_generic(name="Debug-osr-0", shellname="avmshell_d_osr_64", vmargs="-osr=0", config="", scriptargs=""))
    mac64_deep_factory.addStep(test_generic(name="Release-Dgreedy", shellname="avmshell_64", vmargs="-Dgreedy", config="", scriptargs="--timeout=180 --random --threads=1"))
    mac64_deep_factory.addStep(test_generic(name="DebugDebugger_VerifyAll", shellname="avmshell_sd_64", vmargs="", config="", scriptargs="--verify --timeout=300 --random --threads=1"))
    mac64_deep_factory.addStep(test_generic(name="DebugDebugger_VerifyOnly", shellname="avmshell_sd_64", vmargs="", config="", scriptargs="--verifyonly --timeout=300 --random --threads=1"))
    mac64_deep_factory.addStep(test_generic(name="Release-GCthreshold", shellname="avmshell_64", vmargs="-Dgcthreshold 128 -load 1.05,1,1.05,5,1.05,20", config="", scriptargs=""))
    mac64_deep_factory.addStep(test_generic(name="DebugDebugger-GCthreshold", shellname="avmshell_sd_64", vmargs="-Dgcthreshold 128 -load 1.05,1,1.05,5,1.05,20", config="", scriptargs=""))
    mac64_deep_factory.addStep(compile_generic(name="ReleaseHeapGraph", shellname="avmshell_heapgraph_64", args="--enable-heap-graph --target=x86_64-darwin --mac-sdk=105", upload="false", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64 +AVMTWEAK_HEAP_GRAPH"))
    mac64_deep_factory.addStep(test_selftest(name="ReleaseHeapGraph", shellname="avmshell_heapgraph_64"))
    mac64_deep_factory.addStep(deep_run_brightspot(name='ReleaseDebugger', shell='avmshell_s_64', testargs='--quiet'))
    mac64_deep_factory.addStep(compile_generic(name="ReleaseDebugger-Valgrind", shellname="avmshell_s_valgrind_64", args="--enable-debugger --enable-valgrind --mac-sdk=105 --target=x86_64-darwin", upload="false", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64 +AVMFEATURE_DEBUGGER +AVMFEATURE_VALGRIND"))
    mac64_deep_factory.addStep(test_generic(name="ReleaseDebugger-Valgrind", shellname="avmshell_s_valgrind_64", vmargs="", config="", scriptargs="--valgrind"))
    mac64_deep_factory.addStep(test_generic(name="Debug-jit", shellname="avmshell_d_64", vmargs="-Ojit", config="", scriptargs=""))
    mac64_deep_factory.addStep(util_process_clean)
    mac64_deep_factory.addStep(util_clean_buildsdir)
    mac64_deep_factory.addStep(sync_clean)

    mac64_deep_builder = {
                'name': "mac64-deep",
                'slavename': "mac64-deep",
                'factory': mac64_deep_factory,
                'builddir': './mac64-deep',
    }


    ##################################
    #### builder for linux-deep   ####
    ##################################
    linux_deep_factory = factory.BuildFactory()
    linux_deep_factory.addStep(sync_clean)
    linux_deep_factory.addStep(sync_clone_sandbox)
    linux_deep_factory.addStep(sync_update)
    linux_deep_factory.addStep(bb_slaveupdate(slave="linux-deep"))
    linux_deep_factory.addStep(compile_generic(name="Release-wordcode", shellname="avmshell_wordcode_64", args="--enable-wordcode-interp", upload="false", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64 +AVMFEATURE_WORDCODE_INTERP"))
    linux_deep_factory.addStep(compile_generic(name="DebugDebugger-wordcode", shellname="avmshell_sd_wordcode_64", args="--enable-debug --enable-debugger --enable-wordcode-interp", upload="false", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64 +AVMFEATURE_WORDCODE_INTERP"))    
    linux_deep_factory.addStep(download_testmedia)
    linux_deep_factory.addStep(test_selftest(name="Debug", shellname="avmshell_d"))
    linux_deep_factory.addStep(test_selftest(name="ReleaseDebugger", shellname="avmshell_s"))
    linux_deep_factory.addStep(test_selftest(name="DebugDebugger", shellname="avmshell_sd"))
    linux_deep_factory.addStep(test_generic(name="Release-wordcode-interp", shellname="avmshell_wordcode_64", vmargs="-Dinterp", config="", scriptargs=""))
    linux_deep_factory.addStep(test_generic(name="DebugDebugger-wordcode-interp", shellname="avmshell_sd_wordcode_64", vmargs="-Dinterp", config="", scriptargs=""))
    linux_deep_factory.addStep(test_generic(name="Release-Djitordie", shellname="avmshell", vmargs="-Djitordie", config="", scriptargs=""))
    linux_deep_factory.addStep(test_generic(name="ReleaseDebugger-nodebugger", shellname="avmshell_s", vmargs="-Dnodebugger", config="", scriptargs=""))
    linux_deep_factory.addStep(test_generic(name="DebugDebugger-nodebugger", shellname="avmshell_sd", vmargs="-Dnodebugger", config="", scriptargs=""))
    linux_deep_factory.addStep(compile_generic(name="Release-system-malloc", shellname="avmshell_sysmalloc", args="--enable-use-system-malloc", upload="true", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64 +AVMFEATURE_USE_SYSTEM_MALLOC"))
    linux_deep_factory.addStep(test_generic(name="Release-system-malloc", shellname="avmshell_sysmalloc", vmargs="", config="x64-lnx-tvm-release-use-system-malloc", scriptargs=""))
    linux_deep_factory.addStep(test_misc)
    linux_deep_factory.addStep(compile_generic(name="ReleaseDebugger-air", shellname="avmshell_air", args="--enable-override-global-new --enable-use-system-malloc --enable-debugger", upload="true", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64 +AVMFEATURE_DEBUGGER +AVMFEATURE_OVERRIDE_GLOBAL_NEW +AVMFEATURE_USE_SYSTEM_MALLOC"))
    linux_deep_factory.addStep(test_generic(name="ReleaseDebugger-air", shellname="avmshell_air", vmargs="", config="", scriptargs=""))
    linux_deep_factory.addStep(compile_generic(name="Release-osr", shellname="avmshell_osr", args="--enable-osr", upload="false", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64 +AVMFEATURE_OSR"))
    linux_deep_factory.addStep(compile_generic(name="Debug-osr", shellname="avmshell_d_osr", args="--enable-debug --enable-osr", upload="false", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64 +AVMFEATURE_OSR"))
    linux_deep_factory.addStep(test_generic(name="Release-osr-17", shellname="avmshell_osr", vmargs="-osr=17", config="", scriptargs=""))
    linux_deep_factory.addStep(test_generic(name="Release-osr-0", shellname="avmshell_osr", vmargs="-osr=0", config="", scriptargs=""))
    linux_deep_factory.addStep(test_generic(name="Debug-osr-17", shellname="avmshell_d_osr", vmargs="-osr=17", config="", scriptargs=""))
    linux_deep_factory.addStep(test_generic(name="Debug-osr-0", shellname="avmshell_d_osr", vmargs="-osr=0", config="", scriptargs=""))
    linux_deep_factory.addStep(test_generic(name="Release-Dgreedy", shellname="avmshell", vmargs="-Dgreedy", config="", scriptargs="--timeout=180 --random --threads=1"))
    linux_deep_factory.addStep(test_generic(name="DebugDebugger_VerifyAll", shellname="avmshell_sd", vmargs="", config="", scriptargs="--verify --timeout=300 --random --threads=1"))
    linux_deep_factory.addStep(test_generic(name="DebugDebugger_VerifyOnly", shellname="avmshell_sd", vmargs="", config="", scriptargs="--verifyonly --timeout=300 --random --threads=1"))
    linux_deep_factory.addStep(test_generic(name="Release-GCthreshold", shellname="avmshell", vmargs="-Dgcthreshold 128 -load 1.05,1,1.05,5,1.05,20", config="", scriptargs=""))
    linux_deep_factory.addStep(test_generic(name="DebugDebugger-GCthreshold", shellname="avmshell_sd", vmargs="-Dgcthreshold 128 -load 1.05,1,1.05,5,1.05,20", config="", scriptargs=""))
    linux_deep_factory.addStep(test_generic(name="Debug-jit", shellname="avmshell_d", vmargs="-Ojit", config="", scriptargs=""))
    linux_deep_factory.addStep(compile_generic(name="ReleaseHeapGraph", shellname="avmshell_heapgraph_64", args="--enable-heap-graph", upload="false", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64 +AVMTWEAK_HEAP_GRAPH"))
    linux_deep_factory.addStep(test_selftest(name="ReleaseHeapGraph", shellname="avmshell_heapgraph_64"))
    linux_deep_factory.addStep(compile_generic(name="ReleaseDebugger-Valgrind", shellname="avmshell_s_valgrind_64", args="--enable-debugger --enable-valgrind", upload="false", features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64 +AVMFEATURE_DEBUGGER +AVMFEATURE_VALGRIND"))
    linux_deep_factory.addStep(test_generic(name="ReleaseDebugger-Valgrind", shellname="avmshell_s_valgrind_64", vmargs="", config="", scriptargs="--valgrind"))
    linux_deep_factory.addStep(deep_codecoverage(compilecsv="../all/codecoverage-compile.csv", testcsv="../all/codecoverage-test.csv"))
    linux_deep_factory.addStep(deep_codecoverage_process)
    linux_deep_factory.addStep(download_testmedia) # grab test media again, to protect against previous -ES run in code coverage
    # Do a test run where we compile with -ES. MUST be the last step of the build as it recompiles the .abc files used by all the other steps
    linux_deep_factory.addStep(test_generic(name="Release_ES", shellname="avmshell", vmargs="", config="",
                                              scriptargs="--ascargs=-no-AS3 --addtoconfig=-ES -f -x abcasm,ecma3,spidermonkey"))
    linux_deep_factory.addStep(download_testmedia) # grab test media again, to protect against previous -ES run
    linux_deep_factory.addStep(deep_run_brightspot(name='DebugDebugger', shell='avmshell_sd', testargs='--timelimit=120 --random --quiet'))
    linux_deep_factory.addStep(util_process_clean)
    linux_deep_factory.addStep(util_clean_buildsdir)
    linux_deep_factory.addStep(sync_clean)

    linux_deep_builder = {
                'name': "linux-deep",
                'slavename': "linux-deep",
                'factory': linux_deep_factory,
                'builddir': './linux-deep',
    }
    
    
    ######################################
    #### builder for linux-arm-deep   ####
    ######################################
    linux_arm_deep_factory = factory.BuildFactory()
    linux_arm_deep_factory.addStep(sync_clean)
    linux_arm_deep_factory.addStep(sync_clone_sandbox)
    linux_arm_deep_factory.addStep(sync_update)
    linux_arm_deep_factory.addStep(bb_slaveupdate(slave="linux-arm-deep"))
    linux_arm_deep_factory.addStep(download_testmedia)
    linux_arm_deep_factory.addStep(test_generic_ssh(name="Release-softfloat", shellname="avmshell_neon_arm", vmargs="", config="arm-lnx-tvm-release", scriptargs=""))
    linux_arm_deep_factory.addStep(test_selftest_ssh(name="Debug", shellname="avmshell_neon_arm_d"))
    linux_arm_deep_factory.addStep(test_generic_ssh(name="Debug-vfp", shellname="avmshell_neon_arm_d", vmargs="-Darm_arch 7 -Darm_vfp", config="arm-lnx-tvm-debug", scriptargs=""))
    linux_arm_deep_factory.addStep(test_generic_ssh(name="Debug-softfloat", shellname="avmshell_neon_arm_d", vmargs="", config="arm-lnx-tvm-debug", scriptargs=""))
    linux_arm_deep_factory.addStep(test_generic_ssh(name="Release-softfloat-deep", shellname="avmshell_neon_arm", vmargs="", config="arm-lnx-tvm-release-deep", scriptargs=""))
    linux_arm_deep_factory.addStep(test_generic_ssh(name="Release-deep", shellname="avmshell_neon_arm", vmargs="-Darm_arch 7 -Darm_vfp", config="arm-lnx-tvm-release-deep", scriptargs=""))
    linux_arm_deep_factory.addStep(test_generic_ssh(name="Release-Dinterp-deep", shellname="avmshell_neon_arm", vmargs="-Dinterp", config="arm-lnx-tvm-release-Dinterp-deep", scriptargs=""))
    linux_arm_deep_factory.addStep(test_generic_ssh(name="Release-GCthreshold", shellname="avmshell_neon_arm", vmargs="-Darm_arch 7 -Darm_vfp -Dgcthreshold 128 -load 1.05,1,1.05,5,1.05,20", config="arm-lnx-tvm-release", scriptargs=""))
    linux_arm_deep_factory.addStep(test_generic_ssh(name="DebugDebugger-GCthreshold", shellname="avmshell_neon_arm_d", vmargs="-Darm_arch 7 -Darm_vfp -Dgcthreshold 128 -load 1.05,1,1.05,5,1.05,20", config="arm-lnx-tvm-debug", scriptargs=""))
    linux_arm_deep_factory.addStep(compile_generic(name="Release-osr", shellname="avmshell_neon_arm_osr", args="--enable-arm-neon --arm-arch=armv7-a --target=arm-linux --enable-sys-root-dir=/usr/local/arm-linux/debian5 --enable-osr", upload="false", features=""))
    linux_arm_deep_factory.addStep(compile_generic(name="Debug-osr", shellname="avmshell_neon_arm_d_osr", args="--enable-debug --enable-arm-neon --arm-arch=armv7-a --target=arm-linux --enable-sys-root-dir=/usr/local/arm-linux/debian5 --enable-osr", upload="false", features=""))
    linux_arm_deep_factory.addStep(test_generic_ssh(name="Release-osr-17", shellname="avmshell_neon_arm_osr", vmargs="-osr=17", config="arm-lnx-tvm-release-osr=17", scriptargs=""))
    linux_arm_deep_factory.addStep(test_generic_ssh(name="Release-osr-0", shellname="avmshell_neon_arm_osr", vmargs="-osr=0", config="arm-lnx-tvm-release-osr=0", scriptargs=""))
    linux_arm_deep_factory.addStep(test_generic_ssh(name="Debug-osr-17", shellname="avmshell_neon_arm_d_osr", vmargs="-osr=17", config="arm-lnx-tvm-debug-osr=17", scriptargs=""))
    linux_arm_deep_factory.addStep(test_generic_ssh(name="Debug-osr-0", shellname="avmshell_neon_arm_d_osr", vmargs="-osr=0", config="arm-lnx-tvm-debug-osr=0", scriptargs=""))
    linux_arm_deep_factory.addStep(test_generic_ssh(name="Debug-jit-vfp", shellname="avmshell_neon_arm_d", vmargs="-Darm_arch 7 -Darm_vfp -Ojit", config="arm-lnx-tvm-debug-Ojit", scriptargs=""))
    linux_arm_deep_factory.addStep(util_acceptance_clean_ssh)
    linux_arm_deep_factory.addStep(util_clean_buildsdir)
    linux_arm_deep_factory.addStep(sync_clean)

    linux_arm_deep_builder = {
                'name': "linux-arm-deep",
                'slavename': "linux-arm-deep",
                'factory': linux_arm_deep_factory,
                'builddir': './linux-arm-deep',
    }


    #######################################
    #### builder for linux-mips-deep   ####
    #######################################
    linux_mips_deep_factory = factory.BuildFactory()
    linux_mips_deep_factory.addStep(sync_clean)
    linux_mips_deep_factory.addStep(sync_clone_sandbox)
    linux_mips_deep_factory.addStep(sync_update)
    linux_mips_deep_factory.addStep(bb_slaveupdate(slave="linux-mips-deep"))
    linux_mips_deep_factory.addStep(download_testmedia)
    linux_mips_deep_factory.addStep(test_generic_ssh(name="Release", shellname="avmshell_mips", vmargs="-Dinterp", config="mips-lnx-tvm-release-Dinterp", scriptargs=""))
    linux_mips_deep_factory.addStep(test_generic_ssh(name="Debug", shellname="avmshell_mips_d", vmargs="-Dinterp", config="mips-lnx-tvm-debug-Dinterp", scriptargs=""))
    linux_mips_deep_factory.addStep(compile_generic(name="Release-osr", shellname="avmshell_mips_osr", args="--target=mips-linux --enable-osr", upload="false", features=""))
    linux_mips_deep_factory.addStep(compile_generic(name="Debug-osr", shellname="avmshell_mips_d_osr", args="--enable-debug --target=mips-linux --enable-osr", upload="false", features=""))
    linux_mips_deep_factory.addStep(test_generic_ssh(name="Release-osr-17", shellname="avmshell_mips_osr", vmargs="-osr=17", config="mips-lnx-tvm-release-osr=17", scriptargs=""))
    linux_mips_deep_factory.addStep(test_generic_ssh(name="Release-osr-0", shellname="avmshell_mips_osr", vmargs="-osr=0", config="mips-lnx-tvm-release-osr=0", scriptargs=""))
    linux_mips_deep_factory.addStep(test_generic_ssh(name="Debug-osr-17", shellname="avmshell_mips_d_osr", vmargs="-osr=17", config="mips-lnx-tvm-debug-osr=17", scriptargs=""))
    linux_mips_deep_factory.addStep(test_generic_ssh(name="Debug-osr-0", shellname="avmshell_mips_d_osr", vmargs="-osr=0", config="mips-lnx-tvm-debug-osr=0", scriptargs=""))
    linux_mips_deep_factory.addStep(util_acceptance_clean_ssh)
    linux_mips_deep_factory.addStep(util_clean_buildsdir)
    linux_mips_deep_factory.addStep(sync_clean)

    linux_mips_deep_builder = {
                'name': "linux-mips-deep",
                'slavename': "linux-mips-deep",
                'factory': linux_mips_deep_factory,
                'builddir': './linux-mips-deep',
    }


    ##########################################
    #### builder for promote-build ####
    ##########################################
    
    promote_build_factory = factory.BuildFactory()
    promote_build_factory.addStep(sync_pull)
    promote_build_factory.addStep(sync_update)
    # since we do not wipe the repo every time, delete the scripts dir before
    # running bb_slaveupdate
    promote_build_factory.addStep(ShellCommand(
            command=['./delete-bb-scripts-dir.sh'],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='Delete scripts dir' ,
            descriptionDone='Deleted scripts dir',
            name='Delete_Scripts_Dir',
            workdir='../repo/build/buildbot/slaves/all',
            haltOnFailure='False')
        )
    promote_build_factory.addStep(bb_slaveupdate(slave='promote-build'))
    promote_build_factory.addStep(ShellCommand(
            command=['../all/promote-build.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='Promote revision to latest' ,
            descriptionDone='Promoted revision to latest after deep phase pass',
            name='Promote_Build',
            workdir='../repo/build/buildbot/slaves/scripts',
            haltOnFailure='True')
        )
    
    promote_build_builder = {
                'name': 'promote-build',
                'slavename': 'promote-build',
                'factory': promote_build_factory,
                'builddir': './promote-build', 
    }
    
    builders = [
                windows_deep_builder,
                windows_64_deep_builder,                
                mac_deep_builder,
                mac64_deep_builder,
                linux_deep_builder,
                linux_arm_deep_builder,
                linux_mips_deep_builder,
                
                promote_build_builder,
                ]


