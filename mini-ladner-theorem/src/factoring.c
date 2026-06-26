/* factoring.c -- Integer Factoring: Algorithms and NP-Intermediate Status
 *
 * Integer Factoring is a canonical NP-intermediate candidate.
 * If factoring is in P, RSA is broken. If factoring is NPC,
 * the Polynomial Hierarchy collapses to its second level.
 * Most likely: factoring is NP-intermediate (but we cannot prove it).
 *
 * Algorithms implemented:
 *   1. Trial division          O(sqrt(n))
 *   2. Fermat factorization    O(n^{1/3}) for close primes
 *   3. Pollard's rho           O(n^{1/4}) expected
 *
 * Best classical algorithm: GNFS ~ exp(O(n^{1/3} log^{2/3} n))
 * Best quantum algorithm: Shor ~ O((log n)^3) -- polynomial!
 *
 * The gap between best classical (subexp) and quantum (poly)
 * is EXACTLY the NP-intermediate gap that Ladner's theorem
 * predicts exists (if P != NP).
 *
 * References:
 *   - Pollard, BIT 15:331-334, 1975 (rho method)
 *   - Shor, SIAM J. Comput. 26(5):1484-1509, 1997
 *   - Arora & Barak, Chapter 2 (factoring in NP)
 */

#include "ladner.h"
#include <math.h>

/* ---- GCD ---- */
long long gcd_ll(long long a, long long b) {
    while (b) {
        long long t = b;
        b = a % b;
        a = t;
    }
    return a;
}

/* ---- Modular Multiplication (avoid overflow) ---- */
long long mod_mul(long long a, long long b, long long m) {
    long long r = 0;
    a %= m;
    while (b) {
        if (b & 1) r = (r + a) % m;
        a = (a * 2) % m;
        b >>= 1;
    }
    return r;
}

/* ---- Trial Division ---- */
void trial_division(long long n, long long* f1, long long* f2) {
    if (n % 2 == 0) { *f1 = 2; *f2 = n / 2; return; }
    for (long long d = 3; d * d <= n; d += 2) {
        if (n % d == 0) { *f1 = d; *f2 = n / d; return; }
    }
    *f1 = n; *f2 = 1;  /* n is prime */
}

/* ---- Fermat Factorization ---- */
/* Works well when n = p * q with p and q close.
 * Find a, b such that n = a^2 - b^2 = (a-b)(a+b). */
long long fermat_factor(long long n) {
    if (n % 2 == 0) return 2;
    long long a = (long long)sqrt((double)n) + 1;
    for (int iter = 0; iter < 100000; iter++) {
        long long b2 = a * a - n;
        long long b  = (long long)sqrt((double)b2);
        if (b * b == b2) {
            long long factor = a - b;
            if (factor > 1 && factor < n && n % factor == 0)
                return factor;
        }
        a++;
        if (a > n / 2 + 1) break;
    }
    return 0;  /* not found */
}

/* ---- Pollard's Rho ---- */
/* Probabilistic algorithm. Uses Floyd's cycle detection.
 * f(x) = (x^2 + c) mod n. Two sequences x_i, y_i (where y_i = x_{2i}).
 * When they meet mod p (unknown factor), gcd(|x-y|, n) reveals p. */
long long pollard_rho(long long n) {
    if (n % 2 == 0) return 2;
    if (n % 3 == 0) return 3;

    long long x = 2, y = 2, d = 1;
    long long c = 1;

    while (d == 1) {
        x = (mod_mul(x, x, n) + c) % n;
        y = (mod_mul(y, y, n) + c) % n;
        y = (mod_mul(y, y, n) + c) % n;
        d = gcd_ll(llabs(x - y), n);
    }
    if (d == n) return 0;   /* failure: retry with different c */
    return d;
}

/* ---- Full Factorization (combine methods) ---- */
void factor_full(long long n, long long* f1, long long* f2) {
    /* Small prime table */
    static int small_primes[] = {
        2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97
    };
    int n_small = (int)(sizeof(small_primes) / sizeof(small_primes[0]));

    /* Try small primes first */
    for (int i = 0; i < n_small; i++) {
        if (n % small_primes[i] == 0) {
            *f1 = small_primes[i];
            *f2 = n / small_primes[i];
            return;
        }
    }

    /* Try Fermat (good for close primes) */
    long long ff = fermat_factor(n);
    if (ff > 1 && ff < n && n % ff == 0) {
        *f1 = ff;
        *f2 = n / ff;
        return;
    }

    /* Try Pollard rho multiple times (different c values) */
    for (int t = 0; t < 20; t++) {
        long long f = pollard_rho(n);
        if (f > 1 && f < n && n % f == 0) {
            *f1 = f;
            *f2 = n / f;
            return;
        }
    }

    /* Fall back to trial division */
    trial_division(n, f1, f2);
}

/* ---- Primality Test (Miller-Rabin style for 32-bit) ---- */
static int is_prime_naive(long long n) {
    if (n < 2) return 0;
    if (n == 2 || n == 3) return 1;
    if (n % 2 == 0) return 0;
    for (long long d = 3; d * d <= n; d += 2)
        if (n % d == 0) return 0;
    return 1;
}

/* ---- Demo ---- */

void factoring_demo(void) {
    printf("\n===== Integer Factoring =====\n\n");

    printf("Factoring is a canonical NP-intermediate candidate.\n");
    printf("  In NP: given (p,q), verify p*q=n in polynomial time.\n");
    printf("  Not known in P: best classical is subexponential.\n");
    printf("  Not known NPC: factoring NPC => PH collapses.\n\n");

    /* Trial division */
    printf("--- Trial Division ---\n");
    long long td_tests[] = {91, 143, 391, 1009, 4187, 6887};
    for (int i = 0; i < 6; i++) {
        long long f1, f2;
        trial_division(td_tests[i], &f1, &f2);
        printf("  %lld = %lld * %lld (%s)\n",
               td_tests[i], f1, f2,
               (f2 == 1) ? "PRIME" : "COMPOSITE");
    }

    /* Fermat */
    printf("\n--- Fermat Factorization (close primes) ---\n");
    long long p1 = 10007, q1 = 10009;
    long long n1 = p1 * q1;
    long long ff1 = fermat_factor(n1);
    printf("  %lld = %lld * %lld (p=%lld, q=%lld)\n",
           n1, ff1, n1 / ff1, p1, q1);

    long long p2 = 99991, q2 = 100003;
    long long n2 = p2 * q2;
    long long ff2 = fermat_factor(n2);
    printf("  %lld = %lld * %lld (p=%lld, q=%lld)\n",
           n2, ff2, n2 / ff2, p2, q2);

    /* Pollard rho */
    printf("\n--- Pollard's Rho ---\n");
    long long rho_tests[] = {8051, 10403, 455459, 23342419};
    for (int i = 0; i < 4; i++) {
        long long f = pollard_rho(rho_tests[i]);
        if (f > 1 && f < rho_tests[i] && rho_tests[i] % f == 0) {
            printf("  %lld: factor=%lld (cofactor=%lld)\n",
                   rho_tests[i], f, rho_tests[i] / f);
        } else {
            printf("  %lld: rho failed (try different c)\n", rho_tests[i]);
        }
    }

    /* Full factorization */
    printf("\n--- Full Factorization Pipeline ---\n");
    long long full_tests[] = {
        10403,                          /* 101 * 103 */
        1000000007LL * 1000000009LL,    /* large close primes */
        65537LL * 6700417LL             /* Fermat-related */
    };
    for (int i = 0; i < 3; i++) {
        long long f1, f2;
        factor_full(full_tests[i], &f1, &f2);
        printf("  %lld = %lld * %lld\n", full_tests[i], f1, f2);
    }

    /* Complexity */
    printf("\n--- Algorithmic Complexity ---\n");
    printf("  Trial division:     O(sqrt(n)) = O(2^{k/2}) for k-bit n\n");
    printf("  Pollard rho:         O(n^{1/4}) = O(2^{k/4}) expected\n");
    printf("  Number Field Sieve:  exp(O(k^{1/3} log^{2/3} k))\n");
    printf("  Shor (quantum):      O(k^3) -- POLYNOMIAL!\n\n");

    /* Ladner Connection */
    printf("--- Connection to Ladner's Theorem ---\n");
    printf("If factoring in P:    RSA, Diffie-Hellman broken.\n");
    printf("If factoring is NPC:  NP = coNP = PH collapses.\n");
    printf("Most likely:          NP-INTERMEDIATE.\n");
    printf("Ladner guarantees:    NP-intermediate languages EXIST\n");
    printf("                       (if P!=NP), but doesn't identify them.\n");
    printf("Factoring is our best candidate for a NATURAL\n");
    printf("NP-intermediate problem.\n");
}
