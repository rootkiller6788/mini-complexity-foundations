/* space_classes.c — Space Complexity Classes
 *
 * The space complexity hierarchy:
 *   L ⊆ NL ⊆ P ⊆ NP ⊆ PSPACE = NPSPACE ⊆ EXP
 *
 * Key theorems:
 * - Savitch (1970): NSPACE(s) ⊆ SPACE(s²) → PSPACE = NPSPACE
 * - Immerman-Szelepcsenyi (1987): NL = coNL
 * - Space hierarchy: SPACE(f) ⊊ SPACE(g) when f = o(g)
 * - Reingold (2008): Undirected connectivity ∈ L (SL = L) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Simulate logspace computation: compute parity of input using O(log n) space */
static int logspace_parity(const int* input, int n) {
    /* Count ones modulo 2. Only need counter = O(log n) bits. */
    int counter = 0;  /* uses O(log n) space */
    for (int i = 0; i < n; i++) {
        counter ^= input[i];  /* XOR = addition mod 2 */
    }
    return counter;
}

/* Simulate polynomial-space computation: count in binary up to 2^n */
static int polyspace_count(int n) {
    /* Need array of size n (polynomial space) */
    int* binary = calloc((size_t)n, sizeof(int));
    if (!binary) return -1;

    long long steps = 0;
    /* Count from 0 to 2^n - 1 using the binary array */
    for (long long i = 1; i < (1LL << n) && steps < 1000000; i++) {
        /* Increment binary counter */
        int j = 0;
        while (j < n && binary[j] == 1) {
            binary[j] = 0;
            j++;
        }
        if (j < n) binary[j] = 1;
        steps++;
    }
    free(binary);
    printf("  Counted to %lld in %d-bit binary (polynomial space)\\n", steps, n);
    return 1;
}

void space_classes_demo(void) {
    printf("\n===== Space Complexity Classes =====\n\n");

    printf("The Space Hierarchy:\\n");
    printf("  L      = SPACE(log n)     — Deterministic Logspace\\n");
    printf("  NL     = NSPACE(log n)    — Nondeterministic Logspace\\n");
    printf("  P      = TIME(poly)       — Polynomial Time\\n");
    printf("  NP     = NTIME(poly)      — Nondeterministic Poly Time\\n");
    printf("  PSPACE = SPACE(poly)      — Polynomial Space\\n");
    printf("  NPSPACE= NSPACE(poly)     — Nondeterministic Poly Space\\n");
    printf("  EXP    = TIME(2^poly)     — Exponential Time\\n\\n");

    printf("Known relationships:\\n");
    printf("  L ⊆ NL ⊆ P ⊆ NP ⊆ PSPACE = NPSPACE ⊆ EXP\\n\\n");

    printf("Proven separations:\\n");
    printf("  L ⊊ PSPACE  (space hierarchy)\\n");
    printf("  P ⊊ EXP     (time hierarchy)\\n");
    printf("  PSPACE ⊊ EXPSPACE (space hierarchy extends)\\n\\n");

    printf("Open problems:\\n");
    printf("  L vs NL         — unknown\\n");
    printf("  NL vs P         — unknown\\n");
    printf("  P vs PSPACE     — unknown (and P vs NP is open!)\\n");
    printf("  PSPACE vs EXP   — unknown\\n\\n");

    /* Demo logspace */
    printf("--- Logspace Demo: Parity ---\\n");
    int input[] = {1, 0, 1, 1, 0, 1, 0, 0, 1, 1};
    int par = logspace_parity(input, 10);
    printf("  Parity of 10 bits = %d (space used: O(log n))\\n\\n", par);

    /* Demo polyspace */
    printf("--- Polynomial-Space Demo ---\\n");
    polyspace_count(8);

    printf("\nKey insight: Space is more powerful than time:\\n");
    printf("  PSPACE contains the ENTIRE polynomial hierarchy (PH).\\n");
    printf("  PSPACE = NPSPACE (Savitch) is a proven collapse,\\n");
    printf("  unlike NP vs coNP which is open.\\n");
}