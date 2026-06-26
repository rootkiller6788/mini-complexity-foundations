/* graph_isomorphism.c -- Graph Isomorphism: Algorithms and Status
 *
 * Graph Isomorphism (GI): Given G1, G2 of n vertices, are they isomorphic?
 * I.e., does there exist a permutation pi such that pi(G1) = G2?
 *
 * GI is in NP (guess permutation, verify in poly time).
 * GI is NOT known to be in P.
 * GI is NOT known to be NP-complete (would collapse PH to Sigma_2).
 *
 * Babai (STOC 2016): GI in quasipolynomial time: exp(O((log n)^c)).
 * This is the strongest evidence that GI is NP-INTERMEDIATE.
 *
 * Algorithms implemented:
 *   1. Brute-force enumeration (n! time)
 *   2. Degree sequence invariant (necessary condition)
 *   3. Weisfeiler-Lehman (WL) color refinement (poly-time heuristic)
 *
 * WL works for almost all graphs but fails on strongly regular graphs.
 *
 * References:
 *   - Babai, STOC 2016: "Graph Isomorphism in Quasipolynomial Time"
 *   - Weisfeiler & Lehman, 1968
 *   - Cai, Furer, Immerman, Combinatorica 12(4):389-410, 1992
 */

#include "ladner.h"

/* ---- Graph Lifecycle ---- */

Graph* g_new(int n) {
    Graph* g = malloc(sizeof(Graph));
    g->n = n;
    g->adj = malloc((size_t)n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        g->adj[i] = calloc((size_t)n, sizeof(int));
    }
    g->label = NULL;
    return g;
}

void g_free(Graph* g) {
    if (!g) return;
    for (int i = 0; i < g->n; i++) free(g->adj[i]);
    free(g->adj);
    free(g->label);
    free(g);
}

/* ---- Basic Operations ---- */

/* Add an undirected edge */
int g_add_edge(Graph* g, int u, int v) {
    if (u < 0 || v < 0 || u >= g->n || v >= g->n) return 0;
    g->adj[u][v] = 1;
    g->adj[v][u] = 1;
    return 1;
}

/* Apply permutation to graph: h = pi(g) */
Graph* g_permute(Graph* g, int* pi) {
    Graph* h = g_new(g->n);
    for (int i = 0; i < g->n; i++)
        for (int j = 0; j < g->n; j++)
            h->adj[pi[i]][pi[j]] = g->adj[i][j];
    return h;
}

/* Equality check (no isomorphism -- exact adjacency match) */
int g_equal(Graph* a, Graph* b) {
    if (a->n != b->n) return 0;
    for (int i = 0; i < a->n; i++)
        for (int j = 0; j < a->n; j++)
            if (a->adj[i][j] != b->adj[i][j]) return 0;
    return 1;
}

/* ---- Degree Sequence ---- */

/* Compute sorted degree sequence (necessary but not sufficient) */
void deg_seq(Graph* g, int* degs) {
    for (int i = 0; i < g->n; i++) {
        degs[i] = 0;
        for (int j = 0; j < g->n; j++)
            degs[i] += g->adj[i][j];
    }
    for (int i = 0; i < g->n; i++) {
        for (int j = i + 1; j < g->n; j++) {
            if (degs[i] > degs[j]) {
                int t = degs[i]; degs[i] = degs[j]; degs[j] = t;
            }
        }
    }
}

int deg_sequence_match(int* g1_degs, int* g2_degs, int n) {
    for (int i = 0; i < n; i++)
        if (g1_degs[i] != g2_degs[i]) return 0;
    return 1;
}

/* ---- Brute-Force Isomorphism ---- */

static int gi_bt(Graph* a, Graph* b, int* pi, int* used, int depth) {
    if (depth == a->n) {
        Graph* h = g_permute(a, pi);
        int result = g_equal(h, b);
        g_free(h);
        return result;
    }
    for (int i = 0; i < a->n; i++) {
        if (used[i]) continue;
        int deg_a = 0, deg_b = 0;
        for (int k = 0; k < a->n; k++) {
            deg_a += a->adj[depth][k];
            deg_b += b->adj[i][k];
        }
        if (deg_a != deg_b) continue;
        pi[depth] = i;
        used[i] = 1;
        if (gi_bt(a, b, pi, used, depth + 1)) {
            used[i] = 0;
            return 1;
        }
        used[i] = 0;
    }
    return 0;
}

int gi_brute(Graph* a, Graph* b) {
    if (a->n != b->n) return 0;
    int* pi   = malloc((size_t)a->n * sizeof(int));
    int* used = calloc((size_t)a->n, sizeof(int));
    int result = gi_bt(a, b, pi, used, 0);
    free(pi); free(used);
    return result;
}

/* ---- Weisfeiler-Lehman Color Refinement ---- */

static void wl_refine(Graph* g, int* colors, int iters) {
    for (int v = 0; v < g->n; v++) {
        colors[v] = 0;
        for (int u = 0; u < g->n; u++)
            colors[v] += g->adj[v][u];
    }
    for (int iter = 0; iter < iters; iter++) {
        int* new_colors = malloc((size_t)g->n * sizeof(int));
        for (int v = 0; v < g->n; v++) {
            int h = colors[v] * 31337;
            for (int u = 0; u < g->n; u++) {
                if (g->adj[v][u]) h += colors[u] * 1009;
            }
            new_colors[v] = h % 100000;
        }
        int changed = 0;
        for (int v = 0; v < g->n; v++) {
            if (colors[v] != new_colors[v]) changed = 1;
            colors[v] = new_colors[v];
        }
        free(new_colors);
        if (!changed) break;
    }
}

int wl_test(Graph* a, Graph* b, int iters) {
    if (a->n != b->n) return -1;
    int* ca = malloc((size_t)a->n * sizeof(int));
    int* cb = malloc((size_t)b->n * sizeof(int));
    wl_refine(a, ca, iters);
    wl_refine(b, cb, iters);
    for (int i = 0; i < a->n; i++) {
        for (int j = i + 1; j < a->n; j++) {
            if (ca[i] > ca[j]) { int t = ca[i]; ca[i] = ca[j]; ca[j] = t; }
            if (cb[i] > cb[j]) { int t = cb[i]; cb[i] = cb[j]; cb[j] = t; }
        }
    }
    int match = 1;
    for (int i = 0; i < a->n; i++)
        if (ca[i] != cb[i]) { match = 0; break; }
    free(ca); free(cb);
    return match ? 1 : -1;
}

/* ---- Demo ---- */

void graph_isomorphism_demo(void) {
    printf("\n===== Graph Isomorphism =====\n\n");
    printf("GI is a canonical NP-intermediate candidate:\n");
    printf("  In NP: guess permutation, verify in O(n^2).\n");
    printf("  Not in P: best known is quasipoly (Babai 2015).\n");
    printf("  Not NPC: would collapse PH to Sigma_2.\n\n");

    Graph* g1 = g_new(4); Graph* g2 = g_new(4); Graph* g3 = g_new(4);
    g_add_edge(g1, 0, 1); g_add_edge(g1, 1, 2);
    g_add_edge(g1, 2, 3); g_add_edge(g1, 3, 0);
    g_add_edge(g2, 0, 2); g_add_edge(g2, 2, 1);
    g_add_edge(g2, 1, 3); g_add_edge(g2, 3, 0);
    g_add_edge(g3, 0, 1); g_add_edge(g3, 0, 2);
    g_add_edge(g3, 0, 3);

    int d1[4], d2[4], d3[4];
    deg_seq(g1, d1); deg_seq(g2, d2); deg_seq(g3, d3);

    printf("--- Degree Sequences ---\n");
    printf("  G1(4-cycle): ["); for(int i=0;i<4;i++) printf("%d ", d1[i]); printf("]\n");
    printf("  G2(4-cycle): ["); for(int i=0;i<4;i++) printf("%d ", d2[i]); printf("]\n");
    printf("  G3(star):    ["); for(int i=0;i<4;i++) printf("%d ", d3[i]); printf("]\n");
    printf("  deg(G1)==deg(G2): %s\n", deg_sequence_match(d1,d2,4)?"YES":"NO");
    printf("  deg(G1)==deg(G3): %s\n", deg_sequence_match(d1,d3,4)?"YES":"NO");

    printf("\n--- WL Test (iters=5) ---\n");
    printf("  WL(G1,G2): %s\n", wl_test(g1,g2,5)>0?"POSSIBLY isomorphic":"NOT iso");
    printf("  WL(G1,G3): %s\n", wl_test(g1,g3,5)>0?"POSSIBLY isomorphic":"NOT iso");

    printf("\n--- Brute Force ---\n");
    printf("  G1~G2: %s\n", gi_brute(g1,g2)?"YES":"NO");
    printf("  G1~G3: %s\n", gi_brute(g1,g3)?"YES":"NO");

    printf("\n--- State of Art ---\n");
    printf("  Brute: O(n!) -- n>10 impractical\n");
    printf("  WL:    O(n^3) -- works for almost all graphs\n");
    printf("  Babai: exp(O(log^c n)) -- QUASIPOLYNOMIAL (2015)\n");
    printf("  Is GI in P? OPEN since 1971.\n");

    printf("\n--- Ladner Connection ---\n");
    printf("GI is the strongest NATURAL NP-intermediate candidate.\n");
    printf("If P!=NP, Ladner guarantees NP-intermediate EXISTS.\n");
    printf("GI might be the first concrete example we can prove.\n");

    g_free(g1); g_free(g2); g_free(g3);
}
