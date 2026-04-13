# Syux V9

## Philosophy
Simple like Java, low-level awareness like C++

## Why Syux?

- No manual memory management
- Clean, readable syntax
- C++ performance via transpilation
- Deterministic destructors (RAII-like)

## Features
- val variables (int + string)
- set reassignment
- obj declarations (stack object construction, no malloc/free)
- comp.out printing
- comp.in input (line-based string input)
- arithmetic expressions (+, -, *, /)
- comparison operators (==, !=, <, <=, >, >=)
- if/else blocks
- while loops
- top-level functions (`func`) with params and `return`
- top-level `struct` and `class` declarations
- class `ctor(...)` and `dtor()` blocks
- block scopes
- transpiles to C++20

## Vocabulary (Language Reference)

Use this as the canonical Syux vocabulary list.

### Program Entry
- `void.main [ ... ]`  
  Required program entry block.

### Variable and Object Statements
- `val name = expr`  
  Declare variable with inferred type (`int` or `string`).
- `set name = expr`  
  Reassign existing variable.
- `obj TypeName varName(args)`  
  Create stack object (no heap allocation, no `new`).

### I/O Statements
- `comp.out expr`  
  Print expression.
- `comp.in name`  
  Read one line of input into variable.

### Control Flow
- `if (condition) [ ... ] else [ ... ]`  
  Conditional execution.
- `while (condition) [ ... ]`  
  Loop while condition is true.
- `return expr`  
  Return from `func`.

### Top-Level Declarations
- `func name(a, b, ...) [ ... ]`  
  Function declaration with parameters.
- `struct Name [ ... ]`  
  Struct declaration.
- `class Name [ ... ]`  
  Class declaration.

### Type Members
- `val field = expr`  
  Field declaration inside `struct`/`class`.
- `ctor(args) [ ... ]`  
  Constructor block.
- `dtor() [ ... ]`  
  Destructor block.

### Expressions
- Literals: `123`, `"text"`
- Variable refs: `name`
- Function calls: `name(arg1, arg2)`
- Grouping: `(expr)`
- Arithmetic: `+`, `-`, `*`, `/`
- Comparison: `==`, `!=`, `<`, `<=`, `>`, `>=`

### Comments
- `# comment text`  
  Line comment.

## CLI (V10)

Build the Syux compiler:

```bash
g++ -std=c++20 -I include src/main.cpp src/parser.cpp src/scanner.cpp src/codegen.cpp -o syux
```

Run Syux source:

```bash
syux run example.syux
```

Build only (generate + compile C++ output, no execution):

```bash
syux build example.syux
```

## Error Reporting

- Scanner and parser now report line-aware errors.
- Format:

```text
[Syux Error] Line 3: Expected expression after '='.
```

## Example
func add(a, b) [
  return a + b
]

class Person [
  val name = "unknown"
  val age = 0
  ctor(n, a) [
    set name = n
    set age = a
  ]
  dtor() [
    comp.out "Person destroyed"
  ]
]

void.main [
  val i = 0
  while (i < 3) [
    comp.out i
    set i = i + 1
  ]

  val total = add(3, 4)
  comp.out total

  obj Person user("Alex", 16)
]

## Build
mkdir build && cd build
cmake ..
make