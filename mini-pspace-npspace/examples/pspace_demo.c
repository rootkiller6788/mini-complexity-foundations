#include "pspace.h"
#include <stdio.h>
int main(void){setbuf(stdout,NULL);
printf("\n===== PSPACE: Savitch + TQBF =====\n\n");
space_complexity_summary();
printf("\nTQBF (True QBF) is PSPACE-complete.\n");
printf("Savitch: NSPACE(s) subset SPACE(s^2).\n");
printf("Corollary: PSPACE = NPSPACE.\n");
printf("QBF with 2 vars (exists x0 forall x1):\n");
int cls[2][3]={{0,2,-1},{1,3,-1}};
int r=qbf_evaluate(2,"EA",cls,2);
printf("Result: %s\n",r?"TRUE":"FALSE");
return 0;}
