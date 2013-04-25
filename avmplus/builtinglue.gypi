{
    'targets':
    [
        {
            'target_name': 'builtinglue',
            'type': 'none',
            'hard_dependency': 1,

            'actions':
            [
                {
                    'action_name': 'generating_glue',
                    'msvs_cygwin_shell': 0,

                    'inputs':
                    [
                    ],

                    'outputs':
                    [
                        #PRODUCT_DIR is required or else the stamp goes to the folder from which
                        #the command is being run.
                        '<(PRODUCT_DIR)/builtinglue.stamp',
                    ],

                    'action':
                    [
                        '<@(ant_command)',
                        'builtins-<(product)',
                    ],
                },
            ],
            'conditions':
            [
                [
                    'OS=="stagecraft"',
                    {
                        'actions':
                        [
                            {
                                'action_name': 'stamping glue',
                                'inputs':
                                [
                                    '<(PRODUCT_DIR)/builtinglue.stamp',
                                ],
                                'outputs':
                                [
                                    '<(PRODUCT_DIR)/builtinglue.stomp',
                                ],
                                'action':
                                [
                                    'touch',
                                    '<(PRODUCT_DIR)/builtinglue.stamp',
                                    '<(PRODUCT_DIR)/builtinglue.stomp',
                                ],
                            },
                        ],
                    },
                ],
            ],

            'all_dependent_settings':
            {
                'include_dirs':
                [
                    '<(build_system_dep_root_dir)/build/<(OS)/int/FlashPlayer/avmglue.as/builtins/<(build_system_dep_build_type_var)',
                ],
                'conditions':
                [
#                    [
#                       'product == "player"',
#                        {
#                            'include_dirs':
#                            [
#                                '<(build_system_dep_root_dir)/build/<(OS)/int/FlashPlayer/avmglue.as/builtins/<(build_system_dep_build_type_var)',
#                            ],
#                        },
#                        {
#                            'include_dirs':
#                            [
#                                '<(INTERMEDIATE_DIR)/glue/int',
#                            ],
#                        },
#                    ],
                    [
                        'OS == "stagecraft"',
                        {
                            'include_dirs!':
                            [
                                '<(build_system_dep_root_dir)/build/<(OS)/int/FlashPlayer/avmglue.as/builtins/<(build_system_dep_build_type_var)',
                            ],
                            'include_dirs':
                            [
                                '<(INTERMEDIATE_DIR)/glue/int/avmglue.as/builtins/<(build_system_dep_build_type_var)',
                            ],
                        },
                    ],
                ],
            },
        },
    ],
}
