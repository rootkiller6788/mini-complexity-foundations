#include "types.h"
#include "horn_sat.h"
#include <stdio.h>
int main(void){
printf("===== Horn SAT Demo (P-complete, polynomial time) =====\n\n");
CNF*f=cnf_new(4,3);
/* Horn: (x0) ∧ (¬x0 ∨ ¬x1 ∨ x2) ∧ (¬x2 ∨ x3) */
int c1[]={(0<<1)|0}; cnf_add(f,c1,1);
int c2[]={(0<<1)|1,(1<<1)|1,(2<<1)|0}; cnf_add(f,c2,3);
int c3[]={(2<<1)|1,(3<<1)|0}; cnf_add(f,c3,2);
printf("Horn formula:\n"); cnf_print(f);
printf("Is Horn: %s\n",horn_sat_is_horn(f)?"YES":"NO");
int a[4]; int r=horn_sat_solve(f,a);
printf("Result: %s, assign=[%d,%d,%d,%d]\n",r?"SAT":"UNSAT",a[0],a[1],a[2],a[3]);

/* Contrast: 3-CNF is NOT Horn */
CNF*g=cnf_new(3,1);int d[]={(0<<1)|0,(1<<1)|0,(2<<1)|0};cnf_add(g,d,3);
printf("\n3-CNF: "); cnf_print(g);
printf("Is Horn: %s (should be NO - 3 positive lits)\n",horn_sat_is_horn(g)?"YES":"NO");
cnf_free(f);cnf_free(g);
printf("\nKey: Horn-SAT solves in O(n*m) polynomial time.\n");
printf("This shows that not all SAT variants are NP-complete.\n");
return 0;
}
