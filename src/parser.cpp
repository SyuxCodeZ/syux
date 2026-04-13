#include "parser.hpp"
#include <stdexcept>

Parser::Parser(std::vector<Token> t):tokens(t){}

[[noreturn]] void Parser::failHere(const std::string& message) const {
  const int line = current().line;
  throw std::runtime_error("[Syux Error] Line " + std::to_string(line) + ": " + message);
}

const Token& Parser::current() const {
  return tokens[pos];
}

bool Parser::isAtEnd() const {
  return current().type == TokenType::End;
}

bool Parser::check(TokenType type) const {
  if (isAtEnd()) return type == TokenType::End;
  return current().type == type;
}

bool Parser::match(TokenType type) {
  if (check(type)) {
    ++pos;
    return true;
  }
  return false;
}

bool Parser::isExpressionStart() const {
  return check(TokenType::Number) || check(TokenType::String) ||
    check(TokenType::Identifier) || check(TokenType::LParen);
}

const Token& Parser::consume(TokenType type, const char* message) {
  if (!check(type)) {
    failHere(message);
  }
  return tokens[pos++];
}

std::unique_ptr<Block> Parser::parseBlock() {
  consume(TokenType::LBracket, "Expected '[' to start block.");
  auto block = std::make_unique<Block>();

  while (!check(TokenType::RBracket) && !isAtEnd()) {
    block->stmts.push_back(parseStatement());
  }

  consume(TokenType::RBracket, "Expected ']' to close block.");
  return block;
}

std::unique_ptr<Stmt> Parser::parseStatement() {
  if (check(TokenType::Val)) return parseVarDecl();
  if (check(TokenType::Set)) return parseAssign();
  if (check(TokenType::Obj)) return parseObjDecl();
  if (check(TokenType::CompOut)) return parseOut();
  if (check(TokenType::CompIn)) return parseIn();
  if (check(TokenType::If)) return parseIf();
  if (check(TokenType::While)) return parseWhile();
  if (check(TokenType::Return)) return parseReturn();
  failHere("Expected statement (val, set, obj, comp.out, comp.in, if, while, return).");
}

std::unique_ptr<Stmt> Parser::parseVarDecl() {
  consume(TokenType::Val, "Expected 'val'.");
  const Token& name = consume(TokenType::Identifier, "Expected variable name.");
  consume(TokenType::Equal, "Expected '=' in variable declaration.");
  if (!isExpressionStart()) {
    failHere("Expected expression after '='.");
  }

  auto stmt = std::make_unique<VarDecl>();
  stmt->name = name.text;
  stmt->value = parseExpression();
  return stmt;
}

std::unique_ptr<Stmt> Parser::parseAssign() {
  consume(TokenType::Set, "Expected 'set'.");
  const Token& name = consume(TokenType::Identifier, "Expected variable name.");
  consume(TokenType::Equal, "Expected '=' in assignment.");
  if (!isExpressionStart()) {
    failHere("Expected expression after '='.");
  }

  auto stmt = std::make_unique<Assign>();
  stmt->name = name.text;
  stmt->value = parseExpression();
  return stmt;
}

std::unique_ptr<Stmt> Parser::parseObjDecl() {
  consume(TokenType::Obj, "Expected 'obj'.");
  const Token& typeName = consume(TokenType::Identifier, "Expected type name.");
  const Token& varName = consume(TokenType::Identifier, "Expected variable name.");
  consume(TokenType::LParen, "Expected '(' after object variable name.");

  auto stmt = std::make_unique<ObjDecl>();
  stmt->typeName = typeName.text;
  stmt->varName = varName.text;

  if (!check(TokenType::RParen)) {
    stmt->args.push_back(parseExpression());
    while (match(TokenType::Comma)) {
      stmt->args.push_back(parseExpression());
    }
  }

  consume(TokenType::RParen, "Expected ')' after object arguments.");
  return stmt;
}

std::unique_ptr<Stmt> Parser::parseOut() {
  consume(TokenType::CompOut, "Expected 'comp.out'.");

  auto stmt = std::make_unique<Out>();
  stmt->value = parseExpression();
  return stmt;
}

std::unique_ptr<Stmt> Parser::parseIn() {
  consume(TokenType::CompIn, "Expected 'comp.in'.");
  const Token& name = consume(TokenType::Identifier, "Expected variable name.");
  auto stmt = std::make_unique<In>();
  stmt->name = name.text;
  return stmt;
}

std::unique_ptr<Stmt> Parser::parseIf() {
  consume(TokenType::If, "Expected 'if'.");
  consume(TokenType::LParen, "Expected '(' after if.");
  auto condition = parseExpression();
  consume(TokenType::RParen, "Expected ')' after if condition.");
  auto thenBlock = parseBlock();

  std::unique_ptr<Block> elseBlock;
  if (match(TokenType::Else)) {
    elseBlock = parseBlock();
  }

  auto stmt = std::make_unique<IfStmt>();
  stmt->condition = std::move(condition);
  stmt->thenBlock = std::move(thenBlock);
  stmt->elseBlock = std::move(elseBlock);
  return stmt;
}

std::unique_ptr<Stmt> Parser::parseWhile() {
  consume(TokenType::While, "Expected 'while'.");
  consume(TokenType::LParen, "Expected '(' after while.");
  auto condition = parseExpression();
  consume(TokenType::RParen, "Expected ')' after while condition.");
  auto body = parseBlock();

  auto stmt = std::make_unique<WhileStmt>();
  stmt->condition = std::move(condition);
  stmt->body = std::move(body);
  return stmt;
}

std::unique_ptr<Stmt> Parser::parseReturn() {
  consume(TokenType::Return, "Expected 'return'.");
  if (!isExpressionStart()) {
    failHere("Expected expression after 'return'.");
  }
  auto stmt = std::make_unique<ReturnStmt>();
  stmt->value = parseExpression();
  return stmt;
}

FunctionDecl Parser::parseFunction() {
  consume(TokenType::Func, "Expected 'func'.");
  FunctionDecl fn;
  fn.name = consume(TokenType::Identifier, "Expected function name.").text;
  consume(TokenType::LParen, "Expected '(' after function name.");
  if (!check(TokenType::RParen)) {
    fn.params.push_back(consume(TokenType::Identifier, "Expected parameter name.").text);
    while (match(TokenType::Comma)) {
      fn.params.push_back(consume(TokenType::Identifier, "Expected parameter name.").text);
    }
  }
  consume(TokenType::RParen, "Expected ')' after parameters.");
  fn.body = parseBlock();
  return fn;
}

TypeDecl Parser::parseType(bool isClass) {
  if (isClass) consume(TokenType::Class, "Expected 'class'.");
  else consume(TokenType::Struct, "Expected 'struct'.");

  TypeDecl type;
  type.isClass = isClass;
  type.name = consume(TokenType::Identifier, "Expected type name.").text;
  consume(TokenType::LBracket, "Expected '[' after type name.");

  while (!check(TokenType::RBracket) && !isAtEnd()) {
    if (check(TokenType::Val)) {
      consume(TokenType::Val, "Expected 'val'.");
      FieldDecl field;
      field.name = consume(TokenType::Identifier, "Expected field name.").text;
      consume(TokenType::Equal, "Expected '=' in field declaration.");
      field.init = parseExpression();
      type.fields.push_back(std::move(field));
      continue;
    }

    if (check(TokenType::Ctor)) {
      consume(TokenType::Ctor, "Expected 'ctor'.");
      type.ctor.exists = true;
      consume(TokenType::LParen, "Expected '(' after ctor.");
      if (!check(TokenType::RParen)) {
        type.ctor.params.push_back(consume(TokenType::Identifier, "Expected ctor parameter.").text);
        while (match(TokenType::Comma)) {
          type.ctor.params.push_back(consume(TokenType::Identifier, "Expected ctor parameter.").text);
        }
      }
      consume(TokenType::RParen, "Expected ')' after ctor params.");
      type.ctor.body = parseBlock();
      continue;
    }

    if (check(TokenType::Dtor)) {
      consume(TokenType::Dtor, "Expected 'dtor'.");
      type.dtor.exists = true;
      consume(TokenType::LParen, "Expected '(' after dtor.");
      consume(TokenType::RParen, "Expected ')' after dtor.");
      type.dtor.body = parseBlock();
      continue;
    }

    failHere("Expected type member (val, ctor, dtor).");
  }

  consume(TokenType::RBracket, "Expected ']' to close type declaration.");
  return type;
}

std::unique_ptr<Expr> Parser::parseExpression() {
  return parseComparison();
}

std::unique_ptr<Expr> Parser::parseComparison() {
  auto left = parseAddition();

  while (check(TokenType::EqualEqual) || check(TokenType::BangEqual) ||
    check(TokenType::Less) || check(TokenType::LessEqual) ||
    check(TokenType::Greater) || check(TokenType::GreaterEqual)) {
    const Token op = tokens[pos++];
    auto expr = std::make_unique<BinaryExpr>();
    expr->op = op.text;
    expr->left = std::move(left);
    expr->right = parseAddition();
    left = std::move(expr);
  }

  return left;
}

std::unique_ptr<Expr> Parser::parseAddition() {
  auto left = parseMultiplication();

  while (check(TokenType::Plus) || check(TokenType::Minus)) {
    const Token op = tokens[pos++];
    auto expr = std::make_unique<BinaryExpr>();
    expr->op = op.text;
    expr->left = std::move(left);
    expr->right = parseMultiplication();
    left = std::move(expr);
  }

  return left;
}

std::unique_ptr<Expr> Parser::parseMultiplication() {
  auto left = parsePrimary();

  while (check(TokenType::Star) || check(TokenType::Slash)) {
    const Token op = tokens[pos++];
    auto expr = std::make_unique<BinaryExpr>();
    expr->op = op.text;
    expr->left = std::move(left);
    expr->right = parsePrimary();
    left = std::move(expr);
  }

  return left;
}

std::unique_ptr<Expr> Parser::parsePrimary() {
  if (check(TokenType::Number)) {
    const Token token = consume(TokenType::Number, "Expected number.");
    auto expr = std::make_unique<NumberExpr>();
    expr->value = std::stoi(token.text);
    return expr;
  }

  if (check(TokenType::String)) {
    const Token token = consume(TokenType::String, "Expected string.");
    auto expr = std::make_unique<StringExpr>();
    expr->value = token.text;
    return expr;
  }

  if (check(TokenType::Identifier)) {
    const Token token = consume(TokenType::Identifier, "Expected identifier.");
    if (match(TokenType::LParen)) {
      auto call = std::make_unique<CallExpr>();
      call->callee = token.text;
      if (!check(TokenType::RParen)) {
        call->args.push_back(parseExpression());
        while (match(TokenType::Comma)) {
          call->args.push_back(parseExpression());
        }
      }
      consume(TokenType::RParen, "Expected ')' after function call.");
      return call;
    }
    auto var = std::make_unique<VarRef>();
    var->name = token.text;
    return var;
  }

  if (match(TokenType::LParen)) {
    auto expr = parseExpression();
    consume(TokenType::RParen, "Expected ')' after expression.");
    return expr;
  }

  failHere("Expected expression.");
}

std::unique_ptr<MainDecl> Parser::parse(){
  auto m = std::make_unique<MainDecl>();
  while (!check(TokenType::VoidMain) && !isAtEnd()) {
    if (check(TokenType::Func)) {
      m->functions.push_back(parseFunction());
      continue;
    }
    if (check(TokenType::Struct)) {
      m->types.push_back(parseType(false));
      continue;
    }
    if (check(TokenType::Class)) {
      m->types.push_back(parseType(true));
      continue;
    }
    failHere("Expected top-level declaration (func, struct, class, or void.main).");
  }

  consume(TokenType::VoidMain, "Expected 'void.main' entry point.");
  m->block = parseBlock();
  consume(TokenType::End, "Unexpected tokens after program end.");
  return m;
}