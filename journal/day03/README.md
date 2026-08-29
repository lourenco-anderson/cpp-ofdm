# Day 3 — Dynamic Memory & Structs

**Why this matters:** Until now you worked with sizes known at compile time (`double[8]`, `double[4][8]`). A real OFDM signal has a size only known at runtime (depends on configuration — FFT size, allocated subcarriers, etc.). Today: dynamic memory (`malloc`/`free`) and `struct`, which together solve exactly that problem — and this is the C that directly precedes `std::vector` / `new`/`delete` in C++.

## Block 1 — Pure `malloc`/`free`
- Read an integer from the user (`scanf`) representing the size of a `double` array.
- Allocate that array **dynamically** with a single `malloc` call sized for the whole array (`malloc(n * sizeof(double))`) — not one `malloc` per element.
- Fill it with values, print them.
- Free the memory with `free` at the end.

**Investigate before coding:** What does `malloc` return when it fails, and how would you check that *before* using the pointer? Reason it out from what you already know about pointers (what value already represents "points nowhere"?) before looking it up.

**Empirical test:** Print the address of `arr[0]` and `arr[1]` (using `&arr[i]`, not the element's value). Confirm the stride is exactly `sizeof(double)` — proving a single `malloc` call yields the same contiguity as a static array, unlike allocating one pointer per element (which has per-allocation overhead and is not guaranteed contiguous).

**Leak test:** Comment out `free` and run the program repeatedly. Research the term "memory leak" and reason about why this matters far more for long-running processes (e.g. a RAN system running 24/7) than for a short script that runs once and exits.

## Block 2 — Struct to Package Data + Size
Define:
```
struct Signal {
    double *data;
    size_t length;
};
```
Write:
- A function that **creates** a `Signal` — allocates memory internally via `malloc` based on a size parameter, and returns the filled struct.
- A function that **prints** a `Signal`, receiving only the struct (not two separate parameters anymore).
- A function that **frees** a `Signal`'s memory (a single `free` on the `data` pointer — not per element).

**The real design problem to solve:** If the creation function returns `struct Signal` **by value** (not by pointer), what should it return on an error path (invalid size, or `malloc` failure)? There's no obvious `NULL` for a struct returned by value. Consider at least these strategies before picking one:
- **Sentinel struct:** return a struct with `data = NULL`, `length = 0` on error; the caller must check this before use.
- **Output parameter + status code:** change the function to return an `int` status, and pass a `struct Signal *out` to fill — the same output-parameter pattern from Day 1.
- **Heap-allocated struct pointer:** return `struct Signal *` (allocated with `malloc` too), returning `NULL` on error, like a normal pointer.

Whichever you pick, make sure:
- The sentinel is internally consistent (if `data == NULL`, `length` must also be `0`, on **every** error path, including inside the `malloc`-failure branch).
- The caller (`main`) actually checks the result before calling print/free.

## Block 3 — Reflection (no code)
1. What's the fundamental difference between memory `malloc` allocates and memory from a normal in-function array (`double arr[8]`) — in terms of *where* it lives (stack vs. heap) and *when* it disappears?
2. Why doesn't a missing `free` crash the program immediately — what exactly leaks, and why does it only become visible after running for a long time or many times?
3. Why does a real-world variable-size OFDM signal *require* dynamic allocation instead of static arrays?
4. What happens when a struct containing a pointer is copied by value (e.g. `struct Signal s = create_signal(n);`)? Is the pointed-to data duplicated, or just the pointer (address) copied? What is a "double free," and why is it dangerous (undefined behavior in the memory allocator, not just a predictable leak)?

## Key Lessons Learned
- A single `malloc` for the whole array preserves contiguity (and thus cache-locality benefits); many small `malloc` calls do not.
- Always check `malloc`'s return value for `NULL` before use.
- Returning error status from a function whose success type is a plain struct (not a pointer) has no universally "obvious" solution in C — it's a genuine design decision with real trade-offs.
- A double free corrupts the allocator's internal bookkeeping — behavior is undefined (crash now, crash later, or silent corruption), which is categorically worse than a predictable memory leak.
- `free()` must be called on the allocated pointer itself, not on individual elements it points to.