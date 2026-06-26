/* fuzz_tht.c — Fuzz test: verify diagonal language property
 * For each n: L_D(n) should differ from M_{n%k}(n).
 * If ANY agreement is found, diagonalization fails. */
#include "tht.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void fuzz_tht(void) {
    printf("\n===== Diagonal Language Fuzz Test =====\n\n");
    printf("Property: for all n, L_D(n) != M_{n mod k}(n).\n");
    printf("If this fails at any n, the diagonal construction is broken.\n\n");
    
    int violations=0, tested=0;
    srand((unsigned)time(NULL));
    
    for(int n=0; n<100; n++) {
        int ld = diagonal_language(n);
        /* Compute M_{n mod k}(n) — use the universal simulator */
        int machine_idx = n % 4; /* NM=4 */
        int machine_result = universal_simulate(machine_idx, n, 1000000);
        tested++;
        if(ld == (machine_result>0?1:0)) {
            printf("  VIOLATION at n=%d: L_D(%d)=%d but M%d(%d)=%d\n", n, n, ld, machine_idx, n, machine_result);
            violations++;
        }
    }
    printf("\nTested %d inputs. Violations: %d.\n", tested, violations);
    if(violations==0) printf("Diagonal language construction verified.\n");
    else printf("CONSTRUCTION FAILED — diagonalization broken.\n");
}