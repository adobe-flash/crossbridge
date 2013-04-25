{
    'targets':
    [
        {
            'variables' :
            {
                'AVMGLUE_DIR': '<(DEPTH)/flash/avmglue',
            },

            'target_name': 'mmgc',
            'type': 'static_library',

            'include_dirs':
            [
                'core',
                'platform',
                'AVMPI',
                'VMPI',
                'vmbase',
                'MMgc',
                '<(AVMGLUE_DIR)',
            ],

            'dependencies' :
            [
                'builtinglue',
            ],

            'all_dependent_settings':
            {
                'include_dirs':
                [
                    'MMgc',
                    #Including any part of MMGC requires the includes below.
                    'core',
                    'platform',
                    'AVMPI',
                    'VMPI',
                    'vmbase',
                    '<(AVMGLUE_DIR)',
                ],
            },

            'sources':
            [
                'MMgc/GCGlobalNew.cpp',
                'MMgc/GCObject.cpp',
                'MMgc/GCMemoryProfiler.cpp',
                'MMgc/GCLargeAlloc.cpp',
                'MMgc/GCHeap.cpp',
                'MMgc/GCHashtable.cpp',
                'MMgc/GCAllocObject.cpp',
                'MMgc/GCAlloc.cpp',
                'MMgc/GC.cpp',
                'MMgc/PageMap.cpp',
                'MMgc/GCPolicyManager.cpp',
                'MMgc/GCTests.cpp',
                'MMgc/GCStack.cpp',
                'MMgc/ZCT.cpp',
                'MMgc/FixedMalloc.cpp',
                'MMgc/FixedAlloc.cpp',
                'MMgc/GCLog.cpp',
                'MMgc/GCDebug.cpp',
            ],

            'conditions':
            [
                [
                    'OS == "win"',
                    {
                        'conditions':
                        [
                            [
                                'target_arch == "x86"',
                                {
                                    'defines':
                                    [
                                        'AVMPLUS_IA32',
                                    ],
                                },
                            ],
                            [
                                'target_arch == "x64"',
                                {
                                    'defines':
                                    [
                                        'AVMPLUS_AMD64',
                                    ],
                                },
                            ],
                        ],

                        'defines':
                        [
                            'NOMINMAX',
                        ],

                        'msvs_disabled_warnings':
                        [
                            4365, #'action' : conversion from 'type_1' to 'type_2', signed/unsigned mismatch
                            4062, # enumerate has no associated handler in a switch statement, and there is no default label.
                            4061, # enumerator 'identifier' in switch of enum 'enumeration' is not explicitly handled by a case label
                            4668, #'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
                            4820, #'bytes' bytes padding added after member 'member',
                            4625, # copy constructor could not be generated because a base class copy constructor is inaccessible
                            4626, # assignment operator could not be generated because a base class assignment operator is inaccessible
                            4710, # 'function' : function not inlined
                        ],

                        'direct_dependent_settings':
                        {
                            #MMGC generates these warnings if you include it's headers
                            'msvs_disabled_warnings':
                            [
                                4365, # 'action' : conversion from 'type_1' to 'type_2', signed/unsigned mismatch
                                4625, # 'class' : copy constructor could not be generated because a base class copy constructor is inaccessible
                                4626, # assignment operator could not be generated because a base class assignment operator is inaccessible
                                4820, # 'bytes' bytes padding added after member 'member',
                                4710, # 'function' : function not inlined
                            ],
                        },

                        'configurations':
                        {
                            'Debug_Base':
                            {
                                'msvs_settings':
                                {
                                    'VCCLCompilerTool':
                                    {
                                        'BasicRuntimeChecks': '3',             #  /RTC1
                                    },

                                    'VCLibrarianTool':
                                    {
                                        'OutputFile' : '$(OutDir)/MMgc_d.lib',
                                    },
                                },
                            },

                            'Release_Base':
                            {
                                'VCLibrarianTool':
                                {
                                    'OutputFile' : '$(OutDir)/MMgc.lib',
                                },
                            },
                        },

                        'include_dirs':
                        [
                            'generated',
                            '<(DEPTH)/third_party/win/PlatformSDK_v6.0/Include',

                        ],
                        'sources':
                        [
                            'AVMPI/MMgcPortWin.cpp',
                            'AVMPI/SpyUtilsWin.cpp',
                            'VMPI/ThreadsWin.cpp',
                            'VMPI/WinPortUtils.cpp',
                            'VMPI/GenericPortUtils.cpp',
                            'VMPI/WinDebugUtils.cpp',
                        ],
                    },
                ],
                [
                    'OS == "mac"',
                    {
                        'defines':
                        [
                            'DARWIN=1',
                            'SOFT_ASSERTS',
                            'USE_MMAP',
                        ],

                        'sources':
                        [
                            'AVMPI/PosixMMgcPortUtils.cpp',
                            'AVMPI/MMgcPortMac.cpp',
                            'AVMPI/SpyUtilsPosix.cpp',
                            'VMPI/ThreadsPosix.cpp',
                            'MMgc/GCThreads.cpp',
                        ],
                    },
                ],
                [
                    'OS == "linux"',
                    {
                        'defines':
                        [
                            'HAVE_DLADDR',
                        ],

                        'sources':
                        [
                            'AVMPI/PosixMMgcPortUtils.cpp',
                            'AVMPI/MMgcPortUnix.cpp',
                            'VMPI/PosixPortUtils.cpp',
                        ],
                    },
                ],

                [
                    'OS == "stagecraft"',
                    {
                        'defines':
                        [
                            'HAVE_DLADDR',
                            '<@(STAGECRAFT_DEFINES)',
                        ],
                        'sources':
                        [
                            'AVMPI/PosixMMgcPortUtils.cpp',
                            'AVMPI/MMgcPortUnix.cpp',
                            'VMPI/PosixPortUtils.cpp',
                            'VMPI/PosixPortUtils.cpp',
                            'MMgc/GCGlobalNew.cpp',
                        ],
                    },
                ],
                [
                    'OS == "android"',
                    {
                        'sources':
                        [
                            'AVMPI/PosixMMgcPortUtils.cpp',
                            'AVMPI/MMgcPortUnix.cpp',
                            'VMPI/PosixSpecificUtils.cpp',
                        ],
                    },
                ],
            ],
        },
    ],
}
