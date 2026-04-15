#include "scanner.hpp"
#include "parser.hpp"
#include "codegen.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

namespace {
void printUsage() {
  std::cout << "Usage:\n";
  std::cout << "  syux run <file.syux>\n";
  std::cout << "  syux build <file.syux>\n";
}

bool transpileToCpp(const std::string& inputFile) {
  std::ifstream in(inputFile);
  if (!in) {
    std::cerr << "[Syux Error] Could not open file: " << inputFile << "\n";
    return false;
  }

  std::string src((std::istreambuf_iterator<char>(in)), {});
  std::cout << "[Syux] Parsing...\n";
  Scanner sc(src);
  auto tokens = sc.scan();

  Parser p(tokens);
  auto ast = p.parse();

  std::cout << "[Syux] Generating...\n";
  CodeGen g;
  g.generate(ast.get());
  std::ofstream out("out.cpp");
  out << g.str();
  return true;
}

int compileOutput() {
  std::cout << "[Syux] Compiling...\n";
#ifdef _WIN32
  return std::system("g++ -std=c++20 out.cpp -o out.exe");
#else
  return std::system("g++ -std=c++20 out.cpp -o out");
#endif
}

int runOutput() {
  std::cout << "[Syux] Running...\n";
#ifdef _WIN32
  return std::system(".\\out.exe");
#else
  return std::system("./out");
#endif
}
} // namespace

int main(int argc, char** argv){
  if (argc < 3) {
    printUsage();
    return 1;
  }

  const std::string command = argv[1];
  const std::string inputFile = argv[2];

  if (command != "run" && command != "build") {
    std::cerr << "[Syux Error] Unknown command: " << command << "\n";
    printUsage();
    return 1;
  }

  try {
    if (!transpileToCpp(inputFile)) return 1;
  } catch (const std::exception& e) {
    std::cerr << e.what() << "\n";
    return 1;
  }

  if (compileOutput() != 0) {
    std::cerr << "[Syux Error] C++ compilation failed.\n";
    return 1;
  }

  if (command == "run") {
    if (runOutput() != 0) {
      std::cerr << "[Syux Error] Program execution failed.\n";
      return 1;
    }
  }

  return 0;
}