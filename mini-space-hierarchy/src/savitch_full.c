/* savitch_full.c -- Savitch Theorem (JCSS 1970): NSPACE(s) in SPACE(s^2) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
typedef struct{int n;char**adj;}CGraph;
static CGraph* cg_new(int n){CGraph*g=malloc(sizeof(CGraph));g->n=n;g->adj=malloc(n*sizeof(char*));for(int i=0;i<n;i++)g->adj[i]=calloc(n,sizeof(char));return g;}
static void cg_free(CGraph*g){for(int i=0;i<g->n;i++)free(g->adj[i]);free(g->adj);free(g);}
static int savitch_rec(CGraph*g,int s,int t,int steps,int*sp,int d){
  if(steps==0){*sp=d;return s==t;}
  if(steps==1){*sp=d;if(s==t)return 1;for(int i=0;i<g->n;i++)if(g->adj[s][i]&&i==t)return 1;return 0;}
  int mid_steps=steps/2,max_sp=d;
  for(int mid=0;mid<g->n;mid++){int s1,s2;
    if(savitch_rec(g,s,mid,mid_steps,&s1,d+1)&&savitch_rec(g,mid,t,steps-mid_steps,&s2,d+1)){*sp=(s1>s2?s1:s2);return 1;}
    if(s1>max_sp)max_sp=s1;if(s2>max_sp)max_sp=s2;}
  *sp=max_sp;return 0;
}
int savitch_path(int**edges,int m,int s,int t,int steps,int nv){CGraph*g=cg_new(nv);for(int i=0;i<m;i++)g->adj[edges[i][0]][edges[i][1]]=1;int sp,res=savitch_rec(g,s,t,steps,&sp,0);cg_free(g);return res;}
static CGraph* build_line(int N){CGraph*g=cg_new(N);for(int i=0;i<N-1;i++)g->adj[i][i+1]=1;return g;}
static CGraph* build_tree(int d){int N=(1<<(d+1))-1;CGraph*g=cg_new(N);for(int i=0;i<(1<<d)-1;i++){g->adj[i][2*i+1]=1;g->adj[i][2*i+2]=1;}return g;}
static CGraph* build_dag(int L,int W){int N=L*W;CGraph*g=cg_new(N);for(int l=0;l<L-1;l++)for(int u=0;u<W;u++)for(int v=0;v<W;v++)if((u*7+v*13)%5==0)g->adj[l*W+u][(l+1)*W+v]=1;return g;}
void savitch_full_demo(void){
  printf("\n================================================================\n");
  printf("  SAVITCH THEOREM: NSPACE(s) in SPACE(s^2)\n");
  printf("  Savitch (JCSS 1970) -- PhD-Level Implementation\n");
  printf("================================================================\n\n");
  printf("Line Graph (deterministic):\n");
  for(int N=4;N<=256;N*=4){CGraph*g=build_line(N);int sp;clock_t t=clock();int r=savitch_rec(g,0,N-1,N-1,&sp,0);printf("  N=%d PATH=%s (%.3fms depth=%d)\n",N,r?"YES":"NO",1000.0*(clock()-t)/CLOCKS_PER_SEC,sp);cg_free(g);}
  printf("\nBinary Tree (nondeterministic):\n");
  for(int d=2;d<=8;d+=2){CGraph*g=build_tree(d);int leaf=(1<<d)-1,sp;clock_t t=clock();int r=savitch_rec(g,0,leaf,d,&sp,0);printf("  depth=%d PATH=%s (%.3fms depth=%d)\n",d,r?"YES":"NO",1000.0*(clock()-t)/CLOCKS_PER_SEC,sp);cg_free(g);}
  printf("\nDAG (layered configs):\n");
  for(int L=2;L<=6;L+=2){CGraph*g=build_dag(L,4);int sp;clock_t t=clock();int r=savitch_rec(g,0,L*4-1,L*4,&sp,0);printf("  L=%d PATH=%s (%.3fms depth=%d)\n",L,r?"YES":"NO",1000.0*(clock()-t)/CLOCKS_PER_SEC,sp);cg_free(g);}
  printf("\nSpace: O(log N * log steps) = O(s(n)^2). PSPACE = NPSPACE.\n");
}
