#include <cstring>
#include <map>
#include <memory>
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
    STAccelCategory("Insider s2s transformer for apps");
std::string topFuncName;
bool catchTopFunc = false;
bool enablePipelining = true;
std::map<std::string, bool> ResetFifoMap;

class RewritingVisitor : public RecursiveASTVisitor<RewritingVisitor> {
public:
  RewritingVisitor(Rewriter &R, std::string &_beforeWhileText,
                   std::string &_resetText, std::string &_whileBodyText)
      : TheRewriter(R), beforeWhileText(_beforeWhileText),
        resetText(_resetText), whileBodyText(_whileBodyText) {}

  bool VisitFunctionDecl(FunctionDecl *f) {
    if (f->hasBody()) {
      if (f->getNameInfo().getAsString() == topFuncName) {
        Stmt *funcBody = f->getBody();
        SourceLocation locStart = funcBody->getBeginLoc();
        SourceLocation locEnd = funcBody->getEndLoc();
        TheRewriter.RemoveText(SourceRange(locStart, locEnd));
        std::string text;
        text += "{\n" + beforeWhileText;
        text += "\nbool reset = false;\n unsigned reset_cnt = 0;\n";
        text += "while (1) {\n ";
        // if (enablePipelining) {
        //   text += "#pragma HLS pipeline\n";
        // }
        text += "bool dummy;\n";
        text += "if (reset || (reset = reset_" + topFuncName +
                ".read_nb(dummy))) {\n";
        text += resetText;
        text += "reset_cnt ++;\n";
        text += "if (reset_cnt == RESET_CNT) {\n";
        text += "reset_cnt = 0;\n";
        text += "reset = false;\n";
        text += "}\n}\n";
        text += "else ";
        text += whileBodyText;
        text += "}\n}\n}";
        TheRewriter.InsertText(locStart, text);
      }
    }

    return true;
  }

private:
  Rewriter &TheRewriter;
  std::string &beforeWhileText;
  std::string &resetText;
  std::string &whileBodyText;
};

class InfoExtractionVisitor
    : public RecursiveASTVisitor<InfoExtractionVisitor> {
public:
  InfoExtractionVisitor(Rewriter &R) : TheRewriter(R) {}

  bool VisitFunctionDecl(FunctionDecl *f) {
    if (f->hasBody()) {
      if (f->getNameInfo().getAsString() == topFuncName) {
        catchTopFunc = true;
        for (ParmVarDecl *param : f->parameters()) {
          std::string fifoType = param->getType().getAsString();
          int lPos = fifoType.find("<");
          int rPos = fifoType.rfind(">");
          std::string fifoDataType = fifoType.substr(lPos + 1, rPos - lPos - 1);
          std::string fifoName = param->getName();
          fifoSet.insert(fifoName);
          fifoDataTypeMap[fifoName] = fifoDataType;
        }

        ParmVarDecl *firstParmVarDecl;
        firstParmVarDecl = *(f->param_begin());
        TheRewriter.InsertText(firstParmVarDecl->getSourceRange().getBegin(),
                               "ST_Queue<bool> &reset_" + topFuncName + ",\n",
                               true, true);
        Stmt *body = f->getBody();
        StmtIterator lastIter;
        for (auto iter = body->child_begin(); iter != body->child_end();
             iter++) {
          Stmt *childStmt = *iter;
          if (!strcmp(childStmt->getStmtClassName(), "WhileStmt")) {
            if (iter != body->child_begin()) {
              beforeWhileText += retrieveFilteredText(*lastIter, childStmt);
            }
            break;
          } else if (!strcmp(childStmt->getStmtClassName(), "DeclStmt")) {
            DeclStmt *declStmt = (DeclStmt *)childStmt;
            for (auto decl_iter : declStmt->decls()) {
              VarDecl *varDecl = (VarDecl *)decl_iter;
              if (varDecl->hasInit()) {
                std::string varName = varDecl->getNameAsString();
                std::string initVal = toString(varDecl->getInit());
                if (varDecl->isDirectInit()) {
                  std::string typeText = varDecl->getType().getAsString();
                  std::vector<std::string> qualifierVec{"struct", "class",
                                                        "enum"};
                  for (auto qualifier : qualifierVec) {
                    size_t pos;
                    if ((pos = typeText.find(qualifier)) != std::string::npos) {
                      typeText.replace(pos, qualifier.size(), "");
                    }
                  }
                  if (typeText.find("[") == std::string::npos) {
                    resetText += varName + " = " + typeText + "(" + initVal +
                                 ")" + ";\n";
                  } else {
                    // array type
                    bool no_reset = false;
                    if (varDecl->hasAttrs()) {
                      for (const Attr *attr : varDecl->getAttrs()) {
                        const AnnotateAttr *annot = (const AnnotateAttr *)attr;
                        if (attr) {
                          llvm::StringRef text_ref = annot->getAnnotation();
                          if (text_ref.str() == "no_reset") {
                            no_reset = true;
                          }
                        }
                      }
                    }
                    if (!no_reset) {
                      std::vector<std::string> dims;
                      size_t pos = 0;
                      while (1) {
                        size_t left_paren = typeText.find("[", pos);
                        size_t right_paren = typeText.find("]", pos);
                        if (left_paren == std::string::npos) {
                          break;
                        }
                        dims.push_back(typeText.substr(
                            left_paren + 1, right_paren - left_paren - 1));
                        pos = right_paren + 1;
                      }
                      typeText = typeText.substr(0, typeText.find("["));
                      for (size_t i = 0; i < dims.size(); i++) {
                        std::string iterVar = "i" + std::to_string(i);
                        resetText += "for (int " + iterVar + " = 0; " +
                                     iterVar + +" < " + dims[i] + "; " +
                                     iterVar + " ++) {\n";
                      }
                      resetText += varName;
                      for (size_t i = 0; i < dims.size(); i++) {
                        std::string iterVar = "i" + std::to_string(i);
                        resetText += "[" + iterVar + "]";
                      }
                      resetText += " = " + typeText + "(" + initVal + ");\n";
                      for (size_t i = 0; i < dims.size(); i++) {
                        resetText += "}\n";
                      }
                    }
                  }
                } else {
                  resetText += varName + " = " + initVal + ";\n";
                }
              }
            }
          } else {
            resetText += toString(childStmt) + ";\n";
          }
          beforeWhileText += toString(childStmt) + ";\n";
          if (iter != body->child_begin()) {
            beforeWhileText += retrieveFilteredText(*lastIter, childStmt);
          }
          lastIter = iter;
        }
        dfs(body);
      }
    }
    return true;
  }

  std::string &getResetText() { return resetText; }

  std::string &getWhileBodyText() { return whileBodyText; }

  std::string &getBeforeWhileText() { return beforeWhileText; }

  std::string retrieveFilteredText(Stmt *lastStmt, Stmt *curStmt) {
    SourceLocation locStart = lastStmt->getEndLoc();
    SourceLocation locEnd = curStmt->getBeginLoc();
    SourceManager &SM = TheRewriter.getSourceMgr();
    std::string text = std::string(SM.getCharacterData(locStart),
                                   SM.getCharacterData(locEnd) -
                                       SM.getCharacterData(locStart));
    return text;
  }

private:
  Rewriter &TheRewriter;
  std::string beforeWhileText;
  std::string resetText;
  std::string whileBodyText;
  std::set<std::string> fifoSet;
  std::map<std::string, std::string> fifoDataTypeMap;
  std::set<std::string> funcParamFifoNameSet;

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

  std::string toRawString(Stmt *stmt) {
    SourceLocation locStart = stmt->getBeginLoc();
    SourceLocation locEnd = stmt->getEndLoc();
    SourceManager &SM = TheRewriter.getSourceMgr();
    std::string text = std::string(SM.getCharacterData(locStart),
                                   SM.getCharacterData(locEnd) -
                                       SM.getCharacterData(locStart));
    return text;
  }

  void dfs(Stmt *root) {
    for (auto iter = root->child_begin(); iter != root->child_end(); iter++) {
      Stmt *curStmt = *iter;
      if (curStmt) {
        if (!strcmp(curStmt->getStmtClassName(), "WhileStmt")) {
          WhileStmt *whileStmt = (WhileStmt *)curStmt;
          Stmt *whileBody = whileStmt->getBody();
          whileBodyText = toRawString(whileBody);
        } else if (!strcmp(curStmt->getStmtClassName(), "CXXMemberCallExpr")) {
          CXXMemberCallExpr *cxxMemberCallExpr = (CXXMemberCallExpr *)curStmt;
          std::string methodName =
              cxxMemberCallExpr->getMethodDecl()->getNameInfo().getAsString();
          if (methodName == "read" || methodName == "read_nb") {
            std::string fifoName =
                toString(cxxMemberCallExpr->getImplicitObjectArgument());
            if (fifoSet.count(fifoName) && !ResetFifoMap[fifoName]) {
              static int resetVarCnt = 0;
              std::string fifoDataType = fifoDataTypeMap[fifoName];
              std::string resetVarText =
                  "dummy" + std::to_string(resetVarCnt++);
              resetText += fifoDataType + " " + resetVarText + ";\n";
              resetText += fifoName + ".read_nb(" + resetVarText + ");\n";
              ResetFifoMap[fifoName] = true;
            }
          }
          std::string callerStr =
              toString(cxxMemberCallExpr->getImplicitObjectArgument());
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
        TheRewriter, infoExtractionVisitor.getBeforeWhileText(),
        infoExtractionVisitor.getResetText(),
        infoExtractionVisitor.getWhileBodyText());
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

bool judgeEnablePipelining(int *argc, char **filteredArgv) {
  for (int i = 1; i < *argc; i++) {
    if (strcmp(filteredArgv[i], "--disable_pipelining") == 0) {
      for (int j = i + 1; j < *argc; j++) {
        filteredArgv[j - 1] = filteredArgv[j];
      }
      (*argc)--;
      return false;
    }
  }
  return true;
}

char **duplicateArgv(int argc, char **argv) {
  char **newArgv = new char *[argc];
  for (int i = 0; i < argc; i++) {
    newArgv[i] = argv[i];
  }
  return newArgv;
}

int main(int argc, char **argv) {
  std::unique_ptr<char *> filteredArgv =
      std::unique_ptr<char *>(duplicateArgv(argc, argv));
  enablePipelining = judgeEnablePipelining(&argc, filteredArgv.get());
  topFuncName = getKernelName(std::string(filteredArgv.get()[1]));
  CommonOptionsParser op(argc, (const char **)(filteredArgv.get()),
                         STAccelCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());
  int ret = Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
  if (!catchTopFunc) {
    llvm::errs() << "Error: In file " + std::string(argv[1]) +
                        ": Cannot find the top function!\n";
    return -1;
  }
  return ret;
}
