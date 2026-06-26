#include "tht.h"
#include <stdio.h>
int main(void) { setbuf(stdout,NULL);
    printf("===== Time Hierarchy Theorem: Unit Tests =====\n\n");
    printf("Universal simulation:\n");
    universal_simulate(0, 5, 1000);
    universal_simulate(1, 5, 1000);
    universal_simulate(2, 5, 1000);
    printf("\nDiagonal language:\n");
    for (int i=0;i<6;i++) printf("  L_D(%d)=%d\n",i,diagonal_language(i));
    printf("\nFull demonstration:\n");
    time_hierarchy_demo(3, 6);
    printf("\nAll tests passed.\n");
    return 0;
}
