# Syux

**Syux** is a learning-focused programming language designed to bridge the gap between Python and C/C++.

> Learn the logic before fighting the syntax.

---

## 🚀 What is Syux?

Syux is a transpiled language that converts Syux code into C++, combining:

* 🧠 Simple, readable syntax
* ⚙️ Real programming concepts (types, structure, RAII)
* 🚀 Native performance via C++

---

## 🎯 Why Syux?

Many beginners struggle when moving from Python to C/C++.

Syux solves this by introducing:

* Structured syntax (`val`, `set`)
* Type awareness (int, float, string, bool)
* Clean control flow
* Object lifecycle (constructors/destructors)

---

## 🧩 Example

```syux
void.main [
  val x = 5
  set x = x + 1
  comp.out x
]
```

---

## 🔥 Features (v0.4.0)

* ✅ Type inference (int, float, string, bool)
* ✅ Boolean aliases (`true/false`, `on/off`)
* ✅ Arrays with `.len`
* ✅ For, while, and for-each loops
* ✅ Functions and classes (RAII support)
* ✅ Type-safe assignments
* ✅ Smart input system (`comp.in`)
* ✅ Transpiles to C++

---

## ⚙️ How it works

```text
Syux → C++ → Executable
```

Syux generates C++ code, which is then compiled using a C++ compiler (e.g., g++).

---

## 🛠️ Getting Started

### 1. Clone the repository

```bash
git clone https://github.com/SyuxCodeZ/syux.git
cd syux
```

---

### 2. Build the compiler

(Instructions depend on your setup, e.g., CMake or g++)

---

### 3. Run a program

```bash
./scripts/run_syux.ps1 -SourceFile example.syux
```

---

## 📘 Example: Type System

```syux
val x = 42          # int
val pi = 3.14       # float
val name = "sid"    # string
val flag = on       # bool
```

---

## 📦 Example: Arrays

```syux
val nums = [10, 20, 30]
comp.out nums.len

set nums[1] = 99
comp.out nums[1]
```

---

## 🔁 Example: Loops

```syux
for {val i = 0 : i < 3 : i++} [
  comp.out i
]

for val n in nums [
  comp.out n
]
```

## ⚠️ Status

Syux is currently in early development.

Current version: *0.4.0*

## 🗺️ Roadmap

* 0.5.0 → Standard Library (math, string)
* 0.6.0 → Improved error system
* 0.7.0 → Module system (`use`)
* 1.0.0 → Stable release

## 💡 Vision

Syux aims to make programming easier to understand by focusing on:

* clarity over complexity
* structure without intimidation
* learning before optimization

## 👨‍💻 Author

Created by a student passionate about programming and teaching others how computers work.

## ⭐ Contributing

Contributions, ideas, and feedback are welcome!
