#include "tht.h"
#include <stdio.h>
int main(void){setbuf(stdout,NULL);
printf("\n===== Time Hierarchy: P vs EXP =====\n\n");
printf("TIME(n^k) subsetneq TIME(2^n) for any k.\n");
printf("Therefore: P subsetneq EXP (unconditional!).\n\n");
printf("Diagonal language demo:\n");
for(int i=0;i<8;i++) printf("  L_D(%d) = %d\n",i,diagonal_language(i));
printf("\nProof: enumerate all poly-time machines M_1,M_2,...\n");
printf("Define L_D = {n | M_n(n) rejects within 2^n steps}\n");
printf("L_D in EXP but not in P (by Cantor-style diagonalization).\n");
printf("This is why we KNOW P != EXP but cannot prove P != NP.\n");
return 0;}
