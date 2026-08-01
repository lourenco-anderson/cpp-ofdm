# Day 1 — Environment Setup & C Fundamentals

**Goal:** Not to write advanced C, but to build the mental model of C (compilation, pointers, memory) before touching C++.

## 1. Environment Setup
- Confirm `gcc`, `make`, and `git` are installed.
- Initialize the project git repository — this will become the repo published on GitHub after the 6-week sprint.
- Suggested structure: one folder per day/sprint (`day01/`, `day02/`, ...).
- Pick an editor (VS Code recommended) without over-configuring — today is for writing code, not tuning environments.

## 2. Understand the Compilation Pipeline (no logic yet)
Write a trivial "hello world" program. Instead of compiling directly with `gcc file.c -o out`, run the 4 stages manually, each producing an intermediate file:
1. Preprocessing (`-E`)
2. Compilation to assembly (`-S`)
3. Assembly to object file (`-c`)
4. Final linking

Inspect the intermediate files:
- `.i` — see how a single `#include` expands into hundreds of lines.
- `.s` — locate your string literal (e.g. via `grep -n "Hello" file.s`) and the function body markers (`.LFB0`/`.LFE0`). Find the instruction that loads the string's address before calling `printf` (look for `lea` with `%rip`).
- Compare `file hello.o` vs `file hello` to understand the difference between a relocatable object and a linked executable.

**Checkpoint:** Be able to explain, in one sentence, what each of the 4 stages does and why it matters (e.g., "what happens when you compile a C program?").

## 3. Core Exercise — Arrays, Pointers, Functions
Specification (no solution code provided — build it yourself):
- An array of 8 `double` values representing a fictional "signal sample."
- A function that receives the array **by pointer** (plus its size) and computes the sum and the average.
- Since a C function can only `return` one value, find a way to return **two** results (sum and average). Explore pointer-based "output parameters" as one option.
- Print the results with `printf`.

**Constraints (intentional, to force learning):**
- The function must receive the array by pointer — do not reimplement the logic inside `main`.
- Do not use `sizeof` on the array inside the function — only in `main`, where it's still a "real" array.

Compile with warnings enabled:
```
gcc file.c -o file -Wall
```
Pay attention to any warning about incompatible types or unused parameters — the compiler flagging a reasoning error before you even run the program.

## 4. Understanding Checkpoint (no code)
Answer for yourself, from memory:
- What is array-to-pointer decay?
- Why doesn't C know the size of an array received as a function parameter?
- What happens in memory (stack) when you declare `double signal[8]` inside `main()`?

## 5. Close the Day
- Commit with a descriptive message.
- Write a short log entry: what blocked you, what worked, how long it took.

## Key Lessons Learned
- `-Wall` does **not** cover implicit numeric conversion warnings (e.g. `double` → `int` on `return`); that lives under `-Wconversion`. "No warning" does not mean "no problem."
- A function can return one value via `return`, but can return additional values via pointer "output parameters" — a pattern that reappears constantly in C (and later, when passing signal buffers by reference in C++/DSP).
- `main`'s return value is a process exit status (0–255 range on Linux), not a place to return arbitrary computed data.