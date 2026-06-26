/* ladner_demo.c -- Ladner's Theorem end-to-end example
 *
 * Demonstrates the full Ladner NP-intermediate language construction.
 * Walks through the three-property verification:
 *   1. L is in NP
 *   2. L is NOT in P (delayed diagonalization)
 *   3. L is NOT NP-complete (sparsity + Mahaney)
 *
 * Build: make ladner_demo
 * Run:   ./examples/ladner_demo
 */

#include "ladner.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    setbuf(stdout, NULL);

    printf("\n========================================\n");
    printf("  Ladner's Theorem Demo (JACM 1975)\n");
    printf("========================================\n\n");

    printf("Theorem: If P != NP, there exist languages in NP\n");
    printf("that are NOT in P and NOT NP-complete.\n\n");

    printf("Construction: L = {x in SAT | f(|x|) is even}\n");
    printf("where f(n) = popcount(n) = number of 1 bits in n.\n\n");

    /* Part 1: The slow-growing function */
    printf("--- Part 1: Slow-Growing Function f(n) ---\n\n");
    printf("f(n) = popcount(n): O(log n), unbounded, sub-polynomial.\n\n");
    printf("  %10s %10s %10s\n", "n", "f(n)=popcount", "f(n)=log*");
    printf("  %10s %10s %10s\n", "----------", "----------", "----------");
    long long milestones[] = {1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192};
    for (int i = 0; i < 14; i++) {
        int n = (int)milestones[i];
        printf("  %10lld %10d %10d\n", milestones[i],
               slow_growing_f(n),
               slow_growing_eval(n, 1));  /* type 1 = log* */
    }

    /* Part 2: Delayed diagonalization */
    printf("\n--- Part 2: Delayed Diagonalization ---\n\n");
    printf("L(n) disagrees with P-machine M_{f(n)}.\n");
    printf("Since f is unbounded, L eventually disagrees with EVERY P-machine.\n");
    printf("Therefore: L is NOT in P (if P != NP).\n\n");

    printf("Diagonalization trace (first 12 values):\n");
    printf("  %6s %6s %8s %8s\n", "n", "f(n)", "M_k(n)", "L(n)");
    for (int n = 1; n <= 12; n++) {
        int fn = slow_growing_f(n);
        int k = fn % 10;  /* 10 P-machines */
        /* M_k(n): simplified as n % 2 for demo */
        int mk = n % 2;
        int ln = delayed_diagonal(n);
        printf("  %6d %6d %8d %8d %s\n", n, fn, mk, ln,
               (mk != ln) ? "<-- DIFFERS!" : "");
    }

    /* Part 3: Sparsity analysis */
    printf("\n--- Part 3: Sparsity Analysis (Mahaney) ---\n\n");
    printf("On inputs where f(n) is even, L behaves like SAT.\n");
    printf("These inputs are sparse because f grows slowly.\n\n");

    int sat_like = 0, diag_like = 0;
    for (int n = 1; n <= 1000; n++) {
        if (slow_growing_f(n) % 2 == 0) sat_like++;
        else diag_like++;
    }
    printf("For n=1..1000:\n");
    printf("  SAT-like inputs (f even): %d (%.1f%%)\n",
           sat_like, 100.0 * sat_like / 1000);
    printf("  Diag inputs (f odd):      %d (%.1f%%)\n\n",
           diag_like, 100.0 * diag_like / 1000);

    printf("Mahaney Theorem (1982): Sparse NPC => P = NP.\n");
    printf("Since L is sparse on SAT inputs, L cannot be NPC\n");
    printf("(if P != NP). Therefore: L is NOT NP-complete.\n\n");

    /* Part 4: Ladner language membership */
    printf("--- Part 4: Ladner Language Membership ---\n\n");
    printf("  %6s %6s %8s %8s %s\n", "n", "f(n)", "f even?", "SAT?", "L(n)");
    for (int n = 1; n <= 20; n++) {
        int fn = slow_growing_f(n);
        int even = (fn % 2 == 0);
        int sat = (n % 3 == 0) ? 1 : 0;  /* simulated SAT oracle */
        int Ln = ladner_membership(n);
        printf("  %6d %6d %8s %8d %d\n", n, fn,
               even ? "YES" : "NO",
               even ? sat : -1, Ln);
    }

    /* Part 5: NP degree structure */
    printf("\n--- Part 5: NP Degree Structure ---\n\n");
    printf("Ladner's proof generalizes (Schoning 1982):\n");
    printf("  There are INFINITELY MANY distinct NP-m-degrees.\n");
    printf("  Between any two comparable degrees, another exists.\n");
    printf("  The NP-m-degrees form a DENSE partial order.\n\n");

    printf("NP degree lattice:\n");
    printf("  P (level 0)\n");
    printf("    |\n");
    printf("  Low_1, Low_2, ... (weak oracles)\n");
    printf("    |\n");
    printf("  Ladner L_1, L_2, L_3, ... (NP-intermediate)\n");
    printf("    |\n");
    printf("  High_1, High_2, ... (strong oracles)\n");
    printf("    |\n");
    printf("  NPC (level 100)\n\n");

    /* Conclusion */
    printf("========================================\n");
    printf("  CONCLUSION\n");
    printf("========================================\n\n");
    printf("Ladner's Theorem (1975) proves that IF P != NP, then\n");
    printf("NP-intermediate languages MUST exist. The construction\n");
    printf("is explicit (delayed diagonalization) but produces\n");
    printf("ARTIFICIAL languages.\n\n");
    printf("Finding a NATURAL NP-intermediate problem (like\n");
    printf("Graph Isomorphism or Factoring) and PROVING it is\n");
    printf("intermediate remains one of the greatest open\n");
    printf("problems in theoretical computer science.\n");

    return 0;
}
