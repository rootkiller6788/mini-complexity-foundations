#include "two_sat.h"
#include <stdio.h>
#include <string.h>
int main(void){
printf("===== 2-SAT Demo (polynomial via SCC) =====\n\n");
/* (x0 ∨ x1) ∧ (¬x0 ∨ x2) ∧ (¬x1 ∨ ¬x2) — satisfiable */
int clauses[3][2]={{0,(1<<1)|0},{1,(2<<1)|0},{3,(2<<1)|1}};
/* encode: x0 = (0<<1)|0, ¬x0 = (0<<1)|1, x1 = (1<<1)|0, etc. */
int cl[3][2] = {
    {(0<<1)|0, (1<<1)|0},
    {(0<<1)|1, (2<<1)|0},
    {(1<<1)|1, (2<<1)|1}
};
int a[3]; int r=two_sat_solve(3,3,cl,a);
printf("Result: %s, assign=[%d,%d,%d]\n",r?"SAT":"UNSAT",a[0],a[1],a[2]);
printf("2-SAT: O(V+E) via implication graph + SCC (Kosaraju).\n");
printf("3-SAT: NP-complete. The boundary is sharp: k=2 vs k=3.\n");
return 0;
}
