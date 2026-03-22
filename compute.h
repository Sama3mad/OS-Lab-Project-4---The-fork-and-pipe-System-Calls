#ifndef COMPUTE_H
#define COMPUTE_H

// Function pointer type: takes 2 ints, returns an int
typedef int (*func_t)(int a, int b);

// Reads numbers from file, applies f pairwise left-to-right, returns result
int sequential_compute(const char *filepath, func_t f);

// Same as sequential_compute but splits work across n_proc child processes
int parallel_compute(const char *filepath, int n_proc, func_t f);

#endif
