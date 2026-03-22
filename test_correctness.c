#include "compute.h"
#include <stdio.h>
#include <stdlib.h>

/* ── Sample functions ───────────────────────────────────────────── */
int add(int a, int b)      { return a + b; }
int multiply(int a, int b) { return a * b; }
int max_f(int a, int b)    { return a > b ? a : b; }

/* ── Test helpers ────────────────────────────────────────────────── */
static void write_file(const char *path, int *nums, int n) {
    FILE *fp = fopen(path, "w");
    for (int i = 0; i < n; i++) {
        if (i > 0) fprintf(fp, "\n");
        fprintf(fp, "%d", nums[i]);
    }
    fclose(fp);
}

static void check(const char *label, int got, int expected) {
    if (got == expected)
        printf("  PASS  %-45s  got=%d\n", label, got);
    else
        printf("  FAIL  %-45s  got=%d  expected=%d\n", label, got, expected);
}

/* ── Test cases ──────────────────────────────────────────────────── */
void test_sequential() {
    printf("\n=== sequential_compute ===\n");
    int a[] = {1, 2, 3, 4, 5};
    write_file("/tmp/t1.txt", a, 5);

    // add(add(add(add(1,2),3),4),5) = 15
    check("add [1,2,3,4,5]", sequential_compute("/tmp/t1.txt", add), 15);

    // multiply: 1*2*3*4*5 = 120
    check("multiply [1,2,3,4,5]", sequential_compute("/tmp/t1.txt", multiply), 120);

    // max: 5
    check("max [1,2,3,4,5]", sequential_compute("/tmp/t1.txt", max_f), 5);

    // single element
    int b[] = {42};
    write_file("/tmp/t2.txt", b, 1);
    check("single element 42", sequential_compute("/tmp/t2.txt", add), 42);

    // two elements
    int c[] = {10, 7};
    write_file("/tmp/t3.txt", c, 2);
    check("add [10,7]", sequential_compute("/tmp/t3.txt", add), 17);
}

void test_parallel() {
    printf("\n=== parallel_compute ===\n");

    int a[] = {1, 2, 3, 4, 5};
    write_file("/tmp/t1.txt", a, 5);

    // N=5 divisible by n_proc=5: each child gets 1 number
    check("add [1..5] n_proc=5", parallel_compute("/tmp/t1.txt", 5, add), 15);

    // N=5 with n_proc=2: child0=[1,2], child1=[3,4,5]  (remainder goes to last)
    // child0 partial = 1+2 = 3,  child1 partial = 3+4+5 = 12,  final = 3+12 = 15
    check("add [1..5] n_proc=2", parallel_compute("/tmp/t1.txt", 2, add), 15);

    // N=5 with n_proc=3: chunk=1 rem=2
    // child0=[1], child1=[2], child2=[3,4,5]
    // partials: 1, 2, 12  -> 1+2+12 = 15
    check("add [1..5] n_proc=3", parallel_compute("/tmp/t1.txt", 3, add), 15);

    // n_proc=1 should behave like sequential
    check("add [1..5] n_proc=1", parallel_compute("/tmp/t1.txt", 1, add), 15);

    // multiply
    check("multiply [1..5] n_proc=5", parallel_compute("/tmp/t1.txt", 5, multiply), 120);
    check("multiply [1..5] n_proc=2", parallel_compute("/tmp/t1.txt", 2, multiply), 120);

    // 10,20,30 with multiply: 10*20*30 = 6000
    int b[] = {10, 20, 30};
    write_file("/tmp/t4.txt", b, 3);
    check("multiply [10,20,30] n_proc=2", parallel_compute("/tmp/t4.txt", 2, multiply), 6000);
    check("multiply [10,20,30] n_proc=3", parallel_compute("/tmp/t4.txt", 3, multiply), 6000);

    // Edge: N not divisible by n_proc
    // 7 numbers, 3 processes: chunk=2, remainder=1
    // child0=[1,2], child1=[3,4], child2=[5,6,7]  -> 3 + 7 + 18 = 28
    int c[] = {1, 2, 3, 4, 5, 6, 7};
    write_file("/tmp/t5.txt", c, 7);
    check("add [1..7] n_proc=3 (N not div by nproc)", parallel_compute("/tmp/t5.txt", 3, add), 28);
}

void test_both_agree() {
    printf("\n=== sequential vs parallel — must produce identical results ===\n");
    int nums[] = {-3, -7, -2, -9, -1, -5, -8, -4, -6, -10};
    write_file("/tmp/t6.txt", nums, 10);

    for (int np = 1; np <= 5; np++) {
        int s = sequential_compute("/tmp/t6.txt", add);
        int p = parallel_compute("/tmp/t6.txt", np, add);
        char label[64];
        snprintf(label, sizeof(label), "add [-3,-7,-2,-9,-1,-5,-8,-4,-6,-10] n_proc=%d", np);
        check(label, p, s);
    }

    for (int np = 1; np <= 4; np++) {
        int s = sequential_compute("/tmp/t6.txt", multiply);
        int p = parallel_compute("/tmp/t6.txt", np, multiply);
        char label[64];
        snprintf(label, sizeof(label), "multiply same array n_proc=%d", np);
        check(label, p, s);
    }
}

int main() {
    test_sequential();
    test_parallel();
    test_both_agree();

    printf("\nDone.\n");
    return 0;
}
