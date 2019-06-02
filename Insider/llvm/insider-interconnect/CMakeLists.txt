set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  Option
  Support
  )

add_clang_executable(insider_interconnect
  insider_interconnect.cpp
  )

target_link_libraries(insider_interconnect
  PRIVATE
  clangAST
  clangBasic
  clangDriver
  clangFrontend
  clangRewriteFrontend
  clangStaticAnalyzerFrontend
  clangTooling
  )

install(TARGETS insider_interconnect
  RUNTIME DESTINATION bin)
