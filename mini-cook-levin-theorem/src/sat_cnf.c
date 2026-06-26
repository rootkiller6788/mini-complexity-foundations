/*
 * sat_cnf.c — CNF Formula Operations & DIMACS I/O
 *
 * L3 Mathematical Structures:
 *   CNF formula represented as array of Clauses.
 *   Literal encoding: (var << 1) | sign.
 *   Assignment: int array with -1 = unassigned, 0/1 = false/true.
 *
 * L1 Definitions:
 *   Clause = disjunction of literals
 *   CNF = conjunction of clauses
 *   Assignment = mapping from variables to {0,1}
 *
 * L2 Core Concepts:
 *   Satisfiability, validity, equivalence
 *
 * Reference: Arora-Barak section 2.3, Sipser section 7.4
 */

#include "sat.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/* ================================================================
 * Literal Operations
 * ================================================================ */

int lit_value(int lit, const Assignment assign) {
    if (!assign) return -1;                    /* no assignment = undetermined */
    int var = SAT_LIT_VAR(lit);
    int sign = SAT_LIT_SIGN(lit);
    if (assign[var] < 0) return -1;       /* unassigned */
    if (sign) return assign[var] ? 0 : 1;  /* negative literal */
    return assign[var];                     /* positive literal */
}

/* ================================================================
 * CNF Construction
 * ================================================================ */

CNF* cnf_create(int n_vars, int clause_capacity) {
    CNF* cnf = (CNF*)calloc(1, sizeof(CNF));
    if (!cnf) return NULL;
    cnf->n_vars = n_vars;
    cnf->n_clauses = 0;
    cnf->clause_cap = clause_capacity > 0 ? clause_capacity : 64;
    cnf->clauses = (Clause*)calloc((size_t)cnf->clause_cap, sizeof(Clause));
    cnf->add_idx = 0;
    return cnf;
}

void cnf_add_clause(CNF* cnf, const int* lits, int n_lits) {
    if (!cnf || !lits || n_lits <= 0) return;
    if (cnf->add_idx >= cnf->clause_cap) {
        cnf->clause_cap *= 2;
        cnf->clauses = (Clause*)realloc(cnf->clauses,
                         (size_t)cnf->clause_cap * sizeof(Clause));
        /* zero-initialize new clauses */
        for (int i = cnf->n_clauses; i < cnf->clause_cap; i++) {
            cnf->clauses[i].lits = NULL;
            cnf->clauses[i].n = 0;
        }
    }
    Clause* c = &cnf->clauses[cnf->add_idx];
    c->n = n_lits;
    c->lits = (int*)malloc((size_t)n_lits * sizeof(int));
    memcpy(c->lits, lits, (size_t)n_lits * sizeof(int));
    cnf->add_idx++;
    cnf->n_clauses = cnf->add_idx;
}

void cnf_add_unit(CNF* cnf, int lit) {
    int lits[1] = {lit};
    cnf_add_clause(cnf, lits, 1);
}

void cnf_add_binary(CNF* cnf, int lit1, int lit2) {
    int lits[2] = {lit1, lit2};
    cnf_add_clause(cnf, lits, 2);
}

void cnf_add_ternary(CNF* cnf, int lit1, int lit2, int lit3) {
    int lits[3] = {lit1, lit2, lit3};
    cnf_add_clause(cnf, lits, 3);
}

CNF* cnf_clone(const CNF* cnf) {
    if (!cnf) return NULL;
    CNF* clone = cnf_create(cnf->n_vars, cnf->clause_cap);
    for (int i = 0; i < cnf->n_clauses; i++) {
        cnf_add_clause(clone, cnf->clauses[i].lits, cnf->clauses[i].n);
    }
    return clone;
}

void cnf_free(CNF* cnf) {
    if (!cnf) return;
    for (int i = 0; i < cnf->n_clauses; i++) {
        free(cnf->clauses[i].lits);
    }
    free(cnf->clauses);
    free(cnf);
}

/* ================================================================
 * CNF Evaluation
 * ================================================================ */

int clause_value(const Clause* c, const Assignment assign) {
    if (!c) return -1;
    int any_undef = 0;
    for (int j = 0; j < c->n; j++) {
        int val = lit_value(c->lits[j], assign);
        if (val == 1) return 1;    /* clause satisfied */
        if (val < 0) any_undef = 1; /* literal undetermined */
    }
    return any_undef ? -1 : 0;  /* -1=undetermined, 0=conflict */
}

int cnf_value(const CNF* cnf, const Assignment assign) {
    if (!cnf || !assign) return -1;
    int all_sat = 1;
    for (int i = 0; i < cnf->n_clauses; i++) {
        int cv = clause_value(&cnf->clauses[i], assign);
        if (cv == 0) return 0;      /* any clause conflicts -> UNSAT */
        if (cv == -1) all_sat = 0;  /* undetermined */
    }
    return all_sat ? 1 : -1;  /* 1=SAT, -1=undetermined */
}

int cnf_count_satisfied(const CNF* cnf, const Assignment assign) {
    if (!cnf || !assign) return 0;
    int count = 0;
    for (int i = 0; i < cnf->n_clauses; i++) {
        if (clause_value(&cnf->clauses[i], assign) == 1) count++;
    }
    return count;
}

int assignment_is_complete(const Assignment assign, int n_vars) {
    if (!assign) return 0;
    for (int i = 0; i < n_vars; i++)
        if (assign[i] < 0) return 0;
    return 1;
}

/* ================================================================
 * CNF Statistics
 * ================================================================ */

int cnf_max_clause_size(const CNF* cnf) {
    if (!cnf || cnf->n_clauses == 0) return 0;
    int max_sz = 0;
    for (int i = 0; i < cnf->n_clauses; i++)
        if (cnf->clauses[i].n > max_sz) max_sz = cnf->clauses[i].n;
    return max_sz;
}

int cnf_min_clause_size(const CNF* cnf) {
    if (!cnf || cnf->n_clauses == 0) return 0;
    int min_sz = cnf->clauses[0].n;
    for (int i = 1; i < cnf->n_clauses; i++)
        if (cnf->clauses[i].n < min_sz) min_sz = cnf->clauses[i].n;
    return min_sz;
}

int cnf_total_literals(const CNF* cnf) {
    if (!cnf) return 0;
    int total = 0;
    for (int i = 0; i < cnf->n_clauses; i++)
        total += cnf->clauses[i].n;
    return total;
}

void cnf_print_stats(const CNF* cnf) {
    if (!cnf) { printf("NULL CNF\n"); return; }
    printf("CNF Stats: %d vars, %d clauses, %d total literals\n",
           cnf->n_vars, cnf->n_clauses, cnf_total_literals(cnf));
    printf("  Clause sizes: min=%d, max=%d\n",
           cnf_min_clause_size(cnf), cnf_max_clause_size(cnf));
}

/* ================================================================
 * DIMACS CNF Format I/O
 * ================================================================ */

int cnf_write_dimacs(const CNF* cnf, const char* filename) {
    if (!cnf || !filename) return 0;
    FILE* f = fopen(filename, "w");
    if (!f) return 0;
    fprintf(f, "c CNF formula generated by mini-cook-levin\n");
    fprintf(f, "p cnf %d %d\n", cnf->n_vars, cnf->n_clauses);
    for (int i = 0; i < cnf->n_clauses; i++) {
        for (int j = 0; j < cnf->clauses[i].n; j++) {
            int lit = cnf->clauses[i].lits[j];
            int var = SAT_LIT_VAR(lit) + 1;   /* DIMACS uses 1-indexed vars */
            int dimacs_lit = SAT_LIT_SIGN(lit) ? -var : var;
            fprintf(f, "%d ", dimacs_lit);
        }
        fprintf(f, "0\n");
    }
    fclose(f);
    return 1;
}

CNF* cnf_read_dimacs(const char* filename) {
    if (!filename) return NULL;
    FILE* f = fopen(filename, "r");
    if (!f) return NULL;
    
    char line[4096];
    int n_vars = 0, n_clauses = 0;
    CNF* cnf = NULL;
    
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == 'c') continue;  /* comment */
        if (line[0] == 'p') {
            char fmt[32];
            sscanf(line, "p %s %d %d", fmt, &n_vars, &n_clauses);
            if (strcmp(fmt, "cnf") == 0) {
                cnf = cnf_create(n_vars, n_clauses);
            }
            continue;
        }
        if (cnf) {
            /* Parse clause line */
            int lits[256];
            int n = 0;
            char* tok = strtok(line, " \t\n");
            while (tok && n < 256) {
                int val = atoi(tok);
                if (val == 0) break;
                int var = (val > 0 ? val : -val) - 1;  /* back to 0-indexed */
                int sign = (val < 0) ? 1 : 0;
                lits[n++] = (var << 1) | sign;
                tok = strtok(NULL, " \t\n");
            }
            if (n > 0) cnf_add_clause(cnf, lits, n);
        }
    }
    fclose(f);
    return cnf;
}

void cnf_print_dimacs(const CNF* cnf) {
    if (!cnf) { printf("NULL CNF\n"); return; }
    printf("c CNF formula (%d vars, %d clauses)\n", cnf->n_vars, cnf->n_clauses);
    printf("p cnf %d %d\n", cnf->n_vars, cnf->n_clauses);
    for (int i = 0; i < cnf->n_clauses; i++) {
        for (int j = 0; j < cnf->clauses[i].n; j++) {
            int lit = cnf->clauses[i].lits[j];
            int var = SAT_LIT_VAR(lit) + 1;
            int dimacs_lit = SAT_LIT_SIGN(lit) ? -var : var;
            printf("%d ", dimacs_lit);
        }
        printf("0\n");
    }
}

/* ================================================================
 * Assignment Operations
 * ================================================================ */

Assignment assignment_create(int n_vars) {
    Assignment a = (Assignment)malloc((size_t)n_vars * sizeof(int));
    if (a) {
        for (int i = 0; i < n_vars; i++) a[i] = -1;
    }
    return a;
}

Assignment assignment_clone(const Assignment a, int n_vars) {
    if (!a) return NULL;
    Assignment clone = (Assignment)malloc((size_t)n_vars * sizeof(int));
    if (clone) memcpy(clone, a, (size_t)n_vars * sizeof(int));
    return clone;
}

void assignment_free(Assignment a) {
    free(a);
}

void assignment_print(const Assignment a, int n_vars) {
    if (!a) { printf("NULL assignment\n"); return; }
    printf("Assignment: ");
    for (int i = 0; i < n_vars && i < 30; i++) {
        printf("x%d=", i + 1);
        if (a[i] < 0) printf("?");
        else printf("%d", a[i]);
        printf(" ");
    }
    if (n_vars > 30) printf("...(%d more)", n_vars - 30);
    printf("\n");
}

void assignment_set_all(Assignment a, int n_vars, int val) {
    if (!a) return;
    for (int i = 0; i < n_vars; i++) a[i] = val;
}

/* ================================================================
 * SAT Verification (polynomial time — shows SAT in NP)
 * ================================================================ */

int sat_verify(const CNF* cnf, const Assignment assign) {
    if (!cnf || !assign) return 0;
    for (int i = 0; i < cnf->n_clauses; i++) {
        int ok = 0;
        for (int j = 0; j < cnf->clauses[i].n; j++) {
            int lit = cnf->clauses[i].lits[j];
            int var = SAT_LIT_VAR(lit);
            int sign = SAT_LIT_SIGN(lit);
            if ((!sign && assign[var] == 1) || (sign && assign[var] == 0)) {
                ok = 1;
                break;
            }
        }
        if (!ok) return 0;
    }
    return 1;
}

/* ================================================================
 * Exact Model Counting (#SAT)
 * ================================================================ */

long long sat_count_exact(const CNF* cnf) {
    if (!cnf) return 0;
    int n = cnf->n_vars;
    long long max_count = 1LL << n;
    if (n > 26 || max_count > 50000000LL) {
        /* Too many assignments to enumerate */
        return -1;
    }
    int* assign = (int*)malloc((size_t)n * sizeof(int));
    long long total = 0;
    for (long long mask = 0; mask < max_count; mask++) {
        for (int v = 0; v < n; v++)
            assign[v] = (int)((mask >> v) & 1);
        if (sat_verify(cnf, assign)) total++;
    }
    free(assign);
    return total;
}

/* ================================================================
 * Approximate Model Counting
 * ================================================================ */

double sat_count_approx(const CNF* cnf, int samples) {
    if (!cnf || samples <= 0) return 0.0;
    int n = cnf->n_vars, hits = 0;
    int* assign = (int*)malloc((size_t)n * sizeof(int));
    for (int s = 0; s < samples; s++) {
        for (int v = 0; v < n; v++)
            assign[v] = rand() & 1;
        if (sat_verify(cnf, assign)) hits++;
    }
    free(assign);
    return (double)hits / (double)samples;
}

/* ================================================================
 * Enumerate All Solutions
 * ================================================================ */

int sat_enumerate_all(const CNF* cnf, int max_solutions,
                      Assignment** assignments_out) {
    if (!cnf || !assignments_out || max_solutions <= 0) return 0;
    int n = cnf->n_vars;
    long long max_count = 1LL << n;
    if (n > 26 || max_count > 50000000LL) return -1;

    /* First pass: count solutions */
    int* assign = (int*)malloc((size_t)n * sizeof(int));
    int found = 0;
    long long stop_at = max_count;
    if (stop_at > 100000000LL) stop_at = 100000000LL;

    for (long long mask = 0; mask < stop_at && found < max_solutions; mask++) {
        for (int v = 0; v < n; v++)
            assign[v] = (int)((mask >> v) & 1);
        if (sat_verify(cnf, assign)) found++;
    }

    /* Second pass: store them */
    *assignments_out = (Assignment*)malloc((size_t)found * sizeof(Assignment));
    int idx = 0;
    for (long long mask = 0; mask < stop_at && idx < found; mask++) {
        for (int v = 0; v < n; v++)
            assign[v] = (int)((mask >> v) & 1);
        if (sat_verify(cnf, assign)) {
            (*assignments_out)[idx] = assignment_clone(assign, n);
            idx++;
        }
    }
    free(assign);
    return found;
}