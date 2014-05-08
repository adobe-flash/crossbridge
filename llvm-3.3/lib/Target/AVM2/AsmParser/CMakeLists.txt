include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/.. )

add_llvm_library(LLVMAVM2AsmParser
  AVM2AsmParser.cpp
)
add_dependencies(LLVMAVM2AsmParser AVM2CodeGenTable_gen AVM2CommonTableGen)
