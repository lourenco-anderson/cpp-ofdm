# Day 8 — First C++ Class: Constructors, Destructors & RAII

**Why this matters:** This is the first day of Phase 2 (C++). Before touching Eigen/FFTW, today is about understanding what actually changes going from C to C++ at the foundation level — compilation, syntax, and especially **RAII**, the mechanism that replaces the manual `create_X`/`free_X` pattern repeated throughout Phase 1. The Phase 1 retrospective flagged forgetting `free()` as a recurring weakness (Day 6, Day 7) — RAII exists specifically to eliminate that class of bug.

## Block 1 — First Contact: `g++` and Syntax Differences
- Compile a simple "hello world" with `g++` instead of `gcc`:
  ```
  g++ hello.cpp -o hello -Wall -Wextra
  ```
  Note the `.cpp` extension convention (vs. `.c`).
- C uses `printf`/`scanf` from `<stdio.h>`; C++ has `std::cout`/`std::cin` from `<iostream>`. Rewrite "hello world" using `std::cout << "Hello, World!" << std::endl;`.
- Research what `namespace std` is and why `std::cout` is written with the `std::` prefix (and why `using namespace std;`, while common in small examples, is often discouraged in larger projects) — just raise the question for now; it'll come up again later.

## Block 2 — RAII: Turning `Signal` into a Class
Rewrite the `Signal` struct (Day 3/5/7: data + length, `create_signal`/`free_signal`) as a C++ **class**.

**The core design shift:** in C, you had to *remember* to call `create_signal` and *remember* to call `free_signal` — two manual steps, easy to forget (as Day 6/7 demonstrated). In C++, a class has a **constructor** (runs automatically when an object is created) and a **destructor** (runs automatically when the object's scope ends).

```cpp
class Signal {
private:
    double *data;
    size_t length;
public:
    Signal(size_t length);   // constructor: allocates memory
    ~Signal();                 // destructor: frees memory
    void print() const;
    bool is_valid() const;
};
```

**Key syntax notes:**
- A method declared inside the class body is only a signature; implement it outside using `ReturnType Signal::methodName(...) { ... }` — the `::` (scope resolution) ties the definition back to the class.
- Constructors and destructors **never** have a return type (not even `void`) and never use `return value;` — they initialize/destroy the object itself rather than computing a value to hand back.
- `new T[n]` / `delete[] ptr` are the C++ equivalents of `malloc(n * sizeof(T))` / `free(ptr)` for arrays — `new[]` and `delete[]` must always be paired (never mix with singular `new`/`delete`), otherwise behavior is undefined even though it may compile without warning.
- `this` is an implicit pointer to the current object, useful for disambiguating a member variable from a same-named constructor parameter (`this->length = length;`).
- A member function marked `const` (e.g. `void print() const`) promises it will not modify the object's fields — the compiler enforces this.

**Encapsulation in practice:** fields declared `private` (like `data`) are not accessible from outside the class (e.g. `main()` cannot read `s.data` directly — this will produce a compile error). Expose only what's needed through `public` methods (e.g. an `is_valid() const` accessor instead of the raw pointer) — this is a deliberate design improvement over the Day 3 struct, which had no such protection.

**Error handling in the constructor:** reapply the Day 3 sentinel approach (invalid `length` or failed allocation → `data = NULL`, `length = 0`) for now, exposed via `is_valid()`. Note: C++ also offers a more idiomatic mechanism for constructor failure — exceptions (`throw std::invalid_argument(...)`) — worth knowing about, but not required today.

**Verify RAII directly:** create a `Signal` inside a function, use it, and write **no explicit cleanup call**. Add a `std::cout` line inside the destructor (e.g. `"Signal destroyed"`) to observe it firing automatically when the object goes out of scope — including along error paths, where the object still exists (just in an invalid state) and its destructor still runs unconditionally. Confirm `delete[]` on a `NULL` pointer is safe (mirrors `free(NULL)` in C) — no extra guard is needed in the destructor.

## Block 3 — Reflection
1. Compare the responsibility you carried in C (`create_signal` + `free_signal`, remembering to call both everywhere) against the C++ class (constructor + destructor, called automatically). What exactly did RAII eliminate, and what did it *not* eliminate? (Hint: RAII guarantees *when* cleanup happens — it does not by itself decide what "invalid state" means, or handle *object copies* correctly.)
2. If you copy a `Signal` object (`Signal s2 = s1;`), what do you expect happens to the internal `data` pointer — is it copied shallowly (same address, like the Day 3 struct) or does C++ do something different by default? (Don't resolve this yet — it's the subject of Day 9.)
3. Project connection: thinking ahead to `Matrix` as a class too — does the "does this object own or just borrow the memory" question become more or less important once every object has an automatic destructor that will run regardless?

## Key Lessons Learned
- RAII guarantees a destructor runs automatically and exactly once per object's scope exit — it does not eliminate the need to reason about object copies, which (without further work) can trigger the destructor running *twice* on the same memory address (a double free), not zero times.
- `private` fields plus public accessor methods give real protection against uncontrolled external modification — a genuine improvement over the unguarded C struct.
- Constructors/destructors have special syntax rules (name matches class, no return type) because they represent object lifecycle events, not value-computing functions.
- `delete[] NULL` (like `free(NULL)`) is safe and well-defined — a destructor never needs a null check before cleanup.
- The question of object-copy semantics (raised here, unresolved) is deliberately carried forward into Day 9, where it becomes a concrete, reproducible bug.