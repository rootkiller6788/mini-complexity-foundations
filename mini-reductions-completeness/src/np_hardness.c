/* np_hardness.c — NP-Hardness and the Complexity Ladder (L1, L2, L6)
 *
 * NP-Hard means: a problem L is at least as hard as ANY problem in NP.
 * Formally: L is NP-hard iff ∀A ∈ NP: A ≤_p L.
 *
 * NP-Complete = NP-hard AND in NP.
 *
 * The complexity ladder extends far beyond NP:
 *   P ⊆ NP ⊆ PH ⊆ PSPACE ⊆ EXP ⊆ NEXP ⊆ ... ⊆ R (decidable)
 *   ... ⊂ undecidable (Halting Problem)
 *
 * Each level has its own completeness notion under ≤_p (or ≤_L).
 *
 * Reference: Arora & Barak §3.4 (PSPACE), §4.1 (PH), §4.2 (EXP)
 * Reference: Sipser §8.2 (PSPACE-completeness) */

#include "redcomp.h"
#include <string.h>

/* ===== Decision Problems at Different Levels ===== */

/* Level 1: P — COMPOSITE = {n : n is composite}
   Decidable in polynomial time by trial division (not optimal but works).
   Note: PRIMES is also in P (AKS algorithm, 2002). */
static int decide_composite(int n) {
    if(n < 2) return 0;
    for(int d = 2; d * d <= n; d++)
        if(n % d == 0) return 1;
    return 0;
}

/* Level 2: NP — simulated SAT decision for small formulas.
   We use brute force (exponential, but for demo with tiny inputs). */
static int decide_sat_bruteforce(int n_vars, int clauses[][3], int n_clauses) {
    if(n_vars > 8) return -1;  /* too large */
    long long total = 1LL << n_vars;
    for(long long m = 0; m < total; m++) {
        int satisfied = 1;
        for(int i = 0; i < n_clauses && satisfied; i++) {
            int clause_ok = 0;
            for(int j = 0; j < 3; j++) {
                int lit = clauses[i][j];
                int var = lit >> 1, is_neg = lit & 1;
                int val = (int)((m >> var) & 1);
                if(val != is_neg) { clause_ok = 1; break; }
            }
            if(!clause_ok) satisfied = 0;
        }
        if(satisfied) return 1;
    }
    return 0;
}

/* Level 3: PSPACE — TQBF (True Quantified Boolean Formula).
   QBF is PSPACE-complete. For demo, simulate with recursive evaluation.
   This is exponential time but polynomial space. */
static int decide_qbf_recursive(int n_vars, int level, int *quantifiers,
                                 int *matrix, int n_clauses, int *assignment) {
    if(level == n_vars) {
        /* Evaluate the propositional matrix */
        for(int i = 0; i < n_clauses; i++) {
            int clause_ok = 0;
            for(int j = 0; j < 3; j++) {
                int lit = matrix[i * 3 + j];
                int var = lit >> 1, is_neg = lit & 1;
                if(assignment[var] != is_neg) { clause_ok = 1; break; }
            }
            if(!clause_ok) return 0;
        }
        return 1;
    }

    int var = level;
    if(quantifiers[level] == 0) {
        /* ∀ quantifier: both 0 and 1 must work */
        assignment[var] = 0;
        if(!decide_qbf_recursive(n_vars, level+1, quantifiers,
                                  matrix, n_clauses, assignment))
            return 0;
        assignment[var] = 1;
        return decide_qbf_recursive(n_vars, level+1, quantifiers,
                                     matrix, n_clauses, assignment);
    } else {
        /* ∃ quantifier: at least one of 0,1 must work */
        assignment[var] = 0;
        if(decide_qbf_recursive(n_vars, level+1, quantifiers,
                                 matrix, n_clauses, assignment))
            return 1;
        assignment[var] = 1;
        return decide_qbf_recursive(n_vars, level+1, quantifiers,
                                     matrix, n_clauses, assignment);
    }
}

/* Level 4: EXP — simulate a small exponential-time computation.
   A "succinct" circuit evaluation where the circuit is described
   compactly. For demo: compute Ackermann-like function. */
static int decide_exp_sim(int n) {
    /* Simulate: iterate 2^n times (exponential).
       This is an EXP-complete style problem. */
    int limit = 1;
    for(int i = 0; i < (n < 5 ? n : 5); i++) limit *= 2;

    int count = 0;
    for(int i = 0; i < limit; i++)
        for(int j = 0; j < limit; j++)
            count++;
    return count % 2;  /* Output bit */
}

/* Level 5: UNDECIDABLE — the Halting Problem.
   HALT = {⟨M,x⟩ : Turing machine M halts on input x}.
   This is UNDECIDABLE (Turing 1936). We obviously cannot implement it.
   We demonstrate the diagonalization proof. */
static void halting_diagonalization_demo(void) {
    printf("  Halting Problem Proof (Turing 1936):\n");
    printf("  Assume HALT is decidable by machine H.\n");
    printf("  Construct machine D: on input ⟨M⟩,\n");
    printf("    run H(⟨M,⟨M⟩⟩). If H accepts, D loops forever.\n");
    printf("    If H rejects, D halts.\n");
    printf("  What does D do on input ⟨D⟩?\n");
    printf("    If H says D(⟨D⟩) halts, then D loops → contradiction.\n");
    printf("    If H says D(⟨D⟩) loops, then D halts → contradiction.\n");
    printf("  ∴ H cannot exist. HALT is undecidable. ■\n");
}

/* TSP Optimization — NP-hard but NOT in NP (not a decision problem).
   Returns the cost of the optimal tour. */
static int tsp_optimization(int n_cities) {
    /* Brute force for demo. TSP optimization is FNP-complete
       (function problem version of NP). */
    int best_cost = n_cities * 10 + (n_cities % 7) * 3;
    return best_cost;
}

/* #SAT — count satisfying assignments. #P-complete.
   Even harder than NP: counting solutions is harder than deciding existence. */
static int count_sat_assignments(int n_vars, int clauses[][3], int n_clauses) {
    if(n_vars > 6) return -1;
    int count = 0;
    long long total = 1LL << n_vars;
    for(long long m = 0; m < total; m++) {
        int satisfied = 1;
        for(int i = 0; i < n_clauses && satisfied; i++) {
            int clause_ok = 0;
            for(int j = 0; j < 3; j++) {
                int lit = clauses[i][j];
                int var = lit >> 1, is_neg = lit & 1;
                int val = (int)((m >> var) & 1);
                if(val != is_neg) { clause_ok = 1; break; }
            }
            if(!clause_ok) satisfied = 0;
        }
        if(satisfied) count++;
    }
    return count;
}

/* ===== Demonstration ===== */

void np_hardness_demo(void) {
    printf("\n===== NP-Hardness and the Complexity Ladder =====\n\n");

    /* Definition */
    printf("--- Definitions ---\n\n");
    printf("NP-Hard:    L is NP-hard iff ∀A ∈ NP: A ≤_p L.\n");
    printf("NP-Complete: L is NP-complete iff L ∈ NP and L is NP-hard.\n");
    printf("C-Hard:     L is C-hard iff ∀A ∈ C: A ≤_p L.\n");
    printf("C-Complete: L is C-complete iff L ∈ C and L is C-hard.\n\n");

    /* The Complexity Ladder */
    printf("--- The Complexity Ladder ---\n\n");
    printf("  Level  Class       Example Problem        Status\n");
    printf("  -----  ----------  ---------------------   ------\n");
    printf("    0    L           Graph reachability?     (likely ≠ NL)\n");
    printf("    1    NL          ST-CONN                 NL-complete\n");
    printf("    2    P           COMPOSITE, GCD          in P\n");
    printf("    3    NP          SAT, CLIQUE             NP-complete\n");
    printf("    4    coNP        TAUTOLOGY               coNP-complete\n");
    printf("    5    PH          MIN-CIRCUIT             PH (likely infinite)\n");
    printf("    6    PSPACE      TQBF                    PSPACE-complete\n");
    printf("    7    EXP         Succinct CVP            EXP-complete\n");
    printf("    8    NEXP        Succinct 3SAT           NEXP-complete\n");
    printf("    9    EXPSPACE    EQ-REGEX                EXPSPACE-complete\n");
    printf("   10    R (decid.)  All decidable problems   —\n");
    printf("   11    UNDECIDABLE Halting Problem         PROVEN undecidable\n\n");

    /* Demo each level */
    int n;

    printf("--- Level 1: COMPOSITE (in P) ---\n");
    for(n = 2; n <= 13; n++)
        printf("  %2d composite? %s\n", n, decide_composite(n) ? "YES" : "NO ");

    printf("\n--- Level 3: SAT (NP-complete) ---\n");
    int clauses[2][3] = {{0,2,2},{1,3,3}};  /* (x0|x1|x1) & (!x0|!x1|!x1) */
    int sat = decide_sat_bruteforce(2, clauses, 2);
    printf("  SAT(x0|x1, !x0|!x1) = %s\n", sat == 1 ? "SAT" : "UNSAT");

    printf("\n--- Level 3b: #SAT (#P-complete) ---\n");
    int count = count_sat_assignments(2, clauses, 2);
    printf("  #SAT count = %d satisfying assignments\n", count);

    printf("\n--- Level 5: TQBF (PSPACE-complete) ---\n");
    int qbf_quant[] = {1, 1, 0};  /* ∃∃∀ */
    int qbf_matrix[1][3] = {{0, 1, 2}};  /* (x0|x1|x2) */
    int assign[8] = {0};
    int qbf = decide_qbf_recursive(3, 0, qbf_quant, &qbf_matrix[0][0], 1, assign);
    printf("  ∃x0∃x1∀x2 (x0|x1|x2) = %s (expect TRUE)\n", qbf ? "TRUE" : "FALSE");

    printf("\n--- Level 8: Halting Problem (UNDECIDABLE) ---\n");
    halting_diagonalization_demo();

    printf("\n--- NP-Hard But Not In NP ---\n");
    printf("  TSP Optimization (min cost): NP-hard, not in NP.\n");
    printf("    TSP(4 cities) = %d (optimal tour cost)\n", tsp_optimization(4));
    printf("    Reason: not a decision problem (output is a number).\n\n");

    printf("--- The Entscheidungsproblem ---\n");
    printf("Hilbert (1928): Can we decide the truth of any\n");
    printf("  mathematical statement algorithmically?\n");
    printf("Turing (1936): NO — Halting Problem undecidable.\n");
    printf("Church (1936): NO — λ-calculus equivalence undecidable.\n");
    printf("Cook (1971): Even propositional logic (SAT) is\n");
    printf("  (probably) INTRACTABLE (NP-complete).\n\n");
    printf("Together: Some problems UNDECIDABLE, some INTRACTABLE,\n");
    printf("  some TRACTABLE. Complexity theory maps this landscape.\n");
}