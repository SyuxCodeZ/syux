#pragma once
#include "ast.hpp"
#include "token.hpp"
#include <cstddef>
#include <vector>
#include <memory>
#include <string>

class Parser {
  std::vector<Token> tokens;
  size_t pos = 0;

  const Token& current() const;
  bool isAtEnd() const;
  bool check(TokenType type) const;
  bool match(TokenType type);
  bool isExpressionStart() const;
  const Token& consume(TokenType type, const char* message);
  [[noreturn]] void failHere(const std::string& message) const;

  std::unique_ptr<Block> parseBlock();
  std::unique_ptr<Stmt> parseStatement();
  std::unique_ptr<Stmt> parseVarDecl();
  std::unique_ptr<Stmt> parseAssign();
  std::unique_ptr<Stmt> parseObjDecl();
  std::unique_ptr<Stmt> parseOut();
  std::unique_ptr<Stmt> parseIn();
  std::unique_ptr<Stmt> parseIf();
  std::unique_ptr<Stmt> parseWhile();
  std::unique_ptr<Stmt> parseReturn();
  FunctionDecl parseFunction();
  TypeDecl parseType(bool isClass);
  std::unique_ptr<Expr> parseExpression();
  std::unique_ptr<Expr> parseComparison();
  std::unique_ptr<Expr> parseAddition();
  std::unique_ptr<Expr> parseMultiplication();
  std::unique_ptr<Expr> parsePrimary();
public:
  Parser(std::vector<Token>);
  std::unique_ptr<MainDecl> parse();
};