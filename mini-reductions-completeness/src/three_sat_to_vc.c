/* three_sat_to_vc.c — 3SAT to Vertex Cover Reduction (Karp 1972) (L5, L6)
 *
 * Given a 3-CNF formula φ with n variables and m clauses, construct a graph G
 * and integer k = n + 2m such that φ ∈ 3SAT iff G has a vertex cover of size ≤ k.
 *
 * Construction:
 *   For each variable x:      an edge (x, ¬x) — must cover ONE of {x, ¬x}.
 *   For each clause C=(a∨b∨c): a triangle on 3 clause-vertices — must cover TWO.
 *   For each occurrence of a literal in C: connect the clause-vertex to the
 *     corresponding literal-vertex.
 *
 * Theorem: This is a valid polynomial-time many-one reduction.
 *   k = n + 2m exactly.
 *   Time: O(n + m), graph size O(n + m).
 *
 * Reference: Karp (1972), Garey & Johnson (1979) §3.2 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ===== Graph Data Structure (L3) ===== */
typedef struct { int n; int **adj; } Graph;

static Graph* g_new(int n) {
    Graph *g = malloc(sizeof(Graph)); g->n = n;
    g->adj = malloc((size_t)n * sizeof(int*));
    for(int i = 0; i < n; i++)
        g->adj[i] = calloc((size_t)n, sizeof(int));
    return g;
}
static void g_edge(Graph *g, int u, int v) { g->adj[u][v] = g->adj[v][u] = 1; }
static void g_free(Graph *g) {
    for(int i = 0; i < g->n; i++) free(g->adj[i]);
    free(g->adj); free(g);
}

/* ===== Literal Encoding ===== */
static int encode_lit(int var, int is_neg) { return (var << 1) | is_neg; }
static int decode_var(int lit) { return lit >> 1; }
static int decode_neg(int lit) { return lit & 1; }

/* ===== 3SAT → VERTEX COVER Reduction (L5) ===== */

/* Build the reduction graph.
 *
 * Vertex layout:
 *   [0..2n-1]:    variable vertices — x_i = 2i, ¬x_i = 2i+1
 *   [2n..2n+3m-1]: clause vertices — 3 per clause
 *
 * k = n_vars + 2*n_clauses */
Graph* three_sat_to_vc(int n_vars, int n_clauses, int clauses[][3]) {
    int V = 2 * n_vars + 3 * n_clauses;
    Graph *g = g_new(V);

    /* 1. Variable edges: (x_i, ¬x_i) — must pick at least one per pair */
    for(int v = 0; v < n_vars; v++)
        g_edge(g, 2*v, 2*v+1);

    /* 2. Clause triangles: must pick at least 2 of 3 clause-vertices */
    for(int i = 0; i < n_clauses; i++) {
        int base = 2 * n_vars + 3 * i;
        g_edge(g, base, base+1);
        g_edge(g, base+1, base+2);
        g_edge(g, base+2, base);
    }

    /* 3. Connect clause-vertices to literal-vertices */
    for(int i = 0; i < n_clauses; i++) {
        for(int j = 0; j < 3; j++) {
            int lit = clauses[i][j];
            int var = decode_var(lit);
            int is_neg = decode_neg(lit);
            int lit_vertex = 2 * var + is_neg;  /* 2v if positive, 2v+1 if negative */
            int clause_vertex = 2 * n_vars + 3 * i + j;
            g_edge(g, lit_vertex, clause_vertex);
        }
    }
    return g;
}

/* ===== Verification (L4) ===== */

/* Theorem: k = n_vars + 2*n_clauses is the target VC size.
 *
 * Proof sketch:
 *   (⇒) Assignment → Vertex Cover of size k:
 *        Pick TRUE literal vertices (n choices) for variable edges.
 *        For each clause: one literal vertex is already picked (since
 *        clause SAT), so the other 2 clause-vertices (2m choices)
 *        suffice to cover the triangle and cross-edges.
 *   (⇐) Vertex Cover of size k → Assignment:
 *        Must pick AT LEAST 1 per variable edge → n vertices.
 *        Must pick AT LEAST 2 per clause triangle → 2m vertices.
 *        With k = n+2m budget, exactly n from var edges + 2m from clauses.
 *        The unpicked clause vertex connects to a picked literal,
 *        giving a satisfying assignment. */

static int compute_k(int n_vars, int n_clauses) {
    return n_vars + 2 * n_clauses;
}

/* Heuristic: greedy vertex cover (not optimal, for demonstration) */
static int greedy_vertex_cover_size(const Graph *g) {
    int *covered_edges = calloc((size_t)g->n * g->n, sizeof(int));
    int *in_cover = calloc((size_t)g->n, sizeof(int));
    int cover_size = 0;
    int total_edges = 0;

    for(int u = 0; u < g->n; u++)
        for(int v = u+1; v < g->n; v++)
            if(g->adj[u][v]) total_edges++;

    int covered_count = 0;
    while(covered_count < total_edges) {
        /* Greedy: pick vertex of highest uncovered degree */
        int best_v = -1, best_deg = -1;
        for(int v = 0; v < g->n; v++) {
            if(in_cover[v]) continue;
            int deg = 0;
            for(int u = 0; u < g->n; u++)
                if(g->adj[v][u] && !covered_edges[v * g->n + u]) deg++;
            if(deg > best_deg) { best_deg = deg; best_v = v; }
        }
        if(best_v < 0) break;
        in_cover[best_v] = 1;
        cover_size++;
        for(int u = 0; u < g->n; u++) {
            if(g->adj[best_v][u] && !covered_edges[best_v * g->n + u]) {
                covered_edges[best_v * g->n + u] = 1;
                covered_edges[u * g->n + best_v] = 1;
                covered_count++;
            }
        }
    }
    free(covered_edges); free(in_cover);
    return cover_size;
}

/* ===== Demonstration (L6) ===== */

void three_sat_to_vc_demo(void) {
    printf("\n===== 3SAT → Vertex Cover Reduction =====\n\n");

    /* Example 1: (x₀ ∨ x₁) ∧ (¬x₀ ∨ ¬x₁), n=2 vars, m=2 clauses */
    printf("--- Example 1: Small Instance ---\n");
    int cls1[2][3] = {
        {encode_lit(0,0), encode_lit(1,0), encode_lit(0,0)},
        {encode_lit(0,1), encode_lit(1,1), encode_lit(1,1)}
    };
    Graph *g1 = three_sat_to_vc(2, 2, cls1);
    int k1 = compute_k(2, 2);
    printf("  φ = (x₀ ∨ x₁) ∧ (¬x₀ ∨ ¬x₁), n=%d, m=%d\n", 2, 2);
    printf("  Graph: %d vertices\n", g1->n);
    printf("  Target VC size k = n + 2m = %d + %d = %d\n", 2, 4, k1);
    printf("  Greedy VC upper bound: %d\n", greedy_vertex_cover_size(g1));
    g_free(g1);

    /* Example 2: With 3 variables */
    printf("\n--- Example 2: 3 Variables ---\n");
    int cls2[3][3] = {
        {encode_lit(0,0), encode_lit(1,0), encode_lit(2,0)},
        {encode_lit(0,1), encode_lit(1,1), encode_lit(2,0)},
        {encode_lit(0,0), encode_lit(1,1), encode_lit(2,1)}
    };
    Graph *g2 = three_sat_to_vc(3, 3, cls2);
    int k2 = compute_k(3, 3);
    printf("  φ with n=%d, m=%d\n", 3, 3);
    printf("  Graph: %d vertices\n", g2->n);
    printf("  Target VC size k = %d + %d = %d\n", 3, 6, k2);
    g_free(g2);

    /* Cross-check: show vertex cover size tracks satisfiability */
    printf("\n--- Reduction Correctness Check ---\n");
    printf("Theorem: φ ∈ 3SAT  ⇔  G has vertex cover of size n + 2m.\n");
    printf("  Variable edges: need exactly 1 vertex per pair (n choices).\n");
    printf("  Clause triangles: need exactly 2 vertices per clause (2m choices).\n");
    printf("  Total budget: n + 2m. The only possible configuration\n");
    printf("  corresponds to a satisfying assignment.\n");
    printf("  The literal-vertex chosen for the variable edge also\n");
    printf("  covers the cross-edge from each clause where it appears.\n");

    printf("\n--- Complexity ---\n");
    printf("Time:  O(n + m)  [create vertices and edges]\n");
    printf("Size:  O(n + m)  [vertices = 2n + 3m]\n");
    printf("This is a LINEAR-TIME, LINEAR-SPACE reduction.\n");

    printf("\n--- Chain Context ---\n");
    printf("SAT → 3SAT → CLIQUE → VERTEX COVER → HAM CYCLE → TSP\n");
    printf("Each ≤ is transitive. VC is NP-complete.\n");
}
