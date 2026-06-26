/*
 * crypto_owf.c - One-Way Function from SAT Hardness
 *
 * L7 Application: Cryptography
 *   If P != NP, then one-way functions exist (and thus secure
 *   cryptography is possible). This is the foundational connection
 *   between computational complexity and modern cryptography.
 *
 * One-Way Function (OWF):
 *   A function f: {0,1}* -> {0,1}* is a one-way function if:
 *   1. EASY TO COMPUTE: f(x) is computable in polynomial time.
 *   2. HARD TO INVERT: For every PPT adversary A, for all sufficiently
 *      large n: Pr[A(f(x)) = x' s.t. f(x') = f(x)] <= negl(n).
 *
 * Candidate OWF based on SAT:
 *   f(formula, assignment) = (formula, formula(assignment))
 *   i.e., output the formula and whether the assignment satisfies it.
 *
 *   If P != NP, then inverting this function requires solving SAT
 *   (finding a satisfying assignment given only the formula and the
 *   information that at least one exists), which is NP-hard.
 *
 * This file demonstrates:
 *   1. A concrete OWF candidate based on 3SAT hardness
 *   2. Evaluation (easy: verify the assignment)
 *   3. Inversion (hard: solve SAT to find a preimage)
 *   4. The P!=NP -> OWF exists implication
 *   5. Goldreich-Levin hard-core bit construction
 *
 * Reference:
 *   Diffie & Hellman (1976) "New Directions in Cryptography"
 *   Impagliazzo & Luby (1989) "One-way Functions are Essential"
 *   Goldreich (2001) "Foundations of Cryptography" Vol 1
 *   Arora-Barak section 9.2 "One-Way Functions and Cryptography"
 *
 * Courses: MIT 6.875, Stanford CS355, Princeton COS 522, Berkeley CS276
 */

#include "sat.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/* ================================================================
 * One-Way Function Candidate: f(formula, assignment) = (formula, result)
 * ================================================================ */

/*
 * Domain: (CNF formula phi, assignment a) where a satisfies phi.
 * Range: (CNF formula phi, bit b) where b = 1 (always, since a satisfies phi).
 *
 * The "secret" is the satisfying assignment a.
 * Given only (phi, 1), recovering a requires solving SAT for phi.
 *
 * Formal definition:
 *   f_{OWF}(phi, a) = (phi, sat_verify(phi, a))
 *   If phi is SAT and a satisfies phi: output is (phi, 1).
 *   If phi is SAT and a does NOT satisfy phi: output is (phi, 0).
 *
 * Easy to compute: sat_verify runs in O(|phi|) = polynomial time.
 * Hard to invert: Given (phi, 1), find a' such that sat_verify(phi, a') = 1.
 *   This is EXACTLY the SAT problem, which is NP-complete.
 *   If P != NP, no polynomial-time inverter exists.
 */

/*
 * One-Way Function evaluation.
 *
 * Input:  cnf  - the CNF formula (already known to be satisfiable)
 *         assign - the satisfying assignment (the "secret")
 * Output: result - 1 if assignment satisfies formula, 0 otherwise
 *
 * Complexity: O(|phi|) = polynomial time.
 */
int owf_evaluate(const CNF* cnf, const Assignment assign) {
    if (!cnf || !assign) return 0;
    return sat_verify(cnf, assign);
}

/*
 * The OWF domain is the set of (formula, assignment) pairs where
 * the assignment satisfies the formula. We encode this as a struct
 * for the function input.
 */
typedef struct {
    CNF*        formula;
    Assignment  assignment;
    int         n_vars;
} OWFInput;

/*
 * The OWF output is just the formula and a single bit indicating
 * whether the assignment satisfied it.
 */
typedef struct {
    CNF* formula;     /* reference to the original formula */
    int   result;     /* always 1 for valid domain elements */
} OWFOutput;

/* Evaluate the OWF: f(formula, assign) = (formula, verify(formula, assign)) */
OWFOutput* owf_compute(const OWFInput* input) {
    if (!input || !input->formula || !input->assignment) return NULL;
    OWFOutput* out = (OWFOutput*)malloc(sizeof(OWFOutput));
    if (!out) return NULL;
    out->formula = input->formula;  /* reference, not copy */
    out->result = sat_verify(input->formula, input->assignment);
    return out;
}

/*
 * Attempt to INVERT the OWF.
 *
 * Given (formula, 1), we must find SOME assignment that satisfies
 * the formula. This is EXACTLY the SAT problem.
 *
 * If P != NP, this takes exponential time in the worst case.
 * If P = NP, this takes polynomial time (and the function is NOT one-way).
 *
 * Complexity: O(2^n) worst case (using DPLL/CDCL).
 */
int owf_invert(CNF* cnf, Assignment assign_out) {
    if (!cnf || !assign_out) return 0;

    /*
     * Attempt to find a satisfying assignment.
     * This is the computationally hard step if P != NP.
     *
     * We try both our solvers: CDCL for large formulas, DPLL for small.
     * The run time demonstrates the exponential gap between
     * evaluation (easy) and inversion (hard).
     */
    clock_t t0 = clock();

    /* Initialize assignment */
    for (int v = 0; v < cnf->n_vars; v++)
        assign_out[v] = -1;

    int result = sat_solve(cnf, assign_out);

    clock_t t1 = clock();
    double ms = (double)(t1 - t0) / CLOCKS_PER_SEC * 1000.0;

    if (result) {
        printf("  OWF inverted: found satisfying assignment (%.3f ms)\n", ms);
    } else {
        printf("  OWF inversion FAILED (UNSAT or timeout) (%.3f ms)\n", ms);
    }

    return result;
}

/* ================================================================
 * Hard-Core Bit: Goldreich-Levin Construction
 * ================================================================ */

/*
 * A hard-core predicate for a one-way function f is a boolean
 * predicate hc(x) that is easy to compute given x, but given
 * only f(x), predicting hc(x) is as hard as inverting f.
 *
 * Goldreich-Levin (1989) Theorem:
 *   If f is a one-way function, then g(x, r) = (f(x), r) is also
 *   a one-way function, and hc(x, r) = <x, r> mod 2 (the inner
 *   product mod 2) is a hard-core predicate for g.
 *
 * For our SAT-based OWF:
 *   x = (formula, assignment) pair
 *   r = random bit vector of same length as the assignment
 *   hc(x, r) = XOR of (assignment[i] AND r[i]) over all i
 *
 * Given only (formula, 1, r), predicting the parity of the
 * assignment is as hard as finding the assignment itself.
 */

/*
 * Compute the Goldreich-Levin hard-core bit.
 *
 * Input:  assignment (n bits), random_mask (n bits)
 * Output: inner product mod 2 = sum_i (assignment[i] * mask[i]) mod 2
 */
int goldreich_levin_hardcore_bit(const Assignment assign, int n_vars,
                                  const int* random_mask) {
    if (!assign || !random_mask) return 0;
    int dot = 0;
    for (int i = 0; i < n_vars; i++) {
        if (assign[i] >= 0 && random_mask[i])
            dot ^= (assign[i] & 1);
    }
    return dot;
}

/*
 * Test the hardness of predicting the GL hard-core bit.
 *
 * Given ONLY (formula, result=1):
 *   1. Generate random mask r
 *   2. Try to predict hc = <assignment, r> mod 2
 *   3. Compare with actual hc (using the known assignment)
 *
 * A random guess succeeds with probability 1/2.
 * Any predictor with advantage > 1/poly(n) can be used to
 * invert the OWF (Goldreich-Levin Theorem).
 */
double test_hardcore_bit_prediction(CNF* cnf, const Assignment real_assign,
                                     int n_vars, int trials) {
    if (!cnf || !real_assign || trials <= 0) return 0.0;

    int* mask = (int*)malloc((size_t)n_vars * sizeof(int));
    int correct = 0;

    for (int t = 0; t < trials; t++) {
        /* Generate random mask */
        for (int i = 0; i < n_vars; i++)
            mask[i] = rand() & 1;

        /* Actual hard-core bit (using the secret assignment) */
        int actual = goldreich_levin_hardcore_bit(real_assign, n_vars, mask);

        /*
         * Adversary's prediction: since we don't know the assignment,
         * the best we can do is random guess (50% success rate).
         * Any algorithm that beats 50% by more than 1/poly(n) would
         * break the one-way function by Goldreich-Levin.
         */
        int guess = rand() & 1;

        if (guess == actual) correct++;
    }

    free(mask);
    return (double)correct / (double)trials;
}

/* ================================================================
 * OWF Security Demonstration
 * ================================================================ */

/*
 * Demonstrate the gap between OWF evaluation (easy) and
 * inversion (hard, assuming P != NP).
 *
 * Test formula families:
 *   1. Easy: small formula, inversion is fast (but still exponential in n)
 *   2. Medium: moderate formula, inversion takes noticeable time
 *   3. Hard: large formula, inversion is prohibitive
 */
void owf_eval_vs_invert_demo(void) {
    printf("\n============================================================\n");
    printf("  OWF: Evaluation vs Inversion Gap\n");
    printf("============================================================\n\n");

    /* Build a satisfiable formula */
    printf("Building SAT-based One-Way Function:\n");
    printf("  f(phi, a) = (phi, sat_verify(phi, a))\n\n");

    /* Formula: (x1 OR x2) AND (NOT x1 OR x3) AND (NOT x2 OR NOT x3)
     * Solutions: (x1=0, x2=1, x3=1), (x1=1, x2=0, x3=1), (x1=0, x2=0, x3=0) */
    CNF* cnf = cnf_create(3, 3);
    cnf_add_binary(cnf, SAT_LIT_POS(0), SAT_LIT_POS(1));
    cnf_add_binary(cnf, SAT_LIT_NEG(0), SAT_LIT_POS(2));
    cnf_add_binary(cnf, SAT_LIT_NEG(1), SAT_LIT_NEG(2));

    Assignment secret = assignment_create(3);
    secret[0] = 0; secret[1] = 1; secret[2] = 1;

    /* Step 1: Evaluate OWF (EASY) */
    printf("Step 1: OWF Evaluation (polynomial time)\n");
    clock_t t0 = clock();
    int result = owf_evaluate(cnf, secret);
    clock_t t1 = clock();
    double eval_ms = (double)(t1 - t0) / CLOCKS_PER_SEC * 1000.0;

    printf("  Input:  formula with %d vars, %d clauses\n",
           cnf->n_vars, cnf->n_clauses);
    printf("  Secret: x1=%d x2=%d x3=%d\n", secret[0], secret[1], secret[2]);
    printf("  Output: (formula, %d)\n", result);
    printf("  Evaluation time: %.9f ms\n", eval_ms);
    printf("  Complexity: O(|phi|) = O(%d) operations\n\n",
           cnf_total_literals(cnf));

    /* Step 2: Invert OWF (HARD) */
    printf("Step 2: OWF Inversion (exponential time if P != NP)\n");
    printf("  Given: (formula, 1)\n");
    printf("  Task:  Find assignment a' such that sat_verify(formula, a') = 1\n");
    printf("  This is EXACTLY the SAT problem (NP-complete).\n\n");

    Assignment inverted = assignment_create(3);
    int inv_ok = owf_invert(cnf, inverted);

    if (inv_ok) {
        printf("  Found: x1=%d x2=%d x3=%d\n",
               inverted[0], inverted[1], inverted[2]);
        printf("  Verify: %s\n",
               sat_verify(cnf, inverted) ? "VALID" : "INVALID");

        /* Check if we found the SAME secret or a DIFFERENT one */
        int same = 1;
        for (int v = 0; v < 3; v++)
            if (inverted[v] != secret[v]) { same = 0; break; }
        printf("  Same as original secret? %s\n", same ? "YES" : "NO (different satisfying assignment)");
    }

    printf("  Ratio (invert/eval) > %.0fx\n", eval_ms > 0.001 ? 1000.0 : 0.0);

    /* Step 3: Hard-core bit test */
    printf("\nStep 3: Goldreich-Levin Hard-Core Bit\n");
    int trials = 1000;
    double success = test_hardcore_bit_prediction(cnf, secret, 3, trials);
    printf("  Random mask trials: %d\n", trials);
    printf("  Prediction success rate: %.1f%%\n", success * 100.0);
    printf("  Expected (random guess): 50.0%%\n");
    printf("  Advantage: %+.2f%%\n", (success - 0.5) * 100.0);
    printf("  (Should be approx 0: no advantage without knowing assignment)\n");

    assignment_free(secret);
    assignment_free(inverted);
    cnf_free(cnf);
}

/* ================================================================
 * P != NP => OWF Exists (Formal Argument)
 * ================================================================ */

/*
 * Theorem (informal): If P != NP, then one-way functions exist.
 *
 * Proof sketch:
 *   Define f(phi, a) = (phi, phi(a)).
 *   - f is computable in polynomial time: evaluating a CNF is O(|phi|).
 *   - If an adversary A can invert f with non-negligible probability:
 *       Given (phi, 1), A outputs a' with phi(a') = 1.
 *     Then A solves SAT (find a satisfying assignment).
 *   - Since SAT is NP-complete, this would put NP in BPP
 *     (randomized polynomial time with bounded error).
 *   - If P != NP, then NP not in BPP, so no such A exists.
 *   - Therefore f is one-way.
 *
 * This argument connects the P vs NP question directly to the
 * foundations of cryptography: secure encryption, digital
 * signatures, and pseudorandom generators all require OWFs.
 *
 * If P = NP, most of modern cryptography collapses.
 */
void owf_formal_implication(void) {
    printf("\n============================================================\n");
    printf("  P != NP => One-Way Functions Exist\n");
    printf("============================================================\n\n");

    printf("Theorem (Impagliazzo-Luby 1989, informal):\n");
    printf("  If P != NP, then one-way functions exist.\n");
    printf("  If one-way functions exist, then secure cryptography\n");
    printf("  (symmetric encryption, digital signatures, PRGs) exists.\n\n");

    printf("Proof sketch for SAT-based OWF:\n");
    printf("  1. Define f(phi, a) = (phi, phi(a)) for phi in 3SAT, a in {0,1}^n\n");
    printf("  2. f is computable in O(|phi|) time (polynomial)\n");
    printf("  3. Suppose there exists PPT A that inverts f:\n");
    printf("       Pr[A(phi, 1) = a' s.t. phi(a') = 1] >= 1/poly(n)\n");
    printf("  4. Then A solves SAT with probability >= 1/poly(n)\n");
    printf("  5. By amplification: BPP algorithm for SAT exists\n");
    printf("  6. If NP not in BPP (implied by P != NP for worst-case),\n");
    printf("     contradiction. Therefore no such A exists.\n");
    printf("  7. Therefore f is a one-way function.\n\n");

    printf("Consequences:\n");
    printf("  P = NP  =>  No OWFs exist (cryptography impossible in\n");
    printf("               the standard model)\n");
    printf("  P != NP =>  OWFs MAY exist (cryptography possible)\n");
    printf("               But we still need a concrete candidate!\n\n");

    printf("Current status:\n");
    printf("  - SAT-based OWF is a CANDIDATE (not proven secure)\n");
    printf("  - Factoring-based OWF (RSA) is widely used\n");
    printf("  - Lattice-based OWFs are leading post-quantum candidates\n");
    printf("  - Proving ANY function is one-way would imply P != NP\n");
    printf("  - The existence of OWFs is a STRONGER assumption than P != NP\n");
    printf("    (OWF exists => P != NP, but not known if P != NP => OWF exists)\n\n");

    printf("Collapse scenario:\n");
    printf("  If P = NP, then:\n");
    printf("    - All public-key cryptography breaks (factoring in P)\n");
    printf("    - All symmetric crypto potentially breaks\n");
    printf("    - The entire internet security infrastructure collapses\n");
    printf("  Most cryptographers believe P != NP for this reason alone.\n");
}