/* tht.c — Time Hierarchy: P⊊EXP (Hartmanis-Stearns 1965, AB §3, MIT 6.840) */
#include "tht.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef int (*M)(int);
static int ml(int n){volatile int s=0;for(int i=0;i<n;i++)s+=i;return s&1;}
static int mq(int n){volatile int s=0;for(int i=0;i<n;i++)for(int j=0;j<n;j++)s+=(i*j)%7;return s&1;}
static int mc(int n){volatile int s=0;for(int i=0;i<n;i++)for(int j=0;j<n;j++)for(int k=0;k<n&&k<10;k++)s+=(i*j+k)%7;return s&1;}
static int me(int n){int lim=1<<(n<15?n:15);volatile int s=0;for(int i=0;i<lim;i++)s+=i%3;return s&1;}
static M ms[]={ml,mq,mc,me};
static const char*bs[]={"O(n)","O(n^2)","O(n^3)","O(2^n)"};
enum{NM=4};

int universal_simulate(int id,int in,int T){
 if(id<0||id>=NM)return 0;
 clock_t t0=clock();int r=ms[id](in);
 printf("  M%d(%d): %s  %.4fs\n",id,in,r?"ACCEPT":"REJECT",(double)(clock()-t0)/CLOCKS_PER_SEC);
 return r;}

int diagonal_language(int x){return !ms[x%NM](x);}

void time_hierarchy_demo(int k,int n){
 printf("\n===== Time Hierarchy Theorem =====\n");
 printf("TIME(f)!=TIME(g) when f*log f=o(g)\nCorollary: P!=EXP\n\n");
 printf("Machines: ");for(int i=0;i<NM;i++)printf("M%d=%s ",i,bs[i]);
 printf("\n\nDiagonal: L_D={n|M_{n%%%d}(n) rejects}\n",NM);
 printf("L_D in EXP, L_D not in P (by contradiction)\n\n");
 for(int i=0;i<=n;i++)printf("  L_D(%d)=%d\n",i,diagonal_language(i));
 printf("\nP!=EXP is PROVED. P!=NP is OPEN.\n");}

void bgs_limits_demo(void){
 printf("\n===== Baker-Gill-Solovay (1975) =====\n");
 printf("Exist oracles A,B: P^A=NP^A and P^B!=NP^B.\n");
 printf("Diagonalization relativizes => cannot resolve P vs NP.\n");
 printf("Need non-relativizing techniques: circuit lower bounds, IP=PSPACE, PCP.\n");}