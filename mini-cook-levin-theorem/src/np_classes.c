/*
 * np_classes.c — P, NP, NP-complete Class Definitions and Theory
 *
 * L1-L2: Formal definitions of complexity classes, their relations,
 *   and the polynomial-time hierarchy. Also covers Ladner's Theorem
 *   (NP-intermediate problems if P != NP) and oracle separations.
 *
 * Reference: Arora-Barak §1-5, Sipser §7, Papadimitriou
 * Courses: MIT 6.045/6.841, Stanford CS254, Berkeley CS172, CMU 15-855
 */

#include "turing_machine.h"
#include "sat.h"
#include "complexity.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/* ── Extended Complexity Class Enum ────────────────────────── */

typedef enum {
    CLS_P,
    CLS_NP,
    CLS_CONP,
    CLS_NPC,
    CLS_NPH,
    CLS_NPI,
    CLS_PSPACE,
    CLS_NPSPACE,
    CLS_EXP,
    CLS_NEXP,
    CLS_EXPSPACE,
    CLS_PH,
    CLS_SIGMA2,
    CLS_PI2,
    CLS_DELTA2,
    CLS_COUNT
} ComplexityClassExt;

static const char* class_names[CLS_COUNT] = {
    "P", "NP", "coNP", "NP-complete", "NP-hard",
    "NP-intermediate", "PSPACE", "NPSPACE", "EXP", "NEXP",
    "EXPSPACE", "PH", "Sigma_2^p", "Pi_2^p", "Delta_2^p"
};

const char* complexity_class_name_ext(ComplexityClassExt c) {
    if (c >= 0 && c < CLS_COUNT) return class_names[c];
    return "UNKNOWN";
}

/* ── Class Hierarchy ──────────────────────────────────────── */

void print_class_hierarchy(void) {
    printf("\n=== Complexity Class Hierarchy ===\n\n");
    printf("Known containments (all proven):\n");
    printf("  P subseteq NP subseteq Sigma_2^p subseteq ... subseteq PH\n");
    printf("  PH subseteq PSPACE = NPSPACE  (Savitch's Theorem)\n");
    printf("  PSPACE subseteq EXP subseteq NEXP subseteq EXPSPACE\n\n");
    printf("Known separations (Time Hierarchy):\n");
    printf("  P != EXP    (deterministic Time Hierarchy)\n");
    printf("  NP != NEXP  (nondeterministic Time Hierarchy)\n");
    printf("  PSPACE != EXPSPACE  (space hierarchy)\n\n");
    printf("Open problems:\n");
    printf("  P vs NP          (Clay Millennium Prize)\n");
    printf("  NP vs coNP\n");
    printf("  PH vs PSPACE\n");
    printf("  EXP vs NEXP\n");
}

/* ── Ladner's Theorem (1975) ────────────────────────────────── */

/*
 * If P != NP, there exist NP-intermediate languages
 * (neither in P nor NP-complete).
 *
 * Proof uses "delayed diagonalization" with slow-growing f(n) = log* n.
 * Candidate NP-intermediate problems: Graph Isomorphism, Factoring.
 */

double ladner_blowup_function(double n) {
    double r = 0.0;
    while (n > 1.0) { n = log2(n); r += 1.0; }
    return r;
}

int ladner_is_sparse_length(int n) {
    double f = ladner_blowup_function((double)n);
    double int_part;
    double frac = modf(f, &int_part);
    return (frac < 0.001);
}

void ladner_theorem_print(void) {
    printf("\n=== Ladner's Theorem (1975) ===\n\n");
    printf("Theorem: If P != NP, then NP contains languages that\n");
    printf("  are neither in P nor NP-complete (NP-intermediate).\n\n");
    printf("Proof: Delayed diagonalization with f(n) = log* n.\n");
    printf("  The language LAD is:\n");
    printf("    - In NP (obvious from construction)\n");
    printf("    - Not in P (diagonalizes against all P-machines)\n");
    printf("    - Not NP-complete (sparse encoding prevents reduction)\n\n");
    printf("Candidate NP-intermediate problems:\n");
    printf("  - Graph Isomorphism\n");
    printf("  - Integer Factoring (decision)\n");
    printf("  - Discrete Logarithm\n");
    printf("  - Shortest Vector Problem (lattice)\n\n");
    printf("Note: All are in NP intersect coNP, so if NP != coNP,\n");
    printf("  they cannot be NP-complete.\n");
}

/* ── Polynomial Hierarchy Structure ───────────────────────── */

typedef struct {
    int    level;
    char*  sigma;
    char*  pi;
    char*  delta;
} PHLevel;

PHLevel ph_level_desc(int k) {
    PHLevel ph;
    ph.level = k;
    ph.sigma = (char*)malloc(32);
    ph.pi    = (char*)malloc(32);
    ph.delta = (char*)malloc(32);
    if (k == 0) {
        strcpy(ph.sigma, "P"); strcpy(ph.pi, "P"); strcpy(ph.delta, "P");
    } else if (k == 1) {
        strcpy(ph.sigma, "NP"); strcpy(ph.pi, "coNP"); strcpy(ph.delta, "P^NP");
    } else if (k == 2) {
        strcpy(ph.sigma, "Sigma_2^p"); strcpy(ph.pi, "Pi_2^p"); strcpy(ph.delta, "Delta_2^p");
    } else {
        sprintf(ph.sigma, "Sigma_%d^p", k);
        sprintf(ph.pi, "Pi_%d^p", k);
        sprintf(ph.delta, "Delta_%d^p", k);
    }
    return ph;
}

void ph_level_free(PHLevel* ph) {
    if (!ph) return;
    free(ph->sigma); free(ph->pi); free(ph->delta);
}

void ph_print_hierarchy(int max_level) {
    printf("\n=== Polynomial Hierarchy ===\n\n");
    if (max_level < 0) max_level = 0;
    if (max_level > 5) max_level = 5;
    for (int k = 0; k <= max_level; k++) {
        PHLevel ph = ph_level_desc(k);
        printf("Level %d: %-20s %-20s %-20s\n", k, ph.sigma, ph.pi, ph.delta);
        ph_level_free(&ph);
    }
    printf("\n  PH = union_k Sigma_k^p\n");
    printf("  If P = NP => PH collapses to P.\n");
    printf("  If Sigma_k^p = Pi_k^p for some k => PH collapses to level k.\n");
}

/* ── Oracle Separation Results ────────────────────────────── */

typedef struct {
    const char* question;
    const char* oracle_result;
    int         does_relativize;
} OracleResult;

static const OracleResult ORACLE_RESULTS[] = {
    {"P vs NP",     "exists A: P^A = NP^A; exists B: P^B != NP^B", 1},
    {"NP vs coNP",  "exists A: NP^A = coNP^A; exists B: NP^B != coNP^B", 1},
    {"PH vs PSPACE","exists A: PH^A != PSPACE^A", 1},
    {"IP vs PSPACE","IP = PSPACE does NOT relativize!", 0},
    {"MIP vs NEXP", "MIP = NEXP does NOT relativize!", 0},
};

void oracle_separation_table_print(void) {
    int n = (int)(sizeof(ORACLE_RESULTS) / sizeof(ORACLE_RESULTS[0]));
    printf("\n=== Oracle Separation Results ===\n\n");
    printf("%-15s %-55s %s\n", "Question", "Oracle Result", "Relativizes?");
    for (int i = 0; i < 80; i++) putchar('-');
    printf("\n");
    for (int i = 0; i < n; i++) {
        printf("%-15s %-55s %s\n",
               ORACLE_RESULTS[i].question,
               ORACLE_RESULTS[i].oracle_result,
               ORACLE_RESULTS[i].does_relativize ? "Yes" : "NO");
    }
    printf("\nResults that do NOT relativize break the BGS barrier.\n");
}

/* ── Complete Demo ────────────────────────────────────────── */

void np_classes_full_demo(void) {
    print_class_hierarchy();
    ladner_theorem_print();
    ph_print_hierarchy(2);
    oracle_separation_table_print();
}
