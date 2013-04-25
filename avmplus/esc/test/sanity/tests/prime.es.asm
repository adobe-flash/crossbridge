
{ 'abc_class': 'AbcFile'
, 'minor_version': 16
, 'major_version': 46
, 'int_pool': [ 
              , '2'
              , '1000'
              , '0' ]
, 'uint_pool': [  ]
, 'double_pool': [  ]
, 'utf8_pool': [ 
               , ''
               , 'Object'
               , 'Array'
               , 'RegExp'
               , 'prime'
               , 'n'
               , 'm'
               , 'limit'
               , 'i'
               , 'j'
               , 'print' ]
, 'namespace_pool': [ 
                    , { 'kind': 'PackageNamespace'
                      , 'utf8': '1' }
                    , { 'kind': 'AnonymousNamespace'
                      , 'utf8': '1' } ]
, 'nsset_pool': [ 
                , [ '2' ] ]
, 'name_pool': [ 
               , { 'kind': 'QName'
                 , 'ns': '1'
                 , 'utf8': '2' }
               , { 'kind': 'QName'
                 , 'ns': '1'
                 , 'utf8': '3' }
               , { 'kind': 'QName'
                 , 'ns': '1'
                 , 'utf8': '4' }
               , { 'kind': 'QName'
                 , 'ns': '2'
                 , 'utf8': '5' }
               , { 'kind': 'QName'
                 , 'ns': '2'
                 , 'utf8': '6' }
               , { 'kind': 'QName'
                 , 'ns': '2'
                 , 'utf8': '7' }
               , { 'kind': 'QName'
                 , 'ns': '2'
                 , 'utf8': '8' }
               , { 'kind': 'QName'
                 , 'ns': '2'
                 , 'utf8': '9' }
               , { 'kind': 'Multiname'
                 , 'utf8': '6'
                 , 'nsset': '1' }
               , { 'kind': 'Multiname'
                 , 'utf8': '7'
                 , 'nsset': '1' }
               , { 'kind': 'Multiname'
                 , 'utf8': '9'
                 , 'nsset': '1' }
               , { 'kind': 'Multiname'
                 , 'utf8': '8'
                 , 'nsset': '1' }
               , { 'kind': 'QName'
                 , 'ns': '2'
                 , 'utf8': '10' }
               , { 'kind': 'Multiname'
                 , 'utf8': '5'
                 , 'nsset': '1' }
               , { 'kind': 'Multiname'
                 , 'utf8': '10'
                 , 'nsset': '1' }
               , { 'kind': 'Multiname'
                 , 'utf8': '11'
                 , 'nsset': '1' } ]
, 'method_infos': [ { 'ret_type': 
                    , 'param_types': []
                    , 'name': 
                    , 'flags': 
                    , 'optional_count': 
                    , 'value_kind': [  ]
                    , 'param_names': [  ] }
                  , { 'ret_type': 
                    , 'param_types': []
                    , 'name': 
                    , 'flags': 
                    , 'optional_count': 
                    , 'value_kind': [  ]
                    , 'param_names': [  ] }
                  ,  ]
, 'method_bodys': [ { 'method_info': 
                    , 'max_stack': 
                    , 'max_regs': 
                    , 'scope_depth': 
                    , 'max_scope': 
                    , 'code': [ getlocal0
                              , pushscope
                              , newactivation
                              , dup
                              , setlocal2
                              , pushscope
                              , findproperty 05
                              , getlocal1
                              , setproperty 05
                              , pushundefined
                              , pop
                              , findproperty 06
                              , findpropstrict 09
                              , getproperty 09
                              , setproperty 06
                              , pushundefined
                              , pop
                              , findproperty 07
                              , findpropstrict 09
                              , getproperty 09
                              , pushint 01
                              , divide
                              , setproperty 07
                              , pushundefined
                              , pop
                              , findproperty 08
                              , pushint 01
                              , setproperty 08
                              , pushundefined
                              , pop
                              , jump 00 00 00
                              , label
                              , findpropstrict 0a
                              , getproperty 0a
                              , findpropstrict 0b
                              , getproperty 0b
                              , modulo
                              , not
                              , iffalse 00 00 00
                              , pushfalse
                              , setlocal3
                              , getlocal3
                              , returnvalue
                              , kill 03
                              , findpropstrict 0b
                              , dup
                              , getproperty 0b
                              , dup
                              , setlocal3
                              , increment
                              , setproperty 0b
                              , getlocal3
                              , kill 03
                              , pop
                              , findpropstrict 0b
                              , getproperty 0b
                              , findpropstrict 0c
                              , getproperty 0c
                              , lessequals
                              , iftrue d0 ff ff
                              , pushtrue
                              , setlocal3
                              , getlocal3
                              , returnvalue
                              , kill 03
                              , kill 02
                              , returnvoid
                              ,  ]
                    , 'exceptions': [  ]
                    , 'fixtures': [  ] }
                  , { 'method_info': 
                    , 'max_stack': 
                    , 'max_regs': 
                    , 'scope_depth': 
                    , 'max_scope': 
                    , 'code': [ getlocal0
                              , pushscope
                              , findproperty 0d
                              , pushint 02
                              , setproperty 0d
                              , pushundefined
                              , pop
                              , jump 00 00 00
                              , label
                              , findpropstrict 0e
                              , getproperty 0e
                              , pushnull
                              , findpropstrict 0f
                              , getproperty 0f
                              , call 01
                              , iffalse 00 00 00
                              , findpropstrict 10
                              , getproperty 10
                              , pushnull
                              , findpropstrict 0f
                              , getproperty 0f
                              , call 01
                              , pop
                              , findpropstrict 0f
                              , dup
                              , getproperty 0f
                              , dup
                              , setlocal1
                              , decrement
                              , setproperty 0f
                              , getlocal1
                              , kill 01
                              , pop
                              , findpropstrict 0f
                              , getproperty 0f
                              , pushint 03
                              , equals
                              , not
                              , iftrue ca ff ff
                              , pushtrue
                              , pop
                              , returnvoid
                              ,  ]
                    , 'exceptions': [  ]
                    , 'fixtures': [  ] }
                  ,  ] }