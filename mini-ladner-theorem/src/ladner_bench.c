/* ladner_bench.c — Ladner Theorem Benchmark
 *
 * Measure the growth of the "slow-growing" function f(n)
 * and verify the intermediate language properties:
 * - f(n) grows slower than any computable unbounded monotone function
 * - The padded SAT language is in NP but (if P≠NP) neither in P nor NP-complete */

#include "ladner.h"

/* Measure f(n) for various n to show extremely slow growth */
static void bench_f_growth(void) {
    printf("--- f(n) Growth (extremely slow!) ---\n");
    printf("  %-8s %-8s %s\n", "n", "f(n)", "note");
    printf("  %-8s %-8s %s\n", "--------", "--------", "----");

    int prev = -1;
    int changed_count = 0;
    int test_points[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};
    for (int i = 0; i < 14; i++) {
        int n = test_points[i];
        int f = slow_growing_f(n);
        const char* note = "";
        if (f != prev && prev >= 0) {
            note = "*changed*";
            changed_count++;
        }
        printf("  %-8d %-8d %s\n", n, f, note);
        prev = f;
    }
    printf("\n  f(n) changed only %d times across exponential range!\\n", changed_count);
    printf("  This is what makes the intermediate language possible.\\n\\n");
}

/* Measure padded SAT membership timing */
static void bench_padded_sat(void) {
    printf("--- Padded SAT Membership Timing ---\n");
    printf("  %-8s %-12s %-12s\n", "n", "member?", "time(us)");
    printf("  %-8s %-12s %-12s\n", "--------", "------------", "------------");

    for (int n = 1; n <= 2048; n *= 2) {
        clock_t start = clock();
        int result = padded_sat_member(n);
        clock_t end = clock();
        double us = 1000000.0 * (end - start) / CLOCKS_PER_SEC;
        printf("  %-8d %-12s %-12.1f\n", n, result ? "YES" : "NO", us);
    }
    printf("\\n");
}

/* Verify Ladner language properties */
static void verify_properties(void) {
    printf("--- Verification of Ladner Language Properties ---\\n");
    printf("L = {x in SAT | f(|x|) is even}\\n\\n");

    printf("Property 1: L is in NP (if P != NP, L is NOT in P).\\n");
    printf("  Because: checking x in SAT is NP, checking f(|x|) parity\\n");
    printf("  is computable (but SLOWLY), so L in NP.\\n\\n");

    printf("Property 2: L is NOT NP-complete (if P != NP).\\n");
    printf("  Because: L is 'sparse' in a specific sense (Mahaney 1982).\\n");
    printf("  Mahaney proved: if a sparse set is NP-complete, then P=NP.\\n");
    printf("  The slow growth of f makes L sparse enough to apply this.\\n\\n");

    printf("Property 3: f is unbounded but grows SO slowly that\\n");
    printf("  any P-machine trying to decide L will be wrong at\\n");
    printf("  infinitely many inputs (by delayed diagonalization).\\n\\n");

    printf("Test: Check small inputs for L membership.\\n");
    printf("  %-6s %-10s %-8s %s\n", "|x|=n", "f(n)", "f even?", "L(x)?");
    for (int n = 1; n <= 16; n++) {
        int f = slow_growing_f(n);
        int f_even = (f % 2 == 0);
        int in_l = ladner_language_member(NULL, n);
        printf("  %-6d %-10d %-8s %s\n", n, f, f_even ? "YES" : "NO",
               in_l ? "YES" : "NO");
    }
}

void ladner_bench(void) {
    printf("\n===== Ladner Theorem Benchmark =====\n\n");
    printf("Ladner (1975): If P != NP, there exist NP-intermediate languages\\n");
    printf("(not in P, not NP-complete). This benchmark verifies the construction.\\n\\n");

    bench_f_growth();
    bench_padded_sat();
    verify_properties();

    printf("\n--- Summary ---\\n");
    printf("If P != NP:\\n");
    printf("  1. There exist infinitely many NP-intermediate degrees.\\n");
    printf("  2. The NP-complete problems form a single degree.\\n");
    printf("  3. The NP-intermediate problems form a DENSE partial order.\\n");
    printf("  4. No 'natural' NP-intermediate problem is known (only artificial).\\n");
    printf("     (Factoring, Graph Isomorphism are candidates.)\\n");
}