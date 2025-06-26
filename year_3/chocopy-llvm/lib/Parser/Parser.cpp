#include "chocopy-llvm/Parser/Parser.h"
#include "chocopy-llvm/AST/ASTContext.h"
#include "chocopy-llvm/Basic/Diagnostic.h"
#include "chocopy-llvm/Sema/Scope.h"
#include "chocopy-llvm/Sema/Sema.h"

#include <llvm/ADT/APInt.h>

#include <stack>

namespace chocopy {
Parser::Parser(ASTContext &C, Lexer &Lex, Sema &Acts)
    : Diags(Lex.getDiagnostics()), Context(C), TheLexer(Lex) {}

Program *Parser::parse() {
  Program *P = parseProgram();
  return P;
}

bool Parser::consumeToken(tok::TokenKind ExpectedTok) {
  if (Tok.is(ExpectedTok)) {
    TheLexer.lex(Tok);
    return true;
  }
  return false;
}

bool Parser::consumeToken() {
  TheLexer.lex(Tok);
  return true;
}

bool Parser::expect(tok::TokenKind ExpectedTok) {
  if (Tok.is(ExpectedTok))
    return true;
  Diags.emitError(Tok.getLocation().Start, diag::err_near_token) << Tok;
  Diags.emitError(Tok.getLocation().Start, diag::err_expected)
      << tok::getTokenName(ExpectedTok);
  return false;
}

bool Parser::expectAndConsume(tok::TokenKind ExpectedTok) {
  return expect(ExpectedTok) && consumeToken();
}

void Parser::skipToNextLine() {
  while (!Tok.is(tok::eof) && !consumeToken(tok::NEWLINE))
    consumeToken();
}

void Parser::emitUnexpected() {
  Diags.emitError(Tok.getLocation().Start, diag::err_unexpected) << Tok;
}

const Token &Parser::getLookAheadToken(int N) {
  assert(N);
  return TheLexer.LookAhead(N - 1);
}

Program *Parser::parseProgram() {
    DeclList Declarations;
    StmtList Statements;

    auto IsVarDef = [this](Token &Tok) {
        return Tok.isOneOf(tok::identifier, tok::idstring) &&
            TheLexer.LookAhead(0).is(tok::colon);
    };

    consumeToken();

    while (true) {
        if (IsVarDef(Tok)) {
            if (VarDef *V = parseVarDef()) 
                Declarations.push_back(V);
            else 
                skipToNextLine();
        } else if (Tok.is(tok::kw_def)) {
            if (FuncDef *F = parseFuncDef()) 
                Declarations.push_back(F);
            else 
                skipToNextLine();
        } else if (Tok.is(tok::kw_class)) {
            if (ClassDef *C = parseClassDef()) 
                Declarations.push_back(C);
            else 
                skipToNextLine();
        } else
            break;
    }

    while (!Tok.is(tok::eof)) {
        if (Stmt *S = parseStmt()) 
            Statements.push_back(S);
        else 
            skipToNextLine();
    }

    return Context.createProgram(Declarations, Statements);
}


/// var_def = typed_var '=' literal NEWLINE
VarDef *Parser::parseVarDef() {

  if (!expect(tok::identifier))
    return nullptr;

  SymbolInfo *Name = Tok.getSymbolInfo();
  SMRange NameLoc = Tok.getLocation();
  consumeToken();

  if (!expectAndConsume(tok::colon))
    return nullptr;

  TypeAnnotation *T = parseType();
  if (!T) 
    return nullptr;

  if (!expectAndConsume(tok::equal))
    return nullptr;

  if (Literal *L = parseLiteral()) {
    if (expectAndConsume(tok::NEWLINE)) {
      SMLoc ELoc = L->getLocation().End;
      SMRange Loc(NameLoc.Start, ELoc);
      Identifier *V = Context.createIdentifier(NameLoc, Name);
      return Context.createVarDef(Loc, V, T, L);
    }
  }

  return nullptr;
}

FuncDef *Parser::parseFuncDef() {
    SMLoc FuncStart = Tok.getLocation().Start;
    if (!expectAndConsume(tok::kw_def)) 
        return nullptr;

    if (!expect(tok::identifier)) 
        return nullptr;
    Identifier *Id = Context.createIdentifier(Tok.getLocation(), Tok.getSymbolInfo());
    consumeToken();

    if (!expectAndConsume(tok::l_paren)) 
        return nullptr;

    ParamDeclList Params;
    if (!Tok.is(tok::r_paren)) {
        do {
            ParamDecl *Param = parseParamDecl();
            if (!Param) 
                return nullptr;
            Params.push_back(Param);
        } while (consumeToken(tok::comma));
    }

    if (!expectAndConsume(tok::r_paren)) 
        return nullptr;

    TypeAnnotation *ReturnType;
    if (consumeToken(tok::arrow)) {
        ReturnType = parseType();
    } else {
        SMRange Dummy(Tok.getLocation().Start, Tok.getLocation().Start);
        ReturnType = Context.createClassType(Dummy, "<None>");
    }
	  
    if (!(expectAndConsume(tok::colon) && expectAndConsume(tok::NEWLINE) && expectAndConsume(tok::INDENT)))
        return nullptr;

    DeclList Declarations;
    StmtList Statements;
    if (!parseFuncBody(Declarations, Statements)) 
        return nullptr;
    
    SMLoc FuncEnd;
    if (!Statements.empty()) {
        FuncEnd = Statements.back()->getLocation().End;
    } else {
        FuncEnd = Tok.getLocation().End;  
    }

    if (!expectAndConsume(tok::DEDENT)) 
        return nullptr;

    SMRange Loc(FuncStart, FuncEnd);
    return Context.createFuncDef(Loc, Id, Params, ReturnType, Declarations, Statements);
}

ParamDecl *Parser::parseParamDecl() {
    if (!expect(tok::identifier)) 
        return nullptr;
    Identifier *Id = Context.createIdentifier(Tok.getLocation(), Tok.getSymbolInfo());
    consumeToken();

    if (!expectAndConsume(tok::colon)) 
        return nullptr;

    TypeAnnotation *Type = parseType();
    if (!Type) 
        return nullptr;

    SMRange Loc(Id->getLocation().Start, Tok.getLocation().Start);
    return Context.createParamDecl(Loc, Id, Type);
}

bool Parser::parseFuncBody(DeclList &Declarations, StmtList &Statements) {
    auto IsVarDef = [this](Token &Tok) {
        return Tok.isOneOf(tok::identifier, tok::idstring) &&
            getLookAheadToken(1).is(tok::colon);
    };

    while (1) {
        if (IsVarDef(Tok)) {
            if (VarDef *V = parseVarDef()) 
                Declarations.push_back(V);
            else 
                skipToNextLine();
        } else if (Tok.getKind() == tok::kw_def) {
            if (FuncDef *F = parseFuncDef()) 
                Declarations.push_back(F);
            else 
                skipToNextLine();
        } else if (Tok.getKind() == tok::kw_global) {
            if (GlobalDecl *G = parseGlobalDecl()) 
                Declarations.push_back(G);
            else 
                skipToNextLine();
        } else if (Tok.getKind() == tok::kw_nonlocal) {
            if (NonLocalDecl *N = parseNonLocalDecl()) 
                Declarations.push_back(N);
            else 
                skipToNextLine();
        } else
            break;
    }

    while (Tok.isNot(tok::DEDENT) && Tok.isNot(tok::eof)) {
        if (Stmt *S = parseStmt()) 
            Statements.push_back(S);
        else 
            skipToNextLine();
    }

  return true;
}


ClassDef *Parser::parseClassDef(){
    SMLoc ClassStart = Tok.getLocation().Start;

    if (!expectAndConsume(tok::kw_class)) 
        return nullptr;
  
    if (!expect(tok::identifier)) 
        return nullptr;
    Identifier *Id = Context.createIdentifier(Tok.getLocation(), Tok.getSymbolInfo());
    consumeToken();

    if (!expectAndConsume(tok::l_paren)) 
        return nullptr;

    if (!expect(tok::identifier)) 
        return nullptr;
    Identifier *SuperId = Context.createIdentifier(Tok.getLocation(), Tok.getSymbolInfo());
    consumeToken();

    if (!(expectAndConsume(tok::r_paren) && expectAndConsume(tok::colon) && expectAndConsume(tok::NEWLINE) && expectAndConsume(tok::INDENT)))
        return nullptr;

    DeclList Body;
    if (!parseClassBody(Body)) 
        return nullptr;

    SMLoc ClassEnd;
    if (!Body.empty())
        ClassEnd = Body.back()->getLocation().End;
    else
        ClassEnd = Tok.getLocation().End;
    SMRange Loc(ClassStart, ClassEnd);

    if (!expectAndConsume(tok::DEDENT)) 
        return nullptr;

    return Context.createClassDef(Loc, Id, SuperId, Body); 
}

bool Parser::parseClassBody(DeclList &Declarations) {
    if (consumeToken(tok::kw_pass)) 
        return expectAndConsume(tok::NEWLINE);

    auto IsVarDef = [this](Token &Tok) {
        return Tok.isOneOf(tok::identifier, tok::idstring) &&
            TheLexer.LookAhead(0).is(tok::colon);
    };
    bool isEmpty = true;

    while (Tok.isNot(tok::DEDENT) && Tok.isNot(tok::eof)) {
        if (IsVarDef(Tok)) {
            if (VarDef *V = parseVarDef()) {
                Declarations.push_back(V);
                isEmpty = false;
            } else 
                skipToNextLine();
        } else if (Tok.is(tok::kw_def)) {
            if (FuncDef *F = parseFuncDef()) {
                Declarations.push_back(F);
                isEmpty = false;
            } else 
                skipToNextLine();
        }
            else skipToNextLine();
    }

    return !isEmpty;
}


Stmt *Parser::parseStmt() {
    if (Tok.getKind() == tok::kw_if)
        return parseIfStmt();
    else if (Tok.getKind() == tok::kw_while)
        return parseWhileStmt();
    else if (Tok.getKind() == tok::kw_for)
        return parseForStmt();
    else if (Stmt *S = parseSimpleStmt()) {
        if (expectAndConsume(tok::NEWLINE)) 
            return S;
    }
    return nullptr;
}


Stmt *Parser::parseSimpleStmt() {
    SMLoc SimpleStart = Tok.getLocation().Start;

    if (consumeToken(tok::kw_pass)) 
        return nullptr;

    auto IsTarget = [](const Expr *E) { return llvm::isa<DeclRef>(E) || 
                                            llvm::isa<MemberExpr>(E) || 
                                            llvm::isa<IndexExpr>(E); };
    Expr *E = nullptr;

    if (consumeToken(tok::kw_return)) {
        if (!Tok.is(tok::NEWLINE)) {
            E = parseExpr();
            if (!E) 
                return nullptr;
        } else {
            SMLoc None = Tok.getLocation().Start;
            E = Context.createNoneLiteral(SMRange(None, None));
        }
        return Context.createReturnStmt(SMRange(SimpleStart, E->getLocation().End), E);
    }

    ExprList Targets;
    E = parseExpr();
    if (!E) 
        return nullptr;

    while (Tok.is(tok::equal)) {
        if (!IsTarget(E)) 
            return nullptr;
        Targets.push_back(E);
        consumeToken();
        E = parseExpr();
        if (!E) 
            return nullptr;
    }

    SMRange Loc(SimpleStart, Tok.getLocation().End);
    if (!Targets.empty()) 
        return Context.createAssignStmt(Loc, Targets, E);
    return Context.createExprStmt(Loc, E);
}

bool Parser::parseBlock(StmtList &Statements) {
    if (!expectAndConsume(tok::NEWLINE)) 
        return false;

    if (!expectAndConsume(tok::INDENT)) 
        return false;

    while (!Tok.isOneOf(tok::DEDENT, tok::eof)) {
        if (Stmt *S = parseStmt()) 
            Statements.push_back(S);
        else 
            skipToNextLine();
    }

    if (!expectAndConsume(tok::DEDENT)) 
        return false;

    return true;
}

Stmt *Parser::parseIfStmt() {
    SMLoc IfStart = Tok.getLocation().Start;

    if (!expectAndConsume(tok::kw_if)) 
        return nullptr;

    Expr *Cond = parseExpr();
    if (!Cond) 
        return nullptr;

    if (!expectAndConsume(tok::colon)) 
        return nullptr;

    StmtList Then;
    if (!parseBlock(Then)) 
        return nullptr;

    struct Elif {
        Expr *Cond;
        StmtList Body;
        SMLoc Start;
        SMLoc End;
    };

    std::stack <Elif> Elifs;
    Elifs.push({Cond, Then, IfStart, Tok.getLocation().Start});

    while (Tok.is(tok::kw_elif)) {
        SMLoc ElifStart = Tok.getLocation().Start;
        consumeToken(tok::kw_elif);

        Expr *ElifCond = parseExpr();
        if (!ElifCond) 
            return nullptr;

        if (!expectAndConsume(tok::colon)) 
            return nullptr;

        StmtList ElifBody;
        if (!parseBlock(ElifBody)) 
            return nullptr;

        Elifs.push({ElifCond, ElifBody, ElifStart, Tok.getLocation().Start});
    }

    StmtList Else;
    if (Tok.is(tok::kw_else)) {
        consumeToken(tok::kw_else);

        if (!expectAndConsume(tok::colon)) 
            return nullptr;

        if (!parseBlock(Else)) 
            return nullptr;
    }

    while (!Elifs.empty()) {
        Elif Top = Elifs.top();
        Elifs.pop();

        IfStmt *If;
        if (Else.empty()) {
            SMRange Loc(Top.Start, Top.End);
            If = Context.createIfStmt(Loc, Top.Cond, Top.Body, Else);
        } else {
            SMRange Loc(Top.Start, Tok.getLocation().Start);
            If = Context.createIfStmt(Loc, Top.Cond, Top.Body, Else);
        }

        Else = {If};
    }

    return Else.front();
}

WhileStmt *Parser::parseWhileStmt() {
    SMLoc WStart = Tok.getLocation().Start;

    if (!expectAndConsume(tok::kw_while)) 
        return nullptr;

    Expr *Condition = parseExpr();
    if (!Condition) 
        return nullptr;

    if (!expectAndConsume(tok::colon)) 
        return nullptr;

    StmtList Body;
    if (!parseBlock(Body)) 
        return nullptr;

    SMRange Loc(WStart, Tok.getLocation().Start);
    return Context.createWhileStmt(Loc, Condition, Body);
}

ForStmt *Parser::parseForStmt() {
    SMLoc FStart = Tok.getLocation().Start;

    if (!expectAndConsume(tok::kw_for))   
        return nullptr;

    if (!expect(tok::identifier)) 
        return nullptr;
    DeclRef *Target = Context.createDeclRef(Tok.getLocation(), Tok.getSymbolInfo());
    consumeToken();

    if (!expectAndConsume(tok::kw_in)) 
        return nullptr;

    Expr *I = parseExpr();
    if (!I) 
        return nullptr;

    if (!expectAndConsume(tok::colon)) 
        return nullptr;

    StmtList Body;
    if (!parseBlock(Body)) 
        return nullptr;  

    SMRange Loc(FStart, Tok.getLocation().Start);
    return Context.createForStmt(Loc, Target, I, Body);
}


GlobalDecl *Parser::parseGlobalDecl() {
    SMLoc GlobalStart = Tok.getLocation().Start;

    if (!expectAndConsume(tok::kw_global)) 
        return nullptr;

    if (!expect(tok::identifier)) 
        return nullptr;

    SymbolInfo *Name = Tok.getSymbolInfo();
    SMRange Loc(GlobalStart, Tok.getLocation().End);
    SMRange IdLoc = Tok.getLocation();
    consumeToken();

    if (!expectAndConsume(tok::NEWLINE)) 
        return nullptr;

    Identifier *Id = Context.createIdentifier(IdLoc, Name);
    return Context.createGlobalDecl(Loc, Id);
}

NonLocalDecl *Parser::parseNonLocalDecl() {
    SMLoc NonStart = Tok.getLocation().Start;

    if (!expectAndConsume(tok::kw_nonlocal)) 
        return nullptr;

    if (!expect(tok::identifier)) 
        return nullptr;

    SymbolInfo *Name = Tok.getSymbolInfo();
    SMRange Loc(NonStart, Tok.getLocation().End);
    SMRange IdLoc = Tok.getLocation();

    consumeToken();
    if (!expectAndConsume(tok::NEWLINE)) 
        return nullptr; 

    Identifier *Id = Context.createIdentifier(IdLoc, Name);
    return Context.createNonLocalDecl(Loc, Id);
}

BinaryExpr::OpKind Parser::convertTokenToOp(tok::TokenKind Kind) {
    switch (Kind) {
        case tok::equalequal: 
            return BinaryExpr::OpKind::EqCmp;
        case tok::exclaimequal: 
            return BinaryExpr::OpKind::NEqCmp;
        case tok::less: 
            return BinaryExpr::OpKind::LCmp;
        case tok::lessequal: 
            return BinaryExpr::OpKind::LEqCmp;
        case tok::greater: 
            return BinaryExpr::OpKind::GCmp;
        case tok::greaterequal: 
            return BinaryExpr::OpKind::GEqCmp;
        case tok::kw_is: 
            return BinaryExpr::OpKind::Is;
        case tok::plus: 
            return BinaryExpr::OpKind::Add;
        case tok::minus: 
            return BinaryExpr::OpKind::Sub;
        case tok::star: 
            return BinaryExpr::OpKind::Mul;
        case tok::slashslash: 
            return BinaryExpr::OpKind::FloorDiv;
        case tok::percent: 
            return BinaryExpr::OpKind::Mod;
        default: 
            llvm_unreachable("The operator is unavailable");
    }
}

Expr *Parser::parseExpr() {
    SMLoc ExprStart = Tok.getLocation().Start;

    Expr *Then = parseOrExpr();
    if (!Tok.is(tok::kw_if)) 
        return Then;

    consumeToken();
    Expr *Cond = parseOrExpr();
    if (!Cond) 
        return nullptr;

    if (!expectAndConsume(tok::kw_else)) 
        return nullptr;

    Expr *Else = parseExpr();
    if (!Else)
        return nullptr;

    SMRange Loc(ExprStart, Else->getLocation().End);
    return Context.createIfExpr(Loc, Cond, Then, Else);
}

Expr *Parser::parseOrExpr() {
    SMLoc OrStart = Tok.getLocation().Start;

    Expr *L = parseAndExpr();
    if (!L) 
        return nullptr;

    while (Tok.is(tok::kw_or)) {
        consumeToken();
        Expr *R = parseAndExpr();
        if (!R) 
            return nullptr;
        
        SMRange Loc(OrStart, R->getLocation().End);
        L = Context.createBinaryExpr(Loc, L, BinaryExpr::OpKind::Or, R);
    }

    return L;
}

Expr *Parser::parseAndExpr() {
    SMLoc AndStart = Tok.getLocation().Start;

    Expr *L = parseNotExpr();
    if (!L) 
        return nullptr;

    while (Tok.is(tok::kw_and)) {
        consumeToken();
        Expr *R = parseNotExpr();
        if (!R) 
            return nullptr;
        
        SMRange Loc(AndStart, R->getLocation().End);
        L = Context.createBinaryExpr(Loc, L, BinaryExpr::OpKind::And, R);
    }

    return L;
}

Expr *Parser::parseNotExpr() {
    if (Tok.is(tok::kw_not)) {
        SMLoc NotStart = Tok.getLocation().Start;

        consumeToken();
        Expr *Op = parseNotExpr();
        if (!Op) 
            return nullptr;
        
        SMRange Loc(NotStart, Op->getLocation().End);
        return Context.createUnaryExpr(Loc, UnaryExpr::OpKind::Not, Op);
    }

    return parseCompareExpr();
}

Expr *Parser::parseCompareExpr() {
    SMLoc CompareStart = Tok.getLocation().Start;

    Expr *L = parseAddSubExpr();
    if (!L) 
        return nullptr;

    if (Tok.is(tok::equalequal) || Tok.is(tok::exclaimequal) ||
            Tok.is(tok::less) || Tok.is(tok::lessequal) ||
                    Tok.is(tok::greater) || Tok.is(tok::greaterequal) ||
                    Tok.is(tok::kw_is)) {
        BinaryExpr::OpKind Op = convertTokenToOp(Tok.getKind());
        
        consumeToken();
        Expr *R = parseAddSubExpr();
        if (!R) 
            return nullptr;

        SMRange Loc(CompareStart, R->getLocation().End);
        return Context.createBinaryExpr(Loc, L, Op, R);
    }

    return L;
}

Expr *Parser::parseAddSubExpr() {
    SMLoc ASStart = Tok.getLocation().Start;

    Expr *L = parseMulDivModExpr();
    if (!L) 
        return nullptr;

    while (Tok.is(tok::plus) ||  Tok.is(tok::minus)) {
        BinaryExpr::OpKind Op = convertTokenToOp(Tok.getKind());
        
        consumeToken();
        Expr *R = parseMulDivModExpr();
        if (!R) 
            return nullptr;

        SMRange Loc(ASStart, R->getLocation().End);
        L = Context.createBinaryExpr(Loc, L, Op, R);
    }

    return L;
}

Expr *Parser::parseMulDivModExpr() {
    SMLoc MDStart = Tok.getLocation().Start;

    Expr *L = parseUnaryExpr();
    if (!L) 
        return nullptr;

    while (Tok.is(tok::star) ||  Tok.is(tok::slashslash) ||  Tok.is(tok::percent)) {
        BinaryExpr::OpKind Op = convertTokenToOp(Tok.getKind());
        
        consumeToken();
        Expr *R = parseUnaryExpr();
        if (!R) 
            return nullptr;

        SMRange Loc(MDStart, R->getLocation().End);
        L = Context.createBinaryExpr(Loc, L, Op, R);
    }

    return L;
}

Expr *Parser::parseUnaryExpr() {
    if (Tok.is(tok::minus)) {
        SMLoc UStart = Tok.getLocation().Start;
        
        consumeToken();
        Expr *Op = parseUnaryExpr();
        if (!Op) 
            return nullptr;

        SMRange Loc(UStart, Op->getLocation().End);
        return Context.createUnaryExpr(Loc, UnaryExpr::OpKind::Minus, Op);
    }

    return parseAtomicExpr();
}

Expr *Parser::parseAtomicExpr() {
    Expr *literal = nullptr;
    SMRange Loc = Tok.getLocation();

    if (Tok.getKind() == tok::integer_literal) {
        llvm::APInt Val(32, Tok.getLiteralData(), 10);
        literal = Context.createIntegerLiteral(Loc, Val.getSExtValue());
        consumeToken();
    } else if (Tok.getKind() == tok::string_literal) {
        literal = Context.createStringLiteral(Loc, Tok.getLiteralData());
        consumeToken();
    } else if (Tok.getKind() == tok::idstring) {
        literal = Context.createStringLiteral(Loc, Tok.getLiteralData());
        consumeToken();
    } else if (Tok.getKind() == tok::kw_True) {
        literal = Context.createBooleanLiteral(Loc, true);
        consumeToken();
    } else if (Tok.getKind() == tok::kw_False) {
        literal = Context.createBooleanLiteral(Loc, false);
        consumeToken();
    } else if (Tok.getKind() == tok::kw_None) {
        literal = Context.createNoneLiteral(Loc);
        consumeToken();
    } else if (Tok.getKind() == tok::identifier) {
        literal = Context.createDeclRef(Loc, Tok.getSymbolInfo());
        consumeToken();
    } else if (Tok.getKind() == tok::l_paren) {
        consumeToken();
        literal = parseExpr();
        if (!literal) 
            return nullptr;

        if (!expectAndConsume(tok::r_paren)) 
            return nullptr;
    } else if (Tok.getKind() == tok::l_square)
        literal = parseListExpr();
    else {
        emitUnexpected();
        return nullptr;
    }

    if (!literal) 
        return nullptr;
    return parsePostfixExpr(literal);
}

Expr *Parser::parsePostfixExpr(Expr *Primary) {
    while (1) {
        if (Tok.getKind() == tok::l_paren) 
            Primary = parseFuncCall(Primary);
        else if (Tok.getKind() == tok::period) {
            consumeToken();

            if (!expect(tok::identifier)) 
                return nullptr;

            DeclRef *Member = Context.createDeclRef(Tok.getLocation(), Tok.getSymbolInfo());
            SMRange Loc(Primary->getLocation().Start, Member->getLocation().End);
            
            consumeToken();
            Primary = Context.createMemberExpr(Loc, Primary, Member);
        } else if (Tok.getKind() == tok::l_square) {
            consumeToken();
            Expr *I = parseExpr();
            if (!I) 
                return nullptr;

            SMRange Loc(Primary->getLocation().Start, Tok.getLocation().End);

            if (!expectAndConsume(tok::r_square)) 
                return nullptr;

            Primary = Context.createIndexExpr(Loc, Primary, I);
        } else
            return Primary;  
    }
}

Expr *Parser::parseFuncCall(Expr *Func) {
    ExprList Args;
    consumeToken(tok::l_paren);
    
    if (!Tok.is(tok::r_paren)) {
        do {
            Expr *Arg = parseExpr();
            if (!Arg) 
                return nullptr;
            Args.push_back(Arg);
        } while (consumeToken(tok::comma));
    }
    
    if (!expectAndConsume(tok::r_paren)) 
        return nullptr;
    
    SMRange Loc(Func->getLocation().Start, Tok.getLocation().Start);
    if (auto *Meth = dyn_cast<MemberExpr>(Func)) 
        return Context.createMethodCallExpr(Loc, Meth, Args);
    return Context.createCallExpr(Loc, Func, Args);
}

Expr *Parser::parseListExpr() {
    SMLoc ListStart = Tok.getLocation().Start;
    consumeToken(tok::l_square);
    
    ExprList Elts;
    if (!Tok.is(tok::r_square)) {
        do {
            Expr *E = parseExpr();
            if (!E) 
                return nullptr;

            Elts.push_back(E);
        } while (consumeToken(tok::comma));
    }

    SMRange Loc(ListStart, Tok.getLocation().End);
    
    if (!expectAndConsume(tok::r_square))  
        return nullptr;
    
    return Context.createListExpr(Loc, Elts);
}

/// type = ID | IDSTRING | '[' type ']'
TypeAnnotation *Parser::parseType() {
  SMRange Loc = Tok.getLocation();

  if (Tok.getKind() == tok::identifier) {
    StringRef Name = Tok.getSymbolInfo()->getName();
    consumeToken();
    return Context.createClassType(Loc, Name);
  }
  else if (Tok.getKind() == tok::idstring) {
    StringRef Name = Tok.getLiteralData();
    consumeToken();
    return Context.createClassType(Loc, Name);
  }
  else if (Tok.getKind() == tok::l_square) {
    consumeToken();
    if (TypeAnnotation *T = parseType()) {
      if (expectAndConsume(tok::r_square)) {
        Loc = SMRange(Loc.Start, Tok.getLocation().End);
        return Context.createListType(Loc, T);
      }
    }
    return nullptr;
  } else {
    expect(tok::string_literal);
    return nullptr;
  }
}


Literal *Parser::parseLiteral() {
  SMRange Loc = Tok.getLocation();

  if (consumeToken(tok::kw_None)) {
    return Context.createNoneLiteral(Loc);
  } else if (consumeToken(tok::kw_True)) {
    return Context.createBooleanLiteral(Loc, true);
  } else if (consumeToken(tok::kw_False)) {
    return Context.createBooleanLiteral(Loc, false);
  } else if (Tok.is(tok::integer_literal)) {
    llvm::APInt Value(32, Tok.getLiteralData(), 10);
    consumeToken();
    return Context.createIntegerLiteral(Loc, Value.getSExtValue());
  } else if (Tok.isOneOf(tok::idstring, tok::string_literal)) {
    StringRef Str = Tok.getLiteralData();
    consumeToken();
    return Context.createStringLiteral(Loc, Str);
  }

  Diags.emitError(Tok.getLocation().Start, diag::err_near_token) << Tok;
  return nullptr;
}

} 
