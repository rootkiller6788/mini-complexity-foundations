#include "types.h" 
#include "sat.h" 
#include "dpll.h" 
#include <stdio.h> 
#include <time.h> 
int main(void){ 
printf("===== SAT Solver Benchmarks =====\n\n"); 
int sizes[]={20,25,30,35,40,50,60}; 
printf("%6s %8s %8s %10s\n","n","clauses","result","time(s)"); 
for(int si=0;si<7;si++){ 
int n=sizes[si],m=(int)(4.26*n); 
CNF*c=cnf_new(n,m);srand(42); 
for(int i=0;i<m;i++){int lits[3];for(int j=0;j<3;j++)lits[j]=((rand()%n)<<1)|(rand()&1);cnf_add(c,lits,3);} 
int*a=malloc((size_t)n*sizeof(int)); 
clock_t t0=clock();int r=sat_solve_dpll(c,a);double e=(double)(clock()-t0)/CLOCKS_PER_SEC; 
printf("%6d %8d %8s %10.4f\n",n,m,r?"SAT":"UNSAT",e); 
free(a);cnf_free(c);} 
return 0;} 
