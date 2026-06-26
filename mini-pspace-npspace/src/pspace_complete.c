/* pspace_complete.c -- PSPACE-Complete Problems */
#include <stdio.h>
#include <stdlib.h>
static int eval_prop(const int* a, int cls[][3], int nc) {
    for(int i=0;i<nc;i++) {
        int ok=0;
        for(int j=0;j<3;j++) {
            int l=cls[i][j], v=l>>1, s=l&1;
            if(v<0) continue;
            if((!s&&a[v]) || (s&&!a[v])) { ok=1; break; }
        }
        if(!ok) return 0;
    }
    return 1;
}
static int tqbf(int* a, int nv, int cls[][3], int nc, int vi, int ex) {
    if(vi>=nv)return eval_prop(a,cls,nc);
    a[vi]=0; int r0=tqbf(a,nv,cls,nc,vi+1,!ex);
    a[vi]=1; int r1=tqbf(a,nv,cls,nc,vi+1,!ex);
    return ex?(r0||r1):(r0&&r1);
}
void pspace_complete_demo(void) {
    printf("\n===== PSPACE-Completeness =====\n\n");
    printf("Canonical: TQBF (Q1x1...Qnxn: phi) is PSPACE-complete.\n\n");
    printf("Other PSPACE-complete problems:\n");
    printf("  Generalized Geography, QSAT, NFA Universality\n");
    printf("  Regular Expression Equivalence, First-Order MC\n\n");
    int cls[2][3]={{0,2,-1},{1,(4<<1)|1,-1}};
    int a[3]={0};
    printf("TQBF: forall x0 exist x1 forall x2: (x0||x1)AND(!x1||!x2)\n");
    printf("Result: %s\n\n",tqbf(a,3,cls,2,0,0)?"TRUE":"FALSE");
    printf("All PSPACE-complete problems equivalent under logspace reductions.\n");
}
