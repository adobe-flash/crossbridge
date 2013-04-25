add_llvm_library( BugpointPasses
  TestPasses.cpp
  )

add_dependencies(BugpointPasses bugpoint)
