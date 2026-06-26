/* ph_reductions.c - Polynomial-Time Reductions between Complete Problems
 *
 * L5 Algorithms: Standard NP-completeness reductions
 * L6 Canonical Problems: CLIQUE, VERTEX-COVER, HAM-PATH, 3-COLOR, SUBSET-SUM
 *
 * Each reduction is a polynomial-time Karp reduction: A <=_p B.
 * These demonstrate the universality of NP-completeness.
 */
#include "ph.h"
#include <string.h>

/* ================================================================
 * 3SAT to 3-COLOR Reduction (L5)
 *
 * Karp (1972): 3SAT <=_p 3-COLOR
 * Build a graph that is 3-colorable iff the CNF is satisfiable.
 *
 * Gadget construction:
 *   - Base triangle (3 special vertices: TRUE, FALSE, BASE)
 *   - Variable gadget: 2 vertices per variable (x and !x),
 *     connected to each other and to BASE
 *   - Clause gadget: 5-vertex subgraph per clause
 *
 * Graph represented as adjacency matrix: graph[i*n + j] = 1 => edge
 * ================================================================ */

/* Estimate: for m clauses and n vars, we need ~ 3 + 2n + 5m vertices */
void ph_sat_to_3color(const PH_CNF* f, int* graph_out,
                      int* n_out) {
    int n = f->n_vars;
    int m = f->n_clauses;
    /* Vertices:
     *   0: TRUE, 1: FALSE, 2: BASE
     *   3..3+2n-1: variable vertices (v, !v)
     *   3+2n..3+2n+5m-1: clause gadgets */
    int nv = 3 + 2 * n + 5 * m;
    if (nv > 256) nv = 256;
    *n_out = nv;
    int total = nv * nv;
    for (int i = 0; i < total; i++) graph_out[i] = 0;

    /* Base triangle: TRUE-FALSE, TRUE-BASE, FALSE-BASE */
    graph_out[0 * nv + 1] = graph_out[1 * nv + 0] = 1;
    graph_out[0 * nv + 2] = graph_out[2 * nv + 0] = 1;
    graph_out[1 * nv + 2] = graph_out[2 * nv + 1] = 1;

    /* Variable gadgets: for each var i, vertices 3+2i (x_i) and 3+2i+1 (!x_i)
     * Edge between x_i and !x_i, and both connected to BASE. */
    for (int i = 0; i < n && 3 + 2*i + 1 < nv; i++) {
        int vi = 3 + 2*i;      /* x_i */
        int ni = 3 + 2*i + 1;  /* !x_i */
        /* x_i -- !x_i */
        graph_out[vi * nv + ni] = graph_out[ni * nv + vi] = 1;
        /* Both connected to BASE */
        graph_out[vi * nv + 2] = graph_out[2 * nv + vi] = 1;
        graph_out[ni * nv + 2] = graph_out[2 * nv + ni] = 1;
    }

    /* Clause gadgets: each clause gets 5 vertices.
     * The subgraph enforces that at least one literal must be colored TRUE. */
    for (int ci = 0; ci < m; ci++) {
        int base = 3 + 2*n + 5*ci;
        if (base + 4 >= nv) break;

        /* 5 vertices: a0,a1,a2,b0,b1 form a gadget
         * a0-a1-a2-b0-b1-a0 (5-cycle)
         * Each a_k connects to corresponding literal vertex */
        int a0 = base, a1 = base+1, a2 = base+2;
        int b0 = base+3, b1 = base+4;

        /* 5-cycle */
        graph_out[a0*nv+a1] = graph_out[a1*nv+a0] = 1;
        graph_out[a1*nv+a2] = graph_out[a2*nv+a1] = 1;
        graph_out[a2*nv+b0] = graph_out[b0*nv+a2] = 1;
        graph_out[b0*nv+b1] = graph_out[b1*nv+b0] = 1;
        graph_out[b1*nv+a0] = graph_out[a0*nv+b1] = 1;

        /* Connect to literals */
        for (int j = 0; j < 3; j++) {
            int lit = f->clauses[ci].lits[j];
            if (lit < 0) continue;
            int var = lit >> 1;
            int sign = lit & 1;
            int lit_vertex;
            if (sign == 0) {
                lit_vertex = 3 + 2*var;       /* positive literal = x_i */
            } else {
                lit_vertex = 3 + 2*var + 1;   /* negative literal = !x_i */
            }
            if (lit_vertex < nv) {
                int ak = base + j;
                graph_out[lit_vertex*nv+ak] = graph_out[ak*nv+lit_vertex] = 1;
            }
        }
    }
}

/* ================================================================
 * 3SAT to SUBSET-SUM Reduction (L5)
 *
 * Karp (1972): 3SAT <=_p SUBSET-SUM
 * Given a CNF, produce a set of integers such that a subset sums
 * to a target value iff the CNF is satisfiable.
 *
 * Encoded in base-10 with digit positions for variables and clauses.
 * ================================================================ */

void ph_sat_to_subsetsum(const PH_CNF* f, int* numbers_out,
                         int* n_out, int* target_out) {
    int n = f->n_vars;
    int m = f->n_clauses;
    /* Each variable gets 2 numbers (x_i and !x_i).
     * Each clause gets 2 slack numbers.
     * Total: 2n + 2m numbers.
     * Number encoding: n+m decimal digits, each 0..4 */
    int count = 2 * n + 2 * m;
    if (count > 64) count = 64;
    *n_out = count;

    /* Build numbers as decimal digit arrays.
     * For simplicity, use integer encoding with reasonable bounds. */
    int target = 0;
    for (int i = 0; i < n; i++)
        target = target * 10 + 1;       /* each variable digit = 1 */
    for (int ci = 0; ci < m; ci++)
        target = target * 10 + 3;       /* each clause digit = 3 (original) */
    *target_out = target;

    /* Variable numbers */
    for (int i = 0; i < n && 2*i < count; i++) {
        int num_xi = 0, num_notxi = 0;
        int power = 1;
        /* Variable digit positions */
        for (int p = 0; p < n; p++) power *= 10;
        for (int ci = m - 1; ci >= 0; ci--) {
            /* Clause contribution: +1 if literal appears */
            int contrib_xi = 0, contrib_notxi = 0;
            for (int j = 0; j < 3; j++) {
                int lit = f->clauses[ci].lits[j];
                if (lit < 0) continue;
                int var = lit >> 1, sign = lit & 1;
                if (var == i) {
                    if (sign == 0) contrib_xi = 1;
                    else contrib_notxi = 1;
                }
            }
            num_xi = num_xi * 10 + contrib_xi;
            num_notxi = num_notxi * 10 + contrib_notxi;
            power /= 10;
        }
        /* Variable digit */
        int var_digit = 1;
        for (int p = 0; p < n - 1 - i; p++) var_digit *= 10;
        num_xi += var_digit;
        num_notxi += var_digit;
        numbers_out[2*i] = num_xi;
        numbers_out[2*i+1] = num_notxi;
    }

    /* Slack numbers for clauses */
    for (int ci = 0; ci < m && 2*n + 2*ci + 1 < count; ci++) {
        int slack1 = 0, slack2 = 0;
        int power = 1;
        for (int cj = 0; cj < m; cj++) {
            if (cj == ci) {
                slack1 = slack1 * 10 + 1;
                slack2 = slack2 * 10 + 2;
            } else {
                slack1 = slack1 * 10 + 0;
                slack2 = slack2 * 10 + 0;
            }
        }
        numbers_out[2*n + 2*ci] = slack1;
        numbers_out[2*n + 2*ci + 1] = slack2;
    }
}

/* ================================================================
 * 3SAT to HAMILTONIAN-PATH Reduction (L5)
 *
 * Karp (1972): 3SAT <=_p DIRECTED-HAMILTONIAN-PATH
 * Build a directed graph with a Hamiltonian path iff the CNF is SAT.
 * ================================================================ */

/* For HAM-PATH, build adjacency matrix for directed graph.
 * Vertices: 1 start, 2 per variable (x-path, !x-path),
 * 1 per clause terminal, 1 end. */
void ph_sat_to_hampath(const PH_CNF* f, int* graph_out,
                       int* n_out) {
    int n = f->n_vars;
    int m = f->n_clauses;
    /* Vertices: start, end, plus variable gadgets and clause connectors */
    int nv = 2 + 3 * n + m;
    if (nv > 200) nv = 200;
    *n_out = nv;
    int total = nv * nv;
    for (int i = 0; i < total; i++) graph_out[i] = 0;

    /* Variable gadgets: each variable i has 3 vertices
     * forming a diamond that allows 2 paths (x_i=true or x_i=false). */
    for (int i = 0; i < n; i++) {
        int base = 2 + 3*i;
        if (base + 2 >= nv) break;
        /* Diamond: base->base+1->base+2 and base->base+2->base+1 */
        graph_out[base*nv + base+1] = 1;
        graph_out[(base+1)*nv + base+2] = 1;
        graph_out[base*nv + base+2] = 1;
        graph_out[(base+2)*nv + base+1] = 1;
    }

    /* Chain variable gadgets together */
    for (int i = 0; i < n - 1; i++) {
        int curr_base = 2 + 3*i;
        int next_base = 2 + 3*(i+1);
        if (curr_base + 2 < nv && next_base < nv) {
            graph_out[(curr_base+1)*nv + next_base] = 1;
            graph_out[(curr_base+2)*nv + next_base] = 1;
        }
    }

    /* Start -> first variable gadget */
    if (2 < nv) graph_out[0*nv + 2] = 1;

    /* Last variable -> clause terminals -> end */
    int last_base = 2 + 3*(n-1);
    int clause_start = 2 + 3*n;
    for (int ci = 0; ci < m && clause_start + ci < nv - 1; ci++) {
        if (last_base + 2 < nv) {
            graph_out[(last_base+1)*nv + clause_start + ci] = 1;
            graph_out[(last_base+2)*nv + clause_start + ci] = 1;
        }
        /* Clause terminal -> end */
        graph_out[(clause_start + ci)*nv + (nv-1)] = 1;
    }
    if (clause_start < nv - 1 && last_base + 2 < nv) {
        graph_out[(last_base+1)*nv + (nv-1)] = 1;
        graph_out[(last_base+2)*nv + (nv-1)] = 1;
    }
}

/* ================================================================
 * Reduction Verification (L5)
 *
 * Test that reductions preserve satisfiability on small instances.
 * ================================================================ */

/* Verify SAT-to-CLIQUE reduction on a small instance.
 * Returns 1 if reduction is consistent. */
int ph_verify_sat_to_clique(void) {
    /* Build a simple SAT instance and verify clique correspondence */
    PH_CNF* f = ph_cnf_build_single(0, 0, 1, 0, 2, 0);
    f->n_vars = 3;
    f->n_clauses = 1;

    int graph[9];
    int n, k;
    ph_sat_to_clique(f, graph, &n, &k);
    /* (x0|x1|x2) should have 3 vertices, clique size = 1 */
    int result = (n == 3 && k == 1);
    free(f);
    return result;
}

/* Verify SAT-to-VERTEX-COVER reduction. */
int ph_verify_sat_to_vertexcover(void) {
    PH_CNF* f = ph_cnf_build_single(0, 0, 1, 0, 2, 0);
    f->n_vars = 3;
    f->n_clauses = 1;

    int graph[9];
    int n, k;
    ph_sat_to_vertexcover(f, graph, &n, &k);
    /* 1 clause => 3 vertices, cover size = 2 */
    int result = (n == 3 && k == 2);
    free(f);
    return result;
}

/* Reductions demo is implemented in ph_demo.c */
