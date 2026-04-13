#pragma once
#include "ast.hpp"
#include <sstream>
#include <unordered_map>

class CodeGen {
  std::ostringstream out;
  std::unordered_map<std::string, ValueType> symbols;
  std::string currentTypeName;

  void emitBlock(const Block* block);
  void emitBlockScoped(const Block* block);
  void emitStmt(const Stmt* stmt);
  void emitFunction(const FunctionDecl& fn);
  void emitType(const TypeDecl& type);
  std::string cppTypeFor(ValueType type) const;
  std::string emitExpr(const Expr* expr);
  ValueType inferType(const Expr* expr) const;
public:
  void generate(MainDecl*);
  std::string str() const;
};