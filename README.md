# Syux V12

## Philosophy
Simple like Java, low-level awareness like C++

## Why Syux?

- No manual memory management
- Clean, readable syntax
- C++ performance via transpilation
- Deterministic destructors (RAII-like)

## Features
- val variables (int + string + bool)
- set reassignment
- obj declarations (stack object construction, no malloc/free)
- comp.out printing
- comp.in input (line-based string input)
- array literals and indexing (`[1, 2, 3]`, `nums[0]`)
- array length property (`nums.len`)
- for-each loops (`for val x in nums [ ... ]`)
- arithmetic expressions (+, -, *, /)
- comparison operators (==, !=, <, <=, >, >=)
- if/else blocks
- while loops
- C-style for loops (`for {val i = 0 : i < n : i++} [ ... ]`)
- top-level functions (`func`) with params and `return`
- top-level `struct` and `class` declarations
- class `ctor(...)` and `dtor()` blocks
- runtime array bounds checking in generated C++
- mixed-type array rejection with line-aware compile errors
- block scopes
- transpiles to C++20

## Compiler Architecture (V12)

Syux now uses a clean pipeline:

`Scanner -> Parser -> AST -> CodeGen -> C++`

- `scanner.cpp` tokenizes input with line metadata
- `parser.cpp` builds a `ProgramNode` AST
- `ast.h` defines expression/statement/top-level node types
- `codegen.cpp` traverses AST and emits formatted C++
- `main.cpp` provides `run` and `build` CLI commands

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
- `for {val i = 0 : i < 5 : i++} [ ... ]`  
  C-style loop syntax.
- `for val x in nums [ ... ]`  
  For-each loop over arrays.
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
- Literals: `123`, `"text"`, `true`, `false`
- Variable refs: `name`
- Function calls: `name(arg1, arg2)`
- Array access: `nums[0]`
- Property access: `nums.len`
- Grouping: `(expr)`
- Arithmetic: `+`, `-`, `*`, `/`
- Comparison: `==`, `!=`, `<`, `<=`, `>`, `>=`

### Comments
- `# comment text`  
  Line comment.

## CLI

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
- Array literals reject mixed element types.
- Format:

```text
[Syux Error] Line 3: Expected expression after '='.
```

Runtime safety:
- Generated C++ includes bounds checks for array index reads/writes.
- Out-of-range access throws: `Index out of bounds`.

## Quick Regression Tests

Use these sample files in `tests/`:

- `tests/ok_basic.syux` (happy path)
- `tests/error_missing_expr.syux` (missing expression)
- `tests/error_unclosed_block.syux` (block close error)

Example runs:

```bash
syux run tests/ok_basic.syux
syux run tests/error_missing_expr.syux
syux run tests/error_unclosed_block.syux
```

## Example
func add(a, b) [
  return a + b
]

func max2(a, b) [
  if (a >= b) [
    return a
  ] else [
    return b
  ]
]

struct Point [
  val x = 0
  val y = 0
]

class Person [
  val name = "unknown"
  val age = 0

  ctor(n, a) [
    set name = n
    set age = a
    comp.out "Person constructed"
  ]

  dtor() [
    comp.out "Person destructed"
  ]
]

void.main [
  comp.out "Syux V11 demo start"

  val sum = add(4, 5)
  comp.out sum

  val i = 0
  while (i < 4) [
    comp.out i
    set i = i + 1
  ]

  val nums = [10, 20, 30, 40]
  val flag = true
  if (flag) [
    comp.out "flag is true"
  ]

  comp.out nums[0]
  set nums[1] = 99
  comp.out nums[1]
  comp.out nums.len

  for {val j = 0 : j < 4 : j++} [
    comp.out nums[j]
  ]

  for val n in nums [
    comp.out n
  ]

  val who = "Alex"
  comp.out "Type your name:"
  comp.in who
  comp.out who

  val top = max2(sum, i)
  if (top > 5) [
    comp.out "Top is greater than 5"
  ] else [
    comp.out "Top is 5 or less"
  ]

  obj Person user("Alex", 16)
  obj Point p()

  comp.out "End of main is next; destructors run on scope exit"
  comp.out "No malloc/free needed: stack objects + ctor/dtor"
]

## Build
mkdir build && cd build
cmake ..
make