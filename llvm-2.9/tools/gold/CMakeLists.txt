set(MODULE TRUE)

set(EXPORTED_SYMBOL_FILE $(PROJ_SRC_DIR)/gold.exports)

set( LLVM_LINK_COMPONENTS
  support
  LTO
)

include_directories(${BINUTILS_INCDIR})

add_llvm_loadable_module(LLVMgold
  gold-plugin.cpp
)


llvm_target_link_libraries(LLVMgold
LLVMAnalysis LLVMArchive LLVMAsmParser LLVMAsmPrinter LLVMAVM2ShimCodeGen LLVMAVM2ShimInfo LLVMBitReader LLVMBitWriter LLVMCodeGen LLVMCore LLVMExecutionEngine LLVMInstCombine LLVMInstrumentation LLVMInterpreter LLVMipa LLVMipo LLVMJIT LLVMLinker LLVMMC LLVMMCDisassembler LLVMMCJIT LLVMCBackend LLVMCBackendInfo LLVMMCParser LLVMObject LLVMScalarOpts LLVMSelectionDAG LLVMSupport LLVMTarget LLVMTransformUtils LLVMX86AsmParser LLVMX86AsmPrinter LLVMX86CodeGen LLVMX86Disassembler LLVMX86Info LLVMX86Utils LTO)
if(MINGW)
  llvm_target_link_libraries(LLVMgold -lrpcrt4)
endif()

if(CYGWIN OR LLVM_CYGWINMAC)
  llvm_target_link_libraries(LLVMgold -luuid)
endif()

