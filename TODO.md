TODO
====

* Improve OSX compatibility (10.X)
  * /crossbridge/llvm-2.9/lib/CodeGen/ConflictGraph.h:26:10: fatal error: 'tr1/unordered_map' file not found (Using Clang 5.1.0)

* Improve Linux compatibility of the makefiles and sources
  * Fix LLVM-GCC error: "LLVMgold.so: undefined symbol: _ZN4llvm10MCStreamer12GetCommentOSEv" [LOG](https://s3.amazonaws.com/archive.travis-ci.org/jobs/24382789/log.txt)

* Improve MingW+MSys compatibility of the makefiles and sources
  * Cannot build core tool (GNU Make) using MingW32+MSys - maybe patched distribution works...

* Improve pthreads support

* Improve performance

* Improve documentation

* Add more examples, tests and libs

* Fix project naming convention across sources (Alchemy -> FlasCC -> CrossBridge)

* Contact someone at Adobe about merging the fork