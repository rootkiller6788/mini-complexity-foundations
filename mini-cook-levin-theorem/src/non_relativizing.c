/*
 * non_relativizing.c - Baker-Gill-Solovay Theorem & Relativization Barrier
 *
 * L4 Fundamental Law / L8 Advanced:
 *   Baker-Gill-Solovay Theorem (1975): There exist oracles A and B such that:
 *     P^A = NP^A    (collapsing oracle)
 *     P^B != NP^B   (separating oracle)
 *   Corollary: relativizing proof techniques cannot resolve P vs NP.
 *
 * Implementation: Oracle TM model, QBF evaluation, proof classification,
 *   BGS full demonstration, non-relativizing results inventory.
 *
 * Reference: Baker, Gill, Solovay (SIAM J. Comput. 1975); Arora-Barak 3.4
 * Courses: MIT 6.841, Stanford CS254, Berkeley CS278, CMU 15-855
 */

#include "turing_machine.h"
#include "complexity.h"
#include "sat.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/* ---- Oracle Turing Machine Model ---- */

typedef struct {
    TuringMachine*  base_tm;
    int*            oracle_set;
    int             oracle_size;
    int             n_queries;
    int*            query_history;
    int             query_cap;
} OracleTM;

OracleTM* otm_create(TuringMachine* base) {
    OracleTM* otm = (OracleTM*)calloc(1, sizeof(OracleTM));
    if (!otm) return NULL;
    otm->base_tm = base;
    otm->query_cap = 64;
    otm->query_history = (int*)malloc((size_t)otm->query_cap * sizeof(int));
    return otm;
}

void otm_set_oracle(OracleTM* otm, const int* oracle_set, int size) {
    if (!otm) return;
    free(otm->oracle_set);
    otm->oracle_size = size;
    if (size > 0 && oracle_set) {
        otm->oracle_set = (int*)malloc((size_t)size * sizeof(int));
        memcpy(otm->oracle_set, oracle_set, (size_t)size * sizeof(int));
    } else {
        otm->oracle_set = NULL;
    }
}

int otm_query(OracleTM* otm, int query_string) {
    if (!otm || !otm->oracle_set || otm->oracle_size <= 0) return 0;
    if (otm->n_queries >= otm->query_cap) {
        otm->query_cap *= 2;
        otm->query_history = (int*)realloc(otm->query_history,
                                (size_t)otm->query_cap * sizeof(int));
    }
    otm->query_history[otm->n_queries++] = query_string;
    /* Binary search in sorted oracle set */
    int lo = 0, hi = otm->oracle_size - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (otm->oracle_set[mid] == query_string) return 1;
        if (otm->oracle_set[mid] < query_string) lo = mid + 1;
        else hi = mid - 1;
    }
    return 0;
}

int otm_query_count(const OracleTM* otm) {
    return otm ? otm->n_queries : 0;
}

int otm_distinct_queries(const OracleTM* otm) {
    if (!otm || otm->n_queries <= 0) return 0;
    int n = otm->n_queries;
    int* sorted = (int*)malloc((size_t)n * sizeof(int));
    memcpy(sorted, otm->query_history, (size_t)n * sizeof(int));
    for (int i = 1; i < n; i++) {
        int key = sorted[i], j = i - 1;
        while (j >= 0 && sorted[j] > key) { sorted[j + 1] = sorted[j]; j--; }
        sorted[j + 1] = key;
    }
    int distinct = 1;
    for (int i = 1; i < n; i++)
        if (sorted[i] != sorted[i - 1]) distinct++;
    free(sorted);
    return distinct;
}

void otm_free(OracleTM* otm) {
    if (!otm) return;
    free(otm->oracle_set);
    free(otm->query_history);
    free(otm);
}

void otm_print_stats(const OracleTM* otm) {
    if (!otm) return;
    printf("Oracle TM: |oracle|=%d, queries=%d, distinct=%d\n",
           otm->oracle_size, otm->n_queries, otm_distinct_queries(otm));
}

/* ---- QBF: Quantified Boolean Formulas (PSPACE-complete, Stockmeyer-Meyer 1973) ---- */

typedef struct {
    int   n_vars;
    int*  quantifiers;   /* 0 = exists, 1 = forall */
    CNF*  matrix;        /* quantifier-free 3CNF */
} QBF;

static int qbf_eval_rec(const QBF* qbf, int* assign, int idx) {
    if (idx >= qbf->n_vars)
        return sat_verify(qbf->matrix, assign);
    if (qbf->quantifiers[idx] == 0) {
        assign[idx] = 0;
        if (qbf_eval_rec(qbf, assign, idx + 1)) return 1;
        assign[idx] = 1;
        return qbf_eval_rec(qbf, assign, idx + 1);
    } else {
        assign[idx] = 0;
        if (!qbf_eval_rec(qbf, assign, idx + 1)) return 0;
        assign[idx] = 1;
        return qbf_eval_rec(qbf, assign, idx + 1);
    }
}

int qbf_is_true(const QBF* qbf) {
    if (!qbf || !qbf->matrix) return 0;
    int* a = (int*)calloc((size_t)qbf->n_vars, sizeof(int));
    int r = qbf_eval_rec(qbf, a, 0);
    free(a);
    return r;
}

QBF* qbf_create(CNF* matrix, const int* quants, int n_quant) {
    if (!matrix) return NULL;
    QBF* q = (QBF*)calloc(1, sizeof(QBF));
    q->n_vars = n_quant;
    q->quantifiers = (int*)malloc((size_t)n_quant * sizeof(int));
    if (quants) memcpy(q->quantifiers, quants, (size_t)n_quant * sizeof(int));
    q->matrix = matrix;
    return q;
}

void qbf_free(QBF* q) {
    if (!q) return;
    free(q->quantifiers);
    cnf_free(q->matrix);
    free(q);
}

void qbf_print(const QBF* qbf) {
    if (!qbf) return;
    printf("QBF: %d vars, prefix: ", qbf->n_vars);
    for (int i = 0; i < qbf->n_vars; i++)
        printf("%sx%d ", qbf->quantifiers[i] ? "A" : "E", i);
    printf(". CNF(%d clauses, %d vars)\n",
           qbf->matrix->n_clauses, qbf->matrix->n_vars);
}

/* ---- Proof Technique Classification ---- */

typedef enum {
    TECH_DIAGONALIZATION,
    TECH_SIMULATION,
    TECH_PADDING,
    TECH_CIRCUIT_LOWER,
    TECH_INTERACTIVE,
    TECH_ARITHMETIZATION,
    TECH_PCP,
    TECH_ALGEBRAIC
} ProofTechnique;

typedef struct {
    ProofTechnique id;
    const char*    name;
    int            relativizes;
    const char*    example;
} TechniqueInfo;

static const TechniqueInfo TECHS[] = {
    { TECH_DIAGONALIZATION, "Diagonalization",      1, "Time Hierarchy Theorem" },
    { TECH_SIMULATION,      "Simulation",           1, "P in NP in PSPACE" },
    { TECH_PADDING,         "Padding argument",     1, "P != EXP" },
    { TECH_CIRCUIT_LOWER,   "Circuit lower bounds", 0, "PARITY notin AC0" },
    { TECH_INTERACTIVE,     "Interactive proofs",   0, "IP = PSPACE" },
    { TECH_ARITHMETIZATION, "Arithmetization",      0, "IP = PSPACE, PCP" },
    { TECH_PCP,             "PCP characterization", 0, "NP = PCP(log n, 1)" },
    { TECH_ALGEBRAIC,       "Algebraic methods",    0, "Razborov-Smolensky" },
};

int proof_technique_relativizes(const char* name) {
    int n = (int)(sizeof(TECHS) / sizeof(TECHS[0]));
    for (int i = 0; i < n; i++)
        if (strcmp(TECHS[i].name, name) == 0) return TECHS[i].relativizes;
    return -1;
}

/* ---- BGS Theorem: Full Demonstration ---- */

void relativization_barrier_print(void) {
    printf("\n=== Baker-Gill-Solovay: The Relativization Barrier ===\n\n");
    printf("Theorem (BGS 1975):\n");
    printf("  There exist oracles A, B: P^A = NP^A AND P^B != NP^B.\n\n");
    printf("Consequence: No relativizing proof resolves P vs NP.\n\n");

    int n = (int)(sizeof(TECHS) / sizeof(TECHS[0]));
    printf("Relativizing techniques:\n");
    for (int i = 0; i < n; i++)
        if (TECHS[i].relativizes)
            printf("  * %-22s  (e.g., %s)\n", TECHS[i].name, TECHS[i].example);
    printf("\nNon-relativizing techniques (can resolve P vs NP):\n");
    for (int i = 0; i < n; i++)
        if (!TECHS[i].relativizes)
            printf("  * %-22s  (e.g., %s)\n", TECHS[i].name, TECHS[i].example);
    printf("\nStatus: P ?= NP remains open (Clay Millennium Prize, $1M).\n");
}

void bgs_demonstrate(void) {
    printf("\n============================================\n");
    printf("  Baker-Gill-Solovay Theorem Demonstration\n");
    printf("============================================\n\n");
    printf("Oracle A (PSPACE-complete, collapsing):\n");
    printf("  P^A = P^PSPACE = PSPACE.\n");
    printf("  NP^A in NP^PSPACE in PSPACE (NPSPACE = PSPACE).\n");
    printf("  Hence P^A = NP^A.  [check]\n\n");
    printf("Oracle B (Random, separating):\n");
    printf("  Bennett-Gill (1981): random oracle separates P from NP\n");
    printf("  with probability 1. Hence P^B != NP^B.  [check]\n\n");
    printf("Conclusion: No relativizing proof resolves P vs NP.\n");
}

/* ---- Oracle Query Complexity ---- */

double oracle_query_lower_bound(int universe_size, int oracle_size) {
    if (oracle_size <= 0 || oracle_size > universe_size) return 0.0;
    double ratio = (double)universe_size / (double)oracle_size;
    return (double)oracle_size * log2(ratio > 1.0 ? ratio : 1.0);
}

int estimate_relativizing_queries(int input_size, int oracle_size) {
    int rem = oracle_size, q = 0, bound = input_size * input_size + 100;
    while (rem > 1 && q < bound) { rem = (rem + 1) / 2; q++; }
    return q;
}

/* ---- Non-Relativizing Results Inventory ---- */

typedef struct {
    const char* result;
    const char* technique;
    int         year;
    const char* authors;
} NonRelResult;

static const NonRelResult NR[] = {
    {"IP = PSPACE",             "Arithmetization",       1990, "Shamir"},
    {"MIP = NEXP",              "Multi-prover IP",       1991, "Babai-Fortnow-Lund"},
    {"PCP Theorem",             "PCP + composition",     1998, "ALMSS"},
    {"PARITY notin AC0",        "Switching Lemma",       1984, "Furst-Saxe-Sipser"},
    {"NP notsubset P/poly",     "Circuit lower bounds",  1998, "Buhrmann et al."},
    {"NEXP notsubset ACC0",     "Circuit lower bounds",  2011, "Williams"},
    {"QIP = PSPACE",            "Quantum arithmetization",2010, "Jain-Ji-Upadhyay-Watrous"},
};

void non_relativizing_results_print(void) {
    int n = (int)(sizeof(NR) / sizeof(NR[0]));
    printf("\n=== Non-Relativizing Results ===\n\n");
    printf("%-28s %-20s %6s  %s\n", "Result", "Technique", "Year", "Authors");
    for (int i = 0; i < 70; i++) putchar('-');
    printf("\n");
    for (int i = 0; i < n; i++)
        printf("%-28s %-20s %6d  %s\n",
               NR[i].result, NR[i].technique, NR[i].year, NR[i].authors);
    printf("\nAll above break through the relativization barrier.\n");
}
