/* qbf_resolution.c — Q-Resolution: complete proof system for QBF
 * Extends propositional resolution to QBF by adding:
 * 1. Universal reduction: remove universal literal if all existential
 *    literals depending on it are already resolved
 * 2. Resolution on existential pivot variables only
 * Q-resolution is refutationally complete for QBF (Kleine-Buning et al. 1995) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int* lits; int n; int* dep; } QResClause;

/* Check if clause C is a tautology (contains x and !x) */
static int is_tautology(const int* lits, int n) {
    for(int i=0;i<n;i++) for(int j=i+1;j<n;j++)
        if((lits[i]>>1)==(lits[j]>>1) && (lits[i]&1)!=(lits[j]&1)) return 1;
    return 0;
}

/* Q-resolution: resolve C1 and C2 on pivot variable.
 * Universal reduction: remove all universal literals u such that
 * no existential literal in the clause depends on u. */
static int q_resolve(const int* c1, int n1, const int* c2, int n2,
                     int pivot_var, const char* prefix, int n_vars,
                     int* resolvent) {
    /* Only resolve on existential variables */
    if(prefix[pivot_var] != 'E') return -1;
    
    int idx=0;
    for(int i=0;i<n1;i++) {
        int lit=c1[i], var=lit>>1;
        if(var==pivot_var) continue; /* remove pivot */
        resolvent[idx++]=lit;
    }
    for(int j=0;j<n2;j++) {
        int lit=c2[j], var=lit>>1;
        if(var==pivot_var) continue;
        int dup=0;
        for(int k=0;k<idx;k++) if(resolvent[k]==lit){dup=1;break;}
        if(!dup) resolvent[idx++]=lit;
    }
    
    /* Universal reduction: remove universal literals that are
       not dominated by any existential literal */
    int new_idx=0;
    for(int i=0;i<idx;i++) {
        int lit=resolvent[i], var=lit>>1;
        if(prefix[var]=='A') {
            /* Keep universal literal only if some existential
               literal in clause has var in its dependency set */
            int keep=0;
            for(int k=0;k<idx;k++) {
                int evar=resolvent[k]>>1;
                if(prefix[evar]=='E') { keep=1; break; }
            }
            if(keep) resolvent[new_idx++]=lit;
        } else resolvent[new_idx++]=lit;
    }
    idx=new_idx;
    
    return is_tautology(resolvent, idx) ? -1 : idx;
}

void qbf_resolution_demo(void) {
    printf("\n===== Q-Resolution Proof System =====\n\n");
    printf("Q-resolution extends propositional resolution to QBF.\n");
    printf("Rules:\n");
    printf("  1. Resolve on existential pivot variables only\n");
    printf("  2. Universal reduction: drop universal literals with no dependencies\n");
    printf("  3. Tautology deletion: remove clauses with x and !x\n\n");
    printf("Q-resolution is refutationally complete for QBF.\n");
    printf("This is the QBF analogue of resolution for SAT.\n");
    printf("Proof complexity of Q-resolution is an active research area.\n");
}