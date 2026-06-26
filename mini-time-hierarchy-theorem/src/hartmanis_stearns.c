/* hartmanis_stearns.c -- Hartmanis & Stearns (TAMS 1965) -- Turing Award 1993 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tht.h"
typedef struct{int id,ns,nsym,qacc,qrej;int*delta;}TM;
static TM* tm_new(int id){TM*m=malloc(sizeof(TM));m->id=id;m->ns=2+(id%8);m->nsym=2+((id/8)%4);m->qacc=m->ns-1;m->qrej=m->ns-2;int t=m->ns*m->nsym*3;m->delta=malloc(t*sizeof(int));for(int q=0;q<m->ns;q++)for(int s=0;s<m->nsym;s++){int idx=(q*m->nsym+s)*3,h=id*7919+q*1009+s*271;m->delta[idx]=h%m->nsym;m->delta[idx+1]=h%m->ns;m->delta[idx+2]=((h/7)%3)-1;}return m;}
static void tm_free(TM*m){free(m->delta);free(m);}
static int tm_sim(TM*m,int inp,int max_s,int*st){
  int tape[4096]={0},pos=0,tmp=inp;
  do{tape[2048+pos]=tmp&1;tmp>>=1;pos++;}while(tmp>0&&pos<1024);
  int head=2048,state=0,step=0;
  while(step<max_s){if(state==m->qacc){*st=step;return 1;}if(state==m->qrej){*st=step;return 0;}
    int sym=tape[head];if(sym>=m->nsym)sym=0;int idx=(state*m->nsym+sym)*3;
    tape[head]=m->delta[idx];state=m->delta[idx+1];head+=m->delta[idx+2];
    if(head<0||head>=4096){*st=step;return -1;}step++;}
  *st=step;return -1;
}
static int hs_diag(int n){int g=n*(1<<(n<10?n:10));if(g>100000)g=100000;TM*m=tm_new(n);int st,r=tm_sim(m,n,g,&st);tm_free(m);return(r==0)?1:0;}
void hs_demo(void){
  printf("\n================================================================\n");
  printf("  HARTMANIS & STEARNS (TAMS 1965)\n");
  printf("  Turing Award 1993 -- Founded Complexity Theory\n");
  printf("================================================================\n\n");
  printf("TM Enumeration (Godel numbering):\n");
  for(int i=0;i<8;i++){TM*m=tm_new(i);printf("  TM_%d: %d states %d symbols\n",i,m->ns,m->nsym);tm_free(m);}
  printf("\nTM Simulation:\n");
  for(int i=0;i<5;i++){TM*m=tm_new(i);for(int inp=0;inp<5;inp++){int st,r=tm_sim(m,inp,5000,&st);printf("  M_%d(%d)=%s (%d steps)\n",i,inp,r>0?"ACC":(r==0?"REJ":"TIMEOUT"),st);}tm_free(m);}
  printf("\nDiagonal Language L_HS: L(n)=1 iff M_n rejects n quickly.\n");
  for(int n=0;n<15;n++){printf("  L_HS(%d)=%d\n",n,hs_diag(n));}
  printf("\nTIME(f)!=TIME(g) when f*log f=o(g). P!=EXP is a corollary!\n");
  printf("This paper CREATED computational complexity theory.\n");
}
