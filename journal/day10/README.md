# Day 10 — Rule of Five (Move Semantics)

## Objective
Extend the `Signal` class from Day 9's Rule of Three (copy constructor, copy assignment operator) to the Rule of Five by implementing move semantics: a move constructor and a move assignment operator.

## What was built
- `Signal(Signal&& other) noexcept` — move constructor
- `Signal& operator=(Signal&& other) noexcept` — move assignment operator
- Test cases in `main()` forcing each special member function via `std::move` on existing `Signal` objects: move-construction, move-assignment, and self-move-assignment (tested first on an already-empty object, then corrected to test on an object still holding real data)

## Bugs found and fixed
1. **Invalid syntax in move assignment.** The first attempt tried `this(other);` to reuse the copy-constructor logic. This does not compile — `this` is a pointer, not a callable — and conceptually would have defeated the purpose of a move (a deep copy instead of a resource steal). Fixed by directly stealing the `data`/`length` fields, mirroring the pattern already used in the move constructor.
2. **`other` not reset after the steal.** After copying `other.data`/`other.length` into `this` in the move assignment operator, `other` was left pointing at the same heap block instead of being nulled out — a double-free risk once both objects' destructors ran. Fixed by adding `other.data = nullptr; other.length = 0;`, matching the move constructor.

## Verification
- Compiled clean with `g++ -std=c++17 -Wall -Wextra` (no warnings after the fixes, aside from an intentionally triggered `-Wself-move`, used on purpose to exercise the self-assignment guard).
- Ran under `valgrind --leak-check=full`: 8 allocs, 8 frees, 0 errors, no leaks possible.
- Verified self-move-assignment (`s = std::move(s);`) on an object holding real data — confirmed the `this != &other` guard prevents the destructive delete-then-steal-from-freed-memory sequence; data survived intact and matched the pre-move `print()` output.

## Key takeaways
- `std::move` performs no data movement itself — it's a cast that makes an lvalue eligible for rvalue-reference overload resolution. The actual resource transfer is implemented entirely in the move constructor/assignment operator.
- A self-move-assignment test only proves something if the object being moved into itself holds real, non-null data at the time of the test. Testing it on an already-empty object doesn't validate whether the guard protects live data.
- The `this != &other` guard, inherited from the Rule of Three, is exactly what prevents self-move-assignment from becoming destructive.

## Next
Day 11: introduction to Eigen for linear algebra — watch for the column-major vs. row-major convention mismatch relative to the C arrays used in Phase 1.