# Day 2 — Pointer Arithmetic & Multidimensional Arrays

**Why this matters:** An OFDM signal is not a simple vector — it's a matrix (subcarriers × OFDM symbols, and later antennas × subcarriers × symbols in MIMO). Before reaching `std::vector` or Eigen in C++, you need to understand how C organizes this "under the hood," because that understanding is what lets you debug memory/performance issues later (cache locality — directly relevant to the energy/complexity angle of the thesis).

## Block 1 — Pure Pointer Arithmetic
- Declare a simple `int` array (5-6 elements).
- Declare a pointer to it.
- Instead of accessing elements with `arr[i]`, access them using pointer arithmetic (`*(ptr + i)`).
- Print, for each index, the element's **address** (`%p`) alongside its value.

**Investigate:** By how much do the printed addresses increase between consecutive indices? What does that depend on? (Hint: it relates to `sizeof` of the element type — confirm with the compiler, not by assumption. Note: C does not guarantee a fixed size for `int`; it only guarantees a minimum — what you observe is platform-specific behavior, not a language rule.)

**Follow-up test:** Change the array type from `int` to `double` (nothing else) and predict the new stride *before* running.

## Block 2 — Static 2D Array
- Declare a fixed matrix, e.g. `double matrix[4][8]` (think of it as "4 OFDM symbols × 8 subcarriers" — the domain-relevant naming helps build intuition).
- Fill it with values via a nested loop.
- Write a function that receives this matrix and prints all values.

**The real challenge:** How do you declare the function parameter to receive a fixed 2D array? This is not the same as the 1D array from Day 1 — C requires you to specify the size of every dimension except the first in the function signature. Try it, let the compiler error out, read the error, and adjust.

Try to derive `rows`/`cols` programmatically from the array itself (using `sizeof(matrix) / sizeof(matrix[0])`, etc.) rather than hardcoding — but note this trick only works in the scope where the array was declared, not once it has "decayed" as a function parameter.

## Block 3 — Reflection (no code)
1. Is a `double[4][8]` matrix stored as one contiguous block, or as separate blocks per row? **Verify this empirically** — print the addresses of `matrix[0][7]` and `matrix[1][0]` and check the byte difference (it should be exactly `sizeof(double)` if contiguous).
2. If contiguous, does traversing row-by-row (inner loop over columns) benefit from cache locality compared to column-by-column? Why (think about what's physically adjacent in memory, and how a CPU cache line — typically 64 bytes — loads neighboring data "for free")?

## Key Lessons Learned
- Static 2D arrays in C are guaranteed contiguous by the language — this is not an implementation coincidence.
- A function signature for a fixed 2D array must specify all dimensions except the first, because the compiler needs the row size to compute the stride between rows when calculating `matrix[i][j]`'s address; it never needs to know the total number of rows for that calculation.
- Row-major traversal (varying the innermost/rightmost index fastest) improves spatial locality — a real factor in the energy/computational-cost angle of the thesis, and a preview of why row-major vs. column-major matters later with Eigen/NumPy.
- Be precise about magnitude claims (e.g. don't claim performance is "cut in half" without a numeric basis) — you can state the *direction* of the effect confidently without inventing a number.