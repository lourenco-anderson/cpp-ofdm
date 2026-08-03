# Day 7 — Valgrind, Signal/Matrix Integration & Phase 1 Wrap-Up

**Why this matters:** This closes Phase 1 (C fundamentals) before migrating to C++ in Phase 2. Today covers three things: a tool for detecting real memory leaks instead of relying on manual inspection (`valgrind`), an integration exercise bringing `Signal` and `Matrix` together in a more realistic scenario, and a review consolidating everything built so far.

## Block 1 — `valgrind` in Practice
Install if needed:
```
sudo apt install valgrind -y
```

Run it against your existing executables:
```
valgrind --leak-check=full ./test_matrix
valgrind --leak-check=full ./matrix_struct
```

**What "clean" looks like:** the summary line `All heap blocks were freed -- no leaks are possible`, combined with `ERROR SUMMARY: 0 errors from 0 contexts`.

**Deliberate leak test:** comment out one `free_matrix(...)` call on purpose (e.g. at the end of a test function that allocates a real matrix), rebuild, and run `valgrind` again. Pay attention to:
- The `HEAP SUMMARY` now showing bytes still in use at exit (not "0 bytes in 0 blocks").
- A new **`LEAK SUMMARY`** section, with a "definitely lost" line showing the exact byte/block count, plus a **stack trace** pinpointing the exact call chain (e.g. `malloc` → `create_matrix` → `your_test_function` → `main`) responsible for the unfreed allocation.

This is the practical difference between "I think there's no leak" and "I have a tool that proves it." Revert the change afterward and confirm the report goes back to clean.

## Block 2 — Integration: `Signal` Inside `Matrix`
Conceptually, an OFDM matrix is a collection of signals — each row can be thought of as one "OFDM symbol" (a `Signal` of length `cols`). Implement `get_row(matrix, i)`, returning a `struct Signal` representing row `i` — **without copying data**, just pointing into the matrix's already-allocated block.

**The key insight (no native "slicing" in C):** unlike Python's `list[x:x+i]` (which copies) or NumPy's `array[x:x+i]` (which returns a view), C has no slice syntax at all. You construct the equivalent manually with a pointer (start address) + a length (how many elements). A raw pointer carries **no size information on its own** — that's exactly why `struct Signal` (data + length bundled together) exists.

Implementation:
```c
struct Signal get_row(struct Matrix matrix, size_t i){
    // NOTE: this returns only a VIEW into matrix's memory.
    // The returned Signal does not own its data and must NOT be passed to free_signal().
    struct Signal output;
    if (i >= matrix.rows){
        output.data = NULL;
        output.length = 0;
        return output;
    }
    output.length = matrix.cols;
    output.data = matrix.data + i * matrix.cols;
    return output;
}
```

**Critical design point — ownership, not just mechanics:** since `output.data` points *into* `matrix.data` rather than newly-`malloc`'d memory, this `Signal` must never be passed to `free_signal()` — doing so would free memory still owned and used by the parent `Matrix` (a conceptual sibling of the double-free problem from Day 3, but here the danger is a pointer that was never meant to be freed independently at all, not two copies of the same owned pointer). Document this explicitly in a comment. This is the same "owning vs. borrowing" distinction that RAII and `std::span`/references handle more safely in C++ later — but the underlying design question (who owns this memory?) never goes away; C++ tooling only automates the *mechanics* of cleanup once ownership is decided, not the decision itself.

**No bounds protection beyond what you build:** if code using the returned `Signal` iterates past `signal.length`, it silently reads into the *next* row of the matrix (since the whole matrix is one contiguous block) — no crash, no compiler warning, just wrong-looking data that's hard to trace back to its cause. Respecting `length` is entirely the caller's responsibility in C.

**Test it:** create a 4×8 matrix, call `get_row` for a valid row and print it, and separately call it with an out-of-bounds row index (`i >= matrix.rows`) to confirm the sentinel path (`data == NULL`, `length == 0`) works correctly.

**Formalize with `assert`-based tests** (`test_get_row`), not just visual inspection:
```c
struct Signal row = get_row(matrix, 2);
assert(row.data != NULL);
assert(row.length == matrix.cols);
for (size_t j = 0; j < row.length; j++) {
    assert(row.data[j] == get_element(matrix, 2, j));
}
struct Signal invalid_row = get_row(matrix, matrix.rows); // out of bounds
assert(invalid_row.data == NULL);
assert(invalid_row.length == 0);
```
Remember to `free_matrix` the matrix used in this test — allocating a real matrix here means it needs a real, corresponding free, exactly like any other test.

## Block 3 — Consolidation
- Confirm `signal.h`/`signal.c` and `matrix.h`/`matrix.c` follow the **same** error-handling pattern (consistent sentinel struct in both).
- Run `valgrind` on the full test suite integrating `Signal` + `Matrix` + `get_row`, confirming zero leaks.
- Decide explicitly whether `create_signal`/`free_signal` remain available for standalone `Signal` use (owning memory) alongside `get_row`'s view-only `Signal` (non-owning) — document the distinction clearly rather than removing one path by accident.
- Update the Makefile to build everything (`signal.o`, `matrix.o`, `main.o`, `test_signal.o`, `test_matrix.o`) coherently, with `all` producing every relevant executable.

## Block 4 — Phase 1 Wrap-Up Reflection
1. Looking back across Days 1–7, which concept "resisted" the most — needed to be revisited more than once before it truly stuck? (A real recurring pattern: using `assert()` for legitimate runtime errors instead of programming invariants surfaced in Day 5 *and* Day 6; forgetting to free memory allocated inside test functions surfaced in Day 6 *and* Day 7.)
2. C++ RAII (constructors/destructors, containers like `std::vector`) automates memory management, but does it eliminate the need to reason about ownership? Consider `get_row`: even in C++, a function returning a non-owning view (e.g. `std::span`) still requires a design decision about who owns the underlying data and how long it must stay alive — RAII automates *when* to destroy an owned resource, but not *whether* a given object owns it in the first place.
3. What would you do differently, starting this sequence of exercises from scratch, knowing what you know now? (E.g. introducing lightweight tests earlier — as soon as there was testable logic, like the Day 2 static matrix — or establishing a strict "every `create_X` needs a matching `free_X`, including in test code" checklist from Day 3 onward, given how often that specific mistake recurred.)

## Key Lessons Learned (Phase 1 Closing Summary)
- `valgrind --leak-check=full` gives definitive proof of memory correctness (or a precise stack trace pinpointing a leak's origin) — far more reliable than manual code review alone, and a standard tool expected in C/C++ engineering interviews.
- A pointer in C carries no size information by itself — pairing an address with an explicit length (as `struct Signal`/`struct Matrix` do) is the idiomatic way to express bounded, sliceable views over contiguous memory, since C has no native slicing syntax.
- Ownership (who is responsible for freeing a resource) is a distinct concept from mutability (`const`) — both matter, but solve different problems, and neither is automated away entirely by higher-level language features like RAII.
- The same category of mistake (assert misuse; forgetting `free` in test code) recurring across multiple days is a meaningful signal worth tracking deliberately, not just fixing reactively each time it appears — especially heading into C++, where equivalent mistakes may resurface in new forms (shallow copies, missing destructor logic).