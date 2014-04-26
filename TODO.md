# TODO

## CB-0001

* Description: Move FlasCC-specific gcc options into the clang driver. Hook up plumbing so that clang can create SWFs and shell projectors from C code without the  user needing to explicitly invoke llc, asc, etc.
* Estimate: 2 days
* Status: In progress

## CB-0002

* Description: Enhance the AVM2 clang target as necessary. Currently we have a very simple AVM2 clang target, based off of X86, and used by swig. This may be sufficient, but we might also need to improve it to support actual code generation.
* Estimate: 0 - 2 days
* Status: *tick* Complete

## CB-0003

* Description: Get the LLVM gold plugin building and ensure that LTO works.
* Estimate: 1 day
* Status: In progress

## CB-0004

* Description: Build the rest of the SDK (stdlibs, etc) with clang. Hopefully straightforward, but could be more complex if there's any gcc-specific code in the SDK, or if this work uncovers bugs in the code generator.
* Estimate: 1 - 3 days
* Status: In progress

## CB-0005

* Description: Update exception handling for LLVM3.0 style exceptions.
* Estimate: 3 days
* Status: Not started

## CB-0006

* Description: Scrub FlasCC-specific modifications to llvm outside of lib/Target/AVM2.Move necessary modifications to llvm-3.2.
* Estimate: 1 day
* Status: In progress

## CB-0007

* Description: Ensure llvmtests and relevant submittests are passing. Fix any problems that cause test failures.
* Estimate: 2 days
* Status: In progress

## CB-0008

* Description: Set up the clang test suite and add it to our test automation.
* Estimate: 1 day
* Status: In progress

## CB-0009

* Description: Ensure gdb tests are passing. Known issues:  local variables are read incorrectly breakpoints on function names are off by one line  "info breakpoints" doesn't always show line number information for functions other than main
* Estimate: 5 days
* Status: In progress

## CB-0010

* Description: Refactor swig, as necessary, to build against the new version of clang. Ensure swig tests are passing.
* Estimate: 1 day
* Status: Not started

## CB-0011

* Description: Switch the automated build over to llvm-3.2 and clang. Remove all llvm-2.9 and llvm-gcc specific code.
* Estimate: 1 day
* Status: In progress

## CB-0012

* Description: Find bugs, fix bugs, and address any unforeseen problems.
* Estimate: 10 days
* Status: Not started

## CB-0013

* Description: Remove references to 'llvm-gcc-4.2-2.9' should be removed ?!
* Estimate: ? days
* Status: Not started
* Relates to CB-0011

    \avm2_env\usr\src\contrib\libc++\Makefile
    
    \avm2_env\usr\src\lib\libc++\Makefile
    
    \avm2_env\usr\src\lib\libc++abi\Makefile

## CB-0014
* Description: Allow building on Linux and Windows
* Estimate: 7 days
* Status: In progress

## CB-0015
* Description: Cannot build SWIG on windows neither with GCC or Clang
* Estimate: 1 days
* Status: In progress
* Relates to CB-0010
* Exception:
  * GCC
  
> /cygdrive/f/crossbridge/build/win/llvm-debug/lib/libLLVMAVM2CodeGen.a(AVM2AsmPrinter.cpp.o):AVM2AsmPrinter.cpp:(.text+0x9b5): undefined reference to `uuid_generate'

> /cygdrive/f/crossbridge/build/win/llvm-debug/lib/libLLVMAVM2CodeGen.a(AVM2AsmPrinter.cpp.o):AVM2AsmPrinter.cpp:(.text+0xa10): undefined reference to `uuid_unparse'

> /usr/lib/gcc/i686-pc-cygwin/4.8.2/../../../../i686-pc-cygwin/bin/ld: /cygdrive/f/crossbridge/build/win/llvm-debug/lib/libLLVMAVM2CodeGen.a(AVM2AsmPrinter.cpp.o): bad reloc address 0x84 in section `.rdata'
  
  * Clang
  
> mv -f $depbase.Tpo $depbase.Podepbase=`echo Modules/as3.o | sed 's|[^/]*$|.deps/&|;s|\.o$||'`;\

> clang++ -DHAVE_CONFIG_H   -I../Source/Include -I../Source/CParse -I/cygdrive/f/crossbridge/swig-2.0.4/Source/Include\

> -I/cygdrive/f/crossbridge/swig-2.0.4/Source/DOH -I/cygdrive/f/crossbridge/swig-2.0.4/Source/CParse\

> -I/cygdrive/f/crossbridge/swig-2.0.4/Source/Preprocessor -I/cygdrive/f/crossbridge/swig-2.0.4/Source/Swig\

> -I/cygdrive/f/crossbridge/swig-2.0.4/Source/Modules -I/cygdrive/f/crossbridge/build/win/swig/pcre/pcre-swig-install/include\

> -DPCRE_STATIC  -I/cygdrive/f/crossbridge/avm2_env/misc/ -I/cygdrive/f/crossbridge/llvm-3.2/include\

> -I/cygdrive/f/crossbridge/build/win/llvm-debug/include-I/cygdrive/f/crossbridge/llvm-3.2/tools/clang/include\

> -I/cygdrive/f/crossbridge/build/win/llvm-debug/tools/clang/include -I/cygdrive/f/crossbridge/llvm-3.2/tools/clang/lib\

> -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS -fno-rtti -g -Wno-long-long  -MT Modules/as3.o -MD -MP -MF $depbase.Tpo\

> -c -o Modules/as3.o /cygdrive/f/crossbridge/swig-2.0.4/Source/Modules/as3.cxx &&\

> mv -f $depbase.Tpo $depbase.Po

> Stack dump:

> 0.      Program arguments: /cygdrive/f/crossbridge/sdk/usr/platform/cygwin/bin/clang++ -cc1 -triple avm2-unknown-freebsd8 -S -disable-free -disable-llvm-verifier -main-file-name as3.cxx -mrelocation-model static -mdisable-fp-elim -fmath-errno -momit-leaf-frame-pointer -g -coverage-file /tmp/as3-hmEgHk.s -resource-dir /cygdrive/f/crossbridge/sdk/usr/platform/cygwin/bin/../lib/clang/3.2 -dependency-file Modules/.deps/as3.Tpo -sys-header-deps -MP -MT Modules/as3.o -D HAVE_CONFIG_H -D PCRE_STATIC -D __STDC_LIMIT_MACROS -D __STDC_CONSTANT_MACROS -I ../Source/Include -I ../Source/CParse -I /cygdrive/f/crossbridge/swig-2.0.4/Source/Include -I /cygdrive/f/crossbridge/swig-2.0.4/Source/DOH -I /cygdrive/f/crossbridge/swig-2.0.4/Source/CParse -I /cygdrive/f/crossbridge/swig-2.0.4/Source/Preprocessor-I /cygdrive/f/crossbridge/swig-2.0.4/Source/Swig -I /cygdrive/f/crossbridge/swig-2.0.4/Source/Modules -I /cygdrive/f/crossbridge/build/win/swig/pcre/pcre-swig-install/include -I /cygdrive/f/crossbridge/avm2_env/misc/ -I /cygdrive/f/crossbridge/llvm-3.2/include -I /cygdrive/f/crossbridge/build/win/llvm-debug/include -I /cygdrive/f/crossbridge/llvm-3.2/tools/clang/include -I /cygdrive/f/crossbridge/build/win/llvm-debug/tools/clang/include -I /cygdrive/f/crossbridge/llvm-3.2/tools/clang/lib -isysroot /cygdrive/f/crossbridge/sdk/usr/bin/../.. -fmodule-cache-path /var/tmp/clang-module-cache -Wno-long-long -fdeprecated-macro -fno-dwarf-directory-asm -fdebug-compilation-dir /cygdrive/f/crossbridge/build/win/swig/Source -ferror-limit 19 -fmessage-length 100 -mstackrealign -fno-rtti -fobjc-runtime=gcc -fcxx-exceptions -fexceptions -fsjlj-exceptions -fdiagnostics-show-option -fcolor-diagnostics -o /tmp/as3-hmEgHk.s -x c++ /cygdrive/f/crossbridge/swig-2.0.4/Source/Modules/as3.cxx

> 1.      <eof> parser at end of file

> 2.      Code generation

> 3.      Running pass 'Function Pass Manager' on module '/cygdrive/f/crossbridge/swig-2.0.4/Source/Modules/as3.cxx'.

> Error: Failed to open /tmp/as3-hmEgHk.s for input

> clang++: error: assembler command failed with exit code 255 (use -v to see invocation)

## CB-0016
* Description: Cannot build LLVMTests
* Estimate: 1 days
* Status: In progress
* Relates to CB-0010
* Exception:

> /cygdrive/f/crossbridge/llvm-3.2/projects/test-suite/RunSafely.sh: line 129: /cygdrive/f/crossbridge/build/win/llvm-tests/projects/test-suite/tools/timeit-target: No such file or directory

> grep: Output/burg.out-nat.time: No such file or directory

> TEST Output/burg.native FAILED: CAN'T GET EXIT CODE!

> grep "^user" Output/burg.out-nat.time >> Output/burg.nightly.nat.report.txt

> grep: Output/burg.out-nat.time: No such file or directory

> /cygdrive/f/crossbridge/llvm-3.2/projects/test-suite/TEST.nightly.Makefile:51: recipe for target 'Output/burg.nightly.nat.report.txt' failed

> make[3]: [Output/burg.nightly.nat.report.txt] Error 2 (ignored)

> make[3]: *** No rule to make target 'Output/be.bc', needed by 'Output/burg.linked.rbc'.  Stop.

> make[3]: Leaving directory '/cygdrive/f/crossbridge/build/win/llvm-tests/projects/test-suite/MultiSource/Applications/Burg'

> /cygdrive/f/crossbridge/llvm-3.2/projects/test-suite/Makefile.rules:568: recipe for target 'Burg/.maketest' failed

> make[2]: *** [Burg/.maketest] Error 2

> make[2]: Leaving directory '/cygdrive/f/crossbridge/build/win/llvm-tests/projects/test-suite/MultiSource/Applications'

> /cygdrive/f/crossbridge/llvm-3.2/projects/test-suite/Makefile.rules:568: recipe for target 'Applications/.maketest' failed

> make[1]: *** [Applications/.maketest] Error 2

> make[1]: Leaving directory '/cygdrive/f/crossbridge/build/win/llvm-tests/projects/test-suite/MultiSource'

> Makefile:599: recipe for target 'llvmtests' failed

> make: *** [llvmtests] Error 2

## CB-0017
* Description: Cannot build lib DMalloc
* Estimate: 1 days
* Status: In progress
* Relates to CB-0004
* Exception: Cannot run test program while cross compiling 

## CB-0018
* Description: Cannot build lib SDL
* Estimate: 1 days
* Status: In progress
* Relates to CB-0004
* Exception: TBD

## CB-0019
* Description: Cannot build lib FFI
* Estimate: 1 days
* Status: In progress
* Relates to CB-0004
* Exception: FFI_TRAMPOLINE_SIZE undeclared here
