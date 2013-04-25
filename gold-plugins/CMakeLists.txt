set(EXPORTED_SYMBOL_FILE $(PROJ_SRC_DIR)/gold.exports)

set(BUILD_SHARED_LIBS TRUE)
set(LINK_LIBS_IN_SHARED TRUE)

set( LLVM_LINK_COMPONENTS
  support
  LTO
  )

include_directories(${BINUTILS_INCDIR})

add_llvm_loadable_module(LLVMgold
  gold-plugin.cpp
)

target_link_libraries(LLVMgold LLVMAnalysis LLVMArchive LLVMARMAsmParser LLVMARMAsmPrinter LLVMARMCodeGen LLVMARMDisassembler LLVMARMInfo LLVMAsmParser LLVMAsmPrinter LLVMAVM2CodeGen LLVMAVM2Info LLVMBitReader LLVMBitWriter LLVMCodeGen LLVMCore LLVMExecutionEngine LLVMInstCombine LLVMInstrumentation LLVMInterpreter LLVMipa LLVMipo LLVMJIT LLVMLinker LLVMMC LLVMMCDisassembler LLVMMCJIT LLVMMCParser LLVMObject LLVMScalarOpts LLVMSelectionDAG LLVMSupport libLLVMCBackend libLLVMCBackendInfo LLVMTarget LLVMTransformUtils LLVMX86AsmParser LLVMX86AsmPrinter LLVMX86CodeGen LLVMX86Disassembler LLVMX86Info LLVMX86Utils LTO)