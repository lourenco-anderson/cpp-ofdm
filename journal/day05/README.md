# Day 5 — Basic Testing with `assert()` & Multi-Target Makefiles

**Why this matters:** You can't validate an OFDM modulator just by "eyeballing the output" — you need tests that automatically confirm the logic is correct. This also connects directly to the open question from Day 4: how do you test a component (e.g. the `Signal` functions) in isolation, without running the full `main()` program?

## Block 1 — `assert()` and a Simple Test File
Create a new file, `test_signal.c`, that:
- Includes `signal.h` (not `main.c`).
- Defines separate test functions for distinct scenarios (e.g. `test_len5(void)`, `test_len0(void)`) rather than one monolithic `main` — this makes each case independently readable and matches how real test suites are organized.
- Uses **hardcoded, fixed values** (not `scanf` from the user) — a test must run the same way every time, without manual input, so it can be re-run automatically (by you, or by a future CI pipeline) and its pass/fail status trusted.

**`test_len5`:** call `create_signal(5)` and verify with `assert()`:
- `signal.data != NULL`
- `signal.length == 5`
- **Every** element matches the expected alternating pattern (+1, -1, +1, -1, +1) — use a loop, not just the first element. Prefer explicit `if`/`else` over a ternary operator for selecting which assertion to run (a ternary is meant to produce a value, not to select a side effect).
- Free the signal with `free_signal` at the end.

**`test_len0`:** call `create_signal(0)` and verify the **error path**, not the success path:
- `signal.data == NULL`
- `signal.length == 0`
- Do **not** dereference `signal.data` here — the sentinel struct has no valid data to check.

**Common pitfall to watch for:** don't compare a pointer directly to a value (`signal.data == 1.0` is a compile error — `signal.data` is `double*`, not `double`). Dereference it first (`*signal.data` or `signal.data[i]`).

**Verify the test is real, not just passing by luck:** temporarily break one expected value (e.g. change `-1` to `-10` in the loop), rebuild, and confirm `assert` fails with a clear message showing file, line, function, and the exact failing condition. Then revert.

**Key distinction to internalize:** `assert()` is for conditions that should **never** be false if the code is correct (a programming invariant — e.g. "the array I just created has the right values"). The `if (error) { ...; return code; }` pattern from Day 3 is for conditions that **can legitimately happen** in production (e.g. a user entering `0`). Using `assert()` to validate user input is a mistake in practice: `assert` is typically **disabled entirely** in production builds (via `NDEBUG`), so that check would silently vanish.

## Block 2 — Multi-Target Makefile
Expand the Makefile to build **two** executables from the same set of pattern-rule-generated object files:

```makefile
CC = gcc
CFLAGS = -Wall -Wextra

all: app test_signal

app: signal.o main.o
	$(CC) $(CFLAGS) -o $@ $^

test_signal: signal.o test_signal.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
```

Note that `app` and `test_signal` are **separate rules**, each listing only the objects it actually needs (`app` doesn't depend on `test_signal.o`, and vice versa), but both reuse the same pattern rule (`%.o: %.c`) — no need to write an explicit rule for `test_signal.o`.

**Before running:** predict how many compile commands and how many link commands plain `make` should produce, given 3 total `.c` files and 2 final executables. Run `rm -f *.o app test_signal` then `make`, and confirm your prediction.

**Bonus — `clean` target:**
```makefile
.PHONY: clean
clean:
	rm -f *.o app test_signal
```

**Why `.PHONY` is needed:** `make` normally decides whether to run a rule's command by comparing timestamps between the target and its dependencies — it assumes a target name corresponds to a file it should produce. If a real file literally named `clean` ever existed in the folder, `make` would see it already "exists" and skip the `rm` command, thinking it's up to date. `.PHONY: clean` tells `make` explicitly: this target does not correspond to a real output file — always run its command when requested, regardless of timestamps.

Test it:
```
make clean
ls
make
```
Confirm the `.o` files and both executables disappear after `clean`, and everything rebuilds cleanly afterward.

## Block 3 — Reflection
1. What's the difference in purpose between `assert()` and the `if (error) {...; return code;}` pattern? When is each appropriate?
2. Why is testing via a separate `test_signal.c` more reliable than running `app` and manually checking the output each time?

## Key Lessons Learned
- A failing assertion doesn't always mean the tested code has a bug — it can mean the test itself was written incorrectly (this happened with an early version of `test_len0`, which copied `test_len5`'s assertions without adapting them to the error path).
- `assert()` checks a single boolean expression; validating every element of an array requires an explicit loop with one assertion per element (or per iteration).
- A ternary operator is for producing a value, not for selecting which side-effecting statement (like `assert`) to execute — prefer explicit `if`/`else` for that.
- A multi-target Makefile can share object files and a single pattern rule across multiple executables without duplicating any compilation logic — `make` resolves each target's own dependency subgraph independently.
- `.PHONY` targets exist for rules that don't produce a real output file (like `clean`), disabling the default timestamp-based "already up to date" check for that target specifically.