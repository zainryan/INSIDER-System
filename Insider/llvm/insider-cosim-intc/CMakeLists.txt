set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  Option
  Support
  )

add_clang_executable(insider_cosim_intc
  insider_cosim_intc.cpp
  )

target_link_libraries(insider_cosim_intc
  PRIVATE
  clangAST
  clangBasic
  clangDriver
  clangFrontend
  clangRewriteFrontend
  clangStaticAnalyzerFrontend
  clangTooling
  )

install(TARGETS insider_cosim_intc
  RUNTIME DESTINATION bin)
