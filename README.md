# InlineAsmValidator: LLVM-Based Static Analysis for x86-64 Inline Assembly

**InlineAsmValidator** is an industry-standard Clang plugin designed to audit and validate inline assembly blocks within C source code. This project was developed to bridge the safety gap between high-level C logic and low-level assembly execution, specifically targeting **ABI (Application Binary Interface) compliance**.

## Overview
Compilers like Clang and GCC typically treat the string content within `asm()` blocks as a "black box." This tool pierces that box using **LLVM AST Matchers** and stateful analysis to enforce Calling Convention Preservation and ISA (Instruction Set Architecture) compliance.

### Key Features
* **Mnemonic Validation**: Identifies non-existent or misspelled x86-64 instructions (e.g., `move` vs `mov`).
* **Register Schema Check**: Validates register names against the x86-64 architectural specification.
* **Clobber Integrity**: Ensures registers modified by assembly are declared in the clobber list to prevent silent data corruption.
* **Stack Balance Tracking**: Monitors `push`/`pop` operations to ensure the Stack Pointer ($RSP$) remains neutral, preventing stack smashing.

---

## Project Structure
```text
CD_LAB_EL/
├── src/
│   └── InlineAsmValidator.cpp  # Core Plugin Logic (C++)
├── testcases/
│   └── test_all.c              # Comprehensive Test Suite
├── build.sh                    # Automated Build Script (CMake/Make)
├── run.sh                      # Execution Wrapper for Clang -cc1
├── CMakeLists.txt              # LLVM/Clang Build Configuration
├── DESIGN.md                   # Architectural Approach
├── IMPLEMENTATION.md           # LLVM Technical Details
└── EVALUATION.md               # Performance and Metrics
```

---


#  Installation & Build

## Prerequisites

### Operating System
- Linux (Optimized for Garuda/Arch)

### Compiler
- LLVM/Clang 15+
- Clang development headers required

### Required Tools
- CMake
- Make

---

#  Build Steps

Navigate to the project root directory:

```bash
cd InlineAsmV
```

Grant execution permissions:

```bash
chmod +x build.sh run.sh
```

Execute the build script:

```bash
./build.sh
```

---

#  Run Steps

Execute the run script:

```bash
./run.sh testcases/file_to_be_tested
```

