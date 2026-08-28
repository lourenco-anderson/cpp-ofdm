# Day 14 — `Signal::fft()`: The First Real FFT Inside the Class

**Why this matters:** Two separate pieces were already validated independently — (a) `std::complex<double>` and `fftw_complex` have compatible memory layout (Day 12/13, backed by a `static_assert`), and (b) `Signal` already uses aligned allocation via `fftw_malloc` (Day 13), exactly what FFTW needs to run efficiently. Today those pieces combine into a real, encapsulated method, instead of loose test code in a throwaway `main()` like Day 12.

## Block 1 — Recalling the FFTW API (no code yet)
Three steps to run an FFT with FFTW:
1. Create a **plan** (`fftw_plan_dft_1d(...)`) — a preparation/optimization step FFTW performs before execution.
2. **Execute** the plan (`fftw_execute(plan)`).
3. **Destroy** the plan (`fftw_destroy_plan(plan)`).

The plan needs to know the signal's size and the input/output pointers at creation time — think about how this fits inside an instance method (`Signal::fft()`), where `length` and `data` already belong to the object.

## Block 2 — Design Decision: In-Place FFT or Return a New `Signal`?
**Option A — In-place:** `void fft_inplace();` — overwrites the object's own `data`.
**Option B — Return new object:** `Signal fft() const;` — leaves the original untouched, returns a new `Signal` with the result.

**Deciding questions:**
- Which signature can be marked `const`, and why?
- Does returning by value necessarily invoke a copy or move constructor? (Research "return value optimization"/RVO — in modern C++, returning an object by value doesn't always cost a real copy/move.)
- Which option is safer against future bugs (e.g. someone calling `fft()` without realizing the original was destroyed, in Option A)?

**Chosen approach:** Option B — safer, more consistent with treating `Signal` as immutable where possible.

## Block 3 — Implementation
Declare:
```cpp
Signal fft() const;
```

**A real design problem to solve first:** the existing constructor always fills data with the fixed `+1/-1` pattern — not appropriate for an FFT output buffer. Solve this with a **default/empty constructor** (`Signal()`, setting `data = NULL`, `length = 0`) that can be filled manually from within another `Signal` method. This works because **any method of `Signal` has access to the private fields of any `Signal` instance**, not just `this` — a real C++ access rule (`private` restricts access from *outside* the class, not between instances of the same class).

Implementation outline:
1. Create `Signal out;` (using the empty constructor).
2. Allocate `out`'s buffer manually with `fftw_malloc(sizeof(std::complex<double>) * this->length)` + `static_cast`, and set `out.length`.
3. `reinterpret_cast<fftw_complex*>` both `this->data` and `out.data` — safe because of the Day 13 `static_assert`.
4. Create the plan with `FFTW_FORWARD`, execute, destroy.
5. Return `out`.

**Critical reminder (carried from Day 12):** FFTW does not normalize — a forward FFT's magnitude is proportional to `N` (signal length), not `1`. Document this explicitly in a comment inside the method; don't normalize here (that decision is deferred to Day 15's `ifft()`).

**A common mistake to watch for:** trying to write this logic in `main()` instead of as a class method fails — `in.data` (private field) cannot be accessed from outside the class, and `in.data()` is not a method that exists. This is exactly why the FFT logic belongs inside `Signal::fft()`, where `data`/`length` are directly accessible without any getter.

## Block 4 — Testing
Repeat the Day 12 test (a known signal — a pure complex exponential) but now via `signal.fft()` as a class method. Confirm the result still matches expectations (an impulse of magnitude `N` at the correct frequency bin).

Run `valgrind`. A new category may appear here: **"still reachable" bytes**, distinct from "definitely lost." This is FFTW's internal "wisdom" cache (reused plan optimizations), not a bug — call `fftw_cleanup()` at the end of `main()` to release it, and confirm "still reachable" drops after adding that call.

## Block 5 — Reflection
1. How did you resolve the "empty constructor problem"? What was the benefit of choosing a predictable sentinel state (`NULL`)?
2. Why does explicit documentation of the normalization decision matter more here than in the standalone Day 12 test?
3. What's needed for `ifft()` — is it direct symmetry (same structure, `FFTW_BACKWARD`), or is there an asymmetry (normalization) that needs deciding first?

## Key Lessons Learned
- A pointer-typed field being `private` restricts access from outside the class, but any method of that class can access the private fields of *any* instance of that class — not just `this`. This is what makes constructing and filling a second `Signal` object from within another `Signal` method possible.
- "still reachable" in valgrind is not the same as "definitely lost" — it often indicates a library's intentional internal cache (FFTW's plan wisdom) that has an explicit cleanup function (`fftw_cleanup()`), not a bug in your own allocation/free pairing.
- The normalization convention is a deliberate design decision that belongs to the class author, not something FFTW imposes — and it must be documented explicitly, since future users of the class won't have seen the exploratory testing that led to the decision.
- A default/empty constructor with a predictable sentinel state (`NULL`/`0`) is not just a stylistic choice — it made a real allocation bug (forgetting to `fftw_malloc` the output buffer) visible and diagnosable rather than silent memory corruption.