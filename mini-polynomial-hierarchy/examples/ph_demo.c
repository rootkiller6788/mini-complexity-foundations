#include "ph.h"
#include <stdio.h>
int main(void){setbuf(stdout,NULL);
printf("\n===== PH: Sigma-2 SAT Demo =====\n\n");
printf("Sigma_2 = NP^{NP} = exists x forall y: phi(x,y)\n");
printf("Pi_2 = co-Sigma_2 = forall x exists y: phi(x,y)\n\n");
printf("Example: exists x forall y: (x|y) & (!x|!y)\n");
printf("  If x=1: forall y need (1|y)&(0|!y) = y&!y = FALSE\n");
printf("  If x=0: forall y need (0|y)&(1|!y) = y&!y = FALSE\n");
printf("  => Sigma_2-SAT = UNSAT\n\n");
printf("If P=NP, then PH collapses to P.\n");
printf("The polynomial hierarchy generalizes NP.\n");
return 0;}
