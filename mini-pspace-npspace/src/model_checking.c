/* model_checking.c — CTL Model Checking (polynomial space)
 * Clarke-Emerson (1981): CTL model checking is in PSPACE.
 * Given a Kripke structure M and CTL formula phi, decide M,s |= phi.
 * Space = O(|phi| * log|M|) per recursion level. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int n; int** trans; char* labels[32]; } Kripke;

typedef enum { ATOM, AND, OR, NOT, EX, EG, EU } CTLOp;
typedef struct CTLNode { CTLOp op; int atom; struct CTLNode* left; struct CTLNode* right; } CTLNode;

static int ctl_eval(Kripke* k, int state, CTLNode* phi, int depth, int max_depth) {
    if(depth>max_depth) return 0;
    switch(phi->op) {
        case ATOM: return (k->labels[state][phi->atom]=='1');
        case AND:  return ctl_eval(k,state,phi->left,depth+1,max_depth) && ctl_eval(k,state,phi->right,depth+1,max_depth);
        case OR:   return ctl_eval(k,state,phi->left,depth+1,max_depth) || ctl_eval(k,state,phi->right,depth+1,max_depth);
        case NOT:  return !ctl_eval(k,state,phi->left,depth+1,max_depth);
        case EX:   for(int i=0;i<k->n;i++) if(k->trans[state][i]&&ctl_eval(k,i,phi->left,depth+1,max_depth)) return 1; return 0;
        default: return 0;
    }
}

void model_checking_demo(void) {
    printf("\n===== CTL Model Checking (PSPACE) =====\n\n");
    printf("CTL (Computation Tree Logic) model checking is in PSPACE.\n");
    printf("Clarke-Emerson (1981): given Kripke structure + CTL formula,\n");
    printf("decide if state satisfies formula using O(|phi|*log|M|) space.\n");
    printf("This is polynomially space-bounded = PSPACE.\n");
    printf("Used extensively in hardware verification (model checking).\n");
}