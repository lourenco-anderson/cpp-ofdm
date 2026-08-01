# Day 4 — Multi-File Projects & Makefiles

**Why this matters:** Every serious C/C++ project (including OpenAirInterface, which you'll work with later) is organized with `.h` (declarations) separate from `.c` (implementation), plus a build system that knows how to assemble everything. Today is the first hands-on contact with that pattern, at the small scale of the `Signal` struct from Day 3.

## Block 1 — Split `Signal` into Its Own Files
Split the Day 3 code into 3 files:
- **`signal.h`** — the struct **declaration** and function **prototypes** only (no bodies).
- **`signal.c`** — the actual **implementation** of the three functions.
- **`main.c`** — just `main()`, using the functions declared in `signal.h`.

**Challenges to work through:**
1. How does `main.c` "see" the struct and functions declared in `signal.h`? (Same preprocessor directive used since Day 1 for `<stdio.h>` — but with quotes `"signal.h"` instead of angle brackets, because of where the compiler looks for the file first.)
2. **Include guards:** if a header is included more than once (directly or indirectly) in the same translation unit, the compiler may complain about redefinition. Research "include guard" (`#ifndef`/`#define`/`#endif`) or `#pragma once`. **Important placement detail:** the guard goes *inside the header itself*, wrapping its content — not around the `#include` statement in the files that use it.
3. **Naming caution:** avoid naming a header/struct/variable something that collides with a C standard library name (e.g. `signal.h`/`signal()` collide with `<signal.h>`'s OS signal-handling API). Pick a project-specific name instead.
4. Compile multiple `.c` files together:
   ```
   gcc signal.c main.c -o app -Wall
   ```

## Block 2 — Automate the Build with `make`
Create a file named exactly `Makefile` (no extension). Build it up piece by piece:

**Variables:**
```makefile
CC = gcc
CFLAGS = -Wall -Wextra
SRCS = signal.c main.c
OBJS = $(SRCS:.c=.o)
TARGET = app
```

**Default target (built when running plain `make`):**
```makefile
all: $(TARGET)
```

**Pattern rule — how to turn any `.c` into a `.o`:**
```makefile
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
```
(Indentation before the command **must be a literal tab**, not spaces — a very common source of "missing separator" errors.)

**Final link rule:**
```makefile
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
```

**Automatic variables used above:**
- `$@` — the target (left of the colon)
- `$^` — *all* prerequisites (right of the colon)
- `$<` — the *first* prerequisite only

## Block 3 — Reflection & Verification
1. Run `make` twice in a row without changing anything — confirm it reports the target is already up to date (comparison is by file **timestamp**, not content).
2. Edit only one `.c` file (e.g. add a comment) and run `make` again — confirm **only** that file's object is recompiled, not all of them. This is real incremental compilation, which a single flat `gcc file1.c file2.c -o app` command never provides (that always recompiles everything, every time).
3. What's the practical benefit of splitting `signal.h`/`signal.c` from `main.c`, beyond "organization"? Consider: if a third file (e.g. `test_signal.c`) needed to test only the `Signal` functions without running the full `main()` — what would that let you do? (This is the seed of unit testing: isolating a component from the full program flow.)
4. Why have a separate `all` target pointing to `$(TARGET)` instead of just naming the main rule after the target directly? (Consider a future project producing more than one artifact — e.g. a main executable *and* a separate test executable.)

## Key Lessons Learned
- `make`'s default behavior (running plain `make`) is to build the **first** target defined in the file — `all` is a naming convention for "the thing that aggregates everything I normally want built," not a special reserved priority mechanism.
- `make` resolves a **dependency graph**, not a linear script: it starts from the requested target, recursively figures out what each dependency needs, and only then executes commands — this is why editing one `.c` file only triggers recompilation of that file's object, not the whole project.
- `$@`, `$^`, `$<` are `make`-specific automatic variables, not shell/bash variables — don't confuse the two when discussing this in an interview.
- Accepting a code-generation tool's suggestion (e.g. Copilot) without being able to explain every symbol in it (like `$@`/`$^`) is worth catching and correcting — rebuild understanding piece by piece rather than shipping code you can't defend.