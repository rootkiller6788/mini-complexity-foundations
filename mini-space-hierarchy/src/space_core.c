/* space_core.c -- Space Hierarchy: Core Implementation
 * SPACE(f) != SPACE(g) when f = o(g) and g is space-constructible.
 * Corollary: L != PSPACE (unconditional!). */
#include "spaceh.h"

static long long compute_n_squared(int n) { long long r=0; for(int i=0;i<n;i++) for(int j=0;j<n;j++) r++; return r; }

int space_hierarchy_diagonal_language(int n) {
    int g = (n<=0)?1:n*n;
    int f_limit = (n<=0)?1:n;
    int* tape = malloc((size_t)g * sizeof(int));
    if(!tape) return 0;
    int state=0, head_pos=0, space_used=1, steps=0;
    for(int i=0;i<g;i++) tape[i]=0;
    tape[0]=n;
    while(steps<1000) {
        if(space_used>f_limit) { free(tape); return 0; }
        int hash = n*7919+state*6271+tape[head_pos]*4093;
        int action = hash%4;
        switch(action) {
            case 0: free(tape); return 0;
            case 1: free(tape); return 1;
            case 2: tape[head_pos]=(tape[head_pos]+1)%16; break;
            case 3: head_pos=(head_pos+1)%g; if(head_pos+1>space_used) space_used=head_pos+1; break;
        }
        state=(state+1)%16; steps++;
    }
    free(tape); return 0;
}

void space_hierarchy_demo(void) {
    printf("\n===== Space Hierarchy Theorem =====\n\n");
    printf("SPACE(f) != SPACE(g) when f=o(g). Corollary: L!=PSPACE.\n\n");
    printf("Why simpler than TIME hierarchy:\n");
    printf("  Time: O(T log T) overhead\n  Space: O(1) overhead (space reusability)\n\n");
    printf("Proof: enumerate space-bounded TMs M_i, define\n");
    printf("L_D={n | M_n rejects n within g(n) space}.\n");
    printf("L_D in SPACE(g) but not SPACE(f) by diagonalization.\n\n");
    printf("--- Diagonal Language Demo ---\n");
    for(int n=0;n<=12;n++) printf("  L_D(%d)=%d\n",n,space_hierarchy_diagonal_language(n));
    printf("\nSpace hierarchy is UNCONDITIONAL (unlike P vs NP).\n");
}
