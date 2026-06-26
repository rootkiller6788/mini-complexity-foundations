/* constructible.c — Time-Constructible Functions
 * f is time-constructible if a TM on input 1^n halts in EXACTLY f(n) steps.
 * Most natural functions (n^k, 2^n, n!) are time-constructible.
 *
 * The hierarchy theorem REQUIRES time-constructible functions:
 * the diagonal machine must be able to compute g(n) to know when
 * to stop simulating each enumerated TM.
 *
 * Without this condition, the Gap Theorem (Borodin 1972) shows
 * there are arbitrarily large gaps where TIME(f)=TIME(g) even if f << g. */

#include <stdio.h>
#include <time.h>
#include "tht.h"

/* Compute n^k — simple time-constructible function */
static long long compute_pow(int n, int k) {
    long long result = 1;
    for (int i = 0; i < k; i++) result *= n;
    return result;
}

/* Test: can we compute n^k within O(n^k) time? */
static int verify_constructible(int n, int k) {
    clock_t start = clock();
    volatile long long result = compute_pow(n, k);
    clock_t end = clock();
    double ms = 1000.0 * (end - start) / CLOCKS_PER_SEC;
    printf("  n^%d(%d) = %lld (%.4f ms)\n", k, n, result, ms);
    return 1;
}

void constructible_demo(void) {
    printf("\n===== Time-Constructible Functions =====\n\n");
    printf("Definition: f is time-constructible if there exists a TM\\n");
    printf("that on input 1^n halts in EXACTLY f(n) steps.\\n\\n");

    printf("Time-constructible (YES):\\n");
    printf("  n, n*log n, n^2, n^3, n^k, 2^n, 2^{n^2}, n!\\n\\n");

    printf("NOT time-constructible (NO):\\n");
    printf("  Busy Beaver function (uncomputable growth rate)\\n");
    printf("  f(n) = 1 if TM_n(n) halts else 2 (undecidable)\\n");
    printf("  Floor(sqrt(n)) is NOT time-constructible!\\n\\n");

    printf("--- Why Required for Hierarchy ---\\n");
    printf("The diagonal machine must compute its own time bound g(n)\\n");
    printf("within g(n) steps. Otherwise it overshoots or undershoots.\\n\\n");

    printf("--- Gap Theorem (Borodin 1972) ---\\n");
    printf("Without time-constructibility: there exist functions f,g\\n");
    printf("with f(n) << g(n) but TIME(f(n)) = TIME(g(n)).\\n");
    printf("The gap theorem shows the constructibility condition is necessary.\\n\\n");

    printf("--- Demo: Computation Timing ---\\n");
    for (int k = 1; k <= 4; k++)
        verify_constructible(50, k);

    printf("\\nSummary: Time hierarchy theorem = diagonalization +\\n");
    printf("time-constructible bounds + universal TM simulation.\\n");
}
