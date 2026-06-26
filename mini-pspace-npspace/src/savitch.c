/* savitch.c — Savitch's Theorem (1970): NSPACE(s) subset SPACE(s^2)
 *
 * Given a nondeterministic space-s computation, Savitch shows how to
 * simulate it deterministically in O(s^2) space.
 *
 * Key idea: PATH problem. To test if there's a path from config A to
 * config B in <= 2^k steps, recursively test if there's a midpoint M:
 *   PATH(A, B, 2^k) = exists M: PATH(A, M, 2^{k-1}) AND PATH(M, B, 2^{k-1})
 *
 * Space: O(k * sizeof(config)) = O(s * log N) = O(s^2).
 * The s^2 comes from storing k = O(s) levels of recursion. */
#include "pspace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int savitch_reachability(int s, int t, int steps, int n, int** edges, int m) {
    if(steps == 0) return (s == t);
    if(steps == 1) {
        for(int i=0;i<m;i++) if(edges[i][0]==s && edges[i][1]==t) return 1;
        return (s == t);
    }
    int mid_step = steps / 2;
    for(int mid=0; mid<n; mid++) {
        if(savitch_reachability(s, mid, mid_step, n, edges, m) &&
           savitch_reachability(mid, t, steps - mid_step, n, edges, m))
            return 1;
    }
    return 0;
}

void savitch_demo(void) {
    printf("\n===== Savitch Theorem =====\n\n");
    printf("NSPACE(s) subset SPACE(s^2)\n");
    printf("Corollary: PSPACE = NPSPACE\n\n");
    
    printf("Proof technique: recursive reachability.\n");
    printf("Given NTM using O(s) space, has at most 2^{O(s)} configs.\n");
    printf("Can test acceptance by PATH(start, accept, 2^{O(s)}).\n");
    printf("Recursive PATH uses O(s^2) space (depth * per-level storage).\n\n");
    
    printf("Example graph reachability via Savitch:\n");
    int* edges[4]; 
    int e0[]={0,1}; int e1[]={1,2}; int e2[]={2,3}; int e3[]={1,3};
    edges[0]=e0;edges[1]=e1;edges[2]=e2;edges[3]=e3;
    printf("  PATH(0,3,4) = %d (expected 1)\n", savitch_reachability(0,3,4,4,edges,4));
    printf("  PATH(3,0,4) = %d (expected 0)\n", savitch_reachability(3,0,4,4,edges,4));
}