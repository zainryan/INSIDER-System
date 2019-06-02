set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  Option
  Support
  )

add_clang_executable(staccel_s2s_kernel
  staccel_s2s_kernel.cpp
  )

target_link_libraries(staccel_s2s_kernel
  PRIVATE
  clangAST
  clangBasic
  clangDriver
  clangFrontend
  clangRewriteFrontend
  clangStaticAnalyzerFrontend
  clangTooling
  )

install(TARGETS staccel_s2s_kernel
  RUNTIME DESTINATION bin)
