# cpp-ofdm

> ⚠️ **Work in progress.** Core modulation pipeline works and is validated; channel model, BER analysis and test suite are next. See [Roadmap](#roadmap).

An OFDM modulator/demodulator written from scratch in C++17, using FFTW for the transforms. Built as a structured learning project — the full day-by-day log is in [`journal/`](journal/).

## What works today
- `Signal` and `Matrix` classes with RAII, Rule of Five and FFTW-aligned memory
- FFT / normalized IFFT via FFTW
- Gray-coded QPSK modulation and hard-decision demodulation
- OFDM modulator (bits → QPSK → subcarriers → IFFT) and demodulator (FFT → QPSK decision → bits)
- End-to-end bits-to-bits round trip validated; Valgrind reports no leaks or memory errors

## Build & run
Requires CMake ≥ 3.10, a C++17 compiler and FFTW3 (`sudo apt install libfftw3-dev` on Debian/Ubuntu).

```sh
cmake -S lib -B build
cmake --build build
./build/testCXX
```

Expected output:

```
OFDM round-trip OK: 16/16 bits recovered
```

## Project layout
- `lib/` — the OFDM engine (`Signal`, `Matrix`, `modulation`) and demo `main.cpp`
- `journal/` — daily learning notes: design decisions, bugs found and their root causes

## Roadmap
- [ ] AWGN channel and BER vs Eb/N0 curve compared against QPSK theory
- [ ] Unit tests with CTest + GitHub Actions CI
- [ ] Cyclic prefix
- [ ] Multipath channel and one-tap frequency-domain equalization
- [ ] Code cleanup (constellation energy normalization, input validation)

## License
MIT — see [LICENSE](LICENSE).
