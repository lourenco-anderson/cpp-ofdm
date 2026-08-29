# Day 13 — Closing the Signal Design Decision (Aligned Allocation) & Introducing CMake

**Context for this order:** Day 12 ended with an open design decision — unresolved design decisions tend to become technical debt if left unaddressed before moving forward. Today resolves that first, and uses the resulting need to link `libfftw3` correctly as the motivation to introduce CMake — exactly the kind of external dependency (system library, link flags) that exposes the limits of a plain Makefile.

## Block 1 — Closing the Decision: Rule of Zero vs. Manual Control
Two options were left open at the end of Day 12:
- **Rule of Zero:** use `std::vector<std::complex<double>>` internally in `Signal`, letting the vector manage allocation/copy/move itself — no more hand-written copy constructor, destructor, etc.
- **Manual control:** keep a raw pointer (`std::complex<double>*`), but replace `new[]`/`delete[]` with `fftw_malloc`/`fftw_free`, because FFTW requires specific memory alignment to use its fastest SIMD code paths (SSE/AVX) internally.

**The deciding factor:** `std::vector`'s default allocator (`new`) does not guarantee FFTW's required alignment. This isn't a style choice — it's a real technical constraint. A custom-allocator `std::vector` (aligned allocator) can solve this, but adds its own complexity.

**Pragmatic middle ground chosen:** keep the already-implemented and validated Rule of Five with a raw pointer, but swap the *allocation calls* from `new[]`/`delete[]` to `fftw_malloc`/`fftw_free` everywhere memory is allocated/freed — normal constructor, destructor, copy constructor, copy assignment operator. The move constructor and move assignment operator don't allocate at all (they steal an existing pointer), so they don't need this change — reason through why before touching them.

**Implementation steps:**
1. Migrate `Signal`'s internal type from `double*` to `std::complex<double>*` (the Day 12 migration that was left pending). Include `<complex>` and `<fftw3.h>`.
2. Replace `new[]`/`delete[]` with `fftw_malloc(sizeof(std::complex<double>) * length)` / `fftw_free(ptr)` in every allocation/deallocation site.
3. **`fftw_malloc` returns `void*`** — unlike C, C++ does not implicitly convert `void*` to a typed pointer. Every call needs an explicit `static_cast<std::complex<double>*>(...)`.
4. Don't drop the failure-check habit from Phase 1: check the returned pointer for `NULL` after `fftw_malloc`, same as any other allocation.
5. Re-verify: is the `static_assert(sizeof(std::complex<double>) == sizeof(fftw_complex), ...)` from Day 12 still needed? (Yes — not because of allocation, but because it's the formal guarantee that will justify a future `reinterpret_cast<fftw_complex*>(this->data)` when writing an actual FFT method. C++11 guarantees `std::complex<T>`'s memory layout matches a 2-element array of `T` — identical to `fftw_complex` — only for `T` = `float`/`double`/`long double`.)

**A real bug to watch for (and fix if it appears):** the move assignment operator must **steal** the existing pointer from `other`, exactly like the move constructor — it must **not** allocate new memory and copy values. Allocating fresh memory in a "move" operator is a disguised copy: it defeats the O(1) performance benefit of moving, and worse, if the copy loop is accidentally omitted, leaves the destination with allocated-but-uninitialized (garbage) memory. Compare your move constructor and move assignment operator side by side to confirm they follow the same "steal, don't allocate" logic.

**Test thoroughly with `valgrind`,** covering every scenario from Days 9–10 again under the new allocator: copy construction, copy assignment (including assigning an invalid/sentinel signal), move construction, move assignment, and self-move-assignment (`s = std::move(s)`) — confirm the `this != &other` guard still protects correctly and the full test run produces zero leaks and zero errors.

## Block 2 — Why CMake Now
Manually compiling against FFTW requires remembering link flags every time:
```
g++ file.cpp -o program -lfftw3 -lm
```
As the project grows (more libraries), this doesn't scale — every Makefile rule using `Signal` would need those flags added manually.

**What CMake solves that a plain Makefile doesn't:**
- Locating installed system libraries (`find_library`, or `find_package` when a library provides CMake config files) instead of hardcoding paths/flags.
- Portability across systems without rewriting build logic.
- CMake doesn't replace `make` — it *generates* the Makefile (or another backend) for you.

## Block 3 — First `CMakeLists.txt`
Install if needed:
```
sudo apt install cmake -y
```

Structure to build (research exact syntax for each):
1. `cmake_minimum_required(VERSION ...)`
2. `project(<name> CXX)`
3. `set(CMAKE_CXX_STANDARD ...)` / `set(CMAKE_CXX_STANDARD_REQUIRED True)` — pin the C++ standard actually in use (11+, given `std::move`, `static_assert`, and the `std::complex` layout guarantee).
4. **Finding FFTW:** most distros don't ship a `find_package`-compatible config for FFTW (it's an older C library). Use `find_library(<OUTPUT_VAR> <library_name>)` instead — note the argument order: first the *name of the CMake variable* to store the result in, second the *actual library name* to search for (e.g. `find_library(FFTW3_LIB fftw3)`, not `find_library(FFTW3 REQUIRED)` — `REQUIRED` is a `find_package` keyword, not valid here). Optionally add a manual check (`if(NOT FFTW3_LIB) ... message(FATAL_ERROR ...)`) since `find_library` alone doesn't stop configuration automatically like `find_package(... REQUIRED)` does.
5. `add_executable(<target> your_file.cpp)`
6. `target_link_libraries(<target> PRIVATE ${FFTW3_LIB})` — use whatever variable name was actually defined in step 4, not an assumed convention like `${FFTW3_LIBRARIES}` (that naming convention belongs to `find_package`, not `find_library`).

`target_include_directories` is only needed if the compiler doesn't already find `<fftw3.h>` in a default search path — test without it first.

**Out-of-source build convention:**
```
mkdir build
cd build
cmake ..
make
```
Running `cmake ..` from a separate `build/` directory (rather than in the project root) keeps generated artifacts (`CMakeCache.txt`, `CMakeFiles/`, generated Makefiles, `.o` files, the final executable) completely separate from source code you actually wrote. None of these generated files should be versioned in git — they're fully reconstructible at any time (`rm -rf build && mkdir build && cd build && cmake ..`), they encode machine-specific configuration (absolute library paths, compiler version) that shouldn't be shared across machines, and separating them allows multiple simultaneous build configurations (e.g. `build-debug/`, `build-release/`) without conflict.

**Action:** add `build/` to `.gitignore`.

**Test:** confirm the CMake-built executable behaves identically to the manually-compiled one — run it and re-run the full `valgrind` suite from Block 1 against it.

## Block 4 — Reflection
1. Was the deciding factor between Rule of Zero and manual control purely "less code to write," or was FFTW's memory-alignment requirement decisive? This illustrates a broader C++ lesson: choosing between higher-level abstractions and manual control often depends on specific technical requirements (here, hardware/SIMD), not just style preference.
2. Makefile (Days 4–7) vs. CMake (today) — what specific problem in the current project motivated the switch? Would the same motivation exist if the project never needed an external library like FFTW?
3. With `Signal` now using `std::complex<double>` and FFTW-aligned allocation, and the `static_assert` confirmed as load-bearing groundwork, the project is structurally ready for a real `Signal::fft()` method using FFTW directly inside the class — the natural next step.

## Key Lessons Learned
- `fftw_malloc`/`fftw_free` return/accept `void*`, requiring explicit `static_cast` in C++ (unlike C's implicit `void*` conversion) — a real language-level difference, not a stylistic one.
- A move constructor and move assignment operator must both follow the "steal the pointer, null out the source" pattern; allocating fresh memory in a move-assignment operator silently defeats the performance guarantee of moving and can leave uninitialized memory behind if the copy step is forgotten.
- A `static_assert` about type layout compatibility (`std::complex<double>` vs `fftw_complex`) earns its keep not at allocation time, but as the formal justification for a future `reinterpret_cast` — removing it because "nothing currently uses it" would silently remove protection needed one step later.
- `find_library` and `find_package` have different argument conventions and generate different variable names — mixing their idioms (e.g. expecting `find_library` to populate `<NAME>_LIBRARIES` automatically) is a common, easy-to-make CMake mistake.
- Out-of-source builds (`build/` directory, gitignored) separate versioned source from disposable, machine-specific generated artifacts — the same principle behind never committing `.o` files, applied to a build system that generates far more metadata than a plain Makefile.
