#pragma once

#include <memory>
#include <string>
#include <vector>

struct Node {
  virtual ~Node() = default;
};

struct ExprNode : Node {
  virtual ~ExprNode() = default;
};

struct LiteralNode : ExprNode {
  enum class Kind { Int, Float, String, Bool };
  Kind kind = Kind::Int;
  std::string value;
};

struct VariableNode : ExprNode {
  std::string name;
};

struct BinaryExprNode : ExprNode {
  std::string op;
  std::unique_ptr<ExprNode> left;
  std::unique_ptr<ExprNode> right;
};

struct CallNode : ExprNode {
  std::string callee;
  std::vector<std::unique_ptr<ExprNode>> args;
};

struct ArrayLiteralNode : ExprNode {
  std::vector<std::unique_ptr<ExprNode>> elements;
};

struct IndexExprNode : ExprNode {
  std::unique_ptr<ExprNode> collection;
  std::unique_ptr<ExprNode> index;
};

struct PropertyAccessNode : ExprNode {
  std::unique_ptr<ExprNode> object;
  std::string property;
};

struct StmtNode : Node {
  virtual ~StmtNode() = default;
};

struct BlockNode : StmtNode {
  std::vector<std::unique_ptr<StmtNode>> statements;
};

struct VarDeclNode : StmtNode {
  std::string name;
  std::unique_ptr<ExprNode> value;
};

struct AssignmentNode : StmtNode {
  std::string name;
  std::unique_ptr<ExprNode> value;
};

struct IndexAssignmentNode : StmtNode {
  std::string name;
  std::unique_ptr<ExprNode> index;
  std::unique_ptr<ExprNode> value;
};

struct IncrementNode : StmtNode {
  std::string name;
};

struct WhileNode : StmtNode {
  std::unique_ptr<ExprNode> condition;
  std::unique_ptr<BlockNode> body;
};

struct ForNode : StmtNode {
  std::unique_ptr<StmtNode> init;
  std::unique_ptr<ExprNode> condition;
  std::unique_ptr<StmtNode> update;
  std::unique_ptr<BlockNode> body;
};

struct ForEachNode : StmtNode {
  std::string varName;
  std::unique_ptr<ExprNode> iterable;
  std::unique_ptr<BlockNode> body;
};

struct IfNode : StmtNode {
  std::unique_ptr<ExprNode> condition;
  std::unique_ptr<BlockNode> thenBlock;
  std::unique_ptr<BlockNode> elseBlock;
};

struct ReturnNode : StmtNode {
  std::unique_ptr<ExprNode> value;
};

struct OutputNode : StmtNode {
  std::unique_ptr<ExprNode> value;
};

struct InputNode : StmtNode {
  std::string name;
};

struct ObjectDeclNode : StmtNode {
  std::string typeName;
  std::string varName;
  std::vector<std::unique_ptr<ExprNode>> args;
};

struct FunctionNode : Node {
  std::string name;
  std::vector<std::string> params;
  std::unique_ptr<BlockNode> body;
};

struct StructNode : Node {
  std::string name;
  std::vector<std::unique_ptr<VarDeclNode>> fields;
};

struct ClassNode : Node {
  struct CtorNode : Node {
    std::vector<std::string> params;
    std::unique_ptr<BlockNode> body;
  };
  struct DtorNode : Node {
    std::unique_ptr<BlockNode> body;
  };

  std::string name;
  std::vector<std::unique_ptr<VarDeclNode>> fields;
  std::unique_ptr<CtorNode> ctor;
  std::unique_ptr<DtorNode> dtor;
};

struct ProgramNode : Node {
  std::vector<std::unique_ptr<FunctionNode>> functions;
  std::vector<std::unique_ptr<StructNode>> structs;
  std::vector<std::unique_ptr<ClassNode>> classes;
  std::unique_ptr<BlockNode> mainBlock;
};
