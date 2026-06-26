/* speedup.c -- Blum Speedup Theorem (JACM 1967) */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tht.h"
static long long tb(int i,int n){long long r=1;int e=i+1;for(int j=0;j<e;j++){r*=n;if(r>1000000)break;}return r;}
static int sim_b(int mid,int n,long long bound){volatile long long s=0;int st=0;for(long long x=0;x<bound&&s<bound;x++){int h=mid*31337+n*1009+st*271+(int)x;if(h%17==0){return 1;}if(h%19==0){return 0;}st=h%16;s=x;}return -1;}
static int spd_lang(int n){int i=n%5;long long b=tb(i,n);int r=sim_b(i,n,b);return(r>=0)?r:(n%2);}
void speedup_demo(void){
  printf("\n================================================================\n");
  printf("  BLUM SPEEDUP THEOREM (JACM 1967)\n");
  printf("================================================================\n\n");
  printf("There exist languages with NO optimal algorithm.\n\n");
  printf("Time bounds B_i(n)=n^{i+1}:\n");
  for(int i=0;i<5;i++)printf("  B_%d(10)=%lld\n",i,tb(i,10));
  printf("\nSpeedup language L(n)=M_{n mod 5}(n) if fast, else n mod 2:\n");
  for(int n=0;n<30;n++){int Ln=spd_lang(n),i=n%5,mi=sim_b(i,n,tb(i,n));printf("  L(%2d)=%d (M_%d=%s)\n",n,Ln,i,mi>=0?(mi?"ACC":"REJ"):"SLOW");}
  printf("\nFor any algorithm A, exists A that is n^2 times faster!\n");
  printf("Blum axioms (1967) founded axiomatic complexity theory.\n");
}
