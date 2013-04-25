{
    'targets':
    [
        {
            'variables' :
            {
                'AVMGLUE_DIR': '<(DEPTH)/flash/avmglue',
            },

            'target_name': 'avm',
            'type': 'static_library',

            'dependencies':
            [
                'mmgc',
                'builtinglue',
            ],

            'include_dirs':
            [
                'core',
                'pcre',
                'VMPI',
                'AVMPI',
                'vmbase',
                'platform',
                'extensions',
                '<(AVMGLUE_DIR)',
            ],

            'all_dependent_settings':
            {
                'include_dirs':
                [
                    'core',
                    'pcre',
                    'AVMPI',
                    'VMPI',
                    'vmbase',
                    'platform',
                    'extensions',
                    'nanojit',
                    '<(AVMGLUE_DIR)',
                ],
                
                'conditions':
                    [
                    [
                        'OS == "android"',
                        {
                            'include_dirs':
                                [
                                #AndroidDebug.h includes VMPI via VMPI/VMPI.h
                                '.'
                                ],
                            },
                        ],
                    ],
            },

            'sources':
            [
                'core/AbcEnv.cpp',
                'core/AbcData.cpp',
                'core/AbcParser.cpp',
                'core/ActionBlockConstants.cpp',
                'core/ArrayClass.cpp',
                'core/ArrayObject.cpp',
                'core/atom.cpp',
                'core/AvmCore.cpp',
                'core/avm.cpp',
                'core/avmplus.cpp',
                'core/avmplusDebugger.cpp',
                'core/avmplusHashtable.cpp',
                'core/avmplusList.cpp',
                'core/AvmPlusScriptableObject.cpp',
                'core/AvmSerializer.cpp',
                'core/BigInteger.cpp',
                'core/BooleanClass.cpp',
                'core/BuiltinTraits.cpp',
                'core/ByteArrayGlue.cpp',
                'core/ClassClass.cpp',
                'core/ClassClosure.cpp',
				'core/d2a.cpp',
                'core/Date.cpp',
                'core/DateClass.cpp',
                'core/DataIO.cpp',
                'core/DateObject.cpp',
                'core/DescribeTypeClass.cpp',
                'core/DictionaryGlue.cpp',
                'core/Domain.cpp',
                'core/DomainEnv.cpp',
                'core/DomainMgr.cpp',
                'core/E4XNode.cpp',
                'core/ErrorClass.cpp',
                'core/ErrorConstants.cpp',
                'core/Exception.cpp',
                'core/exec.cpp',
                'core/exec-jit.cpp',
                'core/exec-osr.cpp',
                'core/exec-verifyall.cpp',
                'core/Float4Class.cpp',
                'core/FloatClass.cpp',
                'core/FrameState.cpp',
                'core/FunctionClass.cpp',
                'core/instr.cpp',
                'core/IntClass.cpp',
                'core/Interpreter.cpp',
                'core/InvokerCompiler.cpp',
                'core/JSONClass.cpp',
                'core/MathClass.cpp',
                'core/MathUtils.cpp',
                'core/MethodClosure.cpp',
                'core/MethodEnv.cpp',
                'core/MethodInfo.cpp',
                'core/Multiname.cpp',
                'core/MultinameHashtable.cpp',
                'core/Namespace.cpp',
                'core/NamespaceClass.cpp',
                'core/NamespaceSet.cpp',
                'core/NativeFunction.cpp',
                'core/NumberClass.cpp',
                'core/ObjectClass.cpp',
                'core/ObjectIO.cpp',
                'core/peephole.cpp',
                'core/PoolObject.cpp',
                'core/ProxyGlue.cpp',
                'core/PrintWriter.cpp',
                'core/QCache.cpp',
                'core/RegExpClass.cpp',
                'core/RegExpObject.cpp',
                'core/Sampler.cpp',
                'core/ScopeChain.cpp',
                'core/ScriptBuffer.cpp',
                'core/ScriptObject.cpp',
                'core/StackTrace.cpp',
                'core/StringBuffer.cpp',
                'core/StringClass.cpp',
                'core/StringObject.cpp',
                'core/Toplevel.cpp',
                'core/Traits.cpp',
                'core/TypeDescriber.cpp',
                'core/UnicodeUtils.cpp',
                'core/VectorClass.cpp',
                'core/Verifier.cpp',
                'core/VTable.cpp',
                'core/WordcodeEmitter.cpp',
                'core/WordcodeTranslator.cpp',
                'core/wopcodes.cpp',
                'core/XMLClass.cpp',
                'core/XMLListClass.cpp',
                'core/XMLListObject.cpp',
                'core/XMLObject.cpp',
                'core/XMLParser16.cpp',
                'nanojit/Allocator.cpp',
                'nanojit/CodeAlloc.cpp',
                'nanojit/Containers.cpp',
                'nanojit/Assembler.cpp',
                'nanojit/Fragmento.cpp',
                'core/CodegenLIR.cpp',
                'core/LirHelper.cpp',
                'nanojit/LIR.cpp',
                'nanojit/njconfig.cpp',
                'nanojit/RegAlloc.cpp',
                'pcre/pcre_chartables.cpp',
                'pcre/pcre_compile.cpp',
                'pcre/pcre_config.cpp',
                'pcre/pcre_exec.cpp',
                'pcre/pcre_fullinfo.cpp',
                'pcre/pcre_get.cpp',
                'pcre/pcre_globals.cpp',
                'pcre/pcre_info.cpp',
                'pcre/pcre_maketables.cpp',
                'pcre/pcre_newline.cpp',
                'pcre/pcre_ord2utf8.cpp',
                'pcre/pcre_refcount.cpp',
                'pcre/pcre_study.cpp',
                'pcre/pcre_tables.cpp',
                'pcre/pcre_try_flipped.cpp',
                'pcre/pcre_valid_utf8.cpp',
                'pcre/pcre_version.cpp',
                'pcre/pcre_xclass.cpp',
                'AVMPI/AvmAssert.cpp',
                'AVMPI/float4Support.cpp',
                'core/AvmLog.cpp',
                'core/Coder.cpp'
            ],

            'conditions':
            [
                [
                    'OS == "win"',
                    {
                        'defines':
                        [
                            'AVMPLUS_IA32',
                            'NOMINMAX',
                        ],

                        'dependencies':
                            [
                            'zlib', #why the heck does this depend on zlib's header?
                            ],

                        'include_dirs':
                        [
                            'generated',
                            '<(DEPTH)/third_party/win/PlatformSDK_v6.0/Include',
                        ],

                        'msvs_disabled_warnings':
                        [
                            4061, # enumerator 'identifier' in switch of enum 'enumeration' is not explicitly handled by a case label
                            4062, # enumerator 'identifier' in switch of enum 'enumeration' is not handled (and no default)
                            4191, # 'operator/operation' : unsafe conversion from 'type of expression' to 'type required'
                            4365, # 'action' : conversion from 'type_1' to 'type_2', signed/unsigned mismatch
                            4625, # 'class' : copy constructor could not be generated because a base class copy constructor is inaccessible
                            4626, # assignment operator could not be generated because a base class assignment operator is inaccessible
                            
                            #Note these are emitted by MMGC and avm
                            4710, # 'function' : function not inlined

                            #Note these are emitted by the PlatformSDK and AVM
                            4668, # 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
                            4820, # 'bytes' bytes padding added after member 'member',
                        ],
                        'sources':
                        [
                            'nanojit/Nativei386.cpp',
                            'platform/win32/avmplus.cpp',
                            'platform/win32/coff.cpp',
                            'platform/win32/MathUtilsWin.cpp',
                            'platform/win32/NativeFunctionWin.cpp',
                            'platform/win32/OSDepWin.cpp',
                            'platform/win32/win32setjmp.cpp',
                            'VMPI/WinPortUtils.cpp',
                            'VMPI/WinDebugUtils.cpp',
                        ],

                        'msvs_settings':
                        {
                            'VCLibrarianTool':
                            {
                                'AdditionalOptions': '/IGNORE:4221',
                            },
                        },

                        'configurations':
                        {
                            'Debug_Base':
                            {
                                'msvs_settings':
                                {
                                    'VCCLCompilerTool':
                                    {
                                        'BasicRuntimeChecks': '3',             # /RTC1
                                    },
                                },
                            },

                            'Release_Base':
                            {
                                'msvs_settings':
                                {
                                    'VCCLCompilerTool':
                                    {
                                        'FloatingPointModel': '2',             # /fp:fast
                                        'OmitFramePointers': 'true',		   # /Oy
                                    },
                                }
                            },
                        },
                    }
                ],

                [
                    'OS == "mac"',
                    {
                        'include_dirs':
                        [
                            'avmplus/MMgc',
                            'avmplus/VMPI',
                            'avmplus/vmbase',
                            'avmplus/vmbase',
                            'avmplus/platform',
                            'avmplus/generated',
                        ],
                        'sources':
                        [
                            'platform/unix/MathUtilsUnix.cpp',
                            'platform/unix//OSDepUnix.cpp',
                            'VMPI/ThreadsPosix.cpp',
                            'AVMPI/SpyUtilsPosix.cpp',
                            'VMPI/GenericPortUtils.cpp',
                            'VMPI/PosixPortUtils.cpp',
                            'VMPI/MacPortUtils.cpp',
                            'VMPI/MacDebugUtils.cpp',
                        ],
                        'conditions':
                        [
                            [
                                'target_arch == "x86"',
                                {
                                    'sources':
                                    [
                                        'nanojit/Nativei386.cpp', #This needs to be moved to an arch dependant conditional
                                    ]

                                },
                                {
                                    #x64 bit
                                    'sources':
                                    [
                                        'nanojit/NativeX64.cpp', #This needs to be moved to an arch dependant conditional
                                    ]
                                }

                            ],
                        ],
                        'xcode_settings':
                        {
                            'ALWAYS_SEARCH_USER_PATHS' : 'YES',
                            # suppress warnings about offsetof
                            'GCC_WARN_ABOUT_INVALID_OFFSETOF_MACRO' : 'NO',
                            'GCC_PRECOMPILE_PREFIX_HEADER' : 'YES',
                            'GCC_PREFIX_HEADER' : '<(build_system_dep_root_dir)/flash/platform/osx/avmplus_Prefix.pch',
                            'GCC_VERSION' : 'com.apple.compilers.llvmgcc42'
                        },
                        'configurations':
                        {
                            'Debug_Base':
                            {
                                'xcode_settings':
                                {
                                    #TODO: Is this really only for avm? I highly doubt.
                                    'DEBUG_INFORMATION_FORMAT' : 'dwarf-with-dsym'
                                },
                            },
                            'Release_Base':
                            {
                                'xcode_settings':
                                {
                                    'ZERO_LINK' : 'NO'
                                }
                            },
                        }
                    }
                ],

                [
                    'OS == "linux"',
                    {
                        'defines':
                        [
                            'AVMPLUS_CDECL'
                        ],
                        'sources':
                        [
                            'platform/unix/MathUtilsUnix.cpp',
                            'platform/unix/OSDepUnix.cpp',
                            'VMPI/ThreadsPosix.cpp',
                            'AVMPI/SpyUtilsPosix.cpp',
                            'VMPI/GenericPortUtils.cpp',
                            'VMPI/PosixPortUtils.cpp',
                            'VMPI/PosixSpecificUtils.cpp',
                            'VMPI/UnixDebugUtils.cpp',
                        ],
                        'conditions':
                        [
                            [
                                'target_arch == "x86"',
                                {
                                    'sources':
                                    [
                                        'nanojit/Nativei386.cpp',
                                    ]

                                },
                               ],
                            [
                                'target_arch == "x64"',
                                
                                {
                                    'sources':
                                    [
                                        'nanojit/NativeX64.cpp',
                                    ]
                                }
                                ],

                            ],
                    },
                ],
                [
                    'OS == "stagecraft"',
                    {
                        'defines':
                        [
                            '<@(STAGECRAFT_DEFINES)',
                            'AVMPLUS_CDECL'
                        ],
                        'sources':
                        [
                            'platform/unix/MathUtilsUnix.cpp',
                            'platform/unix/OSDepUnix.cpp',
                            'VMPI/ThreadsPosix.cpp',
                            'AVMPI/SpyUtilsPosix.cpp',
                            'VMPI/GenericPortUtils.cpp',
                            'VMPI/PosixSpecificUtils.cpp',
                            'VMPI/UnixDebugUtils.cpp',
                        ],
                        'conditions':
                        [
                            [
                                'target_arch == "x86"',
                                {
                                    'sources':
                                    [
                                        'nanojit/Nativei386.cpp',
                                    ]

                                },
                               ],
                            [
                                'target_arch == "x64"',
                                
                                {
                                    'sources':
                                    [
                                        'nanojit/NativeX64.cpp',
                                    ]
                                }
                                ],

                            ],
                    },
                ],

                [
                    'OS == "android"',
                    {
                         'sources':
                        [
                            'nanojit/NativeARM.cpp',
                            'platform/unix/MathUtilsUnix.cpp',
                            'platform/unix/OSDepUnix.cpp',
                            'VMPI/ThreadsPosix.cpp',
                            'AVMPI/SpyUtilsPosix.cpp',
                            'VMPI/GenericPortUtils.cpp',
                            'VMPI/PosixPortUtils.cpp',
                            'VMPI/PosixSpecificUtils.cpp',
                        ]
                    },
                ],
            ],
        }
    ]
}
