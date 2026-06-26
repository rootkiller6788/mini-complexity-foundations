/* nondeterministic_hierarchy.c -- NTIME Hierarchy
 * Cook (1973): NTIME(f) != NTIME(g) when f(n+1) = o(g(n))
 * Zak (1983): improved with simpler translational diagonalization.
 * Corollary: NP != NEXP (unconditional!). */

#include <stdio.h>
#include <stdlib.h>
#include "tht.h"

/* Simulate a nondeterministic TM with time bound.
   Enumerate all nondeterministic choice sequences. */
static int ntime_simulate(int tm_id, int input, int time_bound) {
    int max_bits = time_bound < 20 ? time_bound : 20;
    long long max_paths = 1LL << max_bits;

    for (long long path = 0; path < max_paths; path++) {
        int state = 0, result = -1;
        for (int step = 0; step < time_bound; step++) {
            int choice = (path >> (step % max_bits)) & 1;
            int t = (tm_id * 31337 + state * 1009 + step * 271 + choice * 8191) % 16;
            if (t == 0) { result = 1; break; }
            if (t == 1) { result = 0; break; }
            state = (state + t + choice) % 8;
        }
        if (result == 1) return 1;
    }
    return 0;
}

void nondet_hierarchy_demo(void) {
    printf("\n===== NTIME Hierarchy Theorem =====\n\n");
    printf("Cook (1973): NTIME(f) != NTIME(g) when f(n+1) = o(g(n)).\n");
    printf("Zak (1983): improved bound, simpler proof.\n\n");

    printf("Technique: Translational diagonalization.\n");
    printf("  1. Assume NTIME(f) = NTIME(g).\n");
    printf("  2. Use padding argument to translate separation.\n");
    printf("  3. This would collapse the hierarchy -> contradiction.\n\n");

    printf("--- Corollary: NP != NEXP ---\n");
    printf("NP = Union_k NTIME(n^k). NEXP = Union_k NTIME(2^{n^k}).\n");
    printf("NTIME(n^k) != NTIME(2^n) for any k -> NP != NEXP.\n");
    printf("This is an UNCONDITIONAL separation!\n\n");

    printf("--- NTM Simulation Demo ---\n");
    for (int tm_id = 0; tm_id < 5; tm_id++) {
        for (int input = 0; input < 4; input++) {
            int r = ntime_simulate(tm_id, input, 8);
            printf("  NTM_%d(%d, time=8) = %s\n",
                   tm_id, input, r ? "ACCEPT" : "REJECT/TIMEOUT");
        }
    }

    printf("\nThe nondeterministic hierarchy is strict and provable.\n");
    printf("But NP vs coNP and P vs NP remain open.\n");
}
