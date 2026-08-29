# Day 9 — Rule of Three (Copy Constructor + Copy Assignment Operator)

## Objective
Extend the `Signal` class (introduced Day 8) to correctly handle object
copying and assignment. Reproduce, diagnose, and fix the classic C++
shallow-copy bug — first via the compiler-generated copy constructor, then
via the compiler-generated copy assignment operator — using valgrind as
the empirical proof at every step.

---

## Block 1 — Triggering the bug on purpose (copy constructor)

Using the `Signal` class exactly as it stood at the end of Day 8, write:

```cpp
Signal s1(5);
Signal s2 = s1;
```

Before compiling: based on Day 3 (shallow copy of a C struct containing a
pointer), predict what the compiler-generated default copy constructor
does with the `data` field — does it allocate new memory and copy values,
or does it copy only the pointer address?

Compile, run, and confirm with `valgrind --leak-check=full`. Read the
valgrind output carefully:
- Which specific line proves two destructors tried to free the *same*
  address?
- Why does valgrind report only one error, not two, even though two
  `delete[]` calls happened on the same block?
- Why does the heap summary report no leaks, even though there clearly is
  a bug?

## Block 2 — Writing a correct copy constructor

Design (on paper/in words first) a copy constructor with signature
`Signal(const Signal &other)`. Identify the ordered steps required so that
`s2.data` and `s1.data` end up pointing to *different* addresses while
holding the same values:
1. Copy `length`
2. Allocate a new block of the correct size
3. Copy values element by element (a loop — this should feel familiar from
   Day 6/7 Matrix indexing)
4. Assign the new block to `this->data`

Implement it, handling both the valid (`other.is_valid()`) and invalid
case. Verify with valgrind: zero errors, zero leaks, one destructor call
per object, each freeing a distinct address.

## Block 3 — Triggering the bug on purpose (copy assignment)

Different scenario — the target object already exists:

```cpp
Signal s1(5);
Signal s3(3);   // s3 already constructed, already owns memory
s3 = s1;
```

Before compiling: why does this line **not** call the copy constructor you
just wrote? What operator does the compiler look for instead, and why is
this operator's default behavior potentially *worse* than the copy
constructor's default (hint: `s3` already owned memory before this line)?

Note the compiler warning about the deprecated implicit `operator=` when a
user-provided copy constructor exists — the compiler itself is flagging
an incomplete Rule of Three. Run with valgrind and confirm **two distinct**
problems in the output: a double free, and a separate memory leak (the
original memory `s3` owned before the assignment overwrote its pointer).

## Block 4 — Writing a correct copy assignment operator

`operator=` differs from the copy constructor in one structurally
important way: `this->data` already points to valid memory when the
function runs. Work through these before writing code:

- **Leak prevention:** what must happen to `this`'s old memory before it's
  overwritten with the new pointer?
- **Self-assignment (`s1 = s1`):** if the naive order is (1) `delete[]
  this->data`, then (2) allocate + copy from `other.data` — what happens
  when `other` and `*this` are the same object? What does step 2 read
  from, after step 1 already freed it? (This is undefined behavior /
  use-after-free — and it can appear to "work" by coincidence, which is
  part of why it's dangerous.)
- **Return type:** why does `operator=` need to return `Signal&` (not
  `void`, not `bool`) to support chained assignment like `a = b = c;`?

Implement `Signal& Signal::operator=(const Signal &other)` with:
1. Self-assignment guard (`this != &other`)
2. Release of `this`'s old memory
3. A validity branch mirroring the copy constructor — handle `other`
   valid *and* `other` invalid (this branch is easy to miss: the copy
   constructor already had it from the start, the assignment operator
   does not get it "for free")
4. `return *this;`

Validate with three separate valgrind runs:
- Normal assignment between two valid signals
- Self-assignment (`s1 = s1`)
- Assignment from an invalid `Signal` (`length == 0`) to a valid one —
  confirm the target's `is_valid()` correctly reports `false` afterward,
  and confirm no leak from the target's prior memory

---

## Reflection Questions
1. Why does owning a raw resource (heap memory via a raw pointer) force
   destructor, copy constructor, *and* copy assignment operator to be
   written together, rather than any one or two of them in isolation?
2. `Matrix` (still a C struct) will face the identical problem once
   converted to a C++ class. Does knowing the Rule of Three requirement
   in advance change how you'll approach that conversion, compared to
   discovering it via a crash the way `Signal` did?
3. Copying a large buffer element-by-element has a real cost. Is that
   copy always necessary, or can "transferring ownership" of the pointer
   sometimes achieve the same result at near-zero cost? (This question is
   left open — it's the seed of Day 10, move semantics / Rule of Five.)

---

## Key Lessons Learned
- The compiler-generated copy constructor and copy assignment operator
  both perform **memberwise (shallow) copy** — correct and safe for plain
  data members, but incorrect whenever a member is a raw pointer that
  represents ownership of a resource.
- A double free and a memory leak are two *distinct* failure modes that
  can occur from the same root cause (shallow copy) depending on whether
  the target object already owned memory before the copy/assignment.
- `operator=` requires an explicit self-assignment guard (`this !=
  &other`); without it, freeing `this->data` before reading from
  `other.data` in the self-assignment case causes a use-after-free that
  may not fail visibly every time.
- Writing a correct copy constructor does **not** automatically give you
  a correct copy assignment operator — they must independently mirror the
  same validity handling (e.g. checking `other.is_valid()`), since the two
  functions solve structurally different problems (initializing a new
  object vs. overwriting an existing one).
- A compiler warning (`-Wdeprecated-copy`) can itself be a diagnostic
  signal that the Rule of Three is incomplete, before any runtime tool
  like valgrind is even run.