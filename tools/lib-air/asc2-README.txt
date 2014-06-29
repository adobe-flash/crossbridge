ASC2:

ActionScript 3.0 Compiler for AVM+
Version 2.0.0 build 354131
Copyright 2003-2012 Adobe Systems Incorporated. All rights reserved.

usage: asc [-abcfuture] [-api <version>] [-AS3] [-avmtarget <vm version
       number>] [-b] [-coach] [-config <ns::name=value>] [-d] [-doc]
       [-ES] [-ES4] [-exe <avmplus path>] [-f] [-h] [-i] [-import
       <filename>] [-in <filename>] [-inline] [-l] [-language <lang>]
       [-le <swc file>] [-li <swc file>] [-log] [-m] [-md] [-merge]
       [-movieclip] [-o] [-o2 <name=value>] [-optimize] [-out
       <basename>] [-outdir <output directory name>] [-p] [-parallel]
       [-removedeadcode] [-sanity] [-static] [-strict] [-swf
       <classname,width,height[,fps]>] [-use <namespace>] [-warnings]
       FILENAME...
options:
 -abcfuture                            future abc
 -api <version>                        compile program as a specfic
                                       version between 660 and 670
 -AS3                                  use the AS3 class based object
                                       model for greater performance and
                                       better error reporting
 -avmtarget <vm version number>        emit bytecode for a target
                                       virtual machine version, 1 is
                                       AVM1, 2 is AVM2
 -b                                    show bytes
 -coach                                warn on common actionscript
                                       mistakes (deprecated)
 -config <ns::name=value>              define a configuration value in
                                       the namespace ns
 -d                                    emit debug info into the bytecode
 -doc                                  emit asdoc info
 -ES                                   use the ECMAScript edition 3
                                       prototype based object model to
                                       allow dynamic overriding of
                                       prototype properties
 -ES4                                  use ECMAScript 4 dialect
 -exe <avmplus path>                   emit an exe file (projector)
 -f                                    print the flow graph to standard
                                       out
 -h,--help                             print this help message
 -i                                    write intermediate code to the
                                       .il file
 -import <filename>                    make the packages in the abc file
                                       available for import
 -in <filename>                        include the specified source file
 -inline                               turn on the inlining of functions
 -l                                    show line numbers
 -language <lang>                      set the language for output
                                       strings
                                       {EN|FR|DE|IT|ES|JP|KR|CN|TW}
 -le,--libraryext <swc file>           import a swc as external library
 -li,--library <swc file>              import a swc library
 -log                                  redirect all error output to a
                                       logfile
 -m                                    write the avm+ assembly code to
                                       the .il file
 -md                                   emit metadata information into
                                       the bytecode
 -merge                                merge the compiled source into a
                                       single output file
 -movieclip                            make movieclip
 -o,--O                                produce an optimized abc file
 -o2,--O2 <name=value>                 optimizer configuration
 -optimize                             produce an optimized abc file
 -out <basename>                       Change the basename of the output
                                       file
 -outdir <output directory name>       Change the directory of the
                                       output files
 -p                                    write parse tree to the .p file
 -parallel                             turn on 'paralle generation of
                                       method bodies' feature for
                                       Alchemy
 -removedeadcode                       remove dead code when -optimize
                                       is set
 -sanity                               system-independent error/warning
                                       output -- appropriate for sanity
                                       testing
 -static                               use static semantics
 -strict,--!                           treat undeclared variable and
                                       method access as errors
 -swf <classname,width,height[,fps]>   emit a SWF file
 -use <namespace>                      automatically use a namespace
                                       when compiling this code
 -warnings                             warn on common actionscript
                                       mistakes 