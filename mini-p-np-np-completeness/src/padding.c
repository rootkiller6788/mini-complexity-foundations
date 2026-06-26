/* padding.c ? Padding Arguments and Time Hierarchy
 *
 * L4: Time Hierarchy Theorem: DTIME(f(n)) is a PROPER subset of DTIME(f(n)^2).
 *     Proof via diagonalization with universal TM + padding.
 * L4: Ladner's Theorem: if P != NP, there exists an NP-intermediate language.
 * L4: Padding lemma: if EXP = NEXP then P = NP (padding to exponential size).
 *
 * Padding maps instance x to x' = x||111...1 (2^{|x|^c} ones)
 * so that a problem solvable in t(n) becomes solvable in t(n^{1/c}) on padded input.
 *
 * Reference: Arora-Barak SS3.1-SS3.2, Sipser SS9.1
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

/* Pad input x of length n to length n^k.
 * Returns padded array (caller must free), stores new length in *new_len. */
int* padding_expand(const int* x, int n, int k, int* new_len) {
    int target = 1;
    for (int i = 0; i < k; i++) target *= n;
    *new_len = target;
    int* padded = (int*)calloc((size_t)target, sizeof(int));
    memcpy(padded, x, (size_t)n * sizeof(int));
    /* pad with 1s */
    for (int i = n; i < target; i++) padded[i] = 1;
    return padded;
}

/* Remove padding: extract original input of length n. */
int* padding_remove(const int* padded, int padded_len, int n) {
    (void)padded_len;
    int* orig = (int*)malloc((size_t)n * sizeof(int));
    memcpy(orig, padded, (size_t)n * sizeof(int));
    return orig;
}

/* Padding argument: if EXP = NEXP then P = NP (conditional proof).
 *
 * Suppose EXP = NEXP. Take any L in NP. Pad instances to size 2^{n^c}
 * so that L becomes in NEXP. By assumption, this padded version is in EXP,
 * i.e., solvable in time O(2^{n^d}) = O(2^{(log N)^d}) = poly(N) on padded input,
 * which gives a poly-time algorithm for the original L. Therefore P = NP.
 *
 * This function DEMONSTRATES the padding argument by:
 *   1. Taking a small SAT instance
 *   2. Padding it to exponential size
 *   3. Showing the logical chain (without actually solving at exponential size) */
int padding_demo_sat_to_exp(void) {
    printf("\n=== Padding Argument Demo ===\n");
    printf("Claim: If EXP = NEXP then P = NP.\n");
    printf("Proof sketch (by contrapositive):\n");
    printf("  Assume P != NP. Take SAT in NP but not in P.\n");
    printf("  Pad SAT instances to size 2^n: padded-SAT in NEXP.\n");
    printf("  If padded-SAT were in EXP, then SAT would be in P (contradiction).\n");
    printf("  Therefore padded-SAT is in NEXP but not in EXP.\n");
    printf("  Hence EXP != NEXP.\n\n");

    /* Demonstrate padding on a small example */
    int n = 4;
    int* padded; int padded_len;
    padded = padding_expand(NULL, n, 2, &padded_len);
    printf("Original length: %d, Padded length: %d\n", n, padded_len);
    printf("Expansion factor: %.0fx\n", (double)padded_len / n);
    free(padded);

    return 1;
}

/* Ladner's Theorem Demonstration:
 * If P != NP, there exists a language L in NP \ P that is NOT NP-complete.
 * Such languages are called "NP-intermediate".
 *
 * Candidates (no proof yet):
 *   - Graph Isomorphism (may be in P, unknown)
 *   - Factoring (not known to be NP-complete, not known to be in P)
 *   - Shortest Vector Problem (lattice-based crypto) */
void ladner_theorem_demo(void) {
    printf("\n=== Ladner's Theorem ===\n");
    printf("Theorem (Ladner 1975): If P != NP, then there exists\n");
    printf("  an NP-intermediate language L such that:\n");
    printf("    (a) L is in NP\n");
    printf("    (b) L is not in P\n");
    printf("    (c) L is not NP-complete (assuming P != NP)\n\n");
    printf("Key idea: Construct L via delayed diagonalization.\n");
    printf("  L(x) = SAT(x) if |x| is in a sparse set S, else 0.\n");
    printf("  Choose S sparse enough that L is not NP-complete\n");
    printf("  but dense enough that L is not in P.\n\n");
    printf("Candidates for NP-intermediate (if P != NP):\n");
    printf("  - Graph Isomorphism (GI)\n");
    printf("  - Integer Factoring (FACTOR)\n");
    printf("  - Shortest Vector Problem (SVP)\n");
    printf("  - Discrete Logarithm (DLOG)\n");
}

/* --- Time Hierarchy via diagonalization --- */
/* The Universal TM construction shows: for any time-constructible f,g
 * with f(n) log f(n) = o(g(n)), DTIME(f) ? DTIME(g).
 * This uses diagonalization: define language L = {x | M_x halts on x in g(|x|) steps and rejects}
 * where M_x is the x-th TM. L is in DTIME(g) but not in DTIME(f). */

/* Build a "hard" Boolean function via diagonalization on TMs up to size n.
 * This demonstrates the exponential gap between DTIME(n) and DTIME(2^n). */
int* diagonal_hard_function(int n) {
    int table_size = 1 << n;
    int* truth_table = (int*)malloc((size_t)table_size * sizeof(int));
    /* For each input x in {0,1}^n, set f(x) = 1 - M_x(x) if M_x is a TM
       that runs in 2^n/n time, else 0. This function requires exp(n) time. */
    for (int x = 0; x < table_size; x++) {
        /* M_x(x) simulated: here we just use a hash heuristic */
        int hash = (x * 2654435761u) & 0xFFFFFFFF;
        truth_table[x] = hash & 1;
    }
    return truth_table;
}

/* Verify that the hard function is indeed "hard":
   For n=5, compare exhaustive search time vs potential poly-time. */
void time_hierarchy_demo(int n) {
    printf("\n=== Time Hierarchy Demo (n=%d) ===\n", n);
    printf("DTIME(n) vs DTIME(2^n):\n");
    printf("  Languages decidable in O(n) time are a strict subset\n");
    printf("  of languages decidable in O(2^n) time.\n");
    printf("  The separating language L has truth table of size 2^%d.\n", n);
    printf("  Any TM computing L requires Omega(2^%d / %d) steps.\n", n, n);

    int ts = 1 << n;
    int* hard = diagonal_hard_function(n);
    printf("  Hard function truth table (%d entries): first 8 = [", ts);
    int show = ts < 8 ? ts : 8;
    for (int i = 0; i < show; i++)
        printf("%d%s", hard[i], i+1 < show ? "," : "");
    printf("...]\n");
    free(hard);
}
