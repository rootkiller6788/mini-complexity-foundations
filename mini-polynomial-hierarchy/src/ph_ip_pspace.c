/* ph_ip_pspace.c - Interactive Proofs and PSPACE (L8 Advanced)
 *
 * Shamir (1992): IP = PSPACE
 * This is a landmark result connecting interactive proofs with
 * the polynomial hierarchy and PSPACE.
 *
 * An interactive proof system has:
 *   - A prover (unbounded computational power)
 *   - A verifier (randomized polynomial-time)
 *   They exchange messages. The verifier accepts/rejects.
 *
 * IP = class of languages with interactive proofs.
 * 
 * Theorem (Lund-Fortnow-Karloff-Nisan 1992): PH subseteq IP
 * Theorem (Shamir 1992): IP = PSPACE
 *
 * This means: interaction + randomness = PSPACE power!
 * In contrast: NP = one-way proof (no interaction, no randomness).
 *
 * Connection to PH:
 *   - PH is in PSPACE (trivially, via TQBF)
 *   - PSPACE = IP, so PH has interactive proofs
 *   - The number of rounds relates to alternation depth
 */
#include "ph.h"
#include <string.h>

/* ================================================================
 * Simple Interactive Proof Simulator
 *
 * We simulate an interactive proof for a PSPACE-complete problem
 * (TQBF) using the sumcheck protocol approach.
 * ================================================================ */

/* Arithmetization: map a Boolean formula to a polynomial over GF(2) or integers.
 * For each clause (l1 | l2 | l3), use polynomial:
 *   1 - (1 - a1)(1 - a2)(1 - a3)  where ai = xi or (1-xi) for !xi
 * The formula is SAT iff the polynomial evaluates to 1 for some assignment. */

/* Evaluate the arithmetized formula over integers.
 * assign: integer array of 0/1 values.
 * Returns integer 0 or 1 (should be 1 if clause satisfied). */
static int arith_clause(const PH_Clause* c, const int* assign, int n_vars) {
    int prod = 1;
    for (int j = 0; j < 3; j++) {
        int lit = c->lits[j];
        if (lit < 0) continue;
        int var = lit >> 1;
        int sign = lit & 1;
        if (var >= n_vars) continue;
        int val = sign ? (1 - assign[var]) : assign[var];
        prod *= (1 - val);
    }
    return 1 - prod;
}

static int arith_formula(const PH_CNF* f, const int* assign) {
    int result = 1;
    for (int ci = 0; ci < f->n_clauses; ci++) {
        result *= arith_clause(&f->clauses[ci], assign, f->n_vars);
    }
    return result;
}

/* Sumcheck-inspired protocol for TQBF.
 * Key idea (simplified): transform a QBF into a sum over all assignments
 * of the arithmetized formula. The prover sends low-degree polynomials,
 * the verifier checks at random points.
 *
 * This is a SIMPLIFIED demonstration of the IP=PSPACE technique. */

/* Evaluate arithmetized QBF: for fixed-variable assignments,
 * compute the polynomial value recursively. */
static int arith_qbf_rec(const PH_QBF* qbf, int block_idx,
                         const int* assign) {
    int bs = qbf->blocks[block_idx].n_vars;
    int offset = qbf->blocks[block_idx].start_var;
    int is_last = (block_idx == qbf->n_blocks - 1);
    int is_exist = (qbf->blocks[block_idx].type == PH_QUANT_EXISTS);

    int total = 1 << bs;
    if (total > 256) total = 256;

    int* local_assign = malloc((size_t)qbf->total_vars * sizeof(int));
    if (!local_assign) return 0;
    memcpy(local_assign, assign, (size_t)qbf->total_vars * sizeof(int));

    int result;
    if (is_exist) {
        result = 0;
        for (int m = 0; m < total; m++) {
            for (int j = 0; j < bs; j++)
                local_assign[offset + j] = (m >> j) & 1;
            int val = is_last ? arith_formula(&qbf->matrix, local_assign)
                              : arith_qbf_rec(qbf, block_idx + 1, local_assign);
            if (val) { result = 1; break; }
        }
    } else {
        result = 1;
        for (int m = 0; m < total; m++) {
            for (int j = 0; j < bs; j++)
                local_assign[offset + j] = (m >> j) & 1;
            int val = is_last ? arith_formula(&qbf->matrix, local_assign)
                              : arith_qbf_rec(qbf, block_idx + 1, local_assign);
            if (!val) { result = 0; break; }
        }
    }
    free(local_assign);
    return result;
}

/* Interactive proof simulation: verifier checks prover's claims.
 * The verifier uses randomness to check polynomial identities.
 * Returns 1 if the interactive proof accepts. */
int ph_ip_verify_tqbf(const PH_QBF* qbf, int n_rounds) {
    /* Simplified IP verifier for TQBF.
     * In each round, the prover sends a low-degree polynomial P(x).
     * The verifier checks P(0) * P(1) = claimed value (exists) or
     * P(0) + P(1) = claimed value (forall), then picks random r and recurses.
     *
     * We simulate by evaluating with random assignments. */
    if (!qbf || qbf->n_blocks == 0) {
        /* Base case: evaluate the formula */
        int assign[PH_MAX_VARS] = {0};
        return arith_formula(&qbf->matrix, assign);
    }

    /* Simulate n_rounds of the interactive proof.
     * For each round, eliminate one quantifier via random evaluation. */
    int result = 0;
    srand((unsigned int)time(NULL));

    /* Use random sampling to simulate the sumcheck protocol.
     * For exists: evaluate at random point, if 1, claim SAT.
     * For forall: evaluate at random points, if all 1, claim SAT. */
    int bs = qbf->blocks[0].n_vars;
    int offset = qbf->blocks[0].start_var;
    int is_exist = (qbf->blocks[0].type == PH_QUANT_EXISTS);

    int* assign = calloc((size_t)qbf->total_vars, sizeof(int));
    if (!assign) return 0;

    if (is_exist) {
        /* Try random assignments for the first block */
        int found = 0;
        for (int trial = 0; trial < n_rounds * 10 && !found; trial++) {
            for (int j = 0; j < bs; j++)
                assign[offset + j] = rand() & 1;
            if (qbf->n_blocks == 1) {
                if (arith_formula(&qbf->matrix, assign)) found = 1;
            } else {
                /* Build restricted QBF and recurse */
                PH_QBF* rest = ph_qbf_restrict(qbf, assign);
                if (rest && ph_qbf_solve(rest)) found = 1;
                ph_qbf_free(rest);
            }
        }
        result = found;
    } else {
        /* Check many random assignments for the first block */
        int all_ok = 1;
        for (int trial = 0; trial < n_rounds * 10 && all_ok; trial++) {
            for (int j = 0; j < bs; j++)
                assign[offset + j] = rand() & 1;
            if (qbf->n_blocks == 1) {
                if (!arith_formula(&qbf->matrix, assign)) all_ok = 0;
            } else {
                PH_QBF* rest = ph_qbf_restrict(qbf, assign);
                if (!rest || !ph_qbf_solve(rest)) all_ok = 0;
                ph_qbf_free(rest);
            }
        }
        result = all_ok;
    }
    free(assign);
    return result;
}

/* ================================================================
 * Multi-Prover Interactive Proofs (MIP)
 *
 * MIP = class with multiple non-communicating provers.
 * Theorem (Babai-Fortnow-Lund 1991): MIP = NEXP.
 * This shows interaction with multiple provers is even more powerful.
 *
 * We provide a simplified conceptual demonstration.
 * ================================================================ */

/* Simulate MIP protocol: 2 provers, 1 verifier.
 * The verifier sends different questions to each prover.
 * If provers' answers are consistent, verifier accepts.
 *
 * This prevents provers from cheating via the "oracle separation" trick. */
int ph_mip_verify(int (*prover1)(int), int (*prover2)(int),
                  int n_questions, int* accept_count) {
    int accepted = 0;
    srand((unsigned int)time(NULL));

    for (int q = 0; q < n_questions && q < 100; q++) {
        /* Verifier generates two correlated questions */
        int seed = rand();
        int q1 = seed & 0xFF;
        int q2 = (seed >> 8) & 0xFF;

        /* Send questions to provers (they cannot communicate) */
        int a1 = prover1(q1);
        int a2 = prover2(q2);

        /* Verifier checks consistency */
        int consistent = (a1 == a2) || (q1 == q2);
        if (consistent) accepted++;
    }
    if (accept_count) *accept_count = accepted;
    return (accepted > n_questions / 2) ? 1 : 0;
}

/* ================================================================
 * Demo: Interactive Proofs and PSPACE
 * ================================================================ */

void ph_demo_ip(void) {
    printf("\n===== Interactive Proofs and PSPACE =====\n\n");
    printf("Theorem (Shamir 1992): IP = PSPACE\n");
    printf("Theorem (LFKN 1992): PH subseteq IP\n\n");

    printf("Interactive Proof System:\n");
    printf("  Prover (all-powerful) <-> Verifier (randomized poly-time)\n");
    printf("  Verifier accepts/rejects after k rounds of interaction.\n\n");

    printf("The arithmetization technique:\n");
    printf("  1. Map Boolean formula to polynomial over integers\n");
    printf("  2. Use sumcheck protocol to verify claims\n");
    printf("  3. Prover sends low-degree polynomials\n");
    printf("  4. Verifier checks at random points\n\n");

    /* Demonstrate arithmetization */
    printf("--- Arithmetization Demo ---\n");
    PH_CNF* f = ph_cnf_build_single(0, 0, 1, 0, 2, 0);
    f->n_vars = 3;
    int assign_true[] = {1, 1, 1};
    int assign_false[] = {0, 0, 0};
    printf("Formula: (x0|x1|x2)\n");
    printf("  Arith(111) = %d (expected 1)\n", arith_formula(f, assign_true));
    printf("  Arith(000) = %d (expected 0)\n", arith_formula(f, assign_false));
    free(f);

    /* IP verification of simple QBF */
    printf("\n--- IP Verification Demo ---\n");
    PH_CNF* f2 = ph_cnf_build_single(0, 0, 0, 0, -1, -1);
    PH_QuantType t[2] = {PH_QUANT_EXISTS, PH_QUANT_FORALL};
    int s[2] = {1, 1};
    PH_QBF* q = ph_qbf_create(2, t, s, f2);
    int ip_result = ph_ip_verify_tqbf(q, 3);
    int exact = ph_qbf_solve(q);
    printf("  QBF: exists x0 forall x1: x0\n");
    printf("  IP result: %s\n", ip_result ? "ACCEPT" : "REJECT");
    printf("  Exact: %s\n", exact ? "TRUE" : "FALSE");
    ph_qbf_free(q);

    printf("\n--- PH subseteq PSPACE = IP ---\n");
    printf("Every level of PH can be verified by an interactive proof.\n");
    printf("The number of rounds corresponds to alternation depth.\n");
    printf("This is a profound connection between interaction,\n");
    printf("randomness, and computational complexity.\n");
}
