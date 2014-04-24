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
* Status: Not started

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
* Status: Not started

## CB-0008

* Description: Set up the clang test suite and add it to our test automation.
* Estimate: 1 day
* Status: Not started

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
* Status: Not started

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
