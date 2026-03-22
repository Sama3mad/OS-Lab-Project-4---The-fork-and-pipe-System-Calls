# OS Lab Project — fork() & pipe()
### Sequential vs Parallel Compute

---

## What this project does

Implements two functions that apply any operation (add, multiply, etc.) on a list of numbers from a file:

- `sequential_compute` — does it in a single process, one number at a time
- `parallel_compute` — splits the work across `n_proc` child processes using pipes

---

## Files

| File | Purpose |
|------|---------|
| `compute.h` | Shared header — function declarations and the `func_t` type |
| `parse.h` | Reads numbers from a file (handles commas and newlines) |
| `sequential_compute.c` | Sequential implementation |
| `parallel_compute.c` | Parallel implementation using `fork()` and `pipe()` |
| `test_correctness.c` | 26 test cases that verify both functions are correct |

---

## How to compile

```bash
gcc -Wall -o test_correctness test_correctness.c sequential_compute.c parallel_compute.c
```

## How to run the tests

```bash
./test_correctness
```

All 26 tests should print `PASS`.

---

## Input file format

Numbers can be separated by **newlines** or **commas**. Negative numbers are supported.

```
1
2
3
4
5
```
or
```
1,2,3,4,5
```

---

## Example

Given the file above and the `add` function:

```
sequential_compute("file.txt", add)  →  15
parallel_compute("file.txt", 4, add) →  15
```

Both always return the same result regardless of how many processes are used.

---

## How parallel_compute works

1. Parent reads all numbers into an array
2. Creates 2 pipes per child: one to send the slice range, one to receive the result
3. Forks `n_proc` children — each gets a slice of the array
4. Each child computes its partial result and sends it back through a pipe
5. Parent collects all partial results and combines them using `f`

If `N` is not divisible by `n_proc`, the last child gets the remainder.

---

## Grade: 14% — Deadline: Monday March 23, 11:59 PM
