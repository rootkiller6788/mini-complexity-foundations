/* bench_space_classes.c — Space Complexity Benchmarks
 * Compare running times for L, NL, PSPACE algorithms.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "../include/spaceh.h"

static double now_ms(void) {
    return (double)clock() / CLOCKS_PER_SEC * 1000.0;
}

int main(void) {
    setbuf(stdout, NULL);
    printf("\n===== Space Complexity Benchmarks =====\n\n");

    printf("--- L: Logspace nth prime ---\n");
    printf("  %6s %10s %12s\n", "n", "prime", "time(ms)");
    for (int n = 50; n <= 400; n *= 2) {
        double t0 = now_ms();
        int cnt = 0, c = 1;
        while (cnt < n) {
            c++; int p = 1;
            for (int d = 2; d * d <= c; d++)
                if (c % d == 0) { p = 0; break; }
            if (p) cnt++;
        }
        double t1 = now_ms();
        printf("  %6d %10d %10.3f\n", n, c, t1 - t0);
    }

    printf("\n--- PSPACE: Matrix Multiply O(n^3) ---\n");
    printf("  %6s %12s %12s\n", "N", "ops", "time(ms)");
    for (int N = 4; N <= 64; N *= 2) {
        double t0 = now_ms();
        /* Allocate and multiply */
        int** A = malloc((size_t)N * sizeof(int*));
        int** B = malloc((size_t)N * sizeof(int*));
        int** C = malloc((size_t)N * sizeof(int*));
        for (int i = 0; i < N; i++) {
            A[i] = calloc((size_t)N, sizeof(int));
            B[i] = calloc((size_t)N, sizeof(int));
            C[i] = calloc((size_t)N, sizeof(int));
        }
        for (int i = 0; i < N; i++)
            for (int k = 0; k < N; k++)
                for (int j = 0; j < N; j++)
                    C[i][j] += A[i][k] * B[k][j];
        double t1 = now_ms();
        long long ops = (long long)N * N * N;
        printf("  %6d %10lld %10.3f\n", N, ops, t1 - t0);
        for (int i = 0; i < N; i++) { free(A[i]); free(B[i]); free(C[i]); }
        free(A); free(B); free(C);
    }

    printf("\n--- Space vs Time Tradeoff ---\n");
    printf("  More space often means less time.\n");
    printf("  PSPACE algorithms can solve problems TQBF that\n");
    printf("  no known polynomial-time algorithm can solve.\n");
    printf("  But PSPACE = P? This is a major open question.\n");

    return 0;
}
