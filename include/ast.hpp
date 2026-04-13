#pragma once
#include <memory>
#include <vector>
#include <string>

enum class ValueType {
  Int,
  String,
  Unknown
};

struct Expr {
  virtual ~Expr() = default;
};

struct NumberExpr : Expr {
  int value = 0;
};

struct StringExpr : Expr {
  std::string value;
};

struct VarRef : Expr {
  std::string name;
};

struct BinaryExpr : Expr {
  std::string op;
  std::unique_ptr<Expr> left;
  std::unique_ptr<Expr> right;
};

struct CallExpr : Expr {
  std::string callee;
  std::vector<std::unique_ptr<Expr>> args;
};

struct Stmt { virtual ~Stmt()=default; };
struct Block;

struct VarDecl : Stmt {
  std::string name;
  std::unique_ptr<Expr> value;
};

struct Out : Stmt {
  std::unique_ptr<Expr> value;
};

struct In : Stmt {
  std::string name;
};

struct Assign : Stmt {
  std::string name;
  std::unique_ptr<Expr> value;
};

struct IfStmt : Stmt {
  std::unique_ptr<Expr> condition;
  std::unique_ptr<Block> thenBlock;
  std::unique_ptr<Block> elseBlock;
};

struct WhileStmt : Stmt {
  std::unique_ptr<Expr> condition;
  std::unique_ptr<Block> body;
};

struct ReturnStmt : Stmt {
  std::unique_ptr<Expr> value;
};

struct ObjDecl : Stmt {
  std::string typeName;
  std::string varName;
  std::vector<std::unique_ptr<Expr>> args;
};

struct Block {
  std::vector<std::unique_ptr<Stmt>> stmts;
};

struct FunctionDecl {
  std::string name;
  std::vector<std::string> params;
  std::unique_ptr<Block> body;
};

struct FieldDecl {
  std::string name;
  std::unique_ptr<Expr> init;
};

struct CtorDecl {
  bool exists = false;
  std::vector<std::string> params;
  std::unique_ptr<Block> body;
};

struct DtorDecl {
  bool exists = false;
  std::unique_ptr<Block> body;
};

struct TypeDecl {
  bool isClass = false;
  std::string name;
  std::vector<FieldDecl> fields;
  CtorDecl ctor;
  DtorDecl dtor;
};

struct MainDecl {
  std::vector<TypeDecl> types;
  std::vector<FunctionDecl> functions;
  std::unique_ptr<Block> block;
};