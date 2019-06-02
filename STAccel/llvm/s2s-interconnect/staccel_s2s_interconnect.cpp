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
    STAccelCategory("ST-Accel s2s transformer for the interconnect");
std::string topFuncName;
bool catchTopFunc = false;

class RewritingVisitor : public RecursiveASTVisitor<RewritingVisitor> {
public:
  RewritingVisitor(Rewriter &R, std::string _tbaText)
      : TheRewriter(R), tbaText(_tbaText) {}

  bool VisitFunctionDecl(FunctionDecl *f) {
    if (f->hasBody()) {
      if (f->getNameInfo().getAsString() == topFuncName) {
        Stmt *funcBody = f->getBody();
        SourceLocation locStart = funcBody->getBeginLoc();
        SourceLocation locEnd = funcBody->getEndLoc();
        TheRewriter.RemoveText(SourceRange(locStart, locEnd));
        TheRewriter.InsertText(locStart, "{\n" + tbaText + "}", true, true);
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
private:
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

  int numChild(DeclStmt *declStmt) {
    int num = 0;
    for (auto iter = declStmt->child_begin(); iter != declStmt->child_end();
         iter++, num++)
      ;
    return num;
  }

  void dfs(Stmt *root, ASTContext &astContext) {
    for (auto iter = root->child_begin(); iter != root->child_end(); iter++) {
      Stmt *curStmt = *iter;
      if (curStmt) {
        if (!strcmp(curStmt->getStmtClassName(), "DeclStmt")) {
          DeclStmt *declStmt = (DeclStmt *)curStmt;
          if (declStmt->isSingleDecl()) {
            Decl *decl = declStmt->getSingleDecl();
            if (numChild(declStmt) == 1) {
              Stmt *child = *(declStmt->child_begin());
              if (!strcmp(child->getStmtClassName(), "CXXConstructExpr")) {
                CXXConstructExpr *cxxConstructExpr = (CXXConstructExpr *)child;
                std::string constructorName = cxxConstructExpr->getConstructor()
                                                  ->getNameInfo()
                                                  .getAsString();
                if (constructorName == "ST_Queue") {
                  Expr *arg = cxxConstructExpr->getArg(0);
                  llvm::APSInt result;
                  if (arg->EvaluateAsInt(result, astContext)) {
                    std::string depthText = result.toString(10);
                    ValueDecl *valueDecl = (ValueDecl *)decl;
                    std::string fifoTypeText =
                        valueDecl->getType().getAsString();
                    std::string fifoNameText = valueDecl->getName();
                    fifoText += fifoTypeText + " " + fifoNameText + ";\n";
                    fifoText += "#pragma HLS stream variable=" + fifoNameText +
                                " depth=" + depthText + "\n";
                  } else {
                    llvm::errs()
                        << "The depth of queue should be known statically!\n";
                    exit(-1);
                  }
                }
              }
            }
          }
        }
      }
      dfs(curStmt, astContext);
    }
  }

  std::string getInterconnectText() {
    return "while (1) {\n #pragma HLS dataflow\n" + interconnectText + "}";
  }

public:
  InfoExtractionVisitor(Rewriter &R) : TheRewriter(R) {}

  bool VisitFunctionDecl(FunctionDecl *f) {
    if (f->hasBody()) {
      if (f->getNameInfo().getAsString() == topFuncName) {
        catchTopFunc = true;
        Stmt *body = f->getBody();
        dfs(body, f->getASTContext());
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
              interconnectText += toString(callExpr) + ";\n";
            }
          }
        }
      }
    }
    return true;
  }

  std::string getText() { return fifoText + getInterconnectText(); }

private:
  Rewriter &TheRewriter;
  std::string interconnectText;
  std::string fifoText;
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
