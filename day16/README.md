# Day 16 — Porting `Matrix` to C++

**Why this matters:** This applies the same pattern already built for `Signal` (Days 8–10, 13): constructor/destructor, encapsulation, Rule of Five, aligned allocation. But `Matrix` has a piece `Signal` didn't have — `get_row()` (Day 7), originally a non-owning "view" into the matrix's memory — and RAII changes the safety calculus for that pattern entirely, making this more than mechanical repetition.

## Block 1 — Basic Migration: Struct → Class
Rewrite the C `struct Matrix` (Day 6/7) as a C++ class, following the same pattern as `Signal`:
```cpp
class Matrix {
private:
    std::complex<double> *data;
    size_t rows;
    size_t cols;
public:
    Matrix(size_t rows, size_t cols);
    ~Matrix();
    // ...
};
```

**Type decision:** use `std::complex<double>` for `data` (not `double`, as in the original Day 6 C version) — consistent with `Signal`, and appropriate since `Matrix` will represent OFDM symbols (subcarriers × symbols) after constellation mapping and FFT/IFFT, which are inherently complex-valued.

Migrate:
- Constructor (allocates via `fftw_malloc`, same alignment rationale as `Signal`).
- Destructor (`fftw_free`).
- `get_element`/`set_element` as methods (`const` for the getter, non-`const` for the setter — same logic as Day 8).
- `print()` as a method.
- `is_valid()` following the same sentinel pattern already used in `Signal`.

**A recurring bug to watch for:** the sentinel branch for `rows == 0 || cols == 0` must zero out *both* `rows` and `cols` (matching the pattern already used in the `malloc`-failure branch) — an inconsistent sentinel (e.g. `data == NULL` but `rows` still non-zero) is the same class of bug already caught and fixed once in C (Day 6); it's easy for it to resurface when rewriting the logic from scratch during migration rather than porting the already-corrected version directly.

## Block 2 — Full Rule of Five
Repeat the same process used for `Signal` (Days 9–10):
1. Copy constructor (deep copy: new `fftw_malloc` + element-by-element copy loop).
2. Copy assignment operator (check self-assignment, free old memory *before* allocating new).
3. Move constructor (steal pointer, null out `other`).
4. Move assignment operator (steal pointer — **remember the Day 13 bug**: don't allocate new memory here, only reassign the pointer).

**A real, instructive compiler error to expect:** declaring a move constructor or move assignment operator **without** an explicit copy constructor causes the compiler to mark the copy constructor as **deleted** (a real C++11+ rule) — attempting `Matrix m2 = m1;` then fails to compile with "use of deleted function." This didn't surface with `Signal` only because the copy constructor happened to be written before the move members, by coincidence of order — not because the rule was understood at the time.

**Validate with `valgrind`** across the same scenarios tested for `Signal`: copy, copy-assignment (including assigning an invalid/sentinel matrix), move, move-assignment, and self-move-assignment.

## Block 3 — The Real Problem: `get_row()` as a Class Method
In the C version (Day 7), `get_row` returned a `struct Signal` that didn't own its memory — a documented, trusted-by-convention view directly into `matrix.data`.

**Now `Signal` has an automatic destructor.** If `get_row()` returned a `Signal` (by value) whose `data` pointed into `matrix.data`, that `Signal`'s destructor would run automatically on scope exit and call `fftw_free()` on memory it doesn't own — guaranteed to happen (RAII always calls the destructor), not merely a possibility depending on someone remembering not to call `free_signal` by convention, as in C.

**Design options considered:**
- Return a true **deep copy** — a real, memory-owning `Signal` — simplest and safest, at the cost of an O(cols) copy instead of an O(1) view.
- A separate **view class** (e.g. `SignalView`, storing just a raw pointer + length, with no ownership-releasing destructor) — closer to modern patterns like `std::span`.
- `std::span` (C++20) — not available without upgrading the project's C++ standard past the C++17 currently pinned in `CMakeLists.txt` (Day 13).

**Chosen approach:** deep copy — a deliberate trade of "zero-copy view performance" for "RAII simplicity and safety" at this stage of the project, worth revisiting later if performance becomes a measured concern rather than a hypothetical one.

Implementation:
```cpp
Signal Matrix::get_row(size_t i) const {
    Signal row(cols, false);  // allocate without the default +1/-1 fill pattern
    for (size_t j = 0; j < cols; j++){
        row.set(j, this->data[i * cols + j]);
    }
    return row;
}
```

**Access problem along the way:** filling `row`'s data from *outside* the `Signal` class (from a `Matrix` method) cannot touch `row.data` directly — private-field access between classes is not the same as private-field access between instances of the *same* class (Day 14's lesson only applies within `Signal`'s own methods). The correct fix is a public setter (`Signal::set(i, value)`), not attempting direct field access from `Matrix`.

**A related overloaded-constructor addition:** to avoid wasting work filling `Signal` with the default `+1/-1` pattern only to immediately overwrite every value, add `Signal(size_t length, bool fill_pattern)` — an overload that allocates without filling when `fill_pattern` is `false`. (Note: this introduces some duplication with the original `Signal(size_t length)` constructor — a candidate for constructor delegation, `Signal(size_t length) : Signal(length, true) {}`, as a later cleanup.)

**A classic pitfall encountered along the way — "most vexing parse":** writing `Signal row();` intending to invoke the default constructor is instead parsed by the compiler as a **function declaration** (a function named `row`, taking no arguments, returning a `Signal`) — one of C++'s most famous ambiguities. The fix is to drop the parentheses entirely: `Signal row;`.

**Test:** create a `Matrix`, call `get_row(i)`, print it, and compare against the corresponding row already shown by `Matrix::print()`. Run `valgrind` with `Signal` and `Matrix` interacting for the first time — confirms the deep-copy approach fully eliminates the double-free risk that existed in the C version.

## Block 4 — Reflection
1. Why did the same "view without ownership" approach that worked in C become a structural problem in C++ with RAII, rather than continuing to work with just a documentation comment?
2. The deep-copy choice trades performance for simplicity — at what point in the project would this trade-off start to hurt enough to reconsider (e.g. `std::span`, or a dedicated view class)?
3. With `Matrix` now in C++, Rule of Five validated, and `get_row()` safe — what's structurally missing before a first real modulator prototype?

## Key Lessons Learned
- A "view without ownership" pattern that relies on documentation/convention (safe enough in C, where nothing runs automatically) becomes a guaranteed bug in C++ once RAII is involved, because the destructor *will* run regardless of whether the object actually owns its data.
- Declaring any move-semantics member (move constructor or move assignment operator) causes the compiler to delete the implicitly-generated copy constructor and copy assignment operator — an explicit copy constructor must be declared if copying is still needed, regardless of whether the class previously "got away" without hitting this by coincidence of declaration order.
- `Signal row();` is not object construction — it's a function declaration (the "most vexing parse"). Default-constructing an object requires omitting the parentheses entirely.
- Private-field access in C++ is scoped to the *class*, not the *instance* — but only within methods of that *same* class. A method of one class (`Matrix::get_row`) cannot access another class's (`Signal`'s) private fields directly, even to help construct an object of that other class; a public setter is required instead.
- A recurring bug (inconsistent sentinel state) can resurface during a straightforward migration/rewrite if the already-fixed version isn't ported directly — rewriting logic from memory reopens the risk of reintroducing a previously-solved bug.