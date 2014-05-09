This page describes the status of the effort to upgrade Crossbridge's compiler infrastructure to LLVM 3.2.

# Tasks and Estimates
* Items in black text represent work that is reasonably well-understood and has been estimated.
* Items in orange represent work that requires further investigation to estimate, but is believed to take less than 5 days.
* Items in red represent work that requires further investigation to estimate, but is expected to take more than 5 days in the worst case.


|Description|Estimate|Status|
|---------------------------|
| *Total* | *28 days* |     |
| Refactor code generator to accommodate changes since LLVM 2.9 | N/A | Complete |
| Move FlasCC-specific gcc options into the clang driver. Hook up plumbing so that clang can create SWFs and shell projectors from C code without the  user needing to explicitly invoke llc, asc, etc. | 2 days | In progress |
| Enhance the AVM2 clang target as necessary. Currently we have a very simple AVM2 clang target, based off of X86, and used by swig. This may be sufficient, but we might also need to improve it to support actual code generation. | 0 - 2 days | *tick* Complete |
| Get the LLVM gold plugin building and ensure that LTO works. | 1 day | In progress |
| Build the rest of the SDK (stdlibs, etc) with clang. Hopefully straightforward, but could be more complex if there's any gcc-specific code in the SDK, or if this work uncovers bugs in the code generator. | 1 - 3 days | Not started |
| Update exception handling for LLVM3.0 style exceptions. | 3 days | Not started |
| Scrub FlasCC-specific modifications to llvm outside of lib/Target/AVM2.Move necessary modifications to llvm-3.2. | 1 day | In progress |
| Ensure llvmtests and relevant submittests are passing. Fix any problems that cause test failures. | 2 days | Not started |
| Set up the clang test suite and add it to our test automation. | 1 day| Not started |
| Ensure gdb tests are passing. Known issues:  local variables are read incorrectly breakpoints on function names are off by one line  "info breakpoints" doesn't always show line number information for functions other than main | 5 days | In progress |
| Refactor swig, as necessary, to build against the new version of clang. Ensure swig tests are passing. | 1 day | Not started |
| Switch the automated build over to llvm-3.2 and clang. Remove all llvm-2.9 and llvm-gcc specific code. | 1 day | Not started |
| Find bugs, fix bugs, and address any unforeseen problems. | 10 days | Not started |

# Dev Notes
These notes are intended to be a rough outline of the steps necessary to add the AVM2 target to the LLVM source tree and to troubleshoot any problems that might occur.

## Adding AVM2 to the LLVM build system
### CMakeLists.txt
# Add AVM2 to LLVM_ALL_TARGETS:
```
set(LLVM_ALL_TARGETS
  ARM
  AVM2
  CellSPU
  CppBackend
  Hexagon
  Mips
  MBlaze
  MSP430
  PowerPC
  PTX
  Sparc
  X86
  XCore
  )
```
# Add the {{avm2_env/misc}} directory to the include path:
```
include_directories( ${LLVM_BINARY_DIR}/include ${LLVM_MAIN_INCLUDE_DIR} 
    ${LLVM_MAIN_INCLUDE_DIR}/../../avm2_env/misc/ )
```
# Add avm2 to {{llvm::Triple}} in {{llvm/include/llvm/ADT/Triple.h}}:
```
enum ArchType {
    UnknownArch,

    arm,     // ARM; arm, armv.*, xscale
    avm2,    // Actionscript VM 2
```
# Also add avm2 to the appropriate functions in {{llvm-3.2/lib/Support/Triple.cpp}}.

### LLVMBuild
# Edit {{llvm/lib/Target/LLVMBuild.txt}} to include a reference to AVM2:
```
[common]
subdirectories = ARM AVM2 CellSPU CppBackend Hexagon MBlaze MSP430 NVPTX Mips PowerPC Sparc X86 XCore
```
# Make sure that each subdirectory in {{llvm/lib/Target/AVM2}} has an LLVMBuild.txt file.

## Running the build
While troublehsooting the build, it can be helpful to run a single threaded
build. This will slow down the build, but it will also make the build output
much easier to understand and debug. To use a single threaded build, edit
the top level Crossbridge Makefile so that it doesn't invoke make with the -j
option when building LLVM. You should not check in this change.

Rather than running a full build every time, you can use an incremental build.
To do this, run the LLVM build once, and then cd to the LLVM build directory.
Use the {{make}} command to run the build as necessary until it completes
successfully. Note that you can't run {{make}} until you've fixed any problems
reported by {{cmake}}. You can know that {{cmake}} has finished successfully if you see
this output in the build log:
```
-- Build files have been written to: /path/to/your/code/build/mac/llvm-debug
```

## Fixing errors
# tablegen changes can cause strange errors. For example:
```
Scanning dependencies of target LLVMAVM2AsmParser
make[3]: *** No rule to make target `lib/Target/AVM2/-gen-subtarget', needed by `lib/Target/AVM2/AsmParser/CMakeFiles/LLVMAVM2AsmParser.dir/AVM2AsmParser.cpp.o'.  Stop.
```
To fix errors like this, compare the contents of the tablegen file you suspect is 
causing the error to the corresponding file in another target, like X86. Pay special
attention to differences in the argument counts or ordering.
# Other changes may cause compile or link errors, which you should debug
and fix as appropriate. It is often useful to find which change in LLVM's
svn tree caused a specific error. These changes often include the modifications
that LLVM engineers made to other targets to accommodate the changes, which
can be very helpful in determining how the AVM2 target should change. You can
browse the LLVM svn tree here:
[https://llvm.org/viewvc/llvm-project/]

## Core LLVM Changes
Generally our modifications to LLVM have been restricted to the AVM2 backend, 
which can be found at llvm/lib/Target/AVM2. But occasionally we've needed to
change LLVM code outside of that directory. These changes must be integrated
with the LLVM tree during an upgrade.

This is a list of the changed files based on the current state of the llvm-2.9 tree.
You can generate this list with the diff command:
```
diff -Bbrq path/to/our/llvm path/to/stock/llvm
```

| File name | Integrated into LLVM-3.2? |
|---------------------------------------|
|  llvm-2.9/CMakeLists.txt  | *tick* |
|  llvm-2.9/cmake/config-ix.cmake  | *tick* |
|  llvm-2.9/cmake/modules/AddLLVM.cmake  | *tick* |
|  llvm-2.9/cmake/modules/HandleLLVMOptions.cmake | *tick* |
|  llvm-2.9/cmake/modules/LLVMConfig.cmake  | *tick* |
|  llvm-2.9/cmake/modules/LLVMLibDeps.cmake  | (x) |
|  llvm-2.9/cmake/modules/LLVMProcessSources.cmake  | *tick* |
|  llvm-2.9/configure  | (x) |
|  llvm-2.9/include/llvm/ADT/Triple.h  |*tick* |
|  llvm-2.9/include/llvm/CodeGen/AsmPrinter.h  |*tick* |
|  llvm-2.9/include/llvm/CodeGen/CallingConvLower.h  |*tick* |
|  llvm-2.9/include/llvm/CodeGen/Passes.h  |*tick* |
|  llvm-2.9/include/llvm/InitializePasses.h  |*tick* |
|  llvm-2.9/include/llvm/LinkAllPasses.h  |*tick* |
|  llvm-2.9/include/llvm/MC/MCStreamer.h  |*tick* |
|  llvm-2.9/include/llvm/Support/CommandLine.h | (x) No longer needed as of LLVM revision 165535 |
|  llvm-2.9/include/llvm/Target/TargetMachine.h  | (x) Reg Alloc removed via AVM2PassConfig |
|  llvm-2.9/include/llvm/Transforms/Scalar.h  |*tick* |
|  llvm-2.9/lib/Analysis/DbgInfoPrinter.cpp  | (x) No longer needed as of LLVM revision 140281 |
|  llvm-2.9/lib/Archive/ArchiveReader.cpp  | |
|  llvm-2.9/lib/CodeGen/AsmPrinter/AsmPrinterInlineAsm.cpp  |*tick* |
|  llvm-2.9/lib/CodeGen/CMakeLists.txt  |*tick* |
|  llvm-2.9/lib/CodeGen/CallingConvLower.cpp  |*tick* |
|  Only in llvm-2.9/lib/CodeGen: ConflictGraph.cpp | *tick*|
|  Only in llvm-2.9/lib/CodeGen: ConflictGraph.h |*tick* |
|  llvm-2.9/lib/CodeGen/LLVMTargetMachine.cpp  | (x) Reg Alloc removed via AVM2PassConfig |
|  llvm-2.9/lib/CodeGen/SelectionDAG/DAGCombiner.cpp  |*tick* |
|  Only in llvm-2.9/lib/CodeGen: VirtRegReduction.cpp |*tick* |
|  llvm-2.9/lib/Linker/LinkModules.cpp  | |
|  llvm-2.9/lib/MC/MCStreamer.cpp  |*tick* |
|  llvm-2.9/lib/Support/Path.cpp  |*tick* |
|  llvm-2.9/lib/Support/Triple.cpp  | *tick*|
|  llvm-2.9/lib/Support/Unix/Path.inc  |*tick* |
|  llvm-2.9/lib/Support/Windows/Path.inc  |*tick* |
|  llvm-2.9/lib/Support/Windows/explicit_symbols.inc  | *tick*|
|  llvm-2.9/lib/Transforms/Hello/CMakeLists.txt  |*tick* |
|  llvm-2.9/lib/Transforms/Scalar/CMakeLists.txt  |*tick* |
|  Only in llvm-2.9/lib/Transforms/Scalar: Phi2Mem.cpp |*tick* |
|  llvm-2.9/lib/Transforms/Scalar/Scalar.cpp  |*tick* |
|  llvm-2.9/tools/CMakeLists.txt  |*tick* |
|  llvm-2.9/tools/bugpoint-passes/CMakeLists.txt  | *tick*|
|  Only in llvm-2.9/tools/gold: CMakeLists.txt |*tick* |
|  llvm-2.9/tools/gold/gold-plugin.cpp  |*tick* |
|  llvm-2.9/tools/llc/llc.cpp  |*tick* |
|  llvm-2.9/tools/llvm-config/CMakeLists.txt  | (x) not sure if change is still needed |
|  llvm-2.9/tools/llvm-ld/llvm-ld.cpp  | (x) llvm-ld has been removed |
|  Only in llvm-2.9/tools/lto: CMakeLists.txt | *tick*|
|  llvm-2.9/tools/lto/LTOCodeGenerator.cpp  |*tick* |
|  llvm-2.9/tools/lto/LTOModule.cpp  |*tick* |
|  Only in llvm-2.9/utils/fpcmp: CMakeLists.txt |*tick* |

## Current Status
This work is unfinished and mostly unstable. Some changes are questionable and were intended to be temporary. These changes can be found by grepping the source code for the string "TODO MCANNIZZ". Such changes should be reviewed and, if necessary, modified before considering the llvm-3.2 toolchain stable.
