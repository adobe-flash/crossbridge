add_llvm_utility(fpcmp
  fpcmp.cpp
  )

target_link_libraries(fpcmp LLVMSupport)

if( LLVM_ENABLE_THREADS AND HAVE_LIBPTHREAD AND NOT BEOS )
  target_link_libraries(fpcmp pthread)
endif()