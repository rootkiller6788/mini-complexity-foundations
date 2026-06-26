/* rl_space.c — RLとBPL: 確率的対数領域クラス, Nisanの擬似乱数生成器
 * 参考: Nisan 1992, Saks-Zhou 1999, Reingold 2008 (SL=L)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/spaceh.h"

/* Randomized Logspace: RL = problems decidable by probabilistic TM using O(log n) space
 * Key result: RL ⊆ NL ⊆ L² (Savitch) → RL ⊆ DSPACE(log² n)
 * Reingold 2008: Undirected ST-Connectivity ∈ L (deterministic logspace!)
 */

/* Nisan's PRG for space-bounded computation */
typedef struct {
    int seed_length;     /* O(log² n) bits */
    int output_length;   /* poly(n) bits */
    unsigned int* state;
} nisan_prg;

nisan_prg* nisan_prg_init(int space_bound, int input_length) {
    nisan_prg* prg = malloc(sizeof(nisan_prg));
    prg->seed_length = (int)(log2(space_bound) * log2(input_length));
    prg->output_length = (int)pow(input_length, 2.0);
    prg->state = calloc(4, sizeof(unsigned int));
    prg->state[0] = 0x67452301; /* initial hash values */
    prg->state[1] = 0xEFCDAB89;
    return prg;
}

/* Generate next pseudorandom bit */
int nisan_prg_next_bit(nisan_prg* prg) {
    /* Simplified pairwise-independent hash */
    unsigned int h = prg->state[0];
    h ^= (h << 13); h ^= (h >> 17); h ^= (h << 5);
    prg->state[0] = h;
    return h & 1;
}

/* Estimate probability of acceptance with confidence */
double rl_estimate_acceptance(int (*verifier)(const char*, const char*),
                               const char* input, int input_len,
                               int n_samples, nisan_prg* prg) {
    int accepts = 0;
    char* witness = calloc(input_len + 1, 1);
    
    for (int i = 0; i < n_samples; i++) {
        /* Generate pseudorandom witness bits */
        for (int j = 0; j < input_len; j++) {
            witness[j] = nisan_prg_next_bit(prg) ? '1' : '0';
        }
        if (verifier(input, witness)) accepts++;
    }
    
    free(witness);
    return (double)accepts / n_samples;
}

/* RL complete problem: s-t connectivity in directed graphs with out-degree ≤ 1
 * (Random walk works: O(n³) expected time, O(log n) space) */
int rl_degree1_connectivity(int n, int* edges, int s, int t, int max_steps) {
    int current = s;
    int steps = 0;
    
    while (current != t && steps < max_steps) {
        current = edges[current]; /* deterministic: out-degree = 1 */
        steps++;
        if (current == -1) break; /* dead end */
    }
    return (current == t) ? 1 : 0;
}

/* Saks-Zhou theorem (1999): BPL ⊆ L^(3/2) — randomized logspace in deterministic L^1.5 
 * Simplified: BPL ⊆ DSPACE(log^(3/2) n) */
double saks_zhou_space_bound(int input_length) {
    return pow(log2(input_length), 1.5);
}

/* BPL = class of problems solvable by bounded-error probabilistic logspace TM
 * Open problem: BPL = L ? (widely conjectured true)
 * Evidence: Nisan's PRG gives BPL ⊆ L², improved to L^(3/2) by Saks-Zhou */
void bpl_open_problem_summary() {
    printf("=== BPL: Bounded-Error Probabilistic Logspace ===\n\n");
    printf("Known containments:\n");
    printf("  L ⊆ RL ⊆ BPL ⊆ NL ⊆ L² (Savitch)\n");
    printf("  BPL ⊆ L^(3/2) (Saks-Zhou 1999)\n");
    printf("  BPL ⊆ DSPACE(log^(3/2) n)\n\n");
    printf("Open questions:\n");
    printf("  BPL = L ? (widely conjectured)\n");
    printf("  BPL = RL ?\n");
    printf("  RL = L ? (Reingold 2008: undirected ST-conn ∈ L)\n");
    printf("  Randomized vs deterministic space: same power?\n\n");
    printf("Reingold 2008 breakthrough: SL = L\n");
    printf("  (Symmetric Logspace = Deterministic Logspace)\n");
    printf("  Undirected graph connectivity ∈ L (deterministic!)\n");
}

void space_derandomization_summary(void) {
    printf("\\n===== Space Derandomization (L9) =====\\n\\n");
    printf("BPL vs L: Is bounded-error probabilistic logspace = deterministic logspace?\\n");
    printf("Widely conjectured: BPL = L.\\n");
    printf("Key results:\\n");
    printf("  Nisan (1992): BPL in DSPACE(log^2 n) via PRG\\n");
    printf("  Saks-Zhou (1999): BPL in DSPACE(log^(3/2) n)\\n");
    printf("  Reingold (2008): SL = L (breakthrough!)\\n");
    printf("Open: Can we derandomize all of BPL?\\n");
}

void randomized_logspace_demo(void) {
    printf("RL/BPL Space Complexity Demo\n");
    printf("=============================\n\n");

    /* Demo: degree-1 connectivity */
    int n = 8;
    int edges[] = {1, 2, 3, 4, 5, 6, 7, -1}; /* path 0->1->...->7 */
    int result = rl_degree1_connectivity(n, edges, 0, 7, 100);
    printf("RL degree-1 connectivity (0->7): %s\n",
           result ? "REACHABLE" : "UNREACHABLE");

    nisan_prg* prg = nisan_prg_init(100, 50);
    printf("Nisan PRG: seed=%d bits, output=%d bits\n",
           prg->seed_length, prg->output_length);
    free(prg->state);
    free(prg);

    printf("\nSaks-Zhou bound for n=2^20: O(log^1.5 n) = %.1f\n",
           saks_zhou_space_bound(1<<20));

    bpl_open_problem_summary();
}
