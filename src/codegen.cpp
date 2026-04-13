#include "codegen.hpp"
#include <stdexcept>
#include <utility>

std::string CodeGen::cppTypeFor(ValueType type) const {
  if (type == ValueType::Int) return "int";
  if (type == ValueType::String) return "std::string";
  return "auto";
}

void CodeGen::emitBlock(const Block* block) {
  for (const auto& stmt : block->stmts) {
    emitStmt(stmt.get());
  }
}

void CodeGen::emitBlockScoped(const Block* block) {
  const auto previousSymbols = symbols;
  emitBlock(block);
  symbols = previousSymbols;
}

void CodeGen::emitStmt(const Stmt* stmt) {
  if (const auto* var = dynamic_cast<const VarDecl*>(stmt)) {
    ValueType type = inferType(var->value.get());
    symbols[var->name] = type;
    out << cppTypeFor(type) << " " << var->name << " = " << emitExpr(var->value.get()) << ";\n";
    return;
  }

  if (const auto* outStmt = dynamic_cast<const Out*>(stmt)) {
    out << "std::cout << " << emitExpr(outStmt->value.get()) << " << std::endl;\n";
    return;
  }

  if (const auto* input = dynamic_cast<const In*>(stmt)) {
    auto it = symbols.find(input->name);
    if (it == symbols.end()) {
      symbols[input->name] = ValueType::String;
      out << "std::string " << input->name << ";\n";
    }
    out << "std::getline(std::cin, " << input->name << ");\n";
    return;
  }

  if (const auto* assign = dynamic_cast<const Assign*>(stmt)) {
    if (symbols.find(assign->name) == symbols.end()) {
      throw std::runtime_error("Assignment to undeclared variable: " + assign->name);
    }
    out << assign->name << " = " << emitExpr(assign->value.get()) << ";\n";
    return;
  }

  if (const auto* ifStmt = dynamic_cast<const IfStmt*>(stmt)) {
    out << "if (" << emitExpr(ifStmt->condition.get()) << ") {\n";
    emitBlockScoped(ifStmt->thenBlock.get());
    out << "}\n";
    if (ifStmt->elseBlock) {
      out << "else {\n";
      emitBlockScoped(ifStmt->elseBlock.get());
      out << "}\n";
    }
    return;
  }

  if (const auto* whileStmt = dynamic_cast<const WhileStmt*>(stmt)) {
    out << "while (" << emitExpr(whileStmt->condition.get()) << ") {\n";
    emitBlockScoped(whileStmt->body.get());
    out << "}\n";
    return;
  }

  if (const auto* ret = dynamic_cast<const ReturnStmt*>(stmt)) {
    out << "return " << emitExpr(ret->value.get()) << ";\n";
    return;
  }

  if (const auto* obj = dynamic_cast<const ObjDecl*>(stmt)) {
    symbols[obj->varName] = ValueType::Unknown;
    if (obj->args.empty()) {
      // Avoid C++ "most vexing parse": `Type x();` becomes a function declaration.
      out << obj->typeName << " " << obj->varName << "{};\n";
      return;
    }
    out << obj->typeName << " " << obj->varName << "(";
    for (size_t i = 0; i < obj->args.size(); ++i) {
      if (i) out << ", ";
      out << emitExpr(obj->args[i].get());
    }
    out << ");\n";
    return;
  }

  throw std::runtime_error("Unknown statement type in code generator.");
}

void CodeGen::emitFunction(const FunctionDecl& fn) {
  out << "auto " << fn.name << "(";
  for (size_t i = 0; i < fn.params.size(); ++i) {
    if (i) out << ", ";
    out << "auto " << fn.params[i];
  }
  out << "){\n";
  for (const auto& param : fn.params) {
    symbols[param] = ValueType::Unknown;
  }
  emitBlockScoped(fn.body.get());
  out << "}\n\n";
}

void CodeGen::emitType(const TypeDecl& type) {
  out << (type.isClass ? "class " : "struct ") << type.name << " {\n";
  out << "public:\n";
  currentTypeName = type.name;

  for (const auto& field : type.fields) {
    ValueType fieldType = inferType(field.init.get());
    symbols[field.name] = fieldType;
    out << "  " << cppTypeFor(fieldType) << " " << field.name << " = " << emitExpr(field.init.get()) << ";\n";
  }

  if (type.ctor.exists) {
    out << "  " << type.name << "(";
    for (size_t i = 0; i < type.ctor.params.size(); ++i) {
      if (i) out << ", ";
      out << "auto " << type.ctor.params[i];
    }
    out << "){\n";
    for (const auto& p : type.ctor.params) {
      symbols[p] = ValueType::Unknown;
    }
    emitBlockScoped(type.ctor.body.get());
    out << "  }\n";
  } else {
    out << "  " << type.name << "() = default;\n";
  }

  if (type.dtor.exists) {
    out << "  ~" << type.name << "(){\n";
    emitBlockScoped(type.dtor.body.get());
    out << "  }\n";
  }

  out << "};\n\n";
  currentTypeName.clear();
}

std::string CodeGen::emitExpr(const Expr* expr) {
  if (const auto* number = dynamic_cast<const NumberExpr*>(expr)) {
    return std::to_string(number->value);
  }

  if (const auto* text = dynamic_cast<const StringExpr*>(expr)) {
    return "\"" + text->value + "\"";
  }

  if (const auto* ref = dynamic_cast<const VarRef*>(expr)) {
    return ref->name;
  }

  if (const auto* call = dynamic_cast<const CallExpr*>(expr)) {
    std::string result = call->callee + "(";
    for (size_t i = 0; i < call->args.size(); ++i) {
      if (i) result += ", ";
      result += emitExpr(call->args[i].get());
    }
    result += ")";
    return result;
  }

  if (const auto* bin = dynamic_cast<const BinaryExpr*>(expr)) {
    return "(" + emitExpr(bin->left.get()) + " " + bin->op + " " + emitExpr(bin->right.get()) + ")";
  }

  throw std::runtime_error("Unknown expression type in code generator.");
}

ValueType CodeGen::inferType(const Expr* expr) const {
  if (dynamic_cast<const NumberExpr*>(expr)) return ValueType::Int;
  if (dynamic_cast<const StringExpr*>(expr)) return ValueType::String;

  if (const auto* ref = dynamic_cast<const VarRef*>(expr)) {
    auto it = symbols.find(ref->name);
    if (it != symbols.end()) return it->second;
    return ValueType::Unknown;
  }

  if (const auto* bin = dynamic_cast<const BinaryExpr*>(expr)) {
    const ValueType left = inferType(bin->left.get());
    const ValueType right = inferType(bin->right.get());
    if (left == ValueType::String || right == ValueType::String) {
      return ValueType::String;
    }
    return ValueType::Int;
  }

  if (dynamic_cast<const CallExpr*>(expr)) return ValueType::Unknown;
  return ValueType::Unknown;
}

void CodeGen::generate(MainDecl* program){
  out << "#include <iostream>\n";
  out << "#include <string>\n\n";
  for (const auto& type : program->types) {
    emitType(type);
  }
  for (const auto& fn : program->functions) {
    emitFunction(fn);
  }
  out << "int main(){\n";
  out << "std::cout.setf(std::ios::boolalpha);\n";
  emitBlock(program->block.get());
  out << "return 0;\n";
  out << "}\n";
}

std::string CodeGen::str() const{ return out.str(); }