#include "tht.h"
#include <stdio.h>
int main(void){setbuf(stdout,NULL);
printf("\n===== Time Hierarchy: Complete Demo =====\n\n");
time_hierarchy_demo(3,8);
printf("\nKey takeaway: P != EXP is PROVEN.\n");
printf("P != NP is OPEN.\n");
printf("The difference: we can diagonalize against ALL polynomial-time machines\n");
printf("but NP is defined existentially (certificates), not by resource bounds.\n");
return 0;}
