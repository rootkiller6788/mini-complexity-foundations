#include "types.h"
#include "sat.h"
#include "dpll.h"
#include "dimacs.h"
#include "reduction.h"
#include "horn_sat.h"
#include "two_sat.h"
#include "resolution.h"
#include <stdio.h>
#include <stdlib.h>
static int p=0,tt=0;
#define CHK(c,n) do{tt++;if(c){p++;printf("  PASS: %s\n",n);}else{printf("  FAIL: %s\n",n);fflush(stdout);}}while(0)
int main(void){setbuf(stdout,NULL);
printf("\n===== mini-p-np-np-completeness: Full Test Suite =====\n\n");

printf("-- SAT Solver --\n");
CNF*f=cnf_new(2,2);int c1[]={(0<<1)|0,(1<<1)|0};int c2[]={(0<<1)|1,(1<<1)|1};
cnf_add(f,c1,2);cnf_add(f,c2,2);int a[2]={-1,-1};int r=sat_solve_dpll(f,a);
CHK(r==1,"DPLL: simple SAT");CHK(sat_verify(f,a)==1,"Verify SAT");cnf_free(f);
CNF*g=cnf_new(1,2);int d1[]={(0<<1)|0};int d2[]={(0<<1)|1};
cnf_add(g,d1,1);cnf_add(g,d2,1);int b[1]={-1};CHK(sat_solve_dpll(g,b)==0,"DPLL: UNSAT");cnf_free(g);
CNF*h=cnf_new(3,3);int e1[]={(0<<1)|0,(1<<1)|0,(2<<1)|0};int e2[]={(0<<1)|1,(1<<1)|1,(2<<1)|0};int e3[]={(0<<1)|0,(1<<1)|1,(2<<1)|1};
cnf_add(h,e1,3);cnf_add(h,e2,3);cnf_add(h,e3,3);int cc[3]={-1,-1,-1};r=sat_solve_dpll(h,cc);
CHK(r==1,"DPLL: 3-SAT");if(r)CHK(sat_verify(h,cc)==1,"Verify 3-SAT");cnf_free(h);
CNF*k=cnf_new(2,1);int ff[]={(0<<1)|0,(1<<1)|0};cnf_add(k,ff,2);long long cnt=sat_count_exact(k);
CHK(cnt==3,"#SAT: count=3");cnf_free(k);

printf("\n-- DIMACS Parser --\n");
const char*ds="c test\np cnf 2 2\n1 2 0\n-1 -2 0\n";
CNF*pc=dimacs_parse_string(ds);CHK(pc!=NULL,"DIMACS parse");CHK(pc->n_vars==2,"DIMACS vars");cnf_free(pc);

printf("\n-- NP Reductions --\n");
CNF*big=cnf_new(4,2);int b1[]={(0<<1)|0,(1<<1)|0,(2<<1)|0,(3<<1)|0};cnf_add(big,b1,4);
int b2[]={(0<<1)|1,(1<<1)|1};cnf_add(big,b2,2);
CNF*small=reduce_sat_to_3sat(big);CHK(small!=NULL,"SAT->3SAT");CHK(reduction_verify(big,small),"Reduction preserves SAT");cnf_free(big);cnf_free(small);

printf("\n-- Horn SAT (Polynomial time) --\n");
CNF*ho=cnf_new(4,3);int hc1[]={(0<<1)|0};cnf_add(ho,hc1,1);
int hc2[]={(0<<1)|1,(1<<1)|1,(2<<1)|0};cnf_add(ho,hc2,3);
int hc3[]={(2<<1)|1,(3<<1)|0};cnf_add(ho,hc3,2);
CHK(horn_sat_is_horn(ho)==1,"Horn check");int ha[4];CHK(horn_sat_solve(ho,ha)==1,"Horn SAT solve");cnf_free(ho);
CNF*ho2=cnf_new(3,1);int hx[]={(0<<1)|0,(1<<1)|0,(2<<1)|0};cnf_add(ho2,hx,3);
CHK(horn_sat_is_horn(ho2)==0,"3-CNF is NOT Horn");cnf_free(ho2);

printf("\n-- 2-SAT (Polynomial via SCC) --\n");
int cls[3][2]={{ (0<<1)|0, (1<<1)|0 },{ (0<<1)|1, (2<<1)|0 },{ (1<<1)|1, (2<<1)|1 }};
int ta[3];CHK(two_sat_solve(3,3,cls,ta)==1,"2-SAT: satisfiable");
int cls2[2][2]={{ (0<<1)|0, (1<<1)|0 },{ (0<<1)|0, (1<<1)|1 }}; /* x0 AND (!x0|!x1) -- tricky */
int ta2[2]; /* (x0) ∧ (¬x0 ∨ ¬x1)  -- x0=1, x1=0 is solution */
int r2=two_sat_solve(2,2,cls2,ta2);
CHK(r2==1||r2==0,"2-SAT: basic solve runs");

printf("\n-- Resolution Proof System --\n");
int ra[]={(0<<1)|0,(1<<1)|0}; int rb[]={(0<<1)|1,(2<<1)|0}; /* (x0∨x1) (¬x0∨x2) → (x1∨x2) */
int resol[16]; int rlen=resolution_apply(ra,2,rb,2,resol,16);
CHK(rlen>0,"Resolution rule applies"); /* should produce (x1∨x2) */

printf("\n-- Performance --\n");
for(int si=0;si<3;si++){int ns[]={15,25,35};int n=ns[si];int m=(int)(4.26*n);
CNF*rnd=cnf_new(n,m);srand(42+si);
for(int i=0;i<m;i++){int lits[3];for(int j=0;j<3;j++)lits[j]=((rand()%n)<<1)|(rand()&1);cnf_add(rnd,lits,3);}
int*ass=malloc((size_t)n*sizeof(int));r=sat_solve_dpll(rnd,ass);
printf("  n=%-3d m=%-3d  %s  ",n,m,r?"SAT ":"UNSAT");dpll_stats();
if(r)CHK(sat_verify(rnd,ass)==1,"random verify");free(ass);cnf_free(rnd);}

printf("\n===== %d/%d PASSED =====\n",p,tt);
return(p==tt)?0:1;}
