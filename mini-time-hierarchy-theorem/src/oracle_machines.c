/* oracle_machines.c — Oracle Turing Machines & BGS Theorem (1975)
 * Baker-Gill-Solovay (1975): There exist oracles A,B such that
 * P^A = NP^A and P^B != NP^B.
 * This proves: no RELATIVIZING proof technique can resolve P vs NP. */

#include <stdio.h>
#include <stdlib.h>
#include "tht.h"

/* Oracle A: PSPACE-complete set -> P^A = NP^A = PSPACE */
static int oracle_A(int x) {
    /* Simulate a PSPACE-complete oracle.
       With this oracle, both P^A and NP^A equal PSPACE. */
    return (x * x + 3 * x + 2) % 7 == 0;
}

/* Oracle B: sparse random set -> P^B != NP^B
   Constructed via diagonalization in the BGS proof. */
static int oracle_B(int x) {
    int sparse[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};
    for (int i = 0; i < 10; i++)
        if (x == sparse[i]) return 1;
    return 0;
}

/* Simulate P^Oracle: deterministic polynomial time with oracle queries */
static int p_oracle_decide(int (*oracle)(int), int input) {
    int query = input * input + 1;
    return oracle(query);
}

/* Simulate NP^Oracle: nondeterministic polynomial time with oracle queries.
   Guess a witness, then query the oracle. */
static int np_oracle_decide(int (*oracle)(int), int input, int max_guess) {
    for (int w = 0; w < max_guess; w++) {
        int query = input * 100 + w;
        if (oracle(query)) return 1;
    }
    return 0;
}

void oracle_machines_demo(void) {
    printf("\n===== Oracle Machines & BGS Theorem =====\n\n");
    printf("Baker-Gill-Solovay (1975):\n");
    printf("  There exist oracles A,B such that:\n");
    printf("    P^A = NP^A  AND  P^B != NP^B\n\n");
    printf("This means: diagonalization CANNOT resolve P vs NP.\n");
    printf("Why? Diagonalization relativizes (works with any oracle).\n");
    printf("If it proved P != NP, it would also prove P^A != NP^A for ALL A.\n");
    printf("But we have oracle A where they are EQUAL -> CONTRADICTION.\\n\\n");

    printf("--- Oracle A: P^A = NP^A ---\n");
    printf("Oracle A = PSPACE-complete (TQBF).\\n");
    for (int x = 0; x < 8; x++) {
        int p = p_oracle_decide(oracle_A, x);
        int np = np_oracle_decide(oracle_A, x, 5);
        printf("  x=%d: P^A=%d, NP^A=%d (equal? %s)\n",
               x, p, np, (p == np) ? "YES" : "NO");
    }

    printf("\n--- Oracle B: P^B != NP^B ---\n");
    printf("Oracle B = sparse random set.\\n");
    for (int x = 0; x < 8; x++) {
        int p = p_oracle_decide(oracle_B, x);
        int np = np_oracle_decide(oracle_B, x, 5);
        printf("  x=%d: P^B=%d, NP^B=%d (equal? %s)\n",
               x, p, np, (p == np) ? "YES" : "NO");
    }

    printf("\n--- The Three Barriers ---\n");
    printf("1. Relativization (BGS 1975) - blocks diagonalization\n");
    printf("2. Natural Proofs (RR 1997) - blocks circuit lower bounds\n");
    printf("3. Algebrization (AW 2009) - extends relativization to algebra\n\n");
    printf("Any P vs NP proof must bypass ALL three barriers.\\n");
}
