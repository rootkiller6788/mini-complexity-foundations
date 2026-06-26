/* l_vs_nl.c */
#include <stdio.h>
#include <stdlib.h>
static int nl_path(int** adj, int n, int s, int t, int ms) {
    int* cur=calloc(n,sizeof(int)); int* nxt=calloc(n,sizeof(int));
    cur[s]=1;
    for(int st=0;st<ms;st++) {
        for(int i=0;i<n;i++)nxt[i]=0;
        for(int u=0;u<n;u++)if(cur[u])for(int v=0;v<n;v++)if(adj[u][v])nxt[v]=1;
        if(nxt[t]){free(cur);free(nxt);return 1;}
        int* tmp=cur;cur=nxt;nxt=tmp;
    }
    free(cur);free(nxt);return 0;
}
void l_vs_nl_demo(void) {
    printf("\n===== L vs NL =====\n\n");
    printf("L=logspace. NL=nondeterministic logspace. PATH is NL-complete.\n\n");
    int n=5; int** adj=malloc(n*sizeof(int*));
    for(int i=0;i<n;i++){adj[i]=calloc(n,sizeof(int));}
    adj[0][1]=1;adj[1][2]=1;adj[2][3]=1;adj[0][4]=1;
    printf("Graph: 0->1->2->3, 0->4\n");
    printf("PATH(0,3)=%s (expected YES)\n",nl_path(adj,5,0,3,10)?"YES":"NO");
    printf("PATH(4,0)=%s (expected NO)\n",nl_path(adj,5,4,0,10)?"YES":"NO");
    printf("\nNL=coNL (proved). Undirected PATH in L (Reingold 2008).\n");
    printf("Directed PATH in L? OPEN. This is L vs NL.\n");
    for(int i=0;i<n;i++) free(adj[i]);
    free(adj);
}
