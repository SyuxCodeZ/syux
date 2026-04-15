#include "codegen.hpp"
#include <stdexcept>

std::string CodeGen::indent() const {
  return std::string(static_cast<size_t>(indentLevel) * 2, ' ');
}

std::string CodeGen::inferArrayElementType(const ArrayLiteralNode* arr) const {
  if (arr->elements.empty()) return "int";
  const std::string first = inferCppType(arr->elements.front().get());
  if (first == "bool") return "bool";
  if (first == "std::string") return "std::string";
  if (first == "double") return "double";
  return "int";
}

std::string CodeGen::extractVectorElementType(const std::string& vecType) const {
  if (vecType.rfind("std::vector<", 0) == 0) {
    size_t start = 12; // length of "std::vector<"
    size_t end = vecType.rfind(">");
    if (end != std::string::npos && end > start) {
      return vecType.substr(start, end - start);
    }
  }
  return "auto";
}

std::string CodeGen::inferCppType(const ExprNode* expr) const {
  if (const auto* literal = dynamic_cast<const LiteralNode*>(expr)) {
    if (literal->kind == LiteralNode::Kind::Bool) return "bool";
    if (literal->kind == LiteralNode::Kind::String) return "std::string";
    if (literal->kind == LiteralNode::Kind::Float) return "double";
    return "int";
  }
  if (const auto* ref = dynamic_cast<const VariableNode*>(expr)) {
    auto it = symbolTable.find(ref->name);
    if (it != symbolTable.end()) return it->second;
  }
  if (const auto* binary = dynamic_cast<const BinaryExprNode*>(expr)) {
    const std::string left = inferCppType(binary->left.get());
    const std::string right = inferCppType(binary->right.get());
    if (left == "std::string" || right == "std::string") return "std::string";
    if (left == "double" || right == "double") return "double";
    if (left == "int" && right == "int") return "int";
  }
  if (const auto* arr = dynamic_cast<const ArrayLiteralNode*>(expr)) {
    return "std::vector<" + inferArrayElementType(arr) + ">";
  }
  if (const auto* idx = dynamic_cast<const IndexExprNode*>(expr)) {
    const std::string collectionType = inferCppType(idx->collection.get());
    if (collectionType == "std::vector<std::string>") return "std::string";
    if (collectionType == "std::vector<double>") return "double";
    if (collectionType == "std::vector<int>") return "int";
    if (collectionType == "std::vector<bool>") return "bool";
  }
  if (dynamic_cast<const PropertyAccessNode*>(expr)) {
    return "int";
  }
  return "auto";
}

std::string CodeGen::emitExpr(const ExprNode* expr) const {
  if (const auto* literal = dynamic_cast<const LiteralNode*>(expr)) {
    if (literal->kind == LiteralNode::Kind::String) return "\"" + literal->value + "\"";
    return literal->value;
  }
  if (const auto* ref = dynamic_cast<const VariableNode*>(expr)) {
    return ref->name;
  }
  if (const auto* call = dynamic_cast<const CallNode*>(expr)) {
    std::string result = call->callee + "(";
    for (size_t i = 0; i < call->args.size(); ++i) {
      if (i) result += ", ";
      result += emitExpr(call->args[i].get());
    }
    result += ")";
    return result;
  }
  if (const auto* binary = dynamic_cast<const BinaryExprNode*>(expr)) {
    return "(" + emitExpr(binary->left.get()) + " " + binary->op + " " + emitExpr(binary->right.get()) + ")";
  }
  if (const auto* arr = dynamic_cast<const ArrayLiteralNode*>(expr)) {
    std::string result = "{";
    for (size_t i = 0; i < arr->elements.size(); ++i) {
      if (i) result += ", ";
      result += emitExpr(arr->elements[i].get());
    }
    result += "}";
    return result;
  }
  if (const auto* idx = dynamic_cast<const IndexExprNode*>(expr)) {
    const std::string collection = emitExpr(idx->collection.get());
    const std::string index = emitExpr(idx->index.get());
    return "([&](){ int __idx = " + index + "; if(__idx >= 0 && __idx < static_cast<int>(" + collection +
      ".size())) return " + collection + "[__idx]; throw std::runtime_error(\"Index out of bounds\"); }())";
  }
  if (const auto* access = dynamic_cast<const PropertyAccessNode*>(expr)) {
    if (access->property == "len") {
      return "static_cast<int>(" + emitExpr(access->object.get()) + ".size())";
    }
    throw std::runtime_error("[Syux Error] Unsupported property access: " + access->property);
  }
  throw std::runtime_error("[Syux Error] Unknown expression node in codegen.");
}

std::string CodeGen::emitInlineStatement(const StmtNode* stmt) const {
  if (const auto* var = dynamic_cast<const VarDeclNode*>(stmt)) {
    return inferCppType(var->value.get()) + " " + var->name + " = " + emitExpr(var->value.get());
  }
  if (const auto* assign = dynamic_cast<const AssignmentNode*>(stmt)) {
    return assign->name + " = " + emitExpr(assign->value.get());
  }
  if (const auto* inc = dynamic_cast<const IncrementNode*>(stmt)) {
    return inc->name + "++";
  }
  throw std::runtime_error("[Syux Error] Invalid statement in inline context (for loop init/update).");
}

void CodeGen::emitStatement(const StmtNode* stmt) {
  if (const auto* var = dynamic_cast<const VarDeclNode*>(stmt)) {
    const std::string type = inferCppType(var->value.get());
    symbolTable[var->name] = type;
    out << indent() << type << " " << var->name << " = " << emitExpr(var->value.get()) << ";\n";
    return;
  }
  if (const auto* assign = dynamic_cast<const AssignmentNode*>(stmt)) {
    auto it = symbolTable.find(assign->name);
    if (it == symbolTable.end()) {
      throw std::runtime_error("[Syux Error] Undefined variable '" + assign->name + "'");
    }
    const std::string varType = it->second;
    const std::string exprType = inferCppType(assign->value.get());
    if (varType != exprType && varType != "auto" && exprType != "auto") {
      bool mismatch = false;
      if ((varType == "int" || varType == "double") && (exprType == "std::string" || exprType == "bool")) mismatch = true;
      if (varType == "std::string" && (exprType == "int" || exprType == "double" || exprType == "bool")) mismatch = true;
      if (varType == "bool" && exprType != "bool") mismatch = true;
      if (mismatch) {
        throw std::runtime_error("[Syux Error] Type mismatch in assignment to '" + assign->name +
                                 "': expected " + varType + ", got " + exprType);
      }
    }
    out << indent() << assign->name << " = " << emitExpr(assign->value.get()) << ";\n";
    return;
  }
  if (const auto* assign = dynamic_cast<const IndexAssignmentNode*>(stmt)) {
    out << indent() << "do {\n";
    ++indentLevel;
    out << indent() << "int __idx = " << emitExpr(assign->index.get()) << ";\n";
    out << indent() << "if(__idx < 0 || __idx >= static_cast<int>(" << assign->name << ".size())) throw std::runtime_error(\"Index out of bounds\");\n";
    out << indent() << assign->name << "[__idx] = " << emitExpr(assign->value.get()) << ";\n";
    --indentLevel;
    out << indent() << "} while(false);\n";
    return;
  }
  if (const auto* inc = dynamic_cast<const IncrementNode*>(stmt)) {
    out << indent() << inc->name << "++;\n";
    return;
  }
  if (const auto* output = dynamic_cast<const OutputNode*>(stmt)) {
    out << indent() << "std::cout << " << emitExpr(output->value.get()) << " << std::endl;\n";
    return;
  }
  if (const auto* input = dynamic_cast<const InputNode*>(stmt)) {
    auto it = symbolTable.find(input->name);
    if (it == symbolTable.end()) {
      symbolTable[input->name] = "std::string";
      out << indent() << "std::string " << input->name << ";\n";
      out << indent() << "std::getline(std::cin, " << input->name << ");\n";
    } else if (it->second == "std::string") {
      out << indent() << "std::getline(std::cin, " << input->name << ");\n";
    } else if (it->second == "int" || it->second == "double") {
      out << indent() << "std::cin >> " << input->name << ";\n";
      out << indent() << "std::cin.ignore();\n";
    } else if (it->second == "bool") {
      out << indent() << "{\n";
      ++indentLevel;
      out << indent() << "std::string __boolInput;\n";
      out << indent() << "std::getline(std::cin, __boolInput);\n";
      out << indent() << input->name << " = (__boolInput == \"true\" || __boolInput == \"on\" || __boolInput == \"1\");\n";
      --indentLevel;
      out << indent() << "}\n";
    } else {
      throw std::runtime_error("[Syux Error] Cannot read input into variable '" + input->name + "' of type " + it->second);
    }
    return;
  }
  if (const auto* obj = dynamic_cast<const ObjectDeclNode*>(stmt)) {
    symbolTable[obj->varName] = obj->typeName;
    if (obj->args.empty()) {
      out << indent() << obj->typeName << " " << obj->varName << "{};\n";
      return;
    }
    out << indent() << obj->typeName << " " << obj->varName << "(";
    for (size_t i = 0; i < obj->args.size(); ++i) {
      if (i) out << ", ";
      out << emitExpr(obj->args[i].get());
    }
    out << ");\n";
    return;
  }
  if (const auto* ifNode = dynamic_cast<const IfNode*>(stmt)) {
    out << indent() << "if (" << emitExpr(ifNode->condition.get()) << ") {\n";
    ++indentLevel;
    emitBlock(ifNode->thenBlock.get());
    --indentLevel;
    out << indent() << "}";
    if (ifNode->elseBlock) {
      out << " else {\n";
      ++indentLevel;
      emitBlock(ifNode->elseBlock.get());
      --indentLevel;
      out << indent() << "}\n";
    } else {
      out << "\n";
    }
    return;
  }
  if (const auto* whileNode = dynamic_cast<const WhileNode*>(stmt)) {
    out << indent() << "while (" << emitExpr(whileNode->condition.get()) << ") {\n";
    ++indentLevel;
    emitBlock(whileNode->body.get());
    --indentLevel;
    out << indent() << "}\n";
    return;
  }
  if (const auto* forNode = dynamic_cast<const ForNode*>(stmt)) {
    out << indent() << "for (" << emitInlineStatement(forNode->init.get()) << "; "
      << emitExpr(forNode->condition.get()) << "; " << emitInlineStatement(forNode->update.get()) << ") {\n";
    ++indentLevel;
    emitBlock(forNode->body.get());
    --indentLevel;
    out << indent() << "}\n";
    return;
  }
  if (const auto* each = dynamic_cast<const ForEachNode*>(stmt)) {
    const std::string iterableType = inferCppType(each->iterable.get());
    const std::string elemType = extractVectorElementType(iterableType);
    out << indent() << "for (" << elemType << " " << each->varName << " : " << emitExpr(each->iterable.get()) << ") {\n";
    ++indentLevel;
    symbolTable[each->varName] = elemType;
    emitBlock(each->body.get());
    --indentLevel;
    out << indent() << "}\n";
    return;
  }
  if (const auto* ret = dynamic_cast<const ReturnNode*>(stmt)) {
    out << indent() << "return " << emitExpr(ret->value.get()) << ";\n";
    return;
  }
  throw std::runtime_error("[Syux Error] Unknown statement node in codegen.");
}

void CodeGen::emitBlock(const BlockNode* block) {
  const auto previousSymbols = symbolTable;
  for (const auto& stmt : block->statements) {
    emitStatement(stmt.get());
  }
  symbolTable = previousSymbols;
}

void CodeGen::emitFunction(const FunctionNode* fn) {
  out << "auto " << fn->name << "(";
  for (size_t i = 0; i < fn->params.size(); ++i) {
    if (i) out << ", ";
    out << "auto " << fn->params[i];
  }
  out << ") {\n";
  ++indentLevel;
  for (const auto& param : fn->params) {
    symbolTable[param] = "auto";
  }
  emitBlock(fn->body.get());
  --indentLevel;
  out << "}\n\n";
}

void CodeGen::emitStruct(const StructNode* node) {
  out << "struct " << node->name << " {\n";
  out << "public:\n";
  ++indentLevel;
  for (const auto& field : node->fields) {
    const std::string type = inferCppType(field->value.get());
    symbolTable[field->name] = type;
    out << indent() << type << " " << field->name << " = " << emitExpr(field->value.get()) << ";\n";
  }
  out << indent() << node->name << "() = default;\n";
  --indentLevel;
  out << "};\n\n";
}

void CodeGen::emitClass(const ClassNode* node) {
  out << "class " << node->name << " {\n";
  out << "public:\n";
  ++indentLevel;
  for (const auto& field : node->fields) {
    const std::string type = inferCppType(field->value.get());
    symbolTable[field->name] = type;
    out << indent() << type << " " << field->name << " = " << emitExpr(field->value.get()) << ";\n";
  }

  if (node->ctor) {
    out << indent() << node->name << "(";
    for (size_t i = 0; i < node->ctor->params.size(); ++i) {
      if (i) out << ", ";
      out << "auto " << node->ctor->params[i];
    }
    out << ") {\n";
    ++indentLevel;
    for (const auto& param : node->ctor->params) {
      symbolTable[param] = "auto";
    }
    emitBlock(node->ctor->body.get());
    --indentLevel;
    out << indent() << "}\n";
  } else {
    out << indent() << node->name << "() = default;\n";
  }

  if (node->dtor) {
    out << indent() << "~" << node->name << "() {\n";
    ++indentLevel;
    emitBlock(node->dtor->body.get());
    --indentLevel;
    out << indent() << "}\n";
  }
  --indentLevel;
  out << "};\n\n";
}

void CodeGen::generate(const ProgramNode* program){
  out.str("");
  out.clear();
  symbolTable.clear();
  indentLevel = 0;

  out << "#include <iostream>\n";
  out << "#include <string>\n\n";
  out << "#include <vector>\n\n";
  out << "#include <stdexcept>\n\n";

  for (const auto& node : program->structs) emitStruct(node.get());
  for (const auto& node : program->classes) emitClass(node.get());
  for (const auto& fn : program->functions) emitFunction(fn.get());

  out << "int main() {\n";
  ++indentLevel;
  out << indent() << "std::cout.setf(std::ios::boolalpha);\n";
  emitBlock(program->mainBlock.get());
  out << indent() << "return 0;\n";
  --indentLevel;
  out << "}\n";
}

std::string CodeGen::str() const { return out.str(); }