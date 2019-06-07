#include <set>
#include <sstream>
#include <string>

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory
    STAccelCategory("ST-Accel csim s2s transformer for the interconnect");
std::string topFuncName;
bool catchTopFunc = false;

std::string toString(Stmt *stmt) {
  std::string string_buf;
  llvm::raw_string_ostream ros(string_buf);
  stmt->printPretty(ros, nullptr, PrintingPolicy(LangOptions()));
  return ros.str();
}

class RewritingVisitor : public RecursiveASTVisitor<RewritingVisitor> {
public:
  RewritingVisitor(Rewriter &R, std::string _tbaText)
      : TheRewriter(R), tbaText(_tbaText) {}

  bool VisitFunctionDecl(FunctionDecl *f) {
    if (f->hasBody()) {
      if (f->getNameInfo().getAsString() == topFuncName) {
        DeclarationNameInfo funcName = f->getNameInfo();
        TheRewriter.RemoveText(
            SourceRange(funcName.getBeginLoc(), funcName.getEndLoc()));
        TheRewriter.InsertText(funcName.getBeginLoc(), "main", true, true);
        TheRewriter.RemoveText(f->getReturnTypeSourceRange());
        TheRewriter.InsertText((f->getReturnTypeSourceRange()).getBegin(),
                               "int", true, true);
        Stmt *funcBody = f->getBody();
        SourceLocation locEnd = funcBody->getEndLoc();
        TheRewriter.InsertText(locEnd.getLocWithOffset(-1), tbaText, true,
                               true);
        TheRewriter.InsertText(locEnd.getLocWithOffset(-1),
                               "\nsimulator();while(1);", true, true);
        TheRewriter.InsertText(locEnd.getLocWithOffset(1),
                               "\n\nvoid user_simulation_function() {\n// PUT "
                               "YOUR CODE HERE\n\nwhile(1);\n}",
                               true, true);
      }
    }

    return true;
  }

private:
  Rewriter &TheRewriter;
  std::string tbaText;
};

class InfoExtractionVisitor
    : public RecursiveASTVisitor<InfoExtractionVisitor> {
public:
  InfoExtractionVisitor(Rewriter &R) : TheRewriter(R), threadCnt(0) {}

  bool VisitFunctionDecl(FunctionDecl *f) {
    if (f->hasBody()) {
      if (f->getNameInfo().getAsString() == topFuncName) {
        catchTopFunc = true;
        Stmt *body = f->getBody();
        for (auto child = body->child_begin(); child != body->child_end();
             child++) {
          Stmt *curStmt = *child;
          if (curStmt) {
            if (!strcmp(curStmt->getStmtClassName(), "CallExpr")) {
              CallExpr *callExpr = (CallExpr *)curStmt;
              SourceLocation locStart = callExpr->getBeginLoc();
              SourceLocation locEnd = callExpr->getEndLoc();
              TheRewriter.RemoveText(
                  SourceRange(locStart, locEnd.getLocWithOffset(1)));
              std::string textDeclThread =
                  "std::thread t" + std::to_string(threadCnt++);
              text += textDeclThread;
              std::string funcName = toString(callExpr->getCallee());
              text += "(" + funcName;
              for (unsigned i = 0; i < callExpr->getNumArgs(); i++) {
                std::string curArgText =
                    "std::ref(" + toString(callExpr->getArg(i)) + ")";
                text += ", " + curArgText;
              }
              text += ");\n";
            }
          }
        }
        text += "std::thread t" + std::to_string(threadCnt++) +
                "(host_pcie_simulator);\n";
        text += "std::thread t" + std::to_string(threadCnt++) +
                "(fpga_dramA_simulator);\n";
        text += "std::thread t" + std::to_string(threadCnt++) +
                "(fpga_dramB_simulator);\n";
        text += "std::thread t" + std::to_string(threadCnt++) +
                "(fpga_dramC_simulator);\n";
        text += "std::thread t" + std::to_string(threadCnt++) +
                "(fpga_dramD_simulator);\n";
      }
    }
    return true;
  }

  std::string getText() { return text; }

private:
  Rewriter &TheRewriter;
  std::string text;
  int threadCnt;
};

class MyASTConsumer : public ASTConsumer {
public:
  MyASTConsumer(Rewriter &R) : TheRewriter(R) {}

  bool HandleTopLevelDecl(DeclGroupRef DR) override {
    InfoExtractionVisitor infoExtractionVisitor(TheRewriter);
    for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b) {
      infoExtractionVisitor.TraverseDecl(*b);
    }
    RewritingVisitor rewritingVisitor(TheRewriter,
                                      infoExtractionVisitor.getText());
    for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b) {
      rewritingVisitor.TraverseDecl(*b);
    }
    return true;
  }

private:
  Rewriter &TheRewriter;
};

class MyFrontendAction : public ASTFrontendAction {
public:
  MyFrontendAction() {}
  void EndSourceFileAction() override {
    SourceManager &SM = TheRewriter.getSourceMgr();
    TheRewriter.getEditBuffer(SM.getMainFileID()).write(llvm::outs());
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return llvm::make_unique<MyASTConsumer>(TheRewriter);
  }

private:
  Rewriter TheRewriter;
};

std::string getKernelName(std::string sourceFileName) {
  int lPos = sourceFileName.rfind("/") + 1;
  int rPos = sourceFileName.find(".");
  return sourceFileName.substr(lPos, rPos - lPos);
}

int main(int argc, const char **argv) {
  topFuncName = getKernelName(std::string(argv[1]));
  CommonOptionsParser op(argc, argv, STAccelCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());
  int ret = Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
  if (!catchTopFunc) {
    llvm::errs() << "Error: In file " + std::string(argv[1]) +
                        ": Cannot find the top function!\n";
    return -1;
  }
  return ret;
}
