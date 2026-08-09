# Day 11 — Eigen Introduction: Storage Order (Row-Major vs Column-Major)

## Objective
Introduce the Eigen linear algebra library and confront, empirically, the storage-order mismatch between Eigen's default convention and the row-major convention used throughout the project's own C `Matrix` class (Phase 1, Day 6).

## What was built
- Installed Eigen (header-only, `libeigen3-dev`), confirmed it compiles with a bare `#include <Eigen/Dense>`.
- Wrote a small program that fills an `Eigen::MatrixXd` and dumps its raw underlying memory via `.data()`, to determine Eigen's default storage order empirically rather than by reading documentation.
- Wrote a comparison program mapping a raw C-style row-major `double[6]` buffer with `Eigen::Map` twice: once using Eigen's default type (`Eigen::MatrixXd`, column-major) and once with an explicit `Eigen::Matrix<double, Dynamic, Dynamic, RowMajor>` type — to see how the same raw bytes get reinterpreted differently depending on the declared storage order.
- Solved a 2x2 linear system `Ax = b` using `A.colPivHouseholderQr().solve(b)`.

## Key correction
Initially assumed MATLAB uses row-major storage, same as C. This is wrong: **MATLAB is column-major**, inherited from the Fortran/BLAS/LAPACK numerical-computing lineage — the same lineage that led Eigen to default to column-major storage as well.

## Verification
- Predicted the raw memory dump of a `[[1,2,3],[4,5,6]]` matrix before running: predicted `1 4 2 5 3 6` (column-major order) — matched exactly.
- Predicted the output of `Eigen::Map` with the wrong (default, column-major) type over a row-major raw buffer before running: predicted `[[1,3,5],[2,4,6]]` — matched exactly. The `RowMajor`-typed map correctly reproduced the original `[[1,2,3],[4,5,6]]`.
- Hand-solved the linear system `2x + y = 5, x + 3y = 10` before running the solver: predicted `x=1, y=3` — matched the Eigen solver's output exactly.

## Key takeaways
- `Eigen::MatrixXd` defaults to column-major storage.
- `Eigen::Map` does **not** infer the layout of an existing raw buffer — it must be told explicitly via the `RowMajor` template parameter, or Eigen will silently reinterpret row-major data as column-major. No compile error, no crash — just wrong numbers. This is exactly the kind of bug that would matter when wrapping the project's own row-major `Matrix`/`Signal` buffers (or FFTW output) with `Eigen::Map` later, without copying.
- Solving `Ax = b` is the canonical form behind linear estimation problems such as MMSE channel estimation in the PHY layer — not a coincidental syntax match with the project's eventual OFDM channel-estimation stage.

## Next
Day 12: FFTW (FFT) introduction, then CMake as a more robust build system than raw Makefiles.