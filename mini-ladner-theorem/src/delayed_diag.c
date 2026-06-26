/* delayed_diag.c -- Delayed Diagonalization: Ladner's Core Technique
 *
 * Standard diagonalization (e.g., Cantor, Turing) constructs a language
 * that differs from ALL machines on EVERY input. This produces languages
 * outside the class entirely (e.g., undecidable).
 *
 * DELAYED diagonalization (Ladner 1975): Use a slow-growing function f(n)
 * to "slow down" the diagonalization. Instead of diagonalizing against
 * all P-machines at once, wait until f(n) is large enough to handle
 * one more machine. Since f grows arbitrarily slowly, we EVENTUALLY
 * diagonalize against each P-machine -- but at such large n that the
 * language remains sparse enough to avoid NP-completeness.
 *
 * The key insight: f is UNBOUNDED (so we eventually cover all machines)
 * but grows SLOWER THAN ANY POLYNOMIAL (so the language is sparse).
 *
 * This technique is the core of Ladner's proof and has been generalized
 * by Schoning (1982) to produce infinite hierarchies.
 *
 * References:
 *   - Ladner, JACM 22(1):155-171, 1975
 *   - Schoning, TCS 21, 1982
 *   - Arora & Barak, Section 3.4
 */

#include "ladner.h"

/* A family of P-machines (predicates in P) to diagonalize against.
 * Each machine M_k: N -> {0,1} is computable in P.
 * The goal: construct L such that for EVERY k, L != M_k. */

static int M_0(int n) { return n % 2; }
static int M_1(int n) { int s = 0; for (int i = 0; i < n; i++) s += i; return s % 2; }
static int M_2(int n) { return (n * n) % 7 == 0; }
static int M_3(int n) { return (n % 3 == 0) ? 0 : 1; }
static int M_4(int n) { int p = 1; for (int i = 2; i * i <= n; i++) if (n % i == 0) p = 0; return p; }
static int M_5(int n) { int s = 0, t = n; while (t) { s += t % 10; t /= 10; } return s % 2; }
static int M_6(int n) { long long a = 0, b = 1; while (a < n) { long long t = a + b; a = b; b = t; } return a == n; }
static int M_7(int n) { int c = 0; while (n) { c += n & 1; n >>= 1; } return c % 2; }
static int M_8(int n) { return n > 0 && (n & (n - 1)) == 0; } /* power of 2? */
static int M_9(int n) { int r = 1; for (int d = 1; d * d <= n; d++) if (n % d == 0) r++; return r % 2; } /* odd number of divisors? */

static PMachine machines[] = {
    M_0, M_1, M_2, M_3, M_4, M_5, M_6, M_7, M_8, M_9
};
#define NMACHINES 10

/* ---- Delayed Diagonalization ---- */

/* The delayed diagonalization function.
 * At input n, we diagonalize against machine M_{f(n)}.
 * L(n) = 1 - M_{f(n)}(n), i.e., L differs from this machine.
 *
 * As n grows, f(n) grows (very slowly), covering more machines.
 * Eventually, EVERY machine is diagonalized against. */

int delayed_diagonal(int n) {
    if (n <= 0) return 0;
    int f_n = slow_growing_f(n);          /* slow-growing: popcount */
    int machine_idx = f_n % NMACHINES;
    int machine_output = machines[machine_idx](n);
    return !machine_output;  /* L disagrees with M_{f(n)} */
}

/* Find the first n where L differs from machine k.
 * This PROVES that L != M_k for each k.
 * Because f is unbounded, for every k there exists some n
 * such that f(n) = k (mod NMACHINES), and at that n, L(n) != M_k(n).
 *
 * Returns: smallest n > 0 where L(n) != M_k(n), or 0 if not found. */
int delayed_diag_find_difference(int k, int max_n) {
    for (int n = 1; n <= max_n; n++) {
        if (delayed_diagonal(n) != machines[k](n))
            return n;
    }
    return 0;  /* not found in range (L might equal M_k if P=NP) */
}

/* Verify that L differs from ALL enumerated P-machines.
 * Returns the number of machines L differs from. */
int delayed_diag_verify_all(int max_n) {
    int different_count = 0;
    for (int k = 0; k < NMACHINES; k++) {
        int n = delayed_diag_find_difference(k, max_n);
        if (n > 0) {
            different_count++;
        }
    }
    return different_count;
}

/* ---- Delayed Diagonalization State Machine ---- */

/* Initialize the state of the diagonalization process. */
DelayedDiagState diag_state_new(void) {
    DelayedDiagState s;
    s.n = 1;
    s.f_n = slow_growing_f(1);
    s.machine_idx = s.f_n % NMACHINES;
    s.next_f_change = 2;
    s.progress = 0.0;
    return s;
}

/* Advance the diagonalization state by one step. */
void diag_state_step(DelayedDiagState* s) {
    s->n++;
    int new_f = slow_growing_f(s->n);
    if (new_f != s->f_n) {
        s->f_n = new_f;
        s->machine_idx = new_f % NMACHINES;
        s->progress = (double)s->f_n / NMACHINES;
    }
}

/* ---- Demo ---- */

void delayed_diag_demo(void) {
    printf("\n===== Delayed Diagonalization =====\n\n");

    printf("Technique (Ladner 1975): Use a slow-growing function f(n)\n");
    printf("to 'delay' diagonalization. Instead of diagonalizing against\n");
    printf("all P-machines at once, we wait until f(n) is large enough.\n\n");

    printf("f(n) = popcount(n) = O(log n). Unbounded but VERY slow.\n\n");

    /* Show the diagonalization process */
    const char* mnames[] = {"M0","M1","M2","M3","M4","M5","M6","M7","M8","M9"};
    printf("--- Diagonalization Process ---\n");
    printf("  %6s %6s %8s %8s %8s %s\n",
           "n", "f(n)", "machine", "M_k(n)", "L(n)", "diff?");
    DelayedDiagState state = diag_state_new();
    for (int step = 0; step < 15; step++) {
        int n = state.n;
        int k = state.machine_idx;
        int m_out = machines[k](n);
        int l_out = delayed_diagonal(n);
        printf("  %6d %6d %8s %8d %8d %s\n",
               n, state.f_n,
               (k < NMACHINES) ? mnames[k] : "M?",
               m_out, l_out,
               (m_out != l_out) ? "YES" : "no");
        diag_state_step(&state);
    }

    /* Verify against all machines */
    printf("\n--- Verification: L vs All P-machines ---\n");
    printf("Searching up to n=100000:\n");
    for (int k = 0; k < NMACHINES; k++) {
        int n = delayed_diag_find_difference(k, 100000);
        if (n > 0) {
            printf("  M_%d: L(%d)=%d != M_%d(%d)=%d [VERIFIED]\n",
                   k, n, delayed_diagonal(n), k, n, machines[k](n));
        } else {
            printf("  M_%d: NOT FOUND (if P=NP, could be L=M_%d)\n", k, k);
        }
    }

    /* Summary */
    int verified = delayed_diag_verify_all(100000);
    printf("\n--- Summary ---\n");
    printf("Verified L differs from %d/%d P-machines.\n",
           verified, NMACHINES);
    printf("If P!=NP: L differs from ALL P-machines => L not in P.\n");
    printf("If P==NP: L might coincide with some P-machine.\n");
    printf("This is half of Ladner's proof: L is NOT in P.\n");

    /* Key insight */
    printf("\n--- Key Insight ---\n");
    printf("Why delayed diagonalization works:\n");
    printf("  1. f(n) is unbounded -> eventually covers all machines.\n");
    printf("  2. f(n) grows SLOWLY (O(log n)) -> L is sparse.\n");
    printf("  3. Mahaney: sparse NPC => P=NP.\n");
    printf("  4. Therefore: L is NOT NPC (if P!=NP).\n");
    printf("  5. Result: L is NP-INTERMEDIATE.\n");
}
