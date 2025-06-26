#ifndef CHOCOPY_LLVM_PARSER_PARSER_H
#define CHOCOPY_LLVM_PARSER_PARSER_H

#include "chocopy-llvm/AST/AST.h"
#include "chocopy-llvm/Lexer/Lexer.h"

namespace chocopy {
class Sema;
class Scope;

class Parser {
  class ParseScope;

public:
  Parser(ASTContext &C, Lexer &Lex, Sema &Acts);

  Program *parse();

private:
  bool consumeToken(tok::TokenKind ExpectedTok);
  bool consumeToken();

  bool expect(tok::TokenKind ExpectedTok);
  bool expectAndConsume(tok::TokenKind ExpectedTok);

  void skipToNextLine();

  void emitUnexpected();

  const Token &getLookAheadToken(int N);

  Program *parseProgram();

  Stmt *parseStmt();
  Stmt *parseIfStmt();
  WhileStmt *parseWhileStmt();
  ForStmt *parseForStmt();
  bool parseBlock(StmtList &Stmts);
  Stmt *parseSimpleStmt();

  Expr *parseExpr();
  Expr *parseOrExpr();
  Expr *parseAndExpr();
  Expr *parseNotExpr();
  Expr *parseCompareExpr();
  Expr *parseAddSubExpr();
  Expr *parseMulDivModExpr();
  Expr *parseUnaryExpr();
  Expr *parseAtomicExpr();
  Expr *parsePostfixExpr(Expr *Primary);
  Expr *parseFuncCall(Expr *Func);
  Expr *parseListExpr();

  TypeAnnotation *parseType();
  VarDef *parseVarDef();
  Literal *parseLiteral();

  ClassDef *parseClassDef();
  bool parseClassBody(DeclList &Decls);
  FuncDef *parseFuncDef();
  bool parseFuncBody(DeclList &Decls, StmtList &Stmts);

  ParamDecl *parseParamDecl();
  GlobalDecl *parseGlobalDecl();
  NonLocalDecl *parseNonLocalDecl();

  BinaryExpr::OpKind convertTokenToOp(tok::TokenKind Kind);

private:
  DiagnosticsEngine &Diags;
  ASTContext &Context;
  Lexer &TheLexer;
  Token Tok;
};
} // namespace chocopy
#endif // CHOCOPY_LLVM_PARSER_PARSER_H