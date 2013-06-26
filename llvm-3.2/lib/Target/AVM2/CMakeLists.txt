set(LLVM_TARGET_DEFINITIONS AVM2.td)

tablegen(LLVM AVM2GenRegisterInfo.h.inc -gen-register-info)
tablegen(LLVM AVM2GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM AVM2GenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM AVM2GenDAGISel.inc -gen-dag-isel)
tablegen(LLVM AVM2GenCallingConv.inc -gen-callingconv)
tablegen(LLVM AVM2GenSubtargetInfo.inc -gen-subtarget)
add_public_tablegen_target(AVM2CommonTableGen)

add_llvm_target(AVM2CodeGen
  AVM2AsmPrinter.cpp
  AVM2MCAsmStreamer.cpp
  AVM2FrameLowering.cpp
  AVM2ISelLowering.cpp
  AVM2ISelDAGToDAG.cpp	
  AVM2InstrInfo.cpp	
  AVM2RegisterInfo.cpp	
  AVM2Subtarget.cpp	
  AVM2TargetAsmInfo.cpp	
  AVM2TargetMachine.cpp
  AVM2SelectionDAGInfo.cpp
  AVM2MCCodeEmitter.cpp
  AVM2AsmBackend.cpp
  )

add_subdirectory(TargetInfo)
add_subdirectory(AsmParser)
