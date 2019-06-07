set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  Option
  Support
  )

add_clang_executable(staccel_s2s_interconnect
  staccel_s2s_interconnect.cpp
  )

target_link_libraries(staccel_s2s_interconnect
  PRIVATE
  clangAST
  clangBasic
  clangDriver
  clangFrontend
  clangRewriteFrontend
  clangStaticAnalyzerFrontend
  clangTooling
  )

install(TARGETS staccel_s2s_interconnect
  RUNTIME DESTINATION bin)
