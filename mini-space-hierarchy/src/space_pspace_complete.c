/* space_pspace_complete.c -- PSPACE-Complete Problems
 *
 * TQBF is the canonical PSPACE-complete problem.
 * Other PSPACE-complete problems:
 *   1. QSAT (Quantified SAT)
 *   2. Generalized Geography
 *   3. Equivalence of regular expressions
 *   4. Universality of NFA (does NFA accept Sigma*?)
 *   5. First-order theory of (N, +1)  (Presburger with successor only)
 *   6. Planarity testing of graphs
 *
 * Reference: Garey & Johnson (1979); Arora & Barak 4.2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/spaceh.h"

/* ---------- QSAT Evaluator ---------- */

/* QSAT = TQBF with all quantifiers at front:
 *   Q1 x1 Q2 x2 ... Qn xn phi(x1,...,xn)
 * where phi is a 3-CNF formula.
 * Evaluation uses polynomial space. */

typedef struct {
    int n_vars;
    int* quantifiers;  /* 1=exists, 0=forall */
    int n_clauses;
    int** clauses;
    int* clause_sizes;
} QSATFormula;

static QSATFormula* qsat_create(int n_vars, int n_clauses) {
    QSATFormula* f = malloc(sizeof(QSATFormula));
    f->n_vars = n_vars;
    f->quantifiers = calloc((size_t)n_vars, sizeof(int));
    f->n_clauses = n_clauses;
    f->clauses = malloc((size_t)n_clauses * sizeof(int*));
    f->clause_sizes = malloc((size_t)n_clauses * sizeof(int));
    for (int i = 0; i < n_clauses; i++) {
        f->clauses[i] = NULL;
        f->clause_sizes[i] = 0;
    }
    return f;
}

static void qsat_free(QSATFormula* f) {
    if (!f) return;
    free(f->quantifiers);
    for (int i = 0; i < f->n_clauses; i++) free(f->clauses[i]);
    free(f->clauses); free(f->clause_sizes); free(f);
}

/* Evaluate QSAT using recursion: polynomial space, exponential time */
static int qsat_eval_rec(QSATFormula* f, int* assign, int var_idx) {
    if (var_idx >= f->n_vars) {
        /* Evaluate CNF */
        for (int ci = 0; ci < f->n_clauses; ci++) {
            int sat = 0;
            for (int li = 0; li < f->clause_sizes[ci]; li++) {
                int lit = f->clauses[ci][li];
                int var = abs(lit) - 1;
                if ((lit > 0 && assign[var]) || (lit < 0 && !assign[var]))
                    { sat = 1; break; }
            }
            if (!sat) return 0;
        }
        return 1;
    }

    int is_exists = f->quantifiers[var_idx];
    assign[var_idx] = 0;
    int r0 = qsat_eval_rec(f, assign, var_idx + 1);
    if (is_exists && r0) return 1;
    if (!is_exists && !r0) return 0;

    assign[var_idx] = 1;
    return qsat_eval_rec(f, assign, var_idx + 1);
}

/* ---------- NFA Universality ---------- */

/* NFA Universality: Given an NFA M, does L(M) = Sigma*?
 * This is PSPACE-complete.
 * Polynomial-space algorithm: check if there exists a string
 * NOT accepted by M. This is equivalent to checking if the
 * complement NFA is non-empty, which is PSPACE. */

/* Simplified NFA representation */
typedef struct {
    int n_states;
    int alphabet_size;
    int q0;
    int* accept;
    int n_accept;
    int*** delta;  /* delta[state][symbol] = set of next states */
    int* delta_sizes;
} NFA;

static NFA* nfa_create(int n_states, int alpha_size) {
    NFA* nfa = malloc(sizeof(NFA));
    nfa->n_states = n_states;
    nfa->alphabet_size = alpha_size;
    nfa->q0 = 0;
    nfa->accept = malloc((size_t)n_states * sizeof(int));
    nfa->n_accept = 0;
    nfa->delta = malloc((size_t)n_states * sizeof(int**));
    nfa->delta_sizes = calloc((size_t)n_states, sizeof(int));
    for (int s = 0; s < n_states; s++) {
        nfa->delta[s] = malloc((size_t)alpha_size * sizeof(int*));
        for (int a = 0; a < alpha_size; a++)
            nfa->delta[s][a] = NULL;
    }
    return nfa;
}

static void nfa_free(NFA* nfa) {
    if (!nfa) return;
    for (int s = 0; s < nfa->n_states; s++) {
        for (int a = 0; a < nfa->alphabet_size; a++)
            free(nfa->delta[s][a]);
        free(nfa->delta[s]);
    }
    free(nfa->delta); free(nfa->delta_sizes);
    free(nfa->accept); free(nfa);
}

/* Check if all strings of length <= max_len are accepted (approximation).
 * Full universality checking requires reachability in subset construction
 * (exponential size, PSPACE via on-the-fly subset exploration). */
static int nfa_universal_upto(NFA* nfa, int max_len) {
    /* BFS over possible subset states */
    int max_subsets = 1 << nfa->n_states;
    int* visited = calloc((size_t)max_subsets, sizeof(int));
    int* queue = malloc((size_t)max_subsets * sizeof(int));
    int qh = 0, qt = 0;
    int start_subset = 1 << nfa->q0;

    queue[qt++] = start_subset;
    visited[start_subset] = 1;

    while (qh < qt) {
        int subset = queue[qh++];

        /* Check if current subset contains NO accept state */
        int has_accept = 0;
        for (int s = 0; s < nfa->n_states; s++) {
            if ((subset >> s) & 1) {
                for (int ai = 0; ai < nfa->n_accept; ai++) {
                    if (s == nfa->accept[ai]) { has_accept = 1; break; }
                }
            }
        }
        if (!has_accept) {
            /* Found a subset with no accept state -> some string not accepted */
            free(visited); free(queue);
            return 0;
        }

        /* For each symbol, compute next subset */
        for (int a = 0; a < nfa->alphabet_size; a++) {
            int next_subset = 0;
            for (int s = 0; s < nfa->n_states; s++) {
                if ((subset >> s) & 1) {
                    /* Add all epsilon-closure + delta(s,a) */
                    for (int t = 0; t < nfa->delta_sizes[s]; t++) {
                        next_subset |= (1 << nfa->delta[s][a][t]);
                    }
                }
            }
            if (next_subset && !visited[next_subset]) {
                visited[next_subset] = 1;
                queue[qt++] = next_subset;
            }
        }
    }

    free(visited); free(queue);
    return 1; /* All reachable subsets have accept states */
}

/* ---------- Demo ---------- */

void pspace_complete_demo(void) {
    printf("\n===== PSPACE-Complete Problems =====\n\n");

    /* QSAT demo */
    printf("--- QSAT (PSPACE-complete) ---\n");
    QSATFormula* f = qsat_create(3, 2);
    f->quantifiers[0] = 1; /* exists */
    f->quantifiers[1] = 0; /* forall */
    f->quantifiers[2] = 1; /* exists */
    /* Clauses: (x1|x2|x3) and (!x1|!x2|x3) */
    f->clauses[0] = malloc(3 * sizeof(int));
    f->clauses[0][0] = 1; f->clauses[0][1] = 2; f->clauses[0][2] = 3;
    f->clause_sizes[0] = 3;
    f->clauses[1] = malloc(3 * sizeof(int));
    f->clauses[1][0] = -1; f->clauses[1][1] = -2; f->clauses[1][2] = 3;
    f->clause_sizes[1] = 3;
    int* assign = calloc(3, sizeof(int));
    int result = qsat_eval_rec(f, assign, 0);
    printf("  Exists x1 Forall x2 Exists x3: (x1|x2|x3)&(!x1|!x2|x3)\n");
    printf("  Result: %s (x1=T,x3=T works for any x2)\n\n",
           result ? "TRUE" : "FALSE");
    free(assign);
    qsat_free(f);

    /* NFA universality demo */
    printf("--- NFA Universality (PSPACE-complete) ---\n");
    NFA* nfa = nfa_create(3, 2);
    nfa->accept[0] = 0; nfa->n_accept = 1;
    /* NFA accepting strings ending in '0' */
    nfa->delta[0][0] = malloc(2 * sizeof(int));
    nfa->delta[0][0][0] = 0; nfa->delta[0][0][1] = 1;
    nfa->delta_sizes[0] = 2;
    nfa->delta[0][1] = malloc(1 * sizeof(int));
    nfa->delta[0][1][0] = 0;
    nfa->delta_sizes[0] = 2; /* shared counter */
    nfa->delta[1][0] = malloc(1 * sizeof(int));
    nfa->delta[1][0][0] = 1;
    nfa->delta[1][1] = malloc(1 * sizeof(int));
    nfa->delta[1][1][0] = 0;
    nfa->delta_sizes[1] = 1;

    printf("  NFA: 2 states, accepting strings ending in 0\n");
    printf("  Universal (accepts all strings)? %s\n",
           nfa_universal_upto(nfa, 10) ? "YES" : "NO");
    printf("  (Actually NO: string '1' is not accepted)\n");
    nfa_free(nfa);

    printf("\n--- Complete List of PSPACE-Complete Problems ---\n");
    printf("  1. TQBF / QSAT\n");
    printf("  2. Generalized Geography\n");
    printf("  3. Equivalence of regular expressions\n");
    printf("  4. NFA Universality\n");
    printf("  5. First-order theory of (N, +1)\n");
    printf("  6. Context-sensitive grammar membership\n");
    printf("  7. Quantified Boolean Formula with bounded alternations\n");
    printf("  8. Rush Hour / Sokoban / Othello (games)\n");
    printf("  9. AND-OR graph evaluation\n");
    printf("  10. Planarity of hierarchical graphs\n");

    printf("\n--- PSPACE vs Other Classes ---\n");
    printf("  PSPACE = NPSPACE (Savitch collapse)\n");
    printf("  PSPACE contains PH (entire polynomial hierarchy!)\n");
    printf("  PSPACE = IP (interactive proofs, Shamir 1990)\n");
    printf("  PSPACE = AP (alternating polynomial time, CKS 1981)\n");
    printf("  P vs PSPACE: MAJOR OPEN PROBLEM.\n");
}
