add_llvm_utility(fpcmp
  fpcmp.cpp
  )

target_link_libraries(fpcmp LLVMSupport)
