/* circuit.c ? Boolean Circuit: DAG construction, evaluation, circuit-SAT
 *
 * Knowledge:
 *   L1: Boolean circuit definition (DAG of gates)
 *   L2: Non-uniform computation, P/poly concept
 *   L3: Circuit size, depth, gate types (basis {AND,OR,NOT})
 *   L4: Karp-Lipton Theorem connection
 *   L5: Circuit-to-CNF transformation (Tseitin)
 *   L6: Circuit-SAT is NP-complete
 *   L7: Circuit model used in hardware verification
 *   L8: AC0, NC1 hierarchies reference
 *
 * Reference: Arora-Barak SS6.1, Vollmer SS1.1-SS1.3
 */
#include "circuit.h"
#include "types.h"
#include "sat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Circuit* circuit_new(int n_inputs, int max_gates) {
    Circuit* c = (Circuit*)malloc(sizeof(Circuit));
    c->gates = (Gate*)calloc((size_t)max_gates, sizeof(Gate));
    c->n_gates = 0; c->n_inputs = n_inputs;
    c->n_input_gates = 0; c->output_id = -1;
    return c;
}

static int add_gate(Circuit* c, GateType t, int in0, int in1, int var) {
    Gate* g = &c->gates[c->n_gates];
    g->id = c->n_gates; g->type = t; g->var = var;
    g->in[0] = in0; g->in[1] = in1; g->value = -1;
    if (t == GATE_INPUT) { g->nin = 0; c->n_input_gates++; }
    else if (t == GATE_NOT) { g->nin = 1; }
    else { g->nin = 2; }
    return c->n_gates++;
}

int circuit_add_input(Circuit* c, int var) { return add_gate(c, GATE_INPUT, -1, -1, var); }
int circuit_add_and(Circuit* c, int in0, int in1)  { return add_gate(c, GATE_AND, in0, in1, -1); }
int circuit_add_or(Circuit* c, int in0, int in1)   { return add_gate(c, GATE_OR, in0, in1, -1); }
int circuit_add_not(Circuit* c, int in0)           { return add_gate(c, GATE_NOT, in0, -1, -1); }
void circuit_set_output(Circuit* c, int gate_id)   { c->output_id = gate_id; }
int circuit_size(const Circuit* c)                  { return c->n_gates; }

/* Evaluate circuit via topological order on DAG */
int circuit_eval(const Circuit* c, const int* input) {
    for (int i = 0; i < c->n_gates; i++) c->gates[i].value = -1;
    for (int i = 0; i < c->n_gates; i++) {
        Gate* g = &c->gates[i];
        switch (g->type) {
        case GATE_INPUT: g->value = input[g->var]; break;
        case GATE_NOT:   g->value = !c->gates[g->in[0]].value; break;
        case GATE_AND:   g->value = c->gates[g->in[0]].value & c->gates[g->in[1]].value; break;
        case GATE_OR:    g->value = c->gates[g->in[0]].value | c->gates[g->in[1]].value; break;
        }
    }
    return (c->output_id >= 0) ? c->gates[c->output_id].value : -1;
}

/* Depth: longest path from input to output (DP with memoization) */
static int depth_rec(const Circuit* c, int gid, int* memo) {
    if (memo[gid] >= 0) return memo[gid];
    Gate* g = &c->gates[gid];
    if (g->type == GATE_INPUT) return memo[gid] = 0;
    if (g->type == GATE_NOT)   return memo[gid] = 1 + depth_rec(c, g->in[0], memo);
    int d0 = depth_rec(c, g->in[0], memo);
    int d1 = depth_rec(c, g->in[1], memo);
    return memo[gid] = 1 + (d0 > d1 ? d0 : d1);
}

int circuit_depth(const Circuit* c) {
    int* memo = (int*)malloc((size_t)c->n_gates * sizeof(int));
    for (int i = 0; i < c->n_gates; i++) memo[i] = -1;
    int d = depth_rec(c, c->output_id, memo);
    free(memo); return d;
}

/* Circuit-SAT via reduction to CNF then SAT solve */
int circuit_sat(const Circuit* c) {
    CNF* cnf = circuit_to_cnf(c);
    if (!cnf) return -1;
    int* assign = (int*)malloc((size_t)cnf->n_vars * sizeof(int));
    int result = sat_solve(cnf, assign);
    free(assign); cnf_free(cnf);
    return result;
}

/* Tseitin Transformation: Circuit -> CNF, linear size
 * For each gate g with output variable y_g:
 *   AND(a,b): (not y or a) and (not y or b) and (not a or not b or y)
 *   OR(a,b):  (not a or y) and (not b or y) and (a or b or not y)
 *   NOT(a):   (a or y) and (not a or not y)
 *   INPUT(v): y <-> x_v
 */
CNF* circuit_to_cnf(const Circuit* c) {
    int n_g = c->n_gates;
    int n_vars = c->n_inputs + n_g;
    int n_clauses = n_g * 4 + 1;
    CNF* cnf = cnf_new(n_vars, n_clauses);
    for (int i = 0; i < n_g; i++) {
        Gate* g = &c->gates[i];
        int y_var = c->n_inputs + i;
        int y_pos = (y_var << 1) | 0;
        int y_neg = (y_var << 1) | 1;
        switch (g->type) {
        case GATE_INPUT: {
            int xv = g->var, xp = (xv << 1) | 0, xn = (xv << 1) | 1;
            int cl1[] = { y_neg, xp };
            int cl2[] = { y_pos, xn };
            cnf_add(cnf, cl1, 2);
            cnf_add(cnf, cl2, 2);
            break;
        }
        case GATE_NOT: {
            int a = c->n_inputs + g->in[0];
            int ap = (a << 1) | 0, an = (a << 1) | 1;
            int cl1[] = { ap, y_pos }; cnf_add(cnf, cl1, 2);
            int cl2[] = { an, y_neg }; cnf_add(cnf, cl2, 2);
            break;
        }
        case GATE_AND: {
            int a = c->n_inputs + g->in[0], b = c->n_inputs + g->in[1];
            int ap = (a << 1) | 0, bp = (b << 1) | 0;
            int an = (a << 1) | 1, bn = (b << 1) | 1;
            int cl1[] = { y_neg, ap }; cnf_add(cnf, cl1, 2);
            int cl2[] = { y_neg, bp }; cnf_add(cnf, cl2, 2);
            int cl3[] = { an, bn, y_pos }; cnf_add(cnf, cl3, 3);
            break;
        }
        case GATE_OR: {
            int a = c->n_inputs + g->in[0], b = c->n_inputs + g->in[1];
            int ap = (a << 1) | 0, bp = (b << 1) | 0;
            int an = (a << 1) | 1, bn = (b << 1) | 1;
            int cl1[] = { an, y_pos }; cnf_add(cnf, cl1, 2);
            int cl2[] = { bn, y_pos }; cnf_add(cnf, cl2, 2);
            int cl3[] = { ap, bp, y_neg }; cnf_add(cnf, cl3, 3);
            break;
        }
        }
    }
    /* Force output gate = true */
    int out_id = c->n_inputs + c->output_id;
    int out_cl[] = { (out_id << 1) | 0 };
    cnf_add(cnf, out_cl, 1);
    return cnf;
}

/* Circuit from CNF: depth-2 OR-of-ANDs structure */
Circuit* circuit_from_cnf(const CNF* cnf) {
    int n = cnf->n_vars, m = cnf->n_clauses;
    Circuit* c = circuit_new(n, n + m * 3 + 2);
    for (int v = 0; v < n; v++) circuit_add_input(c, v);
    int* clause_ands = (int*)malloc((size_t)m * sizeof(int));
    for (int i = 0; i < m; i++) {
        int prev = -1;
        for (int j = 0; j < cnf->clauses[i].n; j++) {
            int lit = cnf->clauses[i].data[j];
            int var = lit >> 1, sign = lit & 1;
            int lit_node = sign ? circuit_add_not(c, var) : var;
            if (prev < 0) prev = lit_node;
            else prev = circuit_add_or(c, prev, lit_node);
        }
        clause_ands[i] = prev;
    }
    int root = clause_ands[0];
    for (int i = 1; i < m; i++)
        root = circuit_add_and(c, root, clause_ands[i]);
    circuit_set_output(c, root);
    free(clause_ands);
    return c;
}

/* MAJORITY circuit: O(n^2) size, deep */
Circuit* circuit_majority(int n) {
    Circuit* c = circuit_new(n, n * n + 10);
    for (int v = 0; v < n; v++) circuit_add_input(c, v);
    int pair_count = 0;
    int* pairs = (int*)malloc((size_t)(n * n) * sizeof(int));
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            pairs[pair_count++] = circuit_add_and(c, i, j);
    int root = pairs[0];
    for (int p = 1; p < pair_count; p++)
        root = circuit_add_or(c, root, pairs[p]);
    circuit_set_output(c, root);
    free(pairs);
    return c;
}

/* PARITY circuit (XOR tree, depth O(log n), size O(n)) */
Circuit* circuit_parity(int n) {
    Circuit* c = circuit_new(n, 2 * n + 10);
    for (int v = 0; v < n; v++) circuit_add_input(c, v);
    int root = 0;
    for (int v = 1; v < n; v++) {
        int na = circuit_add_not(c, root);
        int nb = circuit_add_not(c, v);
        root = circuit_add_or(c,
            circuit_add_and(c, root, nb),
            circuit_add_and(c, na, v));
    }
    circuit_set_output(c, root);
    return c;
}

void circuit_print(const Circuit* c) {
    printf("Circuit: %d gates, %d inputs, output=%d\n",
           c->n_gates, c->n_inputs, c->output_id);
    for (int i = 0; i < c->n_gates; i++) {
        Gate* g = &c->gates[i];
        printf("  g%d: ", g->id);
        switch (g->type) {
        case GATE_INPUT: printf("INPUT(x%d)\n", g->var); break;
        case GATE_NOT:   printf("NOT(g%d)\n", g->in[0]); break;
        case GATE_AND:   printf("AND(g%d,g%d)\n", g->in[0], g->in[1]); break;
        case GATE_OR:    printf("OR(g%d,g%d)\n", g->in[0], g->in[1]); break;
        }
    }
}

void circuit_free(Circuit* c) { free(c->gates); free(c); }
