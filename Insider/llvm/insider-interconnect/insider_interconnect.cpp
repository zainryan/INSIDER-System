#include <cstdlib>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

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
    STAccelCategory("Insider s2s transformer for the interconnect");

std::string topFuncName;
bool catchTopFunc = false;
std::string itc_template_str;
std::string header_template_str;
std::vector<std::string> kernelNameVec;

class RewritingVisitor : public RecursiveASTVisitor<RewritingVisitor> {
public:
  RewritingVisitor(Rewriter &R, std::string &_itc_template_str,
                   std::string &_header_template_str,
                   std::string &_appCallExprText)
      : TheRewriter(R), itc_template_str(_itc_template_str),
        header_template_str(_header_template_str),
        appCallExprText(_appCallExprText) {}

  bool VisitFunctionDecl(FunctionDecl *f) {
    if (f->hasBody()) {
      if (f->getNameInfo().getAsString() == topFuncName) {
        TheRewriter.InsertText(f->getBeginLoc(), header_template_str + "\n");
        Stmt *funcBody = f->getBody();
        Stmt *firstChild = *(funcBody->child_begin());
        std::string itcText = itc_template_str + "\n";
        for (auto kernelName : kernelNameVec) {
          itcText += "ST_Queue<bool> reset_" + kernelName + "(4);\n";
        }
        itcText += "reset_propaganda(";
        for (auto kernelName : kernelNameVec) {
          itcText += "reset_" + kernelName + ", ";
        }

        itcText +=
            "reset_sigs, reset_read_mode_dram_helper_app, "
            "reset_write_mode_dram_helper_app, "
            "reset_read_mode_pcie_helper_app, "
            "reset_write_mode_pcie_helper_app, reset_pcie_data_splitter_app, "
            "reset_app_output_data_demux, reset_app_input_data_mux, "
            "reset_write_mode_app_output_data_caching, "
            "reset_app_input_data_merger, "
            "reset_write_mode_pre_merged_app_input_data_forwarder);\n\n";
        TheRewriter.InsertText(firstChild->getBeginLoc(), itcText);
        TheRewriter.InsertText(funcBody->getEndLoc().getLocWithOffset(-1),
                               appCallExprText + "\n");
      }
    }

    return true;
  }

private:
  Rewriter &TheRewriter;
  std::string &itc_template_str;
  std::string &header_template_str;
  std::string &appCallExprText;
};

class InfoExtractionVisitor
    : public RecursiveASTVisitor<InfoExtractionVisitor> {
public:
  InfoExtractionVisitor(Rewriter &R) : TheRewriter(R) {}

  bool VisitFunctionDecl(FunctionDecl *f) {
    if (f->hasBody()) {
      if (f->getNameInfo().getAsString() == topFuncName) {
        catchTopFunc = true;
        Stmt *body = f->getBody();
        dfs(body);
      }
    }
    return true;
  }

  std::string &getAppCallExprText() { return appCallExprText; }

private:
  Rewriter &TheRewriter;
  std::string appCallExprText;

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

  void dfs(Stmt *root) {
    for (auto iter = root->child_begin(); iter != root->child_end(); iter++) {
      Stmt *curStmt = *iter;
      if (curStmt) {
        if (!strcmp(curStmt->getStmtClassName(), "CallExpr")) {
          CallExpr *callExpr = (CallExpr *)curStmt;
          std::string origCallExprText = toString(callExpr);
          TheRewriter.RemoveText(
              SourceRange(callExpr->getBeginLoc(), callExpr->getEndLoc()));
          int firstLeftParenPos = origCallExprText.find("(");
          std::string calleeName = toString(callExpr->getCallee());
          origCallExprText.replace(firstLeftParenPos, 1,
                                   "(reset_" + calleeName + ",");
          appCallExprText += origCallExprText + ";\n";
        }
        dfs(curStmt);
      }
    }
  }
};

class MyASTConsumer : public ASTConsumer {
public:
  MyASTConsumer(Rewriter &R) : TheRewriter(R) {}

  bool HandleTopLevelDecl(DeclGroupRef DR) override {
    InfoExtractionVisitor infoExtractionVisitor(TheRewriter);
    for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b) {
      infoExtractionVisitor.TraverseDecl(*b);
    }

    RewritingVisitor rewritingVisitor(
        TheRewriter, itc_template_str, header_template_str,
        infoExtractionVisitor.getAppCallExprText());
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

void loadTemplate() {
  std::ifstream itc_istream(
      "/usr/insider/synthesis/template/itc_template_itc.txt",
      std::ifstream::in);
  std::ifstream header_istream(
      "/usr/insider/synthesis/template/itc_template_header.txt",
      std::ifstream::in);
  std::string s;
  while (std::getline(itc_istream, s)) {
    itc_template_str += s + "\n";
  }
  while (std::getline(header_istream, s)) {
    header_template_str += s + "\n";
  }
}

std::string getKernelName(std::string sourceFileName) {
  int lPos = sourceFileName.rfind("/") + 1;
  int rPos = sourceFileName.find(".");
  return sourceFileName.substr(lPos, rPos - lPos);
}

int main(int argc, const char **argv) {
  int kernelNum;
  for (int i = 0; i < argc; i++) {
    if (std::string(argv[i]) == "--") {
      kernelNum = i - 2;
      break;
    }
  }
  int new_argc = argc - kernelNum;
  const char **new_argv =
      (const char **)malloc(new_argc * sizeof(const char *));
  new_argv[0] = argv[0];
  new_argv[1] = argv[1];
  for (int i = 2; i < new_argc; i++) {
    new_argv[i] = argv[i + kernelNum];
  }
  topFuncName = getKernelName(std::string(argv[1]));
  for (int i = 2; i < 2 + kernelNum; i++) {
    kernelNameVec.push_back(getKernelName(std::string(argv[i])));
  }
  loadTemplate();
  CommonOptionsParser op(new_argc, new_argv, STAccelCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());
  int ret = Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
  if (!catchTopFunc) {
    llvm::errs() << "Error: In file " + std::string(argv[1]) +
                        ": Cannot find the top function!\n";
    return -1;
  }
  return ret;
}
