/* alternating_pspace.c — AP = PSPACE (Chandra-Kozen-Stockmeyer 1981)
 *
 * Alternating Polynomial Time = PSPACE.
 * An Alternating TM can switch between existential (OR) and
 * universal (AND) states during computation.
 *
 * This is analogous to: QBF is PSPACE-complete, and QBF is exactly
 * the problem of evaluating an alternating formula.
 *
 * ATIME(poly, bounded alts) = PH (polynomial hierarchy)
 * ATIME(poly, unbounded alts) = PSPACE
 * ALOGSPACE = P */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Simulate an Alternating TM for PSPACE.
   The ATM has:
   - Existential states: accept if SOME branch accepts
   - Universal states: accept if ALL branches accept
   - Poly-space work tape

   For demo: decide TQBF-style formula evaluation.
   QBF = "Q1 x1 Q2 x2 ... Qn xn: phi(x1,...,xn)"
   ATM evaluates this recursively with alternation. */

static int atm_eval_qbf(const int* clauses, int n_clauses,
                         int n_vars, int var_idx, int is_exist,
                         int* current_assign) {
    int total_vars = n_vars;
    if (var_idx >= total_vars) {
        /* All variables assigned: evaluate propositional part */
        for (int ci = 0; ci < n_clauses; ci++) {
            int clause_sat = 0;
            for (int j = 0; j < 3; j++) {
                int lit = clauses[ci * 3 + j];
                if (lit == 0) continue;
                int var = (lit > 0 ? lit : -lit) - 1;
                int val_needed = lit > 0 ? 1 : 0;
                if (current_assign[var] == val_needed) {
                    clause_sat = 1; break;
                }
            }
            if (!clause_sat) return 0;
        }
        return 1;
    }

    /* Try both assignments for the current variable */
    current_assign[var_idx] = 0;
    int r0 = atm_eval_qbf(clauses, n_clauses, n_vars,
                           var_idx + 1, !is_exist, current_assign);

    current_assign[var_idx] = 1;
    int r1 = atm_eval_qbf(clauses, n_clauses, n_vars,
                           var_idx + 1, !is_exist, current_assign);

    if (is_exist) return r0 || r1;  /* EXISTS: accept if either does */
    else          return r0 && r1;  /* FORALL: accept if both do */
}

void alt_pspace_demo(void) {
    printf("\n===== AP = PSPACE =====\n\n");
    printf("Chandra-Kozen-Stockmeyer (1981):\n");
    printf("  Alternating Polynomial Time = PSPACE.\\n\\n");

    printf("An Alternating TM has TWO types of states:\n");
    printf("  EXISTENTIAL (OR):  accept if SOME child branch accepts\n");
    printf("  UNIVERSAL (AND):   accept if ALL child branches accept\\n\\n");

    printf("--- Theorem ---\\n");
    printf("  ATIME(poly, k alts, start EXISTS) = Sigma_k\\n");
    printf("  ATIME(poly, k alts, start FORALL)  = Pi_k\\n");
    printf("  ATIME(poly, unbounded alts)        = PSPACE\\n");
    printf("  ALOGSPACE                           = P\\n\\n");

    /* Demonstrate QBF evaluation as ATM */
    printf("--- Demo: ATM for QBF ---\\n");
    /* phi = (x1 OR x2) AND (NOT x1 OR x2). QBF: forall x1 exists x2 phi.
       This is FALSE (when x1=1, x2 must be 0 to satisfy NOT x1 OR x2,
       but then x1 OR x2 = 1 OR 0 = 1, so it IS satisfiable).
       Let's use: exists x1 forall x2: (x1 AND x2) OR (NOT x1).
       When x1=0: (0 AND x2) OR 1 = 1. So exists x1=0 works. SAT! */
    int clauses[] = {
        1,  2,  0,   /* x1 OR x2 */
        -1, 2,  0    /* NOT x1 OR x2 */
    };
    int assign[4] = {0};
    int result = atm_eval_qbf(clauses, 2, 2, 0, 1, assign);
    printf("Formula: (x1 OR x2) AND (NOT x1 OR x2)\\n");
    printf("QBF: exists x1 forall x2: phi(x1,x2)\\n");
    printf("ATM result: %s\\n\\n", result ? "TRUE" : "FALSE");

    printf("--- AP = PSPACE: Why? ---\\n");
    printf("TQBF is PSPACE-complete. An ATM can evaluate TQBF\\n");
    printf("by alternating between EXISTS and FORALL for each quantifier.\\n");
    printf("With unbounded alternations, the ATM evaluates the ENTIRE\\n");
    printf("QBF formula, solving any PSPACE-complete problem.\\n\\n");

    printf("--- Significance ---\\n");
    printf("AP = PSPACE unifies:\\n");
    printf("  - Alternation (ATM)\\n");
    printf("  - Games (2-player perfect-information)\\n");
    printf("  - PSPACE (polynomial space)\\n");
    printf("  - QBF (quantified Boolean formulas)\\n");
    printf("All are EQUIVALENT characterizations of the same class.\\n");
}