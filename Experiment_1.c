#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "compute.h"

#define NUM_READINGS 100
#define REPEATS 7            // for smoothing

// Function pointer
int fun(int a, int b) { return a * b; }


int sequential_compute(const char *filename, int (*f)(int, int));
int parallel_compute(const char *filename, int n_proc, int (*f)(int, int));

// Time helper
double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int cmp_double(const void *a, const void *b) {
    double x = *(double *)a;
    double y = *(double *)b;
    if (x < y) return -1;
    if (x > y) return 1;
    return 0;
}

// Generate input file with N numbers
void generate_file(const char *filename, int N) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("File error");
        exit(1);
    }

    for (int i = 1; i <= N; i++) {
        fprintf(fp, "%d\n", i);
    }

    fclose(fp);
}

// Experiment: Fix n_proc, vary N
void experiment_fixed_nproc(int n_proc) {
    const char *filename = "data.txt";

    printf("N,Sequential,Parallel\n");

    for (int i = 1; i <= NUM_READINGS; i++) {
        int N = i * 20000;       // edit as needed

        generate_file(filename, N);

        double seq_times[REPEATS];
        double par_times[REPEATS];

        for (int r = 0; r < REPEATS; r++) {
            double start, end;

            // Sequential timing
            start = get_time();
            sequential_compute(filename, fun);
            end = get_time();
            seq_times[r] = (end - start);

            // Parallel timing
            start = get_time();
            parallel_compute(filename, n_proc, fun);
            end = get_time();
            par_times[r] = (end - start);
        }

        // Median
        qsort(seq_times, REPEATS, sizeof(double), cmp_double);
        qsort(par_times, REPEATS, sizeof(double), cmp_double);

        double seq_time, par_time;

        if (REPEATS % 2 == 0) {
            seq_time = (seq_times[REPEATS/2 - 1] + seq_times[REPEATS/2]) / 2.0;
            par_time = (par_times[REPEATS/2 - 1] + par_times[REPEATS/2]) / 2.0;
        } else {
            seq_time = seq_times[REPEATS/2];
            par_time = par_times[REPEATS/2];
        }

        printf("%d,%f,%f\n", N, seq_time, par_time);
    }
}

int main() {
    experiment_fixed_nproc(9);
    return 0;
}
