/* sparse_operations.c -- Sparse Set Operations and Analysis
 *
 * Operations on sparse sets relevant to Ladner's construction:
 *   - Union and intersection of sparse sets
 *   - Self-reducibility preservation
 *   - Polynomial closure properties
 *   - Density analysis
 *
 * A key property: The class of sparse sets is closed under
 * union, intersection, and polynomial-time many-one reductions
 * to sufficiently sparse codomains.
 *
 * Connection to Ladner: The intermediate language L is built
 * by taking a sparse "slice" of SAT (using the slow-growing f).
 * Understanding sparse set operations clarifies why L avoids
 * both P and NPC status.
 */

#include "ladner.h"

/* ---- Basic Sparse Set Operations ---- */

/* Union: S1 cup S2. Returns new sparse set.
 * WARNING: Union of two sparse sets may not be sparse.
 * |(S1 cup S2)_n| <= |(S1)_n| + |(S2)_n| <= 2*poly(n) */
SparseSet* sparse_union(SparseSet* s1, SparseSet* s2) {
    int max_n = (s1->max_n > s2->max_n) ? s1->max_n : s2->max_n;
    SparseSet* result = sparse_new(max_n);
    result->poly_deg = (s1->poly_deg > s2->poly_deg) ? s1->poly_deg : s2->poly_deg;

    for (int n = 1; n <= s1->max_n; n++)
        for (int i = 0; i < s1->sizes[n]; i++)
            sparse_add(result, n, s1->data[n][i]);

    for (int n = 1; n <= s2->max_n; n++)
        for (int i = 0; i < s2->sizes[n]; i++)
            if (!sparse_contains(result, n, s2->data[n][i]))
                sparse_add(result, n, s2->data[n][i]);

    return result;
}

/* Intersection: S1 cap S2 */
SparseSet* sparse_intersect(SparseSet* s1, SparseSet* s2) {
    int max_n = (s1->max_n < s2->max_n) ? s1->max_n : s2->max_n;
    SparseSet* result = sparse_new(max_n);
    result->poly_deg = (s1->poly_deg < s2->poly_deg) ? s1->poly_deg : s2->poly_deg;

    for (int n = 1; n <= max_n; n++) {
        for (int i = 0; i < s1->sizes[n]; i++) {
            if (sparse_contains(s2, n, s1->data[n][i]))
                sparse_add(result, n, s1->data[n][i]);
        }
    }
    return result;
}

/* Check if a set respects polynomial density at a given length.
 * Returns: 1 if count <= n^{deg}, 0 otherwise. */
int sparse_density_at_n(SparseSet* s, int n, int deg) {
    if (n > s->max_n || n <= 0) return 1;
    double bound = pow((double)n, (double)deg);
    return (s->sizes[n] <= (int)(bound + 0.5)) ? 1 : 0;
}

/* Maximum density ratio: max_n count(n) / n^deg */
double sparse_max_density(SparseSet* s, int deg) {
    double max_ratio = 0.0;
    for (int n = 1; n <= s->max_n; n++) {
        double bound = pow((double)n, (double)deg);
        if (bound > 0) {
            double ratio = s->sizes[n] / bound;
            if (ratio > max_ratio) max_ratio = ratio;
        }
    }
    return max_ratio;
}

/* ---- Sparse Set Properties for Ladner's Proof ---- */

/* Check if S is "Ladner-sparse": sparse enough that Mahaney's
 * theorem applies, making S non-NPC (if P!=NP). */
int ladner_sparse_condition(SparseSet* s) {
    /* Must be sparse: |S_n| <= n^k for some k */
    for (int deg = 1; deg <= 5; deg++) {
        if (sparse_is_bound(s, deg)) {
            /* S is n^deg-sparse => Mahaney applies => S is not NPC */
            return 1;
        }
    }
    return 0;
}

/* Estimate the "Ladner level": how intermediate is this set?
 * Returns a value in [0, 100] where:
 *   0   = in P (or too sparse to be NPC)
 *   100 = NPC (dense enough to be complete)
 * Intermediate values = NP-intermediate degree */
int ladner_level_estimate(SparseSet* s) {
    if (!sparse_is_bound(s, 1)) {
        /* Dense: could be NPC */
        return 100;
    }
    if (!sparse_is_bound(s, 3)) {
        /* Moderate sparsity: high intermediate */
        return 70;
    }
    if (!sparse_is_bound(s, 5)) {
        return 40;
    }
    /* Very sparse: low in degree hierarchy */
    return 10;
}

/* ---- Demo ---- */

void sparse_operations_demo(void) {
    printf("\n===== Sparse Set Operations =====\n\n");

    printf("Sparse sets are central to Ladner's proof:\n");
    printf("  - L is sparse-like (by the slow-growing f)\n");
    printf("  - Mahaney: sparse => not NPC (if P!=NP)\n");
    printf("  - Sparsity guarantees NP-intermediate status\n\n");

    /* Build two sparse sets */
    SparseSet* s1 = sparse_new(10);
    SparseSet* s2 = sparse_new(10);

    printf("--- Building S1 (even-heavy sparse set) ---\n");
    for (int n = 1; n <= 8; n++) {
        int added = 0;
        for (int v = 0; v < n * n && added < n; v++) {
            if (sparse_add(s1, n, n * 100 + v * 2)) added++;
        }
        printf("  n=%d: %d elements\n", n, s1->sizes[n]);
    }

    printf("\n--- Building S2 (odd-heavy sparse set) ---\n");
    for (int n = 1; n <= 8; n++) {
        int added = 0;
        for (int v = 0; v < n * n && added < n; v++) {
            if (sparse_add(s2, n, n * 100 + v * 2 + 1)) added++;
        }
        printf("  n=%d: %d elements\n", n, s2->sizes[n]);
    }

    /* Operations */
    SparseSet* s_union = sparse_union(s1, s2);
    SparseSet* s_inter = sparse_intersect(s1, s2);
    printf("\n--- Union S1 U S2 ---\n");
    { int t=0; for(int n=1;n<=s_union->max_n;n++) t+=s_union->sizes[n]; printf("  Total elements: %d\n", t); }
    printf("  Max density(deg=2): %.4f\n", sparse_max_density(s_union, 2));

    printf("\n--- Intersection S1 cap S2 ---\n");
    { int t=0; for(int n=1;n<=s_inter->max_n;n++) t+=s_inter->sizes[n]; printf("  Total elements: %d\n", t); }
    printf("  Max density(deg=2): %.4f\n", sparse_max_density(s_inter, 2));

    /* Ladner analysis */
    printf("\n--- Ladner Degree Analysis ---\n");
    printf("  S1: Ladner-sparse? %s  Level: %d\n",
           ladner_sparse_condition(s1) ? "YES" : "NO",
           ladner_level_estimate(s1));
    printf("  S_union: Ladner-sparse? %s  Level: %d\n",
           ladner_sparse_condition(s_union) ? "YES" : "NO",
           ladner_level_estimate(s_union));

    /* Density table */
    printf("\n--- Density Table (S1) ---\n");
    printf("  %6s %8s %10s %s\n", "n", "count(n)", "n^2 bound", "sparse?");
    for (int n = 1; n <= 8; n++) {
        int bound = n * n;
        printf("  %6d %8d %10d %s\n", n, s1->sizes[n], bound,
               (s1->sizes[n] <= bound) ? "YES" : "NO");
    }

    printf("\n--- Ladner Connection ---\n");
    printf("Ladner's L is a sparse-like slice of SAT.\n");
    printf("Its sparsity is what makes it NP-INTERMEDIATE:\n");
    printf("  - Too sparse to be NPC (by Mahaney)\n");
    printf("  - Not in P (by delayed diagonalization)\n");
    printf("  - In NP (contains SAT instances as sub-language)\n");

    sparse_free(s1); sparse_free(s2);
    sparse_free(s_union); sparse_free(s_inter);
}
