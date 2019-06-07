#include <set>
#include <map>
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
    STAccelCategory("ST-Accel s2s transformer for kernels");

std::string topFuncName;
bool catchTopFunc = false;

std::set<std::string> rewritingFuncNames;
std::map<std::string, std::string> rewritingNewTextMappings;

class RewritingVisitor : public RecursiveASTVisitor<RewritingVisitor> {
public:
  RewritingVisitor(Rewriter &R)
      : TheRewriter(R) {}

  bool VisitFunctionDecl(FunctionDecl *f) {
    if (f->hasBody()) {
      std::string funcName = f->getNameInfo().getAsString();
      if (rewritingFuncNames.count(funcName)) {
        Stmt *funcBody = f->getBody();
        SourceLocation locStart = funcBody->getBeginLoc();
        SourceLocation locEnd = funcBody->getEndLoc();
        TheRewriter.RemoveText(SourceRange(locStart, locEnd));
        std::string newText = rewritingNewTextMappings[funcName];
        TheRewriter.InsertText(locStart, "{\n" + newText + "}", true, true);
      }
    }

    return true;
  }

private:
  Rewriter &TheRewriter;
};

class InfoExtractionVisitor
    : public RecursiveASTVisitor<InfoExtractionVisitor> {
public:

  void updateFifoNameSet(FunctionDecl *f) {
    funcParamFifoNameSet.clear();
    for (auto param = f->param_begin(); param != f->param_end(); param++) {
      funcParamFifoNameSet.insert((*param)->getName());
    }
  }

  bool VisitFunctionDecl(FunctionDecl *f) {
    if (f->hasBody()) {
      if (f->getNameInfo().getAsString() == topFuncName) {
        catchTopFunc = true;
        rewritingFuncNames.insert(topFuncName);
        updateFifoNameSet(f);
        Stmt *body = f->getBody();
        dfs(body, topFuncName);
      }
    }
    return true;
  }

private:
  std::set<std::string> funcParamFifoNameSet;
  std::map<Stmt *, bool> visited;

  std::string toString(Stmt *stmt) {
    std::string string_buf;
    llvm::raw_string_ostream ros(string_buf);
    stmt->printPretty(ros, nullptr, PrintingPolicy(LangOptions()));
    return ros.str();
  }

  std::string toString(Decl *decl) {
    std::string string_buf;
    llvm::raw_string_ostream ros(string_buf);
    decl->print(ros);
    return ros.str();
  }

  void dfs(Stmt *root, std::string scopedFuncName) {
    if (visited[root]) {
      return;
    }
    visited[root] = true;

    std::string &text = rewritingNewTextMappings[scopedFuncName];
    for (auto iter = root->child_begin(); iter != root->child_end(); iter++) {
      Stmt *curStmt = *iter;

      if (curStmt) {
        if (!strcmp(curStmt->getStmtClassName(), "CompoundStmt")) {
          text += "{";
        } else if (!strcmp(curStmt->getStmtClassName(), "ForStmt")) {
          text += "{";
        } else if (!strcmp(curStmt->getStmtClassName(), "DeclStmt")) {
          DeclStmt *declStmt = (DeclStmt *)curStmt;
          for (auto iter = declStmt->decl_begin(); iter != declStmt->decl_end();
               iter++) {
            Decl *decl = *iter;
            std::string str = toString(decl);
            auto substrLen = str.find('=');
            if (substrLen == std::string::npos) {
              substrLen = str.size();
            }
            str = str.substr(0, substrLen) + ";\n";
            text += str;
          }
        } else if (!strcmp(curStmt->getStmtClassName(), "CXXMemberCallExpr")) {
          CXXMemberCallExpr *cxxMemberCallExpr = (CXXMemberCallExpr *)curStmt;
          std::string callerStr =
              toString(cxxMemberCallExpr->getImplicitObjectArgument());
          if (funcParamFifoNameSet.find(callerStr) !=
              funcParamFifoNameSet.end()) {
            std::string str = toString(cxxMemberCallExpr) + ";\n";
            text += str;
          }
        } else if (!strcmp(curStmt->getStmtClassName(), "CallExpr")) {
          CallExpr *callExpr = (CallExpr *)curStmt;
          Expr **allArgs = callExpr->getArgs();
          for (int i = 0; i < (int)(callExpr->getNumArgs()); i++) {
            Expr *arg = allArgs[i];
            auto argStr = toString(arg);
            if (funcParamFifoNameSet.find(argStr) !=
                funcParamFifoNameSet.end()) {
              std::string str = toString(callExpr) + ";\n";
              text += str;
              const FunctionDecl *directCallee = callExpr->getDirectCallee();
              if (directCallee) {
                std::string directCalleeFuncName =
                    directCallee->getNameInfo().getAsString();
                rewritingFuncNames.insert(directCalleeFuncName);
                dfs(directCallee->getBody(), directCalleeFuncName);
              }
              break;
            }
          }
        }
        dfs(curStmt, scopedFuncName);
        if (!strcmp(curStmt->getStmtClassName(), "CompoundStmt")) {
          text += "}";
        } else if (!strcmp(curStmt->getStmtClassName(), "ForStmt")) {
          text += "}";
        }
      }
    }
  }
};

class InfoExtractionConsumer : public ASTConsumer {
public:
  bool HandleTopLevelDecl(DeclGroupRef DR) override {
    InfoExtractionVisitor infoExtractionVisitor;
    for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b) {
      infoExtractionVisitor.TraverseDecl(*b);
    }
    return true;
  }
};

class InfoExtractionAction : public ASTFrontendAction {
public:
  InfoExtractionAction() {}

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return llvm::make_unique<InfoExtractionConsumer>();
  }

private:
  Rewriter TheRewriter;
};

class RewritingConsumer : public ASTConsumer {
public:
  RewritingConsumer(Rewriter &R) : TheRewriter(R) {}

  bool HandleTopLevelDecl(DeclGroupRef DR) override {
    RewritingVisitor rewritingVisitor(TheRewriter);
    for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b) {
      rewritingVisitor.TraverseDecl(*b);
    }
    return true;
  }

private:
  Rewriter &TheRewriter;
};

class RewritingAction : public ASTFrontendAction {
public:
  RewritingAction() {}

  void EndSourceFileAction() override {
    SourceManager &SM = TheRewriter.getSourceMgr();
    TheRewriter.getEditBuffer(SM.getMainFileID()).write(llvm::outs());
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return llvm::make_unique<RewritingConsumer>(TheRewriter);
  }

private:
  Rewriter TheRewriter;
};

std::string getKernelName(std::string sourceFileName) {
  int lPos = sourceFileName.rfind("/") + 1;
  int rPos = sourceFileName.find(".");
  return sourceFileName.substr(lPos, rPos - lPos);
}

bool isProcessingHeader(std::string topFuncName) {
  return topFuncName.find(".h") != std::string::npos;
}

int main(int argc, const char **argv) {
  std::string fileName = std::string(argv[1]);
  topFuncName = getKernelName(fileName);
  CommonOptionsParser op(argc, argv, STAccelCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());
  int ret = Tool.run(newFrontendActionFactory<InfoExtractionAction>().get());
  if (!catchTopFunc && !isProcessingHeader(fileName)) {
    llvm::errs() << "Error: In file " + std::string(argv[1]) +
                        ": Cannot find the top function: "
                 << topFuncName << "!\n";
    return -1;
  }
  if (ret) {
    return ret;
  }
  ret = Tool.run(newFrontendActionFactory<RewritingAction>().get());
  return ret;
}
