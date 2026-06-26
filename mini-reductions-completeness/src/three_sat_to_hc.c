/* three_sat_to_hc.c — 3SAT to Hamiltonian Cycle Reduction (Karp 1972) (L5, L6)
 *
 * This is the most COMPLEX of Karp's 21 reductions, using an elaborate
 * "gadget" construction. Each variable becomes a chain of diamond gadgets
 * that force the Hamiltonian cycle to choose one of two paths
 * (true or false). Clause nodes act as "must-visit" checkpoints.
 *
 * Gadget structure (per variable x_i):
 *   A chain of m+1 diamond gadgets, where m = number of clauses.
 *   Each diamond: 4 vertices {a,b,c,d} with edges forming a diamond shape.
 *   Left-to-right path through diamonds = x_i TRUE.
 *   Right-to-left zigzag path = x_i FALSE.
 *
 * Clause connection:
 *   If clause C_j contains literal x_i, connect the j-th diamond's
 *   "true" vertex to clause node j. If C_j contains ¬x_i, connect
 *   to the "false" vertex instead. This forces the HC to visit C_j
 *   via a satisfied literal.
 *
 * Total vertices: O(n·m), total edges: O(n·m). Polynomial.
 *
 * Reference: Karp (1972), Garey & Johnson (1979) §3.1.4
 * Reference: Sipser §7.4 (NP-completeness of HAMPATH) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ===== Directed Graph for HC ===== */
typedef struct {
    int n;        /* Number of vertices */
    int **adj;    /* Directed adjacency: adj[u][v] = 1 if u→v */
} Digraph;

static Digraph* dg_new(int n) {
    Digraph *g = malloc(sizeof(Digraph));
    g->n = n;
    g->adj = malloc((size_t)n * sizeof(int*));
    for(int i = 0; i < n; i++)
        g->adj[i] = calloc((size_t)n, sizeof(int));
    return g;
}

static void dg_edge(Digraph *g, int u, int v) {
    g->adj[u][v] = 1;
}

static void dg_biedge(Digraph *g, int u, int v) {
    g->adj[u][v] = g->adj[v][u] = 1;
}

static void dg_free(Digraph *g) {
    for(int i = 0; i < g->n; i++) free(g->adj[i]);
    free(g->adj); free(g);
}

/* ===== Literal Encoding ===== */
static int encode_lit(int var, int is_neg) { return (var << 1) | is_neg; }
static int decode_var(int lit) { return lit >> 1; }
static int decode_neg(int lit) { return lit & 1; }

/* ===== 3SAT → DIRECTED HC Reduction (L5) ===== */

/* Build the reduction digraph.
 *
 * Vertex layout:
 *   Variable gadgets: n_vars * 2*(m+1) vertices (diamond chains)
 *   Clause nodes:     m vertices
 *   Start/end nodes:  2 vertices
 *
 * The construction follows Arora & Barak §2.6.4. */
Digraph* three_sat_to_hc(int n_vars, int n_clauses, int clauses[][3]) {
    int chain_len = n_clauses + 1;  /* m+1 diamonds per variable */
    int per_var = 2 * chain_len;    /* Each diamond: 2 vertices on each side */
    int var_total = per_var * n_vars;
    int V = var_total + n_clauses + 2;  /* + start/end */
    Digraph *g = dg_new(V);

    /* --- 1. Variable diamond chains ---
     *
     * For variable v, we create a chain of chain_len diamonds.
     * Diamond i: vertices {a=2i, b=2i+1, c=2i+2, d=2i+3}
     *     a ─── c
     *     │ ╲ ╱ │
     *     │  ╳  │
     *     │ ╱ ╲ │
     *     b ─── d
     * (actually a cycle a→b→c→d→a with diagonals for HC routing)
     *
     * "True path" goes through a→b chain.
     * "False path" goes through c→d chain. */
    for(int v = 0; v < n_vars; v++) {
        int base = v * per_var;
        for(int i = 0; i < chain_len - 1; i++) {
            int a = base + 2*i;
            int b = base + 2*i + 1;
            int c = base + 2*i + 2;
            int d = base + 2*i + 3;
            /* Diamond edges (bidirectional = two directed edges) */
            dg_biedge(g, a, b);
            dg_biedge(g, b, c);
            dg_biedge(g, c, d);
            dg_biedge(g, d, a);
            /* Diagonal for Hamiltonicity: a↔c, b↔d */
            dg_biedge(g, a, c);
            dg_biedge(g, b, d);
        }
    }

    /* Chain variables together */
    for(int v = 0; v < n_vars - 1; v++) {
        int end_prev = (v + 1) * per_var - 1;  /* last vertex of var v */
        int start_next = (v + 1) * per_var;     /* first vertex of var v+1 */
        dg_biedge(g, end_prev, start_next);
    }

    /* --- 2. Clause nodes ---
     *
     * Each clause gets a vertex that the HC MUST visit.
     * Connect clause node to the corresponding literal-vertices
     * in the variable diamond chains. */
    int clause_base = var_total;
    for(int i = 0; i < n_clauses; i++) {
        int cn = clause_base + i;
        for(int j = 0; j < 3; j++) {
            int lit = clauses[i][j];
            int var = decode_var(lit);
            int is_neg = decode_neg(lit);
            /* In the i-th diamond of variable var:
             *   a = var*per_var + 2i  (top-left, true path)
             *   c = var*per_var + 2i+2 (bottom-left, false path) */
            int var_node = var * per_var + 2*i + (is_neg ? 2 : 0);
            dg_biedge(g, var_node, cn);
        }
    }

    /* --- 3. Start and End nodes --- */
    int start_node = var_total + n_clauses;
    int end_node   = var_total + n_clauses + 1;
    /* Start connects to first variable's first vertex */
    dg_edge(g, start_node, 0);
    dg_edge(g, 0, start_node);
    /* Last variable's last vertex connects to end */
    dg_edge(g, per_var * n_vars - 1, end_node);
    dg_edge(g, end_node, per_var * n_vars - 1);
    /* Edge from end to start for cycle */
    dg_edge(g, end_node, start_node);
    dg_edge(g, start_node, end_node);

    return g;
}

/* ===== Verification (L4) ===== */

/* Check structural properties of the constructed graph.
   Does NOT solve HC (that's NP-complete), but validates
   that the reduction produces a well-formed graph. */
static int verify_hc_graph_structure(const Digraph *g, int n_vars, int n_clauses) {
    int per_var = 2 * (n_clauses + 1);
    int var_total = per_var * n_vars;
    int V_expected = var_total + n_clauses + 2;

    if(g->n != V_expected) return 0;

    /* Check: every variable diamond has edges */
    for(int v = 0; v < n_vars; v++) {
        int base = v * per_var;
        for(int i = 0; i < n_clauses; i++) {
            int a = base + 2*i;
            int b = base + 2*i + 1;
            /* Check at least one edge exists */
            if(!g->adj[a][b] && !g->adj[b][a]) return 0;
        }
    }

    /* Check: clause nodes are connected to at least one variable node */
    int clause_base = var_total;
    for(int i = 0; i < n_clauses; i++) {
        int cn = clause_base + i;
        int has_connection = 0;
        for(int v = 0; v < g->n && !has_connection; v++)
            if(g->adj[cn][v] || g->adj[v][cn]) has_connection = 1;
        if(!has_connection) return 0;
    }
    return 1;
}

/* ===== Demonstration (L6) ===== */

void three_sat_to_hc_demo(void) {
    printf("\n===== 3SAT → Hamiltonian Cycle Reduction =====\n\n");

    printf("--- Gadget Overview ---\n");
    printf("This is the most elaborate of Karp's 21 reductions.\n\n");
    printf("Core gadget: DIAMOND CHAIN.\n");
    printf("  Each variable xᵢ: m+1 diamonds in a chain.\n");
    printf("  HC traverses left→right diamonds = xᵢ = TRUE.\n");
    printf("  HC zigzags right→left = xᵢ = FALSE.\n\n");
    printf("Clause connection:\n");
    printf("  Each clause Cⱼ has a vertex. HC must visit it.\n");
    printf("  Cⱼ connects to variable diamonds at position j.\n");
    printf("  HC enters Cⱼ from a TRUE literal and returns to chain.\n\n");

    /* Example construction */
    printf("--- Example: 2 vars, 2 clauses ---\n");
    int cls[2][3] = {
        {encode_lit(0,0), encode_lit(1,0), encode_lit(0,0)},
        {encode_lit(0,1), encode_lit(1,1), encode_lit(1,1)}
    };
    Digraph *g = three_sat_to_hc(2, 2, cls);
    int per_var = 2 * (2 + 1);
    int V_expected = per_var * 2 + 2 + 2;
    printf("  Expected vertices: %d (per_var=%d * %d + %d + 2)\n",
           V_expected, per_var, 2, 2);
    printf("  Actual vertices:   %d\n", g->n);
    printf("  Structure valid:   %s\n",
           verify_hc_graph_structure(g, 2, 2) ? "YES ✓" : "NO ✗");

    /* Count edges */
    int edge_count = 0;
    for(int u = 0; u < g->n; u++)
        for(int v = 0; v < g->n; v++)
            edge_count += g->adj[u][v];
    printf("  Directed edges:    %d\n", edge_count);

    dg_free(g);

    /* Significance */
    printf("\n--- Theoretical Significance ---\n");
    printf("HAMPATH/HAMCYCLE was one of the first problems proven\n");
    printf("NP-complete after SAT (Karp #10, #11).\n");
    printf("It showed that even a 'simple' graph problem about\n");
    printf("finding a cycle through all vertices is INTENSELY hard.\n");
    printf("The diamond-gadget technique is a cornerstone of\n");
    printf("reduction design, used extensively in Garey & Johnson.\n");

    printf("\n--- Complexity ---\n");
    printf("Vertices: O(n·m)  [n vars × m clauses]\n");
    printf("Edges:    O(n·m)  [sparse graph]\n");
    printf("Time:     O(n·m)  [polynomial]\n");
    printf("The graph grows LINEARLY with formula size.\n");
}
