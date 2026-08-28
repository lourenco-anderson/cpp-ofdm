# Day 15 — `Signal::ifft()`: Closing the Normalization Decision

**Why this matters:** Day 14 ended with an open decision — where to normalize (`fft()`, `ifft()`, neither, or both partially). Today resolves this definitively, implements `ifft()`, and validates the full round-trip `ifft(fft(x)) == x` — the definitive test that the pair is mathematically consistent, not just individually correct.

## Block 1 — Deciding the Normalization Convention
Three common conventions:
1. **Asymmetric/unitary (most common — MATLAB/NumPy default):** `fft()` unnormalized, `ifft()` divided by `N`. Result: `ifft(fft(x)) == x` exactly.
2. **Symmetric:** both divided by `√N`. Preserves Parseval's theorem symmetrically — common in certain theoretical contexts.
3. **No normalization anywhere (FFTW's raw convention):** documented in Day 14, but `ifft(fft(x))` would yield `N·x`, not `x` — technically correct if well documented, but surprising to anyone expecting standard behavior.

**Deciding factor:** in an OFDM context, `ifft()` is used in modulation (frequency symbols → time domain for transmission) and `fft()` in demodulation (the inverse, at the receiver). The convention minimizing future bugs (forgetting to normalize manually somewhere in the pipeline) is the deciding criterion.

**Chosen convention:** #1 (asymmetric/unitary, MATLAB/NumPy style) — most predictable for future users of the class, and `ifft(fft(x)) == x` without surprises is a valuable property to test and rely on going forward.

## Block 2 — Implementing `ifft()`
Declare:
```cpp
Signal ifft() const;
```

Nearly identical structure to `fft()`:
- Plan direction: `FFTW_BACKWARD` instead of `FFTW_FORWARD`.
- **Normalization happens here** (per the chosen convention): after `fftw_execute(p)`, divide **every element** of the result by `N` (`this->length`), before returning.

**Implementation detail worth reasoning through:** the normalization loop must run *after* `fftw_execute()` (the data isn't valid until execution completes) but can run either before or after `fftw_destroy_plan()` — since the plan object and the output data buffer are independent once execution is done, destroying the plan doesn't affect the data already written to the output buffer.

Reuses the same structure as `fft()`: empty constructor for `out`, `fftw_malloc` for the buffer, `reinterpret_cast`, plan, execute, destroy — plus the normalization loop.

## Block 3 — Round-Trip Test: `ifft(fft(x)) == x`
The most important test of the day — validates that `fft()` and `ifft()` are consistent *with each other*, not just individually correct.

1. Create `Signal in(N)`.
2. `Signal freq = in.fft();`
3. `Signal recovered = freq.ifft();`
4. Compare `recovered` against `in`, element by element.

**Real challenge:** floating-point numbers rarely match *exactly* after a round trip through FFT (rounding errors accumulate across the FFT's internal butterfly stages). Compare using `std::abs(a - b) < tolerance` (e.g. `1e-9`) instead of `==`, where `std::abs` on two `std::complex<double>` values returns the magnitude of their difference.

**Access problem to solve:** comparing individual elements from `main()` requires a public accessor, since `data` is private. A named getter (`std::complex<double> get(size_t i) const`) is a straightforward, idiomatic solution — the same pattern already used for `is_valid()` and `Matrix::get_element` (Day 6). Operator overloading (`operator[]`) is a more elegant alternative but introduces its own subtleties (reference vs. value return, `const`/non-`const` overloads) — worth deferring as a separate exercise.

Write the comparison loop using `assert()`, following the established Day 5 testing pattern.

## Block 4 — Reflection
1. Which normalization convention was chosen, and does the reasoning match the suggestion, or is there a different justification preferred?
2. Why is floating-point comparison with a tolerance margin a genuinely new testing concept, not encountered in Day 5–7's tests (which used exact integer or fixed `+1/-1` values with no floating-point accumulation)?
3. With `fft()`/`ifft()` consistent and validated, what's structurally missing before a first modulator prototype (mapping symbols → `ifft()` → time-domain signal)?

## Key Lessons Learned
- The normalization convention is a design decision with real downstream consequences — choosing it deliberately (rather than defaulting to FFTW's raw behavior) is what makes `fft()`/`ifft()` safe to compose without manual correction elsewhere in a pipeline.
- Floating-point round-trip results are never bit-for-bit identical to the original due to accumulated rounding error across FFT's internal computation stages — testing such results requires a tolerance-based comparison (`std::abs(difference) < epsilon`), a genuinely different testing technique from exact-value assertions used with integers or fixed patterns.
- A named getter (`get(i)`) is a simple, sufficient way to expose read-only element access without breaking encapsulation — operator overloading (`operator[]`) is a valid alternative but adds complexity better addressed separately.
- `fftw_cleanup()` should be a standing habit in any new test `main()` that creates FFTW plans, not something to remember only once.