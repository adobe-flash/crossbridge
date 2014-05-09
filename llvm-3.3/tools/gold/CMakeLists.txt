set(MODULE TRUE)

set(EXPORTED_SYMBOL_FILE $(PROJ_SRC_DIR)/gold.exports)

set( LLVM_LINK_COMPONENTS
  support
  lto
)

include_directories(${LLVM_BINUTILS_INCDIR})

add_llvm_loadable_module(LLVMgold
  gold-plugin.cpp
)


target_link_libraries(LLVMgold LTO LLVMAnalysis LLVMArchive LLVMAsmParser LLVMAsmPrinter LLVMBitReader LLVMBitWriter LLVMCodeGen LLVMExecutionEngine LLVMInstCombine LLVMInstrumentation LLVMInterpreter LLVMipa LLVMipo LLVMJIT LLVMLinker LLVMMC LLVMMCDisassembler LLVMMCJIT LLVMObject LLVMOption LLVMScalarOpts LLVMSelectionDAG LLVMSupport LLVMTarget LLVMTransformUtils LLVMX86AsmParser LLVMX86AsmPrinter LLVMX86CodeGen LLVMX86Disassembler LLVMX86Info LLVMX86Utils LLVMAVM2AsmParser LLVMAVM2Info LLVMAVM2CodeGen)
if(MINGW)
  target_link_libraries(LLVMgold -lrpcrt4)
endif()

if(CYGWIN OR LLVM_CYGWINMAC)
   target_link_libraries(LLVMgold -luuid)
endif()

if(CMAKE_HOST_UNIX)
    if (NOT APPLE)
        target_link_libraries(LLVMgold -luuid)
    endif()
endif()



