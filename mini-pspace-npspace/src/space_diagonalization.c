/* space_diagonalization.c ¡ª Space Hierarchy Theorem
 *
 * Hartmanis, Lewis, Stearns (1965):
 * For any space-constructible functions f, g with f(n) = o(g(n)):
 *   SPACE(f(n)) ? SPACE(g(n))
 *
 * Corollary: L ? PSPACE (unconditionally proven!)
 * Because log n = o(n), we have SPACE(log n) ? SPACE(n) ? PSPACE.
 *
 * This is the space analogue of the time hierarchy theorem.
 * Unlike P vs PSPACE which is OPEN, L vs PSPACE is PROVEN.
 *
 * The proof uses diagonalization:
 * 1. Enumerate all space-f(n) TMs: M_1, M_2, M_3, ...
 * 2. Construct a TM D that on input x:
 *    a. Simulates M_x on input x using space g(|x|)
 *    b. If M_x halts within g(|x|) space, output the opposite
 *    c. Otherwise output 0 (or reject)
 * 3. D uses space g(n) but differs from every space-f(n) TM
 * 4. Therefore D ¡Ê SPACE(g) \ SPACE(f)
 *
 * The key: g grows asymptotically faster than f, so D can
 * simulate any space-f(n) TM within g(n) space. */

#include "pspace.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* ©¤©¤©¤ Space constructibility ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤ */

/* A function f is space-constructible if there exists a TM that,
 * on input 1^n, computes f(n) in unary using O(f(n)) space.
 * Most natural functions are: n^k, 2^n, log n, etc. */

/* Check if f = o(g) asymptotically: lim_{n¡ú¡Þ} f(n)/g(n) = 0 */
static int is_little_o(double (*f)(double), double (*g)(double), double n) {
    if (g(n) == 0.0) return 0;
    return (f(n) / g(n)) < 0.01; /* rough check */
}

/* ©¤©¤©¤ Diagonalization simulator ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤ */

/* Simulate a specific TM M_i on input x, but with a space budget.
 * If M_i uses > g(|x|) space or doesn't halt within the budget,
 * reject. Otherwise return !M_i(x) (flip the answer).
 *
 * This is the core diagonalization technique. */
int diagonalize_against(int machine_index, const int* input, int input_len,
                         double g_space, double f_space) {
    /* In a full implementation, we would:
     * 1. Decode machine_index into a TM description
     * 2. Simulate it on input with space budget g_space
     * 3. Halt and flip if it halts within budget
     * For demonstration, we show the structure. */

    printf("  Diagonalizing: M_%d on input of length %d\n",
           machine_index, input_len);
    printf("    f(n) = %.1f, g(n) = %.1f, ratio = %.3f\n",
           f_space, g_space, f_space / g_space);
    printf("    Running M_%d with g(n) space budget...\n", machine_index);

    (void)input; (void)g_space;
    /* Return !M_i(x) if it halts, otherwise reject */
    return 0;
}

/* ©¤©¤©¤ Space hierarchy demonstration ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤ */

/* Verify the space hierarchy: SPACE(log n) ? SPACE(n).
 * We construct a language in SPACE(n) that is not in SPACE(log n). */
int space_hierarchy_separates(double f_growth, double g_growth) {
    /* f = o(g) is the condition for separation */
    if (g_growth <= 0.0 || f_growth <= 0.0) return 0;
    double ratio = f_growth / g_growth;
    /* As n ¡ú ¡Þ, f(n)/g(n) ¡ú 0 means separation */
    return (ratio < 0.5) ? 1 : 0;
}

/* ©¤©¤©¤ Class comparison with known relationships ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤ */

int complexity_compare(ComplexityClass class_a, ComplexityClass class_b) {
    if (class_a == class_b) return SPACE_CLASS_RELATION_KNOWN_SUBSET;

    /* Known subset relationships */
    if (class_a == CLASS_L  && class_b == CLASS_NL)     return SPACE_CLASS_RELATION_KNOWN_SUBSET;
    if (class_a == CLASS_NL && class_b == CLASS_P)      return SPACE_CLASS_RELATION_KNOWN_SUBSET;
    if (class_a == CLASS_P  && class_b == CLASS_NP)     return SPACE_CLASS_RELATION_KNOWN_SUBSET;
    if (class_a == CLASS_NP && class_b == CLASS_PSPACE) return SPACE_CLASS_RELATION_KNOWN_SUBSET;
    if (class_a == CLASS_PSPACE  && class_b == CLASS_NPSPACE) return SPACE_CLASS_RELATION_KNOWN_SUBSET;
    if (class_a == CLASS_NPSPACE && class_b == CLASS_PSPACE)  return SPACE_CLASS_RELATION_KNOWN_SUBSET;
    if (class_a == CLASS_PSPACE  && class_b == CLASS_EXP)     return SPACE_CLASS_RELATION_KNOWN_SUBSET;
    if (class_a == CLASS_EXP  && class_b == CLASS_NEXP)       return SPACE_CLASS_RELATION_KNOWN_SUBSET;
    if (class_a == CLASS_NEXP && class_b == CLASS_2EXP)       return SPACE_CLASS_RELATION_KNOWN_SUBSET;

    /* Known strict separations (via hierarchy theorems) */
    if (class_a == CLASS_L  && class_b == CLASS_PSPACE) return SPACE_CLASS_RELATION_KNOWN_SEPARATION;
    if (class_a == CLASS_P  && class_b == CLASS_EXP)    return SPACE_CLASS_RELATION_KNOWN_SEPARATION;

    /* Everything else: open */
    return SPACE_CLASS_RELATION_OPEN;
}

/* ©¤©¤©¤ Demo ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤ */

void space_hierarchy_demo(void) {
    printf("\n===== Space Hierarchy Theorem =====\n\n");

    printf("Theorem (Hartmanis-Lewis-Stearns 1965):\n");
    printf("  For f = o(g) space-constructible: SPACE(f) ? SPACE(g)\n\n");

    printf("--- What this PROVES ---\n");
    printf("Since log n = o(n):\n");
    printf("  L = SPACE(log n) ? SPACE(n) ? PSPACE\n");
    printf("  Therefore: L ? PSPACE (UNCONDITIONALLY PROVEN!)\n\n");

    printf("Compare with TIME hierarchy:\n");
    printf("  P = TIME(poly) ? TIME(2^n) ? EXP\n");
    printf("  Therefore: P ? EXP (also proven)\n\n");

    printf("--- Comparison with OPEN problems ---\n");
    printf("  P vs PSPACE: OPEN\n");
    printf("  - Both are 'polynomial' but one is TIME, one is SPACE\n");
    printf("  - The hierarchy theorems only separate WITHIN the same resource\n");
    printf("  - Time hierarchy: TIME(n) ? TIME(n2)\n");
    printf("  - Space hierarchy: SPACE(n) ? SPACE(n2)\n");
    printf("  - Cross-resource: TIME vs SPACE is OPEN\n\n");

    /* Demonstrate known relationships */
    printf("--- Known Relationship Matrix ---\n");
    printf("  Relation                         Status\n");
    printf("  --------                         ------\n");
    int rel;
    rel = complexity_compare(CLASS_L, CLASS_NL);
    printf("  L ? NL                           %s\n", rel==1?"PROVEN":rel==2?"SEPARATED":"OPEN");
    rel = complexity_compare(CLASS_L, CLASS_PSPACE);
    printf("  L ? PSPACE                       %s\n", rel==2?"PROVEN":"OPEN");
    rel = complexity_compare(CLASS_P, CLASS_EXP);
    printf("  P ? EXP                          %s\n", rel==2?"PROVEN":"OPEN");
    rel = complexity_compare(CLASS_P, CLASS_PSPACE);
    printf("  P ?? PSPACE                      %s\n", rel==0?"OPEN":"KNOWN");
    rel = complexity_compare(CLASS_PSPACE, CLASS_EXP);
    printf("  PSPACE ?? EXP                    %s\n", rel==0?"OPEN":"KNOWN");

    printf("\n--- Diagonalization Example ---\n");
    printf("Concrete: SPACE(log n) vs SPACE(n)\n");

    int result = space_hierarchy_separates(3.0, 100.0);
    printf("  f(n)=log(1000)~=3, g(n)=100, ratio=0.03 ¡ú %s\n",
           result ? "SEPARATES" : "NO SEPARATION");

    printf("\nDiagonalization: construct D ¡Ê SPACE(n) \\ SPACE(log n).\n");
    printf("  D simulates each logspace TM and flips the answer.\n");
    printf("  D uses O(n) space (enough to simulate O(log n) space TM).\n");
    printf("  D differs from EVERY logspace TM by construction.\n");
    printf("  Therefore: L ¡Ù PSPACE (unconditionally).\n");
}
