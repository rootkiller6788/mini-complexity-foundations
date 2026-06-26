/* three_sat_to_clique.c — 3SAT to CLIQUE Reduction (Karp 1972) (L5, L6)
 *
 * Given a 3-CNF formula φ with k clauses, we construct an undirected graph G
 * with 3k vertices such that φ is satisfiable iff G has a k-clique.
 *
 * Construction:
 *   For each clause Cᵢ, create 3 vertices vᵢ₁, vᵢ₂, vᵢ₃
 *     (one per literal in that clause).
 *   Add edge between vᵢₐ and vⱼ_b iff:
 *     (1) i ≠ j (different clauses), AND
 *     (2) literal a in Cᵢ is NOT the negation of literal b in Cⱼ.
 *
 * Theorem (Karp 1972): This is a valid polynomial-time many-one reduction.
 *   • Completeness: SAT ⇒ choose one true literal per clause ⇒ k-clique.
 *   • Soundness:    k-clique chooses one compatible literal per clause ⇒ SAT.
 *   • Time: O(k²) — quadratic in the number of clauses.
 *
 * Reference: Karp (1972), Arora & Barak §2.6 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ===== Graph Data Structure (L3) ===== */

typedef struct {
    int n;          /* Number of vertices */
    int **adj;      /* Adjacency matrix: adj[u][v] = 1 if edge exists */
} Graph;

static Graph* graph_new(int n) {
    Graph *g = malloc(sizeof(Graph));
    g->n = n;
    g->adj = malloc((size_t)n * sizeof(int*));
    for(int i = 0; i < n; i++)
        g->adj[i] = calloc((size_t)n, sizeof(int));
    return g;
}

static void graph_edge(Graph *g, int u, int v) {
    g->adj[u][v] = g->adj[v][u] = 1;
}

static int graph_has_edge(const Graph *g, int u, int v) {
    return g->adj[u][v];
}

static void graph_free(Graph *g) {
    for(int i = 0; i < g->n; i++) free(g->adj[i]);
    free(g->adj);
    free(g);
}

/* Count edges in the graph */
static int graph_edge_count(const Graph *g) {
    int cnt = 0;
    for(int u = 0; u < g->n; u++)
        for(int v = u + 1; v < g->n; v++)
            cnt += g->adj[u][v];
    return cnt;
}

/* ===== 3SAT → CLIQUE Reduction (L5) ===== */

/* Literal encoding: same as in sat_to_3sat.c */
static int encode_lit(int var, int is_neg) {
    return (var << 1) | is_neg;
}
static int decode_var(int lit)   { return lit >> 1; }
static int decode_neg(int lit)   { return lit & 1; }

/* Build the reduction graph.
 * n_clauses: number of clauses in the 3-CNF
 * clauses[i][3]: three literals for clause i (using encode_lit)
 *
 * Returns: Graph* with 3*n_clauses vertices.
 * Vertex index for clause i, literal j: 3*i + j. */
Graph* three_sat_to_clique(int n_clauses, int clauses[][3]) {
    int V = 3 * n_clauses;
    Graph *g = graph_new(V);

    for(int i = 0; i < n_clauses; i++) {
        for(int a = 0; a < 3; a++) {
            int vi = 3 * i + a;
            for(int j = 0; j < n_clauses; j++) {
                if(i == j) continue;  /* No edges within same clause */
                for(int b = 0; b < 3; b++) {
                    int vj = 3 * j + b;
                    int lit_i = clauses[i][a];
                    int lit_j = clauses[j][b];

                    /* Compatible: literals are not complementary */
                    int var_i = decode_var(lit_i);
                    int var_j = decode_var(lit_j);
                    int neg_i = decode_neg(lit_i);
                    int neg_j = decode_neg(lit_j);

                    /* Complementary if same variable, opposite polarity */
                    int complementary = (var_i == var_j) && (neg_i != neg_j);

                    if(!complementary) {
                        graph_edge(g, vi, vj);
                    }
                }
            }
        }
    }
    return g;
}

/* ===== Clique Finding Algorithms (L5) ===== */

/* Check if a specific set of vertices forms a clique */
static int is_clique(const Graph *g, const int *verts, int k) {
    for(int i = 0; i < k; i++)
        for(int j = i + 1; j < k; j++)
            if(!g->adj[verts[i]][verts[j]])
                return 0;
    return 1;
}

/* Brute-force k-clique detection: enumerate all subsets of size k.
   Works for small graphs only (n ≤ 12). Returns 1 if k-clique found. */
static int has_k_clique(const Graph *g, int k) {
    if(g->n > 12) return -1;  /* Too large for brute force */
    int verts[12];

    /* Enumerate all subsets using bitmask */
    for(int mask = 0; mask < (1 << g->n); mask++) {
        int cnt = 0;
        for(int v = 0; v < g->n; v++)
            if((mask >> v) & 1) verts[cnt++] = v;
        if(cnt == k && is_clique(g, verts, k))
            return 1;
    }
    return 0;
}

/* Heuristic: compute maximum clique size via greedy growing.
   Not exact, but useful for demonstration. */
static int greedy_max_clique(const Graph *g) {
    int best = 0;
    int *in_clique = calloc((size_t)g->n, sizeof(int));

    for(int start = 0; start < g->n; start++) {
        int size = 0;
        memset(in_clique, 0, (size_t)g->n * sizeof(int));
        /* Add start vertex */
        in_clique[start] = 1;
        size = 1;
        /* Greedily add vertices connected to all current members */
        for(int v = 0; v < g->n; v++) {
            if(v == start) continue;
            int can_add = 1;
            for(int u = 0; u < g->n && can_add; u++) {
                if(in_clique[u] && !g->adj[u][v]) can_add = 0;
            }
            if(can_add) { in_clique[v] = 1; size++; }
        }
        if(size > best) best = size;
    }
    free(in_clique);
    return best;
}

/* ===== Reduction Verification (L4) ===== */

/* Verify the reduction property:
   For a small test formula with known satisfiability,
   check that the graph has a k-clique iff the formula is SAT. */
static int verify_reduction(int n_clauses, int clauses[][3],
                             int formula_is_sat) {
    Graph *g = three_sat_to_clique(n_clauses, clauses);
    int clique_result = has_k_clique(g, n_clauses);

    graph_free(g);

    if(clique_result < 0) return 2;  /* Too large, can't verify */
    return (clique_result == formula_is_sat);
}

/* ===== Demonstration (L6) ===== */

void three_sat_to_clique_demo(void) {
    printf("\n===== 3SAT → CLIQUE Reduction =====\n\n");

    /* Example 1: (x₀ ∨ x₁ ∨ x₂) ∧ (¬x₀ ∨ ¬x₁ ∨ x₂) — 2 clauses */
    printf("--- Example 1: Satisfiable Formula ---\n");
    int cls1[2][3] = {
        {encode_lit(0,0), encode_lit(1,0), encode_lit(2,0)},
        {encode_lit(0,1), encode_lit(1,1), encode_lit(2,0)}
    };
    Graph *g1 = three_sat_to_clique(2, cls1);
    printf("  3SAT: (x₀ ∨ x₁ ∨ x₂) ∧ (¬x₀ ∨ ¬x₁ ∨ x₂)\n");
    printf("  Graph: %d vertices, %d edges\n", g1->n, graph_edge_count(g1));
    printf("  k=2 clique: %s (expected: FOUND — formula is SAT)\n",
           has_k_clique(g1, 2) == 1 ? "FOUND" : "NOT FOUND");
    printf("  Verification: %s\n",
           verify_reduction(2, cls1, 1) == 1 ? "PASS ✓" : "MISMATCH");
    graph_free(g1);

    /* Example 2: (x₀) ∧ (¬x₀) — UNSAT */
    printf("\n--- Example 2: Unsatisfiable Formula ---\n");
    int cls2[2][3] = {
        {encode_lit(0,0), encode_lit(0,0), encode_lit(0,0)},
        {encode_lit(0,1), encode_lit(0,1), encode_lit(0,1)}
    };
    Graph *g2 = three_sat_to_clique(2, cls2);
    printf("  3SAT: (x₀ ∨ x₀ ∨ x₀) ∧ (¬x₀ ∨ ¬x₀ ∨ ¬x₀)\n");
    printf("  Graph: %d vertices, %d edges\n", g2->n, graph_edge_count(g2));
    /* All edges between clauses are blocked because literals are
       complementary (x₀ vs ¬x₀), so no 2-clique can span both clauses. */
    printf("  k=2 clique: %s (expected: NOT FOUND — formula is UNSAT)\n",
           has_k_clique(g2, 2) == 1 ? "FOUND" : "NOT FOUND");
    graph_free(g2);

    /* Example 3: Larger example with greedy bound */
    printf("\n--- Example 3: Greedy Clique Heuristic ---\n");
    int cls3[3][3] = {
        {encode_lit(0,0), encode_lit(1,0), encode_lit(2,0)},
        {encode_lit(0,0), encode_lit(1,1), encode_lit(2,1)},
        {encode_lit(1,0), encode_lit(2,0), encode_lit(3,0)}
    };
    Graph *g3 = three_sat_to_clique(3, cls3);
    printf("  3SAT: 3 clauses → %d vertices, %d edges\n",
           g3->n, graph_edge_count(g3));
    printf("  Greedy max clique: %d (looking for k=3)\n",
           greedy_max_clique(g3));
    graph_free(g3);

    /* Complexity and significance */
    printf("\n--- Complexity Analysis ---\n");
    printf("Reduction time: O(k²) where k = #clauses\n");
    printf("Graph size:     V = 3k vertices, E = O(k²) edges\n");
    printf("Output size:    O(k²) — polynomial\n");
    printf("Work space:     O(log k) → LOGSPACE reduction\n\n");

    printf("--- Theoretical Significance ---\n");
    printf("CLIQUE was one of Karp's 21 NP-complete problems.\n");
    printf("It's a FUNDAMENTALLY DIFFERENT type of problem:\n");
    printf("  SAT = logical satisfaction\n");
    printf("  CLIQUE = graph-theoretic density\n");
    printf("The reduction shows these are EQUIVALENT.\n");
    printf("This is the essence of NP-completeness:\n");
    printf("  problems from different domains are all the SAME problem!\n");
}
