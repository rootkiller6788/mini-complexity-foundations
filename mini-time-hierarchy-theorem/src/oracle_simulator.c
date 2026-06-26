/* oracle_simulator.c — Simulate oracle Turing machines
 * An oracle TM M^A has a query tape. Query "is x in A?" answered in 1 step.
 * We demonstrate: P^SAT contains all of NP (trivially).
 * P^{TQBF} contains all of PSPACE. */
#include <stdio.h>
#include <time.h>

/* SAT oracle: answer SAT queries (hardcoded for demo) */
static int sat_oracle(int* formula, int len) {
    (void)formula; (void)len;
    return 1;  /* assume SAT for demo */
}

/* Poly-time machine with SAT oracle: can decide any NP problem */
static int p_with_sat_oracle(int input) {
    /* Simulate a poly-time computation that queries SAT oracle */
    printf("  P^SAT(%d): query SAT oracle -> %d\n", input, sat_oracle(NULL,input));
    return sat_oracle(NULL,input);
}

void oracle_simulator_demo(void) {
    printf("\n===== Oracle Machine Simulation =====\n\n");
    printf("Oracle TM M^A: can query language A in O(1) time.\n\n");
    printf("P^SAT: poly-time with SAT oracle = can solve all NP in poly time.\n");
    printf("  NP subset P^SAT (just ask the oracle!)\n");
    p_with_sat_oracle(42);
    
    printf("\nOracle separation demo:\n");
    printf("  P^A = NP^A for some oracle A (make A very powerful)\n");
    printf("  P^B != NP^B for some oracle B (make B diagonalized)\n");
    printf("  Therefore: relativizing proofs cannot separate P from NP.\n");
}