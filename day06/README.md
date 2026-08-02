# Day 6 — Dynamic 2D Matrices via Flat Arrays

**Why this matters:** You know how to make a **static** matrix (`double[4][8]`, Day 2) and how to do **dynamic 1D allocation** (Day 3), but haven't combined the two. A real OFDM signal needs a matrix (subcarriers × symbols) whose size is only known at runtime — and native C 2D arrays (`double m[rows][cols]`) don't allow variable `rows`/`cols` easily. Today you solve this the same way Eigen, NumPy, and most linear algebra libraries do under the hood: a **contiguous 1D block** with manual indexing. This is the exact mechanism you'll rely on (or at least understand) once you move to Eigen in Phase 2.

## Block 1 — Manual Row-Major Indexing (no struct yet)
- Dynamically allocate a **single block** of `double` (`malloc(rows * cols * sizeof(double))`), where `rows`/`cols` are determined at runtime (e.g. via `scanf`), not hardcoded into the type.
- Instead of `matrix[i][j]`, compute the index manually: `data[i * cols + j]`.
- Fill the matrix with the same pattern as Day 2 (`data[i * cols + j] = i * cols + j`) and print it. Compare visually against the Day 2 static-matrix output — the values should match exactly, just now coming from a manually-indexed 1D block instead of a native 2D array.

**Why `i * cols + j` and not some other formula:** think in terms of row-major layout — to move from one row to the next (`i` increases by 1, `j` resets to 0), how many elements do you need to "walk" in the flat block? That stride should match the number of columns, not rows — the same stride logic empirically discovered in Day 2, except now you're writing the formula the compiler used to compute for you automatically with native arrays.

**Important note for later:** this convention (`i * cols + j`) is **row-major** — native to C, and also NumPy's default. Eigen, which you'll use in Phase 2, is **column-major by default**. This mismatch is a real source of subtle performance (not correctness) bugs when migrating between libraries without noticing the difference.

**Common pitfall to test deliberately:** if a function parameter is declared as `double *data[]` instead of `double data[]` (or `double *data`), it decays to `double **` (an array of pointers), not `double *` (a pointer to a contiguous block) — a completely different type. Passing a real `malloc`'d flat block to a function expecting `double **` triggers an "incompatible pointer type" warning, and indexing it (`data[k]`) would return an individual pointer, not a number. Test this on purpose to see the compiler's warnings, then confirm the correct form (`double data[]` / `double *data`) compiles cleanly.

## Block 2 — Package into a `Matrix` Struct
```c
struct Matrix {
    double *data;
    size_t rows;
    size_t cols;
};
```

Write:
1. **`create_matrix(rows, cols)`** — allocates `data` with a single `malloc`, fills `rows`/`cols`. Apply the same sentinel-struct error strategy from Day 3 for `rows == 0`, `cols == 0`, or `malloc` failure. **Do not use `assert()` to check for allocation failure** — `malloc` failing is a legitimate runtime condition, not a programming invariant, and `assert` is typically stripped out of production builds (`NDEBUG`), silently removing the check.
2. **`get_element(matrix, i, j)`** and **`set_element(matrix, i, j, value)`** — functions that **hide** the `i * cols + j` arithmetic from callers. This is the most important design decision of the day: nobody using `Matrix` from outside should need to know or write the index formula manually — only these two functions should.
3. **`print_matrix(matrix)`** — uses `get_element` internally, never touches `data` directly.
4. **`free_matrix(matrix)`** — frees `data`.

In `main`, after calling `create_matrix`, always check `matrix.data == NULL` before calling `print_matrix`/`free_matrix` — the sentinel strategy only works if the caller actually checks it.

**Design question:** why encapsulate access via `get_element`/`set_element` instead of letting callers write `matrix.data[i * matrix.cols + j]` directly everywhere? Consider: if you decided to switch from row-major to column-major tomorrow (e.g. to match Eigen's convention later), how many places in your code would need to change under each approach?

## Block 3 — Tests (building on Day 5)
Create `test_matrix.c` with fixed values (no `scanf`) and `assert()`-based checks:
- **`test_row0`** / **`test_column0`** — call `create_matrix` with `rows = 0` or `cols = 0` and verify the sentinel struct (`data == NULL`, `rows == 0`, `cols == 0`).
- **`test_get_element`** — create a valid matrix (e.g. 4×4), verify every element via `get_element` matches the expected pattern, then **free the matrix** at the end (don't forget this — even test code needs a `create`/`free` pair).
- Add `test_matrix` as a new Makefile target, reusing the existing `%.o: %.c` pattern rule — no duplicated compilation logic.

## Block 4 — Reflection
1. The Day 2 static matrix is contiguous because the **compiler** guarantees it for native multidimensional arrays (it knows the row stride at compile time). The Day 6 matrix is contiguous because **you** chose a single `malloc` and wrote the indexing arithmetic manually — contiguity here is a design decision, not an automatic language guarantee. Do these feel like meaningfully different mechanisms even though the end result (contiguity) is the same?
2. If you had used an array-of-pointers approach instead (`double **`, one `malloc` per row), what would you lose in terms of contiguity and cache locality compared to the single flat block? (Connects directly to the Day 3 mistake of allocating one pointer per element before correcting to a single `malloc`.)
3. Project connection: why is `struct Matrix` (representing `symbols × subcarriers`) strictly better for this project than the static 2D array from Day 2? (Hint: a real OFDM signal's size depends on runtime configuration — FFT size, allocated subcarriers — not something fixed at compile time.)

## Key Lessons Learned
- A contiguous dynamic matrix is achieved via a single `malloc(rows * cols * sizeof(T))` plus manual index arithmetic (`i * cols + j` for row-major) — this is exactly the mechanism underlying real linear-algebra libraries.
- `double data[]`, `double *data`, and `double *data[]` are not all equivalent: the first two both decay to a plain pointer (`double *`) as a function parameter; the third is an array of pointers (`double **`) — a fundamentally different type, confirmed empirically by compiler warnings when misused.
- Encapsulating index arithmetic behind accessor functions (`get_element`/`set_element`) centralizes future changes (e.g. row-major → column-major) to one location instead of requiring an audit of every call site.
- `assert()` remains inappropriate for checking allocation failure or other legitimate runtime error conditions — that lesson from Day 5 carries forward and must be reapplied consistently, not just recalled once.
- Row-major (C, NumPy default) vs. column-major (Eigen default) is a real, practical distinction to keep in mind before Phase 2.