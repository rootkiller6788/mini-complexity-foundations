/* space_hierarchy_demo.c — Space Hierarchy Theorem Demo
 * Demonstrates SPACE(f) != SPACE(g) for f = o(g).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "../include/spaceh.h"

int main(void) {
    setbuf(stdout, NULL);
    printf("========================================\n");
    printf("  Space Hierarchy Theorem Demo\n");
    printf("========================================\n\n");

    printf("SPACE(f) != SPACE(g) when f = o(g)\n");
    printf("and g is space-constructible.\n\n");

    printf("--- Known Separations ---\n");
    printf("  L != PSPACE     (sp: log n vs n)\n");
    printf("  PSPACE != EXPSPACE (sp: n^k vs 2^{n^k})\n");
    printf("  NL != PSPACE    (sp: log n vs n)\n");
    printf("  P != EXP        (via time hierarchy)\n\n");

    printf("--- Diagonal Language ---\n");
    printf("L_D = {n | M_n rejects n within g(n) space}\n");
    printf("L_D in SPACE(g) but NOT in SPACE(f).\n");
    for (int n = 0; n <= 10; n++) {
        printf("  L_D(%2d) = %d\n", n, space_hierarchy_diagonal_language(n));
    }

    printf("\n--- Space Growth Rates ---\n");
    printf("  %8s %12s %12s %12s\n", "n", "log n", "n", "n^2");
    for (int n = 16; n <= 65536; n *= 16) {
        double ln = log2((double)n);
        printf("  %8d %10.0f %10d %10d\n", n, ln, n, n * n);
    }

    printf("\n--- Why Space Hierarchy is Simpler than Time ---\n");
    printf("Time hierarchy: O(T log T) overhead for tape reduction.\n");
    printf("Space hierarchy: NO overhead — space is reusable!\n");
    printf("  Universal TM can clear work tape, reset heads, reuse.\n");
    printf("  This makes the proof much simpler.\n\n");

    printf("--- The Big Picture ---\n");
    printf("Known: L < NL <= P < PSPACE < EXP < EXPSPACE.\n");
    printf("(Some inclusions use Savitch: NL subset SPACE(log^2 n) subset P)\n");
    printf("Open: L vs NL, NL vs P, P vs PSPACE, PSPACE vs EXP.\n");
    printf("Among these, only non-containment of L vs PSPACE is proven!\n");

    return 0;
}
