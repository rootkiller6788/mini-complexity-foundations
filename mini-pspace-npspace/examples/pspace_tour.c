/* pspace_tour.c — Guided tour of PSPACE module */
#include "pspace.h"
#include <stdio.h>
int main(void){setbuf(stdout,NULL);
printf("\n===== PSPACE Module Tour =====\n\n");
space_complexity_summary();
qbf_demo();
savitch_demo();
immerman_demo();
qbf_dpll_demo();
printf("\nTotal: L->NL->P->NP->PSPACE=NPSPACE->EXP\n");
printf("TQBF = PSPACE-complete. Savitch = PSPACE=NPSPACE.\n");
printf("Immerman = NL=coNL. Open: L?=NL, NL?=P, P?=PSPACE.\n");
return 0;}