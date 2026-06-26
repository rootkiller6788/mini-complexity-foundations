/* factoring_demo.c -- End-to-end factoring example
 *
 * Demonstrates integer factoring as an NP-intermediate candidate.
 * Uses Pollard rho, Fermat, and trial division algorithms
 * from src/factoring.c.
 *
 * Build: make factoring_demo
 * Run:   ./examples/factoring_demo
 */

#include "ladner.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    setbuf(stdout, NULL);

    printf("\n========================================\n");
    printf("  Factoring Demo: NP-Intermediate Candidate\n");
    printf("========================================\n\n");

    printf("Factoring is in NP: given factors (p,q), verify p*q=n.\n");
    printf("Best classical: GNFS ~ exp(O(n^{1/3} log^{2/3} n)).\n");
    printf("Not known in P. If NPC, PH collapses.\n");
    printf("Most likely: NP-INTERMEDIATE (Ladner's theorem).\n\n");

    srand((unsigned)time(NULL));

    /* Demo 1: Small composites with trial division */
    printf("--- Trial Division ---\n");
    long long small_tests[] = {91, 143, 391, 1009, 4187};
    for (int i = 0; i < 5; i++) {
        long long f1, f2;
        trial_division(small_tests[i], &f1, &f2);
        printf("  %lld = %lld * %lld", small_tests[i], f1, f2);
        if (f2 == 1) printf(" (PRIME)");
        printf("\n");
    }

    /* Demo 2: Fermat for close primes */
    printf("\n--- Fermat Factorization (close primes) ---\n");
    long long primes[][2] = {{10007,10009}, {99991,100003}, {1000003,1000033}};
    for (int i = 0; i < 3; i++) {
        long long n = primes[i][0] * primes[i][1];
        long long f = fermat_factor(n);
        printf("  %lld = %lld * %lld (p=%lld, q=%lld)\n",
               n, f, n/f, primes[i][0], primes[i][1]);
    }

    /* Demo 3: Pollard rho */
    printf("\n--- Pollard Rho ---\n");
    long long rho_tests[] = {8051, 10403, 455459};
    for (int i = 0; i < 3; i++) {
        long long f = pollard_rho(rho_tests[i]);
        if (f > 1 && f < rho_tests[i] && rho_tests[i] % f == 0) {
            printf("  %lld has factor %lld (cofactor %lld)\n",
                   rho_tests[i], f, rho_tests[i] / f);
        } else {
            printf("  %lld: rho failed (retry with different c)\n",
                   rho_tests[i]);
        }
    }

    /* Demo 4: Full pipeline */
    printf("\n--- Full Factorization Pipeline ---\n");
    long long full_tests[] = {
        8051,
        10007LL * 10009LL,
        65537LL * 6700417LL
    };
    for (int i = 0; i < 3; i++) {
        long long f1, f2;
        factor_full(full_tests[i], &f1, &f2);
        printf("  %lld = %lld * %lld\n", full_tests[i], f1, f2);
    }

    /* Complexity comparison */
    printf("\n--- Complexity Comparison ---\n");
    printf("  Trial division:     O(sqrt(n))\n");
    printf("  Pollard rho:         O(n^{1/4}) expected\n");
    printf("  Number Field Sieve:  subexponential\n");
    printf("  Shor (quantum):      O((log n)^3) -- polynomial!\n\n");

    printf("--- Ladner Connection ---\n");
    printf("If factoring is in P: RSA broken, crypto revolution.\n");
    printf("If factoring is NPC: PH collapses (unlikely).\n");
    printf("Most likely: NP-intermediate (Ladner proves such\n");
    printf("languages exist, factoring might be one).\n\n");

    printf("Factoring is our best natural candidate for a\n");
    printf("problem in the NP-intermediate zone predicted by\n");
    printf("Ladner's Theorem (1975).\n");

    return 0;
}
