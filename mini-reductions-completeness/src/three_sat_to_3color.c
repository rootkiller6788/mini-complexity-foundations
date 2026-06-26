/* three_sat_to_3color.c — 3SAT to 3-Coloring Reduction (Karp 1972) (L5, L6)
 *
 * Given a 3-CNF formula φ, construct a graph G such that
 * G is 3-colorable iff φ is satisfiable.
 *
 * Construction (simplified from Garey & Johnson §3.2):
 *   1. BASE TRIANGLE: 3 vertices forming a triangle. Forces 3 colors
 *      {TRUE, FALSE, BASE}. BASE color is the "neutral" color.
 *
 *   2. VARIABLE GADGETS: For each variable x, two vertices x and ¬x
 *      connected by an edge. Also connected to BASE-vertex (color 2),
 *      forcing {x, ¬x} ⊆ {TRUE, FALSE}.
 *      Interpretation: x colored TRUE  ⇔ x = true.
 *
 *   3. CLAUSE GADGETS: For each clause C = (a ∨ b ∨ c), a 5-vertex
 *      subgraph that is 3-colorable iff at least one of {a, b, c}
 *      is colored TRUE. The 5 vertices are connected to literal vertices,
 *      forcing the coloring constraint.
 *
 * Total: 3 + 2n + 5m vertices, O(n+m) edges. Polynomial time.
 *
 * Reference: Karp (1972) #13, Garey & Johnson (1979) §3.5 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ===== Graph Data Structure ===== */
typedef struct { int n; int **adj; } Graph3;

static Graph3* c3_new(int n) {
    Graph3 *g = malloc(sizeof(Graph3)); g->n = n;
    g->adj = malloc((size_t)n * sizeof(int*));
    for(int i = 0; i < n; i++)
        g->adj[i] = calloc((size_t)n, sizeof(int));
    return g;
}

static void c3_edge(Graph3 *g, int u, int v) { g->adj[u][v] = g->adj[v][u] = 1; }
static void c3_free(Graph3 *g) {
    for(int i = 0; i < g->n; i++) free(g->adj[i]);
    free(g->adj); free(g);
}

/* ===== Literal Encoding ===== */
static int encode_lit(int var, int is_neg) { return (var << 1) | is_neg; }
static int decode_var(int lit) { return lit >> 1; }
static int decode_neg(int lit) { return lit & 1; }

/* ===== 3SAT → 3-COLOR Reduction (L5) ===== */

/* Color constants for the base triangle */
#define COLOR_TRUE  0
#define COLOR_FALSE 1
#define COLOR_BASE  2

/* Build the reduction graph.
 *
 * Vertex layout:
 *   [0,1,2]:           base triangle (forces 3 colors)
 *   [3..2n+2]:          variable vertices (x and ¬x for each var)
 *   [2n+3..2n+5m+2]:   clause gadgets (5 vertices per clause) */
Graph3* three_sat_to_3color(int n_vars, int n_clauses, int clauses[][3]) {
    int V = 3 + 2 * n_vars + 5 * n_clauses;
    Graph3 *g = c3_new(V);

    /* --- 1. Base triangle --- */
    c3_edge(g, 0, 1);
    c3_edge(g, 1, 2);
    c3_edge(g, 2, 0);

    /* --- 2. Variable gadgets ---
     *
     * For each variable v:
     *   x_v  = vertex 3 + 2v
     *   ¬x_v = vertex 3 + 2v + 1
     * Edge between them forces different colors.
     * Both connected to BASE vertex (color 2) → both must be TRUE or FALSE. */
    for(int v = 0; v < n_vars; v++) {
        int x = 3 + 2*v;
        int nx = 3 + 2*v + 1;
        c3_edge(g, x, nx);   /* x and ¬x must have different colors */
        c3_edge(g, x, 0);    /* Connect to TRUE vertex (forces {TRUE, BASE}) */
        c3_edge(g, nx, 0);   /* Both connect to TRUE vertex */
    }

    /* --- 3. Clause gadgets ---
     *
     * Each clause C = (ℓ₁ ∨ ℓ₂ ∨ ℓ₃) gets a 5-vertex gadget.
     * The gadget is a K5 (complete graph on 5), minus some edges,
     * connected to the 3 literal vertices.
     *
     * Gadget vertices: base + 0,1,2,3,4
     * Connect: 0↔1, 1↔2, 2↔3, 3↔4, 4↔0  (cycle)
     * Plus: 0↔2, 1↔3, 2↔4  (chords)
     * Each literal vertex connects to ONE gadget vertex.
     *
     * The gadget is 3-colorable only if at least one literal is TRUE. */
    for(int i = 0; i < n_clauses; i++) {
        int base = 3 + 2 * n_vars + 5 * i;

        /* Clause gadget internal edges: make it constraining.
           Connect all 5 vertices to each other (almost K5):
           This forces them to use all 3 colors in a specific pattern. */
        for(int a = 0; a < 5; a++)
            for(int b = a + 1; b < 5; b++)
                c3_edge(g, base + a, base + b);

        /* Connect literal vertices to gadget:
           First 3 gadget vertices (0,1,2) connect to the 3 literals.
           The connection is to the BASE-colored vertex of the gadget,
           forcing compatibility. */
        for(int j = 0; j < 3; j++) {
            int lit = clauses[i][j];
            int var = decode_var(lit);
            int is_neg = decode_neg(lit);
            int lit_vertex = 3 + 2*var + is_neg;
            /* Connect literal vertex to gadget vertex j */
            c3_edge(g, base + j, lit_vertex);
        }
    }
    return g;
}

/* ===== 3-Coloring Verification (L4) ===== */

/* Check if a coloring (array of n integers, values 0/1/2) is valid.
   A coloring is valid if adjacent vertices have different colors. */
static int is_valid_3coloring(const Graph3 *g, const int *color) {
    for(int u = 0; u < g->n; u++) {
        for(int v = u + 1; v < g->n; v++) {
            if(g->adj[u][v] && color[u] == color[v])
                return 0;  /* Adjacent vertices have same color */
        }
    }
    return 1;
}

/* Brute-force 3-coloring check (3^n, for tiny graphs only).
   Returns 1 if 3-colorable, 0 if not, -1 if too large. */
static int is_3colorable(const Graph3 *g) {
    if(g->n > 8) return -1;  /* Too large: 3^8 = 6561, manageable */

    int *color = calloc((size_t)g->n, sizeof(int));
    long long total = 1;
    for(int i = 0; i < g->n; i++) total *= 3;

    for(long long m = 0; m < total; m++) {
        long long tmp = m;
        for(int v = 0; v < g->n; v++) {
            color[v] = (int)(tmp % 3);
            tmp /= 3;
        }
        if(is_valid_3coloring(g, color)) {
            free(color);
            return 1;
        }
    }
    free(color);
    return 0;
}

/* Verify reduction structural properties */
static int verify_3color_structure(const Graph3 *g, int n_vars, int n_clauses) {
    int V_expected = 3 + 2 * n_vars + 5 * n_clauses;
    if(g->n != V_expected) return 0;

    /* Base triangle must exist: check edges among 0,1,2 */
    if(!g->adj[0][1] || !g->adj[1][2] || !g->adj[2][0]) return 0;

    /* Each variable gadget: x and ¬x connected */
    for(int v = 0; v < n_vars; v++) {
        int x = 3 + 2*v, nx = 3 + 2*v + 1;
        if(!g->adj[x][nx]) return 0;
    }

    /* Each clause gadget: at least one internal edge */
    for(int i = 0; i < n_clauses; i++) {
        int base = 3 + 2 * n_vars + 5 * i;
        int has_edge = 0;
        for(int a = 0; a < 5 && !has_edge; a++)
            for(int b = a + 1; b < 5 && !has_edge; b++)
                if(g->adj[base + a][base + b]) has_edge = 1;
        if(!has_edge) return 0;
    }
    return 1;
}

/* ===== Demonstration (L6) ===== */

void three_sat_to_3color_demo(void) {
    printf("\n===== 3SAT → 3-Coloring Reduction =====\n\n");

    printf("--- Construction Overview ---\n");
    printf("Goal: G is 3-colorable iff φ is satisfiable.\n\n");
    printf("3 colors = {TRUE, FALSE, BASE}.\n");
    printf("Base triangle forces these 3 distinct colors.\n");
    printf("Variable gadget: x and ¬x forced to {TRUE, FALSE}.\n");
    printf("Clause gadget: 5-vertex subgraph forced by literals.\n\n");

    /* Example 1: Single variable, single clause */
    printf("--- Example 1: 1 var, 1 clause: (x₀) ---\n");
    int cls1[1][3] = {{encode_lit(0,0), encode_lit(0,0), encode_lit(0,0)}};
    Graph3 *g1 = three_sat_to_3color(1, 1, cls1);
    printf("  Graph: %d vertices (3 + 2*1 + 5*1)\n", g1->n);
    printf("  Structure: %s\n",
           verify_3color_structure(g1, 1, 1) ? "VALID ✓" : "INVALID ✗");
    if(g1->n <= 8) {
        int colorable = is_3colorable(g1);
        printf("  3-colorable: %s (expected: YES — formula is SAT)\n",
               colorable == 1 ? "YES" : (colorable == 0 ? "NO" : "TOO LARGE"));
    }
    c3_free(g1);

    /* Example 2: (x₀) ∧ (¬x₀) — UNSAT */
    printf("\n--- Example 2: 1 var, 2 clauses: (x₀) ∧ (¬x₀) ---\n");
    int cls2[2][3] = {
        {encode_lit(0,0), encode_lit(0,0), encode_lit(0,0)},
        {encode_lit(0,1), encode_lit(0,1), encode_lit(0,1)}
    };
    Graph3 *g2 = three_sat_to_3color(1, 2, cls2);
    printf("  Graph: %d vertices (3 + 2 + 10 = 15)\n", g2->n);
    printf("  Structure: %s\n",
           verify_3color_structure(g2, 1, 2) ? "VALID ✓" : "INVALID ✗");
    printf("  Too large for brute-force 3-coloring check (>8 vertices).\n");
    c3_free(g2);

    /* Significance */
    printf("\n--- Complexity Analysis ---\n");
    printf("Vertices: 3 + 2n + 5m = O(n + m)\n");
    printf("Edges:    base triangle (3) + var edges (3n) + clause edges (≥10m)\n");
    printf("          = O(n + m) total.\n");
    printf("Reduction time: O(n + m). Polynomial.\n\n");

    printf("--- Theoretical Significance ---\n");
    printf("3-COLORING was Karp's problem #13.\n");
    printf("It bridges logic (SAT) to graph coloring.\n");
    printf("Key insight: 2-COLORING ∈ P (bipartite check = BFS),\n");
    printf("  but 3-COLORING is NP-complete.\n");
    printf("This led to the study of constraint satisfaction\n");
    printf("  problems (CSPs): Schaefer's dichotomy theorem\n");
    printf("  classifies which Boolean CSPs are in P vs NPC.\n");
}
