#include "pspace.h"
#include <stdio.h>
int main(void){setbuf(stdout,NULL);
printf("\n===== QBF Solver CLI =====\n\n");
printf("TQBF: Q1 x1 ... Qn xn. phi(x)\n\n");
printf("Example: E x0 A x1: (x0|x1)&(!x0|!x1)\n");
int cls[2][3]={{0,2,-1},{1,3,-1}};
int r=qbf_evaluate(2,"EA",cls,2);
printf("Result: %s\n",r?"TRUE":"FALSE");
printf("(Exists x0 s.t. forall x1 the formula holds -> TRUE with x0=1,x1=0 OR x0=0,x1=1)\n");
return 0;}