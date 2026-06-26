/* space_reach.h — Reachability in Space-Bounded Computation
 *
 * Reachability is THE unifying problem in space complexity:
 * 
 *   STCON (directed s-t connectivity)          → NL-complete
 *   USTCON (undirected s-t connectivity)       → L (Reingold 2008!)
 *   1-STCON (out-degree ≤ 1)                   → RL
 *   STRONG-CONN (strong connectivity)          → NL-complete
 *   CONFIG-PATH (config graph reachability)    → PSPACE-complete
 *
 * The config graph approach: any space-s(n) TM computation can be
 * reduced to PATH in a graph of size 2^{O(s(n))}. This is how
 * Savitch's theorem works: PATH in N-vertex graph takes O(log² N) space.
 *
 * Reference: Arora & Barak §4.2; Wigderson 1992; Jones 1975
 */

#ifndef SPACE_REACH_H
#define SPACE_REACH_H

#include "spaceh.h"

/* ================================================================
 * Graph structures for reachability problems
 * ================================================================ */

/* Simple directed graph suitable for small-space algorithms */
typedef struct {
    int n;              /* number of vertices */
    int m;              /* number of edges */
    char** adj;         /* adjacency matrix (n × n) */
    int* out_degree;    /* out-degree per vertex */
    int* in_degree;     /* in-degree per vertex */
} DirectedGraph;

/* Undirected graph */
typedef struct {
    int n;
    int m;
    char** adj;         /* symmetric adjacency */
    int* degree;
} UndirectedGraph;

/* ================================================================
 * L: Deterministic Logspace
 * ================================================================ */

/* BFS/DFS use O(n) space — NOT logspace. These are reference implementations. */

/* Logspace palindrome: O(log n) space for two pointers */
int reach_l_palindrome(const char* s, int len);

/* Logspace addition: O(log n) space for carry + index */
int reach_l_add(const char* a, const char* b, char* result, int len);

/* Logspace binary counter: O(log n) space */
void reach_l_counter(int max_val);

/* Logspace modular exponentiation: a^b mod m in O(log n) space */
long long reach_l_modpow(long long a, long long b, long long m);

/* Logspace GCD: O(log(a+b)) space */
int reach_l_gcd(int a, int b);

/* ================================================================
 * NL: Nondeterministic Logspace
 * ================================================================ */

/* STCON (directed s-t connectivity): NL-complete [Jones 1975].
 * NL algorithm: guess next vertex nondeterministically.
 * Keeps only current-vertex + step-counter = O(log n) space.
 * Simulated here via enumeration of all possible paths (for demo).
 */
int reach_nl_path(DirectedGraph* g, int s, int t, int max_depth);

/* Non-reachability in NL via Immerman-Szelepcsenyi inductive counting */
int reach_nl_non_path(DirectedGraph* g, int s, int t);

/* Inductive counting: compute number of vertices reachable in ≤k steps */
int reach_nl_count_reachable(DirectedGraph* g, int s, int max_steps);

/* ================================================================
 * Savitch: NSPACE(s) ⊆ DSPACE(s²)
 * ================================================================ */

/* Savitch's recursive PATH algorithm:
 * PATH(u, v, k) = ∃w: PATH(u, w, k/2) ∧ PATH(w, v, k/2)
 * Space: O(log² N) = O(s²) for config graphs. */
int reach_savitch_path(DirectedGraph* g, int u, int v, int steps, int* depth_used);

/* Savitch for config graphs directly */
int reach_savitch_config(int** edges, int m, int s, int t, int steps, int n_vertices);

/* ================================================================
 * Immerman-Szelepcsenyi: NL = coNL
 * ================================================================ */

/* The Immerman-Szelepcsenyi inductive counting algorithm:
 *
 * To verify vertex t is NOT reachable from s in NL:
 *   1. Compute c_k = number of vertices reachable in ≤ k steps
 *   2. For each vertex v, nondeterministically verify v is reachable
 *   3. Keep running count; verify count matches c_k
 *   4. Verify t is NOT among the reachable vertices
 *
 * All steps use O(log n) space. This is LOGSPACE!
 */
int reach_immerman_non_path(DirectedGraph* g, int s, int t);

/* Full inductive counting with verification oracle */
int reach_immerman_count(DirectedGraph* g, int s, int k);

/* ================================================================
 * Reingold's Theorem (2008): USTCON ∈ L
 * ================================================================ */

/* Undirected s-t connectivity is in deterministic logspace!
 * Reingold (STOC 2005, JACM 2008): breakthrough result.
 * Method: zig-zag product to convert the input graph into
 * an expander graph, then explore the expander in logspace.
 *
 * This is a simplified demonstration of the key ideas. */
int reach_reingold_ustcon(UndirectedGraph* g, int s, int t);

/* Zig-zag product basics (for conceptual demonstration) */
void reach_zig_zag_demo(int n);

/* ================================================================
 * RL: Randomized Logspace
 * ================================================================ */

/* 1-STCON: out-degree-1 connectivity is in RL
 * Random walk solves it: O(n³) expected time, O(log n) space */
int reach_rl_degree1(int* next, int n, int s, int t, int max_steps);

/* Random walk for general STCON: O(n³) time, O(log n) space */
int reach_rl_random_walk(DirectedGraph* g, int s, int t, int max_steps, unsigned int seed);

/* ================================================================
 * Utility
 * ================================================================ */

DirectedGraph* dg_create(int n);
void dg_free(DirectedGraph* g);
void dg_add_edge(DirectedGraph* g, int u, int v);
UndirectedGraph* ug_create(int n);
void ug_free(UndirectedGraph* g);
void ug_add_edge(UndirectedGraph* g, int u, int v);

/* Print graph structure */
void dg_print(DirectedGraph* g);
void ug_print(UndirectedGraph* g);

#endif /* SPACE_REACH_H */
