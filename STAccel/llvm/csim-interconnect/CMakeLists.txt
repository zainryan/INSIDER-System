set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  Option
  Support
  )

add_clang_executable(staccel_csim_interconnect
  staccel_csim_interconnect.cpp
  )

target_link_libraries(staccel_csim_interconnect
  PRIVATE
  clangAST
  clangBasic
  clangDriver
  clangFrontend
  clangRewriteFrontend
  clangStaticAnalyzerFrontend
  clangTooling
  )

install(TARGETS staccel_csim_interconnect
  RUNTIME DESTINATION bin)
