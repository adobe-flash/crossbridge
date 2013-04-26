include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/.. )

add_llvm_library(LLVMAVM2Info
  AVM2TargetInfo.cpp
  )

add_dependencies(LLVMAVM2Info AVM2CodeGenTable_gen AVM2CommonTableGen)
