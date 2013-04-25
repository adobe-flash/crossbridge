set(LLVM_TARGET_DEFINITIONS AVM2.td)

tablegen(AVM2GenRegisterInfo.h.inc -gen-register-desc-header)
tablegen(AVM2GenRegisterNames.inc -gen-register-enums)
tablegen(AVM2GenRegisterInfo.inc -gen-register-desc)
tablegen(AVM2GenInstrNames.inc -gen-instr-enums)
tablegen(AVM2GenInstrInfo.inc -gen-instr-desc)
tablegen(AVM2GenAsmWriter.inc -gen-asm-writer)
tablegen(AVM2GenDAGISel.inc -gen-dag-isel)
tablegen(AVM2GenCallingConv.inc -gen-callingconv)
tablegen(AVM2GenSubtarget.inc -gen-subtarget)

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
