#pragma once
#include <string>

enum class TokenType {
  VoidMain,
  Val,
  Set,
  Obj,
  If,
  Else,
  While,
  Func,
  Return,
  Struct,
  Class,
  Ctor,
  Dtor,
  CompOut,
  CompIn,
  Identifier,
  Number,
  String,
  Plus,
  Minus,
  Star,
  Slash,
  Equal,
  EqualEqual,
  BangEqual,
  Less,
  LessEqual,
  Greater,
  GreaterEqual,
  Comma,
  LParen,
  RParen,
  LBracket,
  RBracket,
  End
};

struct Token {
  TokenType type;
  std::string text;
  int line = 1;
};