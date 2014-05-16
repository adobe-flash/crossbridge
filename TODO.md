TODO
====

* Solve OSX issues using the wrong compiler (Issue #3)

* Finish Tamarin upgrading to rev. 5571cf86fc68

* Finish Futures branch LLVM upgrades (C++11)
  * 3.2 have some parts working (on Windows SWIG compilation crashes the LLVM backend driver)
  * 3.3 is patched but not working (code generation fails)
  * 3.4 have some parts patched - needs solution for deprecated Path class (Maybe include from 3.3?! - some commits should be reverted if so...)

* Apply naming convention across the code base (Alchemy -> FlasCC -> CrossBridge)

* Solve Linux build issues: [LOG](https://s3.amazonaws.com/archive.travis-ci.org/jobs/24450137/log.txt)

* Try to get missing 'qa/gdbunit/build.xml' build script

* Use Adobe AIR SDK ASC2 if available (Building the SDK and Projects)

* Improve test coverage and documentation

* Merge GLSL2AGAL into the tool-chain