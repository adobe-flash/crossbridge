TODO
====

* Apply naming convention across the code base (Alchemy -> FlasCC -> CrossBridge)

* Finish Futures branch LLVM upgrades (C++11)
  * 3.2 have some parts working (on Windows SWIG compilation crashes the LLVM backend driver)
  * 3.3 is patched but not working (code generation fails)
  * 3.4 have some parts patched - needs solution for deprecated Path class (Maybe include from 3.3?! - some commits should be reverted if so...)