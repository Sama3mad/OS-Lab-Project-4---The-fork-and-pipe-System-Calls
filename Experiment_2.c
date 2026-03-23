#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define REPEATS 7   // odd

int fun(int a, int b) { return a * b; }


int sequential_compute(const char *filename, int (*f)(int, int));
int parallel_compute(const char *filename, int n_proc, int (*f)(int, int));

// Time
double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

// Sort helper
int cmp_double(const void *a, const void *b) {
    double x = *(double *)a;
    double y = *(double *)b;
    return (x > y) - (x < y);
}

// Median
double median(double arr[]) {
    qsort(arr, REPEATS, sizeof(double), cmp_double);
    return arr[REPEATS / 2];
}

void generate_file(const char *filename, int N) {
    FILE *fp = fopen(filename, "w");
    for (int i = 1; i <= N; i++) {
        fprintf(fp, "%d\n", i);
    }
    fclose(fp);
}

// Experiment 2
void experiment_fixed_N() {
    const char *filename = "data.txt";
    int N = 200000;

    generate_file(filename, N);

    printf("n_proc,Sequential,Parallel\n");

    for (int n_proc = 1; n_proc <= 16; n_proc++) {
        double seq_times[REPEATS];
        double par_times[REPEATS];

        for (int r = 0; r < REPEATS; r++) {
            double start, end;

            // Sequential
            start = get_time();
            sequential_compute(filename, fun);
            end = get_time();
            seq_times[r] = end - start;

            // Parallel
            start = get_time();
            parallel_compute(filename, n_proc, fun);
            end = get_time();
            par_times[r] = end - start;
        }

        double seq_time = median(seq_times);
        double par_time = median(par_times);

        printf("%d,%f,%f\n", n_proc, seq_time, par_time);
    }
}

int main() {
    experiment_fixed_N();
    return 0;
}