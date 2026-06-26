/* graph_np.h — NP-complete graph problem solvers
 *
 * Uses the Graph type defined in np_catalog.h (no redefinition).
 * Provides exponential-time solvers for canonical NP-complete graph problems.
 */
#ifndef GRAPH_NP_H
#define GRAPH_NP_H
#include "np_catalog.h"  /* provides: Graph, g_new(), g_free() */

/* CLIQUE: find a k-clique via branch-and-bound. Returns 1 if found. */
int clique_solve(const Graph* g, int k, int* result, int max_k);

/* Build complement graph (edge (i,j) iff not an edge in original). */
Graph* graph_complement(const Graph* g);

/* VERTEX COVER via CLIQUE reduction: VC(G,k) iff Clique(comp(G), n-k). */
int vertex_cover_via_clique(const Graph* g, int k);

/* GRAPH COLORING: can G be colored with at most k colors? Backtracking. */
int graph_k_colorable(const Graph* g, int k);

/* INDEPENDENT SET via CLIQUE reduction: IS(G,k) iff Clique(comp(G), k). */
int independent_set_solve(const Graph* g, int k);

/* SUBGRAPH ISOMORPHISM: Does G contain a subgraph isomorphic to H? */
int subgraph_isomorphism(const Graph* g, const Graph* h);

/* Build the "Karp graph": the standard 3SAT → CLIQUE reduction graph. */
Graph* reduce_3sat_to_clique_graph(int n_vars, int n_clauses, int clauses[][3]);

/* Solve 3SAT by reducing to CLIQUE and running the clique solver. */
int sat_via_clique(int n_vars, int n_clauses, int clauses[][3]);

#endif
