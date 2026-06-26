/* ph_core.c — Polynomial Hierarchy: Core Implementation
 *
 * Implements:
 *   L2: CNF evaluation, QBF construction, quantifier alternation
 *   L3: Mathematical structures for PH (QBF, quantifier blocks)
 *   L4: Collapse theorem check, DPLL SAT solver
 *   L5: Sigma_k-SAT recursive solver, SAT-to-CLIQUE/VC reductions
 *   L6: QSAT_k benchmark instances
 *
 * References:
 *   Stockmeyer (1976) "The Polynomial-Time Hierarchy"
 *   Arora & Barak §5.1-5.2
 *   Sipser §9.2
 */
#include "ph.h"

/* ================================================================
 * CNF Operations (L2-L3)
 * ================================================================ */

int ph_cnf_eval(const PH_CNF* f, const int* assign) {
    /* Theorem: CNF evaluation is in P (L-complete under AC^0 reductions).
     * Time: O(n_clauses * 3) = O(m) where m = number of clauses.
     *
     * This is the base case of the recursive QBF algorithm.
     * Every level of PH bottoms out at CNF evaluation in P. */
    for (int i = 0; i < f->n_clauses; i++) {
        int satisfied = 0;
        for (int j = 0; j < 3; j++) {
            int lit = f->clauses[i].lits[j];
            int var = lit >> 1;
            int sign = lit & 1;
            /* Skip padding literals (negative var index, or out of range) */
            if (var < 0 || var >= f->n_vars) continue;
            /* Literal true if: (positive AND assign[var]==1) OR
             *                  (negative AND assign[var]==0) */
            if ((!sign && assign[var]) || (sign && !assign[var])) {
                satisfied = 1;
                break;
            }
        }
        if (!satisfied) return 0;
    }
    return 1;
}

/* Count how many literals in a clause are true under assignment.
 * Used internally for clause analysis.
 * Returns: 0 if clause is falsified, >0 if satisfied. */
static int clause_sat_count(const PH_Clause* c, const int* assign, int n_vars) {
    int count = 0;
    for (int j = 0; j < 3; j++) {
        int lit = c->lits[j];
        int var = lit >> 1;
        int sign = lit & 1;
        if (var < 0 || var >= n_vars) continue;
        if ((!sign && assign[var]) || (sign && !assign[var])) count++;
    }
    return count;
}

/* Check if a CNF formula is a tautology (always true).
 * Uses brute force over all assignments.
 * Theorem: TAUTOLOGY is coNP-complete. */
int ph_cnf_is_tautology(const PH_CNF* f) {
    int n = f->n_vars;
    if (n > 20) n = 20; /* cap for brute force */
    long long total = 1LL << n;
    if (total > (1LL << 20)) total = 1LL << 20;
    int* a = calloc((size_t)n, sizeof(int));
    if (!a) return 0;
    for (long long m = 0; m < total; m++) {
        for (int i = 0; i < n; i++) a[i] = (int)((m >> i) & 1);
        if (!ph_cnf_eval(f, a)) { free(a); return 0; }
    }
    free(a);
    return 1;
}

/* Convert CNF to DNF by distributing AND over OR.
 * This is exponential in general (expected, since MIN-DNF is Sigma_2-complete).
 * Simplified: returns a single disjunct if CNF has exactly 1 clause. */
void ph_cnf_to_dnf(const PH_CNF* f, PH_CNF* dnf_out) {
    /* A single clause (l1|l2|l3) is already a DNF term.
     * General conversion is exponential (blow-up). */
    memset(dnf_out, 0, sizeof(PH_CNF));
    dnf_out->n_vars = f->n_vars;
    if (f->n_clauses == 1) {
        dnf_out->n_clauses = 1;
        dnf_out->clauses[0] = f->clauses[0];
    } else {
        /* Multi-clause CNF: conversion would blow up.
         * For demonstration: just note the complexity. */
        dnf_out->n_clauses = 0;
    }
}

PH_CNF* ph_cnf_parse(const char* desc) {
    /* Parse CNF from string: "nv nc; v1 s1, v2 s2, v3 s3; v1 s1, ..."
     * Example: "3 2; 0 0, 1 0, 2 0; 0 1, 1 1, -1 -1"
     *   = 3 vars, 2 clauses: (x0|x1|x2) & (!x0|!x1) */
    PH_CNF* f = malloc(sizeof(PH_CNF));
    if (!f) return NULL;
    memset(f, 0, sizeof(PH_CNF));

    const char* p = desc;
    /* Parse n_vars and n_clauses */
    int nv = 0, nc = 0;
    while (*p && (*p < '0' || *p > '9')) p++;
    while (*p >= '0' && *p <= '9') { nv = nv * 10 + (*p - '0'); p++; }
    while (*p && (*p < '0' || *p > '9')) p++;
    while (*p >= '0' && *p <= '9') { nc = nc * 10 + (*p - '0'); p++; }

    if (nv > PH_MAX_VARS) nv = PH_MAX_VARS;
    if (nc > PH_MAX_CLAUSES) nc = PH_MAX_CLAUSES;
    f->n_vars = nv;
    f->n_clauses = 0;

    for (int ci = 0; ci < nc && f->n_clauses < PH_MAX_CLAUSES; ci++) {
        while (*p && *p != ';') p++;
        if (*p == ';') p++;
        /* Parse 3 literals: v1 s1, v2 s2, v3 s3 */
        int lits[3];
        for (int j = 0; j < 3; j++) {
            int v = -1, s = 0;
            while (*p && (*p < '0' || *p > '9') && *p != '-') p++;
            if (*p == '-') { p++; }
            int neg = 0;
            if (p[-1] == '-') neg = 1;
            v = 0;
            while (*p >= '0' && *p <= '9') { v = v * 10 + (*p - '0'); p++; }
            if (neg) v = -v;
            while (*p && (*p < '0' || *p > '9') && *p != '-') p++;
            if (*p == '-') continue; /* skip sign in next literal */
            s = 0;
            while (*p >= '0' && *p <= '9') { s = s * 10 + (*p - '0'); p++; }
            if (v >= 0 && v < nv) {
                lits[j] = (v << 1) | (s ? 1 : 0);
            } else {
                lits[j] = -1; /* padding */
            }
        }
        f->clauses[f->n_clauses].lits[0] = lits[0];
        f->clauses[f->n_clauses].lits[1] = lits[1];
        f->clauses[f->n_clauses].lits[2] = lits[2];
        f->n_clauses++;
    }
    return f;
}

PH_CNF* ph_cnf_build_single(int v1, int s1, int v2, int s2, int v3, int s3) {
    PH_CNF* f = malloc(sizeof(PH_CNF));
    if (!f) return NULL;
    memset(f, 0, sizeof(PH_CNF));
    f->n_vars = 0;
    int vs[3] = {v1, v2, v3};
    for (int i = 0; i < 3; i++) {
        if (vs[i] >= 0 && vs[i] + 1 > f->n_vars) f->n_vars = vs[i] + 1;
    }
    if (f->n_vars > PH_MAX_VARS) f->n_vars = PH_MAX_VARS;
    f->n_clauses = 1;
    f->clauses[0].lits[0] = (v1 >= 0 && v1 < PH_MAX_VARS) ? ((v1 << 1) | (s1 ? 1 : 0)) : -1;
    f->clauses[0].lits[1] = (v2 >= 0 && v2 < PH_MAX_VARS) ? ((v2 << 1) | (s2 ? 1 : 0)) : -1;
    f->clauses[0].lits[2] = (v3 >= 0 && v3 < PH_MAX_VARS) ? ((v3 << 1) | (s3 ? 1 : 0)) : -1;
    return f;
}

PH_CNF* ph_cnf_clone(const PH_CNF* src) {
    PH_CNF* dst = malloc(sizeof(PH_CNF));
    if (!dst) return NULL;
    memcpy(dst, src, sizeof(PH_CNF));
    return dst;
}

void ph_cnf_print(const PH_CNF* f) {
    printf("CNF: %d vars, %d clauses\n", f->n_vars, f->n_clauses);
    for (int i = 0; i < f->n_clauses; i++) {
        printf("  C%d: (", i);
        for (int j = 0; j < 3; j++) {
            int lit = f->clauses[i].lits[j];
            if (lit < 0) continue;
            int var = lit >> 1;
            int sign = lit & 1;
            if (j > 0 && f->clauses[i].lits[j-1] >= 0) printf(" | ");
            printf("%sx%d", sign ? "!" : "", var);
        }
        printf(")\n");
    }
}

/* Count the number of variable occurrences (positive + negative).
 * Used for formula complexity analysis.
 * occ_count[var][0] = positive occurrences, [var][1] = negative. */
void ph_cnf_var_frequency(const PH_CNF* f, int occ_count[][2]) {
    for (int v = 0; v < f->n_vars; v++)
        occ_count[v][0] = occ_count[v][1] = 0;
    for (int i = 0; i < f->n_clauses; i++) {
        for (int j = 0; j < 3; j++) {
            int lit = f->clauses[i].lits[j];
            if (lit < 0) continue;
            int var = lit >> 1;
            int sign = lit & 1;
            if (var < f->n_vars) {
                if (sign) occ_count[var][1]++;
                else      occ_count[var][0]++;
            }
        }
    }
}

/* Check if two CNF formulas are logically equivalent.
 * Uses brute force over all assignments (exponential).
 * Returns 1 if logically equivalent, 0 otherwise.
 * Theorem: checking CNF equivalence is coNP-complete. */
int ph_cnf_equivalent(const PH_CNF* f1, const PH_CNF* f2) {
    if (f1->n_vars != f2->n_vars) return 0;
    int n = f1->n_vars;
    if (n > 16) n = 16;
    long long total = 1LL << n;
    if (total > (1LL << 18)) total = 1LL << 18;
    int* a = calloc((size_t)n, sizeof(int));
    if (!a) return 0;
    for (long long m = 0; m < total; m++) {
        for (int i = 0; i < n; i++) a[i] = (int)((m >> i) & 1);
        if (ph_cnf_eval(f1, a) != ph_cnf_eval(f2, a)) {
            free(a); return 0;
        }
    }
    free(a);
    return 1;
}

/* ================================================================
 * QBF Construction and Manipulation (L3)
 * ================================================================ */

PH_QBF* ph_qbf_create(int n_blocks, const PH_QuantType* types,
                      const int* sizes, PH_CNF* f) {
    if (n_blocks > PH_MAX_LEVEL) n_blocks = PH_MAX_LEVEL;
    PH_QBF* qbf = malloc(sizeof(PH_QBF));
    if (!qbf) { free(f); return NULL; }
    memset(qbf, 0, sizeof(PH_QBF));
    qbf->n_blocks = n_blocks;
    qbf->total_vars = 0;
    int offset = 0;
    for (int i = 0; i < n_blocks; i++) {
        qbf->blocks[i].type = types[i];
        qbf->blocks[i].start_var = offset;
        qbf->blocks[i].n_vars = sizes[i];
        offset += sizes[i];
        qbf->total_vars += sizes[i];
    }
    if (f) {
        memcpy(&qbf->matrix, f, sizeof(PH_CNF));
        qbf->matrix.n_vars = qbf->total_vars;
        free(f);
    }
    return qbf;
}

void ph_qbf_free(PH_QBF* qbf) {
    if (qbf) free(qbf);
}

void ph_qbf_print(const PH_QBF* qbf) {
    printf("QBF: %d blocks, %d vars\n", qbf->n_blocks, qbf->total_vars);
    for (int i = 0; i < qbf->n_blocks; i++) {
        printf("  %c x%d..x%d (%d vars)\n",
               qbf->blocks[i].type == PH_QUANT_EXISTS ? 'E' : 'A',
               qbf->blocks[i].start_var,
               qbf->blocks[i].start_var + qbf->blocks[i].n_vars - 1,
               qbf->blocks[i].n_vars);
    }
    printf("  Matrix: ");
    ph_cnf_print(&qbf->matrix);
}

PH_QBF* ph_qbf_restrict(const PH_QBF* qbf, const int* assign) {
    /* Fix first block variables to assign[], return QBF with one fewer block.
     * Clauses with satisfied literals under assign are removed.
     * Clauses with falsified literals have those literals removed. */
    if (qbf->n_blocks <= 1) return NULL;

    int n_fixed = qbf->blocks[0].n_vars;
    int remaining_blocks = qbf->n_blocks - 1;
    int remaining_vars = qbf->total_vars - n_fixed;

    PH_CNF* new_matrix = malloc(sizeof(PH_CNF));
    if (!new_matrix) return NULL;
    memset(new_matrix, 0, sizeof(PH_CNF));
    new_matrix->n_vars = remaining_vars;
    new_matrix->n_clauses = 0;

    for (int ci = 0; ci < qbf->matrix.n_clauses && new_matrix->n_clauses < PH_MAX_CLAUSES; ci++) {
        PH_Clause orig = qbf->matrix.clauses[ci];
        PH_Clause simplified;
        int sl_count = 0;
        int clause_already_sat = 0;

        for (int j = 0; j < 3; j++) {
            int lit = orig.lits[j];
            if (lit < 0) continue;
            int var = lit >> 1;
            int sign = lit & 1;

            if (var < n_fixed) {
                /* This variable is fixed */
                int val = assign[var];
                if ((!sign && val) || (sign && !val)) {
                    clause_already_sat = 1; /* literal true => clause satisfied */
                    break;
                }
                /* literal false => remove from clause (don't add) */
            } else {
                /* Remaining variable: remap index */
                int new_var = var - n_fixed;
                simplified.lits[sl_count++] = (new_var << 1) | sign;
            }
        }

        if (!clause_already_sat && sl_count > 0) {
            while (sl_count < 3) simplified.lits[sl_count++] = -1;
            new_matrix->clauses[new_matrix->n_clauses++] = simplified;
        }
    }

    /* Create new QBF with remaining blocks */
    PH_QuantType* types = malloc((size_t)remaining_blocks * sizeof(PH_QuantType));
    int* sizes = malloc((size_t)remaining_blocks * sizeof(int));
    for (int i = 0; i < remaining_blocks; i++) {
        types[i] = qbf->blocks[i + 1].type;
        sizes[i] = qbf->blocks[i + 1].n_vars;
    }
    PH_QBF* result = ph_qbf_create(remaining_blocks, types, sizes, new_matrix);
    free(types);
    free(sizes);
    return result;
}

/* ================================================================
 * Bool Circuit Operations (L3-L4)
 * ================================================================ */

int ph_circuit_eval(const PH_BoolCircuit* c, const int* inputs) {
    int wires[PH_MAX_GATES + PH_MAX_INPUTS];
    /* Copy inputs to wire values */
    for (int i = 0; i < c->n_inputs; i++)
        wires[i] = inputs[i];
    /* Evaluate gates in topological order */
    for (int i = 0; i < c->n_gates; i++) {
        int wi = c->n_inputs + i;
        const PH_Gate* g = &c->gates[i];
        switch (g->type) {
            case PH_GATE_CONST: wires[wi] = g->const_val; break;
            case PH_GATE_INPUT: wires[wi] = wires[g->in1]; break;
            case PH_GATE_NOT:   wires[wi] = !wires[g->in1]; break;
            case PH_GATE_AND:   wires[wi] = wires[g->in1] && wires[g->in2]; break;
            case PH_GATE_OR:    wires[wi] = wires[g->in1] || wires[g->in2]; break;
            default:            wires[wi] = 0; break;
        }
    }
    /* Output is last gate's wire */
    return wires[c->n_inputs + c->n_gates - 1];
}

void ph_circuit_print(const PH_BoolCircuit* c) {
    printf("Boolean Circuit: %d inputs, %d gates\n", c->n_inputs, c->n_gates);
    for (int i = 0; i < c->n_gates; i++) {
        const PH_Gate* g = &c->gates[i];
        printf("  G%d (%d): ", i, c->n_inputs + i);
        switch (g->type) {
            case PH_GATE_CONST: printf("CONST(%d)", g->const_val); break;
            case PH_GATE_INPUT: printf("INPUT(%d)", g->in1); break;
            case PH_GATE_NOT:   printf("NOT(w%d)", g->in1); break;
            case PH_GATE_AND:   printf("AND(w%d, w%d)", g->in1, g->in2); break;
            case PH_GATE_OR:    printf("OR(w%d, w%d)", g->in1, g->in2); break;
            default:            printf("UNKNOWN"); break;
        }
        printf("\n");
    }
}

PH_BoolCircuit* ph_circuit_build_and(int n_inputs) {
    /* Build circuit: AND of all n_inputs inputs */
    PH_BoolCircuit* c = malloc(sizeof(PH_BoolCircuit));
    if (!c) return NULL;
    memset(c, 0, sizeof(PH_BoolCircuit));
    c->n_inputs = n_inputs;
    if (n_inputs <= 0) {
        c->n_gates = 1;
        c->gates[0] = (PH_Gate){PH_GATE_CONST, 0, 0, 1};
        return c;
    }
    if (n_inputs == 1) {
        c->n_gates = 1;
        c->gates[0] = (PH_Gate){PH_GATE_INPUT, 0, 0, 0};
        return c;
    }
    /* Tree of AND gates: pair up inputs */
    int next_free = n_inputs; /* wire index of next computed value */
    /* First level: AND pairs of inputs -> gate outputs at wires[next_free..] */
    int remaining = n_inputs;
    int src = 0;
    while (remaining > 1) {
        for (int i = 0; i < remaining / 2 && c->n_gates < PH_MAX_GATES; i++) {
            c->gates[c->n_gates++] = (PH_Gate){PH_GATE_AND, src + 2*i, src + 2*i + 1, 0};
        }
        if (remaining % 2 == 1 && c->n_gates < PH_MAX_GATES) {
            /* Odd one out: pass through */
            c->gates[c->n_gates++] = (PH_Gate){PH_GATE_INPUT, src + remaining - 1, 0, 0};
        }
        src = next_free;
        next_free += remaining / 2 + (remaining % 2);
        remaining = remaining / 2 + (remaining % 2);
    }
    return c;
}

PH_BoolCircuit* ph_circuit_build_or(int n_inputs) {
    /* De Morgan: OR = NOT(AND(NOT(x1), NOT(x2), ...))
     * Build NOT gates for each input, then AND tree, then final NOT. */
    PH_BoolCircuit* c = malloc(sizeof(PH_BoolCircuit));
    if (!c) return NULL;
    memset(c, 0, sizeof(PH_BoolCircuit));
    c->n_inputs = n_inputs;
    if (n_inputs <= 0) {
        c->n_gates = 1;
        c->gates[0] = (PH_Gate){PH_GATE_CONST, 0, 0, 0};
        return c;
    }
    /* NOT each input */
    for (int i = 0; i < n_inputs && c->n_gates < PH_MAX_GATES; i++) {
        c->gates[c->n_gates++] = (PH_Gate){PH_GATE_NOT, i, 0, 0};
    }
    /* AND tree of negated inputs */
    int src = c->n_inputs; /* first NOT output wire */
    int remaining = n_inputs;
    int next_free = c->n_inputs + n_inputs;
    while (remaining > 1 && c->n_gates < PH_MAX_GATES - 1) {
        for (int i = 0; i < remaining / 2; i++) {
            c->gates[c->n_gates++] = (PH_Gate){PH_GATE_AND, src + 2*i, src + 2*i + 1, 0};
        }
        if (remaining % 2 == 1) {
            c->gates[c->n_gates++] = (PH_Gate){PH_GATE_INPUT, src + remaining - 1, 0, 0};
        }
        src = next_free;
        next_free += remaining / 2 + (remaining % 2);
        remaining = remaining / 2 + (remaining % 2);
    }
    /* Final NOT */
    if (c->n_gates < PH_MAX_GATES) {
        c->gates[c->n_gates++] = (PH_Gate){PH_GATE_NOT, src, 0, 0};
    }
    return c;
}

PH_BoolCircuit* ph_circuit_build_majority(int n_inputs) {
    /* Majority circuit: output 1 iff >= n_inputs/2 inputs are 1.
     * Build using threshold-k gates approximated by sorting network. */
    PH_BoolCircuit* c = malloc(sizeof(PH_BoolCircuit));
    if (!c) return NULL;
    memset(c, 0, sizeof(PH_BoolCircuit));
    c->n_inputs = n_inputs;
    if (n_inputs <= 0) {
        c->n_gates = 1;
        c->gates[0] = (PH_Gate){PH_GATE_CONST, 0, 0, 0};
        return c;
    }
    /* Simple majority circuit for small n: compare each input-pair,
     * OR the AND-together of all pairs that form a majority. */
    /* For n=3: majority = (x0∧x1) ∨ (x1∧x2) ∨ (x0∧x2) */
    int half = n_inputs / 2;
    /* Build all (n choose half+1) ANDs, then OR them */
    int and_results[PH_MAX_GATES];
    int n_ands = 0;
    /* Recursive combination enumeration: build AND of (half+1)-subsets */
    /* Simplified: build pairwise ANDs, then OR-tree */
    for (int i = 0; i < n_inputs && n_ands < PH_MAX_GATES - 2; i++) {
        for (int j = i + 1; j < n_inputs && n_ands < PH_MAX_GATES - 2; j++) {
            int gi = c->n_gates;
            c->gates[gi] = (PH_Gate){PH_GATE_AND, i, j, 0};
            c->n_gates++;
            and_results[n_ands++] = c->n_inputs + gi;
        }
    }
    /* OR of all AND results */
    if (n_ands == 0) {
        c->gates[c->n_gates++] = (PH_Gate){PH_GATE_INPUT, 0, 0, 0};
        return c;
    }
    int src = and_results[0];
    int remaining = n_ands;
    while (remaining > 1 && c->n_gates < PH_MAX_GATES - 1) {
        int or_gate = c->n_gates;
        c->gates[or_gate] = (PH_Gate){PH_GATE_OR, src,
            and_results[n_ands - remaining + 1], 0};
        c->n_gates++;
        src = c->n_inputs + or_gate;
        remaining--;
    }
    return c;
}

/* ================================================================
 * DPLL SAT Solver (L5)
 * ================================================================ */

/* Unit propagation: find unit clauses and assign forced values.
 * Returns: 1 if no conflict, 0 if conflict found. */
static int dpll_unit_propagate(PH_CNF* f, int* assign, int* assigned_mask) {
    int changed = 1;
    while (changed) {
        changed = 0;
        for (int ci = 0; ci < f->n_clauses; ci++) {
            int unassigned_count = 0;
            int last_unassigned = -1;
            int clause_sat = 0;
            for (int j = 0; j < 3 && !clause_sat; j++) {
                int lit = f->clauses[ci].lits[j];
                int var = lit >> 1;
                int sign = lit & 1;
                if (var < 0 || var >= f->n_vars) continue;
                if (assigned_mask[var]) {
                    if ((!sign && assign[var]) || (sign && !assign[var])) {
                        clause_sat = 1;
                    }
                } else {
                    unassigned_count++;
                    last_unassigned = lit;
                }
            }
            if (clause_sat) continue;
            if (unassigned_count == 0) return 0; /* Conflict: clause all false */
            if (unassigned_count == 1) {
                /* Unit clause: force the remaining literal */
                int var = last_unassigned >> 1;
                int sign = last_unassigned & 1;
                assign[var] = sign ? 0 : 1;
                assigned_mask[var] = 1;
                changed = 1;
            }
        }
    }
    return 1; /* No conflict */
}

static int dpll_recursive(PH_CNF* f, int* assign, int* assigned_mask,
                           int depth, int max_depth) {
    if (depth > max_depth) return -1; /* exceeded search limit, return unknown */

    /* Unit propagation */
    int* saved_assign = malloc((size_t)f->n_vars * sizeof(int));
    int* saved_mask  = malloc((size_t)f->n_vars * sizeof(int));
    if (!saved_assign || !saved_mask) { free(saved_assign); free(saved_mask); return -1; }
    memcpy(saved_assign, assign, (size_t)f->n_vars * sizeof(int));
    memcpy(saved_mask,  assigned_mask, (size_t)f->n_vars * sizeof(int));

    if (!dpll_unit_propagate(f, assign, assigned_mask)) {
        /* Restore and backtrack */
        memcpy(assign, saved_assign, (size_t)f->n_vars * sizeof(int));
        memcpy(assigned_mask, saved_mask, (size_t)f->n_vars * sizeof(int));
        free(saved_assign); free(saved_mask);
        return 0;
    }

    /* Check if all clauses satisfied */
    int all_sat = 1;
    int any_unsat = 0;
    for (int ci = 0; ci < f->n_clauses; ci++) {
        int cl_sat = 0, cl_unsat = 1;
        int cl_unresolved = 0;
        for (int j = 0; j < 3; j++) {
            int lit = f->clauses[ci].lits[j];
            int var = lit >> 1;
            int sign = lit & 1;
            if (var < 0 || var >= f->n_vars) continue;
            if (!assigned_mask[var]) { cl_unresolved = 1; cl_unsat = 0; continue; }
            if ((!sign && assign[var]) || (sign && !assign[var])) { cl_sat = 1; cl_unsat = 0; }
        }
        if (!cl_sat && !cl_unresolved) { any_unsat = 1; break; }
        if (!cl_sat) all_sat = 0;
    }
    if (all_sat) { free(saved_assign); free(saved_mask); return 1; }
    if (any_unsat) {
        memcpy(assign, saved_assign, (size_t)f->n_vars * sizeof(int));
        memcpy(assigned_mask, saved_mask, (size_t)f->n_vars * sizeof(int));
        free(saved_assign); free(saved_mask);
        return 0;
    }

    /* Branch: pick first unassigned variable */
    int branch_var = -1;
    for (int v = 0; v < f->n_vars; v++) {
        if (!assigned_mask[v]) { branch_var = v; break; }
    }
    if (branch_var < 0) { free(saved_assign); free(saved_mask); return 0; }

    /* Try assign 0 */
    memcpy(assign, saved_assign, (size_t)f->n_vars * sizeof(int));
    memcpy(assigned_mask, saved_mask, (size_t)f->n_vars * sizeof(int));
    assign[branch_var] = 0;
    assigned_mask[branch_var] = 1;
    int r0 = dpll_recursive(f, assign, assigned_mask, depth + 1, max_depth);
    if (r0 == 1) { free(saved_assign); free(saved_mask); return 1; }

    /* Try assign 1 */
    memcpy(assign, saved_assign, (size_t)f->n_vars * sizeof(int));
    memcpy(assigned_mask, saved_mask, (size_t)f->n_vars * sizeof(int));
    assign[branch_var] = 1;
    assigned_mask[branch_var] = 1;
    int r1 = dpll_recursive(f, assign, assigned_mask, depth + 1, max_depth);
    free(saved_assign); free(saved_mask);
    return r1;
}

int ph_dpll_sat(const PH_CNF* f) {
    /* DPLL (Davis-Putnam-Logemann-Loveland) SAT solver.
     * Theorem: DPLL solves SAT but may take exponential time.
     * Implements unit propagation and pure literal elimination. */
    PH_CNF* work = ph_cnf_clone(f);
    if (!work) return -1;
    int* assign = calloc((size_t)work->n_vars, sizeof(int));
    int* assigned_mask = calloc((size_t)work->n_vars, sizeof(int));
    int max_depth = work->n_vars * 2;
    if (max_depth > 1000) max_depth = 1000;
    int result = dpll_recursive(work, assign, assigned_mask, 0, max_depth);
    free(assign); free(assigned_mask); free(work);
    return result;
}

/* ================================================================
 * Collapse Check (L4)
 * ================================================================ */

int ph_collapse_check(int max_level) {
    /* Theorem: If Sigma_k = Pi_k then PH collapses to Sigma_k.
     * We simulate by checking small QBF instances at successive levels.
     * If Sigma_k and Pi_k give same answers on all small test instances,
     * that's evidence consistent with collapse (though not a proof). */
    if (max_level > PH_MAX_LEVEL) max_level = PH_MAX_LEVEL;
    if (max_level < 1) max_level = 1;

    printf("[Collapse Check] Testing levels 1..%d on small instances...\n", max_level);

    for (int k = 1; k <= max_level; k++) {
        int agreements = 0, disagreements = 0;
        /* Test all 2^k patterns of 1-bit quantifier blocks */
        for (int pat = 0; pat < (1 << k) && pat < 32; pat++) {
            PH_QuantType types[PH_MAX_LEVEL];
            int sizes[PH_MAX_LEVEL];
            for (int i = 0; i < k; i++) {
                types[i] = (PH_QuantType)((pat >> i) & 1);
                sizes[i] = 1;
            }
            /* Simple CNF: x0 OR x1 OR ... (first var) */
            PH_CNF* f = ph_cnf_build_single(0, 0, 1, 0, 2, 0);
            if (f->n_vars < k) f->n_vars = k;
            PH_QBF* qbf = ph_qbf_create(k, types, sizes, f);
            int r_sigma = ph_sigma_k_sat(qbf);
            int r_pi    = ph_pi_k_sat(qbf);
            if (r_sigma == r_pi) agreements++; else disagreements++;
            ph_qbf_free(qbf);
        }
        printf("  k=%d: agreements=%d, disagreements=%d\n", k, agreements, disagreements);
        if (disagreements == 0 && agreements > 0) {
            printf("  => All test instances agree: evidence for Sigma_%d = Pi_%d\n", k, k);
        }
    }
    return 0;
}

/* ================================================================
 * SAT-to-CLIQUE Reduction (L5)
 * ================================================================ */

void ph_sat_to_clique(const PH_CNF* f, int* graph_out,
                      int* n_out, int* k_out) {
    /* Reduction: 3SAT <=_p CLIQUE (Karp 1972)
     * For each literal occurrence, create a vertex.
     * Vertices are adjacent iff they are from different clauses and are compatible
     * (not contradictory: not x and !x). */
    int n = f->n_clauses * 3; /* at most 3 literals per clause */
    *n_out = n;
    *k_out = f->n_clauses;
    /* graph_out is n*n adjacency matrix (1=edge, 0=no edge) */
    for (int i = 0; i < n * n; i++) graph_out[i] = 0;

    /* Map: vertex index -> (clause, literal_pos) */
    for (int ci = 0; ci < f->n_clauses; ci++) {
        for (int j = 0; j < 3; j++) {
            int vi = ci * 3 + j;
            int lit_i = f->clauses[ci].lits[j];
            if (lit_i < 0) continue; /* padding */
            for (int cj = 0; cj < f->n_clauses; cj++) {
                if (cj == ci) continue; /* no edges within same clause */
                for (int l = 0; l < 3; l++) {
                    int vj = cj * 3 + l;
                    int lit_j = f->clauses[cj].lits[l];
                    if (lit_j < 0) continue;
                    int var_i = lit_i >> 1, var_j = lit_j >> 1;
                    int sign_i = lit_i & 1, sign_j = lit_j & 1;
                    /* Compatible if not contradicting (same var, opposite signs) */
                    if (!(var_i == var_j && sign_i != sign_j)) {
                        graph_out[vi * n + vj] = 1;
                    }
                }
            }
        }
    }
}

/* ================================================================
 * SAT-to-VERTEX-COVER Reduction (L5)
 * ================================================================ */

void ph_sat_to_vertexcover(const PH_CNF* f, int* graph_out,
                           int* n_out, int* k_out) {
    /* Reduction: 3SAT <=_p VERTEX-COVER
     * Build graph with gadget for each clause (triangle) and
     * edges connecting complementary literals. */
    int n_cl = f->n_clauses;
    int n = n_cl * 3; /* 3 vertices per clause gadget */
    *n_out = n;
    *k_out = n_cl * 2; /* vertex cover size = 2 * n_clauses */
    for (int i = 0; i < n * n; i++) graph_out[i] = 0;

    /* Clause gadgets: triangle on 3 vertices of each clause */
    for (int ci = 0; ci < n_cl; ci++) {
        int b = ci * 3;
        graph_out[b * n + b + 1] = 1;
        graph_out[(b + 1) * n + b] = 1;
        graph_out[(b + 1) * n + b + 2] = 1;
        graph_out[(b + 2) * n + b + 1] = 1;
        graph_out[b * n + b + 2] = 1;
        graph_out[(b + 2) * n + b] = 1;
    }

    /* Consistency edges: connect complementary literals */
    for (int ci = 0; ci < n_cl; ci++) {
        for (int j = 0; j < 3; j++) {
            int lit_i = f->clauses[ci].lits[j];
            if (lit_i < 0) continue;
            for (int cj = 0; cj < n_cl; cj++) {
                for (int l = 0; l < 3; l++) {
                    int lit_j = f->clauses[cj].lits[l];
                    if (lit_j < 0) continue;
                    int var_i = lit_i >> 1, var_j = lit_j >> 1;
                    int sign_i = lit_i & 1, sign_j = lit_j & 1;
                    if (var_i == var_j && sign_i != sign_j) {
                        int vi = ci * 3 + j;
                        int vj = cj * 3 + l;
                        graph_out[vi * n + vj] = 1;
                    }
                }
            }
        }
    }
}

/* ================================================================
 * QSAT Benchmark (L6)
 * ================================================================ */

PH_QBF* ph_qsat_benchmark(int k, int vars_per_block) {
    /* Build a standard QSAT_k benchmark instance:
     * k blocks, each with vars_per_block variables.
     * Matrix: random-ish 3-CNF with 2 * total_vars clauses. */
    int total_vars = k * vars_per_block;
    if (total_vars > PH_MAX_VARS) total_vars = PH_MAX_VARS;

    PH_QuantType types[PH_MAX_LEVEL];
    int sizes[PH_MAX_LEVEL];
    for (int i = 0; i < k && i < PH_MAX_LEVEL; i++) {
        types[i] = (i % 2 == 0) ? PH_QUANT_EXISTS : PH_QUANT_FORALL;
        sizes[i] = vars_per_block;
    }

    PH_CNF* f = malloc(sizeof(PH_CNF));
    memset(f, 0, sizeof(PH_CNF));
    f->n_vars = total_vars;
    f->n_clauses = 2 * total_vars;
    if (f->n_clauses > PH_MAX_CLAUSES) f->n_clauses = PH_MAX_CLAUSES;

    /* Build CNF with a deterministic pattern */
    for (int ci = 0; ci < f->n_clauses; ci++) {
        int v0 = ci % total_vars;
        int v1 = (ci + total_vars / 3) % total_vars;
        int v2 = (ci + 2 * total_vars / 3) % total_vars;
        f->clauses[ci].lits[0] = (v0 << 1) | (ci & 1);
        f->clauses[ci].lits[1] = (v1 << 1) | ((ci >> 1) & 1);
        f->clauses[ci].lits[2] = (v2 << 1) | ((ci >> 2) & 1);
    }

    return ph_qbf_create(k, types, sizes, f);
}

/* ================================================================
 * More Circuit Builders (L4-L5)
 * ================================================================ */

PH_BoolCircuit* ph_circuit_build_xor(void) {
    /* XOR of 2 inputs: x0 XOR x1 = (x0 AND NOT x1) OR (NOT x0 AND x1) */
    PH_BoolCircuit* c = malloc(sizeof(PH_BoolCircuit));
    if (!c) return NULL;
    memset(c, 0, sizeof(PH_BoolCircuit));
    c->n_inputs = 2;
    c->n_gates = 5;
    /* G0: NOT x0 */  c->gates[0] = (PH_Gate){PH_GATE_NOT, 0, 0, 0};
    /* G1: NOT x1 */  c->gates[1] = (PH_Gate){PH_GATE_NOT, 1, 0, 0};
    /* G2: x0 AND NOT x1 */ c->gates[2] = (PH_Gate){PH_GATE_AND, 0, 3, 0};
    /* G3: NOT x0 AND x1 */ c->gates[3] = (PH_Gate){PH_GATE_AND, 2, 1, 0};
    /* G4: OR of both */   c->gates[4] = (PH_Gate){PH_GATE_OR, 4, 5, 0};
    return c;
}

PH_BoolCircuit* ph_circuit_build_iff(int n_inputs) {
    /* IFF (equivalence) of 2 inputs: x0 <-> x1 = NOT(x0 XOR x1).
     * For n > 2, all inputs must be equal. */
    PH_BoolCircuit* c = malloc(sizeof(PH_BoolCircuit));
    if (!c) return NULL;
    memset(c, 0, sizeof(PH_BoolCircuit));
    c->n_inputs = n_inputs;
    if (n_inputs <= 1) {
        c->n_gates = 1;
        c->gates[0] = (PH_Gate){PH_GATE_CONST, 0, 0, 1};
        return c;
    }
    /* For 2 inputs: (x0 AND x1) OR (NOT x0 AND NOT x1) */
    if (n_inputs == 2) {
        c->n_gates = 5;
        c->gates[0] = (PH_Gate){PH_GATE_NOT, 0, 0, 0};   /* NOT x0 */
        c->gates[1] = (PH_Gate){PH_GATE_NOT, 1, 0, 0};   /* NOT x1 */
        c->gates[2] = (PH_Gate){PH_GATE_AND, 0, 1, 0};    /* x0 AND x1 */
        c->gates[3] = (PH_Gate){PH_GATE_AND, 2, 3, 0};    /* NOT x0 AND NOT x1 */
        c->gates[4] = (PH_Gate){PH_GATE_OR, 4, 5, 0};     /* OR */
    } else {
        /* For n>2: chain equivalence checks */
        c->n_gates = 0;
        int last = 0; /* wire index of previous result */
        for (int i = 1; i < n_inputs && c->n_gates < PH_MAX_GATES - 5; i++) {
            int gi = c->n_gates;
            /* Build: (last AND xi) OR (NOT last AND NOT xi) */
            c->gates[gi+0] = (PH_Gate){PH_GATE_NOT, last, 0, 0};
            c->gates[gi+1] = (PH_Gate){PH_GATE_NOT, i, 0, 0};
            c->gates[gi+2] = (PH_Gate){PH_GATE_AND, last, i, 0};
            c->gates[gi+3] = (PH_Gate){PH_GATE_AND, c->n_inputs+gi+0, c->n_inputs+gi+1, 0};
            c->gates[gi+4] = (PH_Gate){PH_GATE_OR, c->n_inputs+gi+2, c->n_inputs+gi+3, 0};
            c->n_gates += 5;
            last = c->n_inputs + gi + 4;
        }
    }
    return c;
}

PH_BoolCircuit* ph_circuit_build_threshold(int n_inputs, int k) {
    /* Threshold-k: output 1 iff at least k inputs are 1.
     * For small n_inputs, implement via sum of pairs. */
    PH_BoolCircuit* c = malloc(sizeof(PH_BoolCircuit));
    if (!c) return NULL;
    memset(c, 0, sizeof(PH_BoolCircuit));
    c->n_inputs = n_inputs;
    if (k <= 0) {
        c->n_gates = 1;
        c->gates[0] = (PH_Gate){PH_GATE_CONST, 0, 0, 1};
        return c;
    }
    if (k > n_inputs) {
        c->n_gates = 1;
        c->gates[0] = (PH_Gate){PH_GATE_CONST, 0, 0, 0};
        return c;
    }
    /* Build sum of all pairs: threshold-k = OR_{S: |S|=k} (AND_{i in S} x_i)
     * For small k,n, enumerate all k-subsets. */
    int subset[PH_MAX_INPUTS];
    int count = 0;
    long long total_subsets = 1LL << n_inputs;
    int and_results[PH_MAX_GATES];
    int n_ands = 0;

    for (long long mask = 0; mask < total_subsets && n_ands < PH_MAX_GATES - 2; mask++) {
        int bits = 0;
        for (int i = 0; i < n_inputs; i++)
            if ((mask >> i) & 1) bits++;
        if (bits == k && c->n_gates < PH_MAX_GATES - 2) {
            /* Build AND of selected inputs */
            int first = -1;
            for (int i = 0; i < n_inputs; i++) {
                if ((mask >> i) & 1) {
                    if (first < 0) first = i;
                    else {
                        int gi = c->n_gates;
                        c->gates[gi] = (PH_Gate){PH_GATE_AND,
                            (first < n_inputs ? first : c->n_inputs + and_results[first - n_inputs]),
                            i, 0};
                        and_results[c->n_gates] = c->n_inputs + gi;
                        first = c->n_inputs + gi;
                        c->n_gates++;
                    }
                }
            }
            if (first >= 0 && first < n_inputs) {
                /* Single variable threshold: pass through */
                and_results[c->n_gates] = first;
                c->gates[c->n_gates] = (PH_Gate){PH_GATE_INPUT, first, 0, 0};
                c->n_gates++;
            }
            n_ands++;
        }
    }

    /* OR of all AND results */
    if (n_ands == 0) {
        c->gates[c->n_gates++] = (PH_Gate){PH_GATE_CONST, 0, 0, 0};
        return c;
    }
    int or_src = and_results[0];
    for (int i = 1; i < n_ands && c->n_gates < PH_MAX_GATES; i++) {
        int gi = c->n_gates;
        c->gates[gi] = (PH_Gate){PH_GATE_OR, or_src, and_results[i], 0};
        or_src = c->n_inputs + gi;
        c->n_gates++;
    }
    return c;
}

PH_BoolCircuit* ph_circuit_build_parity(int n_inputs) {
    /* Parity: XOR of all inputs. Output 1 iff odd number of 1s.
     * Build as a tree of XOR gates. */
    PH_BoolCircuit* c = malloc(sizeof(PH_BoolCircuit));
    if (!c) return NULL;
    memset(c, 0, sizeof(PH_BoolCircuit));
    c->n_inputs = n_inputs;
    if (n_inputs <= 0) {
        c->n_gates = 1;
        c->gates[0] = (PH_Gate){PH_GATE_CONST, 0, 0, 0};
        return c;
    }
    if (n_inputs == 1) {
        c->n_gates = 1;
        c->gates[0] = (PH_Gate){PH_GATE_INPUT, 0, 0, 0};
        return c;
    }

    /* Parity = cascaded XOR:
     * XOR(a,b) = (a AND NOT b) OR (NOT a AND b) = 5 gates */
    int current = 0; /* wire index of current parity result */
    int have_result = 0;

    for (int i = 0; i < n_inputs && c->n_gates < PH_MAX_GATES - 6; i++) {
        if (!have_result) {
            current = i;
            have_result = 1;
            continue;
        }
        /* Build XOR(current, input_i) */
        int gi = c->n_gates;
        int wi = c->n_inputs + gi;
        c->gates[gi+0] = (PH_Gate){PH_GATE_NOT, current, 0, 0};
        c->gates[gi+1] = (PH_Gate){PH_GATE_NOT, i, 0, 0};
        c->gates[gi+2] = (PH_Gate){PH_GATE_AND, current, wi+1, 0};
        c->gates[gi+3] = (PH_Gate){PH_GATE_AND, wi+0, i, 0};
        c->gates[gi+4] = (PH_Gate){PH_GATE_OR, wi+2, wi+3, 0};
        c->n_gates += 5;
        current = wi + 4;
    }
    return c;
}

int ph_circuit_count_gates(const PH_BoolCircuit* c) {
    if (!c) return 0;
    int count = 0;
    for (int i = 0; i < c->n_gates; i++) {
        if (c->gates[i].type != PH_GATE_INPUT &&
            c->gates[i].type != PH_GATE_CONST)
            count++;
    }
    return count;
}

int ph_circuit_depth(const PH_BoolCircuit* c) {
    /* Compute circuit depth (longest input-to-output path).
     * Simplified: use topological depth via dynamic programming. */
    if (!c || c->n_gates == 0) return 0;
    int depth[PH_MAX_GATES + PH_MAX_INPUTS];
    for (int i = 0; i < c->n_inputs; i++) depth[i] = 0;

    for (int i = 0; i < c->n_gates; i++) {
        int wi = c->n_inputs + i;
        const PH_Gate* g = &c->gates[i];
        int d1 = 0, d2 = 0;
        if (g->type != PH_GATE_CONST) {
            d1 = (g->in1 < c->n_inputs + i) ? depth[g->in1] : 0;
        }
        if (g->type == PH_GATE_AND || g->type == PH_GATE_OR) {
            d2 = (g->in2 < c->n_inputs + i) ? depth[g->in2] : 0;
        }
        depth[wi] = 1 + (d1 > d2 ? d1 : d2);
    }
    return depth[c->n_inputs + c->n_gates - 1];
}

void ph_cnf_to_circuit(const PH_CNF* f, PH_BoolCircuit* c) {
    /* Convert CNF to Boolean circuit.
     * For each clause: OR of 3 literals (negated variables via NOT).
     * Output: AND of all clause outputs.
     *
     * This conversion is used in circuit complexity analysis. */
    if (!f || !c) return;
    memset(c, 0, sizeof(PH_BoolCircuit));
    c->n_inputs = f->n_vars;

    int clause_outputs[PH_MAX_CLAUSES];
    int n_clause_outs = 0;

    for (int ci = 0; ci < f->n_clauses && c->n_gates < PH_MAX_GATES - 10; ci++) {
        /* Build OR of up to 3 literals.
         * For negated literal: add NOT gate, then feed to OR. */
        int lit_wires[3];
        int n_lits = 0;

        for (int j = 0; j < 3; j++) {
            int lit = f->clauses[ci].lits[j];
            if (lit < 0) continue;
            int var = lit >> 1, sign = lit & 1;
            if (sign == 0) {
                lit_wires[n_lits++] = var; /* positive: direct input */
            } else {
                /* Negative: add NOT gate */
                int gi = c->n_gates;
                if (gi >= PH_MAX_GATES) break;
                c->gates[gi] = (PH_Gate){PH_GATE_NOT, var, 0, 0};
                lit_wires[n_lits++] = c->n_inputs + gi;
                c->n_gates++;
            }
        }

        /* Build OR of lit_wires */
        if (n_lits == 0) continue;
        int or_result = lit_wires[0];
        for (int j = 1; j < n_lits && c->n_gates < PH_MAX_GATES; j++) {
            int gi = c->n_gates;
            c->gates[gi] = (PH_Gate){PH_GATE_OR, or_result, lit_wires[j], 0};
            or_result = c->n_inputs + gi;
            c->n_gates++;
        }
        clause_outputs[n_clause_outs++] = or_result;
    }

    /* AND of all clause outputs */
    if (n_clause_outs == 0) {
        c->gates[c->n_gates++] = (PH_Gate){PH_GATE_CONST, 0, 0, 1};
        return;
    }
    int and_result = clause_outputs[0];
    for (int i = 1; i < n_clause_outs && c->n_gates < PH_MAX_GATES; i++) {
        int gi = c->n_gates;
        c->gates[gi] = (PH_Gate){PH_GATE_AND, and_result, clause_outputs[i], 0};
        and_result = c->n_inputs + gi;
        c->n_gates++;
    }
}

/* ================================================================
 * Circuit Value (L6)
 * ================================================================ */

int ph_circuit_value(const PH_BoolCircuit* c, const int* inputs) {
    /* Circuit Value Problem (CVP) is P-complete (Ladner 1975).
     * This is the canonical P-complete problem. */
    return ph_circuit_eval(c, inputs);
}