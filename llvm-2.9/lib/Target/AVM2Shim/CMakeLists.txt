add_llvm_target(AVM2ShimCodeGen
  AVM2ShimSubtarget.cpp
  AVM2ShimTargetMachine.cpp
  AVM2ShimTargetAsmInfo.cpp
  AVM2ShimFrameLowering.cpp
  AVM2ShimAsmPrinter.cpp
  )

add_subdirectory(TargetInfo)

