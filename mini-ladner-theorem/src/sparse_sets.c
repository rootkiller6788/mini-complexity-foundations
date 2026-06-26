/* sparse_sets.c -- Sparse Sets, Mahaney Theorem, and the Ladner Connection
 *
 * A set S is "sparse" if there exists a polynomial p such that
 * for all n, |S cap {0,1}^n| <= p(n).
 *
 * Mahaney (JCSS 1982): If a sparse set is NP-complete, then P = NP.
 * Corollary: If P != NP, no sparse set can be NP-complete.
 *
 * This is a key ingredient in Ladner's proof:
 * Ladner's intermediate language L is sparse in a controlled sense,
 * therefore (if P != NP) L cannot be NP-complete.
 *
 * Reference: Mahaney, "Sparse Complete Sets for NP: Solution of a
 * Conjecture of Berman and Hartmanis", JCSS 25(2):130-143, 1982.
 */
#include "ladner.h"

/* ---- SparseSet lifecycle ---- */

SparseSet* sparse_new(int max_n) {
    SparseSet* s = malloc(sizeof(SparseSet));
    s->data  = calloc((size_t)(max_n + 1), sizeof(int*));
    s->sizes = calloc((size_t)(max_n + 1), sizeof(int));
    s->max_n = max_n;
    s->poly_deg = 2;  /* default polynomial bound: n^2 */
    return s;
}

int sparse_add(SparseSet* s, int n, int val) {
    if (n > s->max_n || n <= 0) return 0;
    /* Enforce sparsity bound: at most n^{poly_deg} elements at length n */
    int bound = 1;
    for (int i = 0; i < s->poly_deg; i++) {
        if (bound > 1000000) { bound = 1000000; break; }
        bound *= n;
    }
    if (s->sizes[n] >= bound) return 0;  /* would violate sparsity */
    s->data[n] = realloc(s->data[n], (size_t)(s->sizes[n] + 1) * sizeof(int));
    s->data[n][s->sizes[n]++] = val;
    return 1;
}

int sparse_contains(SparseSet* s, int n, int val) {
    if (n > s->max_n || n <= 0) return 0;
    for (int i = 0; i < s->sizes[n]; i++)
        if (s->data[n][i] == val) return 1;
    return 0;
}

void sparse_free(SparseSet* s) {
    if (!s) return;
    for (int i = 0; i <= s->max_n; i++) free(s->data[i]);
    free(s->data);
    free(s->sizes);
    free(s);
}

/* Check if S respects its polynomial sparsity bound */
int sparse_is_bound(SparseSet* s, int poly_deg) {
    for (int n = 1; n <= s->max_n; n++) {
        int bound = 1;
        for (int i = 0; i < poly_deg; i++) {
            if (bound > 1000000) break;
            bound *= n;
        }
        if (s->sizes[n] > bound) return 0;
    }
    return 1;
}

/* Count total elements across all lengths up to max_n */
int sparse_total_count(SparseSet* s) {
    int total = 0;
    for (int n = 1; n <= s->max_n; n++)
        total += s->sizes[n];
    return total;
}

/* Verify the sparsity condition empirically for a range */
void sparse_verify_density(SparseSet* s, int max_check) {
    printf("Sparsity density check up to n=%d:\n", max_check);
    printf("%6s %8s %10s %s\n", "n", "count(n)", "bound(n^2)", "sparse?");
    for (int n = 1; n <= max_check && n <= s->max_n; n++) {
        int bound = n * n;
        int cnt   = s->sizes[n];
        int ok    = (cnt <= bound);
        printf("%6d %8d %10d %s\n", n, cnt, bound, ok ? "YES" : "NO");
    }
}

/* ---- Mahaney Pruning Algorithm ---- */

/* Simulate the effect of Mahaney pruning on SAT search tree.
 * Given n variables, exponential 2^n branches are pruned down
 * to poly(n) by the sparse-NPC oracle.
 *
 * This computes the *pruned* search space size, demonstrating
 * that if a sparse set is NPC, SAT is in P (hence P=NP). */
long long mahaney_prune(int n_vars, int sparsity_deg) {
    long long orig = 1LL << n_vars;     /* original search space */
    long long pruned = orig;

    for (int d = 0; d < n_vars; d++) {
        /* At each depth, at most O(n^{sparsity_deg}) branches survive */
        long long max_d = 1;
        for (int i = 0; i < sparsity_deg; i++) {
            if (max_d > orig) break;
            max_d *= n_vars;
        }
        if (pruned > max_d) pruned = max_d;
    }
    /* pruned is now polynomial in n_vars */
    return pruned;
}

/* Note: mahaney_sparse_check() is defined in mahaney.c */

/* ---- Demo ---- */

void sparse_sets_demo(void) {
    printf("\n===== Sparse Sets & Mahaney Theorem =====\n\n");

    printf("Definition: S is SPARSE if |S_n| <= poly(n) for all n.\n");
    printf("Mahaney (1982): If a sparse set is NP-complete, then P = NP.\n\n");

    /* Build a sparse set for demonstration */
    SparseSet* S = sparse_new(15);
    printf("Building sparse set S (bound: n^2 per length):\n");
    for (int n = 1; n <= 10; n++) {
        int added = 0;
        /* Add elements at length n: only n^2/2 of the n^2 bound */
        for (int j = 0; j < n * n / 2; j++) {
            if (sparse_add(S, n, n * 100 + j)) added++;
        }
        printf("  n=%2d: added %3d elements (max=%3d, ratio=%.2f)\n",
               n, added, n * n, (double)added / (n * n));
    }
    printf("\nTotal elements in S: %d\n", sparse_total_count(S));

    /* Demonstrate membership */
    printf("\nMembership queries:\n");
    printf("  S contains (n=3, val=305)? %s\n",
           sparse_contains(S, 3, 305) ? "YES" : "NO");
    printf("  S contains (n=5, val=999)? %s\n",
           sparse_contains(S, 5, 999) ? "YES" : "NO");

    /* Mahaney pruning simulation */
    printf("\n--- Mahaney Pruning Simulation ---\n");
    printf("If S is NPC, SAT search tree is pruned from 2^n to poly(n).\n");
    printf("  %-8s %-16s %-16s %s\n", "n_vars", "2^n", "pruned", "speedup");
    for (int v = 3; v <= 20; v++) {
        long long orig = 1LL << v;
        long long pruned = mahaney_prune(v, 2);
        long long ratio = pruned > 0 ? orig / pruned : -1;
        printf("  %-8d %-16lld %-16lld %lldx\n", v, orig, pruned, ratio);
    }

    /* The Ladner connection */
    printf("\n--- Why Ladner Needs Mahaney ---\n");
    printf("1. Ladner's L is sparse-like (by slow-growing f).\n");
    printf("2. If L were NPC, Mahaney => P=NP (contradiction).\n");
    printf("3. Therefore L is NOT NP-complete (if P!=NP).\n");
    printf("4. Plus: L differs from all P-machines => L is NOT in P.\n");
    printf("5. Conclusion: L is NP-INTERMEDIATE. QED.\n\n");

    printf("--- Broader Implications ---\n");
    printf("* Mahaney's result was a breakthrough: settled a conjecture\n");
    printf("  of Berman and Hartmanis (1977).\n");
    printf("* It shows sparse sets are 'too small' to encode NP-complete\n");
    printf("  information, unless P=NP collapses the hierarchy.\n");
    printf("* The technique (pruning with sparse oracles) foreshadows\n");
    printf("  later work on instance complexity and Kolmogorov complexity.\n\n");

    /* Cleanup */
    printf("Is S sparse? %s\n",
           sparse_is_bound(S, 2) ? "YES" : "NO");

    sparse_free(S);
}
