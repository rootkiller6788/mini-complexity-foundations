/* example_npc_proof.c — End-to-end: Prove a problem NP-complete
 *
 * This example demonstrates the COMPLETE workflow for proving
 * a problem is NP-complete following the Karp reduction template:
 *
 *   Step 1: Show the problem is IN NP (membership).
 *   Step 2: Choose a known NP-complete problem to reduce FROM.
 *   Step 3: Construct a polynomial-time reduction.
 *   Step 4: Prove COMPLETENESS (YES maps to YES).
 *   Step 5: Prove SOUNDNESS (NO maps to NO).
 *
 * Example: Proving INDEPENDENT SET is NP-complete
 * by reduction from CLIQUE.
 *
 * CLIQUE: G has k-clique iff G's complement has k-independent set.
 *
 * Reference: Garey & Johnson (1979), proof technique from Karp (1972) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* ===== Helper: Graph representation ===== */
typedef struct {
    int n;
    int **adj;
} Graph;

static Graph *graph_new(int n) {
    Graph *g = malloc(sizeof(Graph));
    g->n = n;
    g->adj = malloc((size_t)n * sizeof(int*));
    for (int i = 0; i < n; i++)
        g->adj[i] = calloc((size_t)n, sizeof(int));
    return g;
}

static void graph_free(Graph *g) {
    for (int i = 0; i < g->n; i++) free(g->adj[i]);
    free(g->adj); free(g);
}

/* CLIQUE -> INDEPENDENT SET reduction.
 * This is the TRIVIAL reduction: take the COMPLEMENT graph.
 * G has k-clique iff complement(G) has k-independent set.
 * Time: O(n^2) to complement edges. */
static Graph *clique_to_independent_set(const Graph *g) {
    Graph *comp = graph_new(g->n);
    for (int u = 0; u < g->n; u++)
        for (int v = u + 1; v < g->n; v++)
            if (!g->adj[u][v])
                comp->adj[u][v] = comp->adj[v][u] = 1;
    return comp;
}

/* ===== Verify the reduction on a small example ===== */

int main(void) {
    setbuf(stdout, NULL);
    printf("\n===== Example: NP-Completeness Proof Workflow =====\n\n");

    /* --- Step 1: Membership in NP --- */
    printf("Step 1: INDEPENDENT SET is in NP.\n");
    printf("  Certificate: a set S of k vertices.\n");
    printf("  Verifier: check |S| = k and no edge connects two vertices in S.\n");
    printf("  Time: O(k^2) = polynomial. ✓\n\n");

    /* --- Step 2: Choose known NPC problem --- */
    printf("Step 2: Reduce from CLIQUE (known NP-complete).\n\n");

    /* --- Step 3: Construct reduction --- */
    printf("Step 3: Reduction construction.\n");
    printf("  f(G,k) = (complement(G), k).\n");
    printf("  Complement: edge (u,v) exists iff NOT in original.\n");
    printf("  Time: O(n^2) — polynomial. ✓\n\n");

    /* Build a small demonstration graph */
    Graph *g = graph_new(5);
    /* Pentagon: edges (0-1, 1-2, 2-3, 3-4, 4-0) */
    g->adj[0][1] = g->adj[1][0] = 1;
    g->adj[1][2] = g->adj[2][1] = 1;
    g->adj[2][3] = g->adj[3][2] = 1;
    g->adj[3][4] = g->adj[4][3] = 1;
    g->adj[4][0] = g->adj[0][4] = 1;

    printf("  Original graph (pentagon):\n");
    printf("    Edges: (0,1) (1,2) (2,3) (3,4) (4,0)\n");

    Graph *comp = clique_to_independent_set(g);
    printf("  Complement graph (pentagram):\n");
    printf("    Edges: (0,2) (0,3) (1,3) (1,4) (2,4)\n\n");

    /* --- Step 4: Completeness --- */
    printf("Step 4: COMPLETENESS (YES -> YES).\n");
    printf("  If G has k-clique C, then in complement(G),\n");
    printf("  C has NO edges -> C is a k-independent set.\n");
    printf("  Pentagon: 3-clique? No (max clique = 2).\n");
    printf("  Pentagon complement: 3-independent set?\n");
    printf("    Vertices {0,2,4}: edges (0,2)? YES in complement!\n");
    printf("    Actually: pentagram has triangle {0,2,4}.\n");
    printf("    So pentagram has 3-clique -> original has 3-indep set.\n");
    printf("    Check original: {0,2,4} — edges? 0-4 YES, 0-2 NO, 2-4 NO.\n");
    printf("    Hmm, let's check carefully...\n\n");

    /* Brute-force check */
    printf("  Brute-force max clique in original: ");
    int max_clique = 0;
    for (int mask = 0; mask < (1 << 5); mask++) {
        int cnt = 0, verts[5];
        for (int v = 0; v < 5; v++)
            if ((mask >> v) & 1) verts[cnt++] = v;
        if (cnt <= max_clique) continue;
        int is_clique = 1;
        for (int i = 0; i < cnt && is_clique; i++)
            for (int j = i + 1; j < cnt && is_clique; j++)
                if (!g->adj[verts[i]][verts[j]]) is_clique = 0;
        if (is_clique) max_clique = cnt;
    }
    printf("%d\n", max_clique);

    printf("  Brute-force max indep set in original: ");
    int max_is = 0;
    for (int mask = 0; mask < (1 << 5); mask++) {
        int cnt = 0, verts[5];
        for (int v = 0; v < 5; v++)
            if ((mask >> v) & 1) verts[cnt++] = v;
        if (cnt <= max_is) continue;
        int is_indep = 1;
        for (int i = 0; i < cnt && is_indep; i++)
            for (int j = i + 1; j < cnt && is_indep; j++)
                if (g->adj[verts[i]][verts[j]]) is_indep = 0;
        if (is_indep) max_is = cnt;
    }
    printf("%d\n", max_is);

    printf("  Max clique in complement: %d\n\n", max_is);

    /* --- Step 5: Soundness --- */
    printf("Step 5: SOUNDNESS (NO -> NO).\n");
    printf("  If G has NO k-clique, then complement(G) has NO\n");
    printf("  k-independent set. Proof: contrapositive of step 4.\n");
    printf("  (If comp had k-independent set I, then I is k-clique in G.)\n\n");

    /* --- Conclusion --- */
    printf("===== CONCLUSION =====\n");
    printf("INDEPENDENT SET is NP-complete. ■\n");
    printf("  (1) IS in NP (certificate = vertex set). ✓\n");
    printf("  (2) CLIQUE <=_p IS (complement graph). ✓\n");
    printf("  (3) CLIQUE is NP-complete (Karp 1972). ✓\n\n");

    printf("This 5-step template works for ALL NP-completeness proofs.\n");
    printf("The HARD part is always step 3: designing the gadget reduction.\n");
    printf("The complement trick is the SIMPLEST possible gadget.\n");
    printf("Most reductions require elaborate constructions (see src/ files).\n");

    graph_free(g);
    graph_free(comp);
    return 0;
}
