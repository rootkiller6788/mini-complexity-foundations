/* space_bench.c -- Space Benchmarks: L, NL, PSPACE, EXP algorithms */
#include "spaceh.h"

/* L: nth prime (iterative, O(log n) space) */
static int nth_prime_l(int n) {
    int cnt=0,c=1;
    while(cnt<n){c++;int p=1;for(int d=2;d*d<=c;d++)if(c%d==0){p=0;break;}if(p)cnt++;}
    return c;
}

/* PSPACE: alpha-beta minimax game tree */
static int minimax(int d,int a,int b,int mx) {
    if(d==0)return(rand()%201)-100;
    if(mx){int best=-999;for(int i=0;i<5;i++){int v=minimax(d-1,a,b,0);if(v>best)best=v;if(best>a)a=best;if(a>=b)break;}return best;}
    else{int best=999;for(int i=0;i<5;i++){int v=minimax(d-1,a,b,1);if(v<best)best=v;if(best<b)b=best;if(a>=b)break;}return best;}
}

/* Fibonacci (iterative, O(1) space beyond input) */
static long long fib_mod(long long n,long long m){
    if(n<=1)return n;long long a=0,b=1;
    for(long long i=2;i<=n;i++){long long t=(a+b)%m;a=b;b=t;}
    return b;
}

void space_bench(void){
    printf("\n===== Space Benchmark: L vs PSPACE vs EXP =====\n\n");

    printf("--- L (Logspace): nth prime ---\n");
    printf("  Only O(log n) workspace: loop counters.\\n");
    for(int n=50;n<=400;n*=2) {
        clock_t t=clock(); int p=nth_prime_l(n);
        double ms=1000.0*(clock()-t)/CLOCKS_PER_SEC;
        printf("  %d-th prime = %d (%.3f ms)\\n",n,p,ms);
    }

    printf("\n--- PSPACE: Game Tree (alpha-beta pruning) ---\n");
    printf("  O(depth) stack space. Minimax evaluates game positions.\\n");
    for(int d=6;d<=14;d+=2) {
        clock_t t=clock(); int v=minimax(d,-999,999,1);
        double ms=1000.0*(clock()-t)/CLOCKS_PER_SEC;
        printf("  depth=%d best=%d (%.3f ms, O(d) space)\\n",d,v,ms);
    }

    printf("\n--- O(n^2) Space: Matrix Multiply ---\n");
    for(int n=32;n<=256;n*=2){int N=n>128?128:n;
        int**A=malloc(N*sizeof(int*));int**B=malloc(N*sizeof(int*));int**C=malloc(N*sizeof(int*));
        for(int i=0;i<N;i++){A[i]=calloc(N,sizeof(int));B[i]=calloc(N,sizeof(int));C[i]=calloc(N,sizeof(int));}
        clock_t t=clock();for(int i=0;i<N;i++)for(int k=0;k<N;k++)for(int j=0;j<N;j++)C[i][j]+=A[i][k]*B[k][j];
        double ms=1000.0*(clock()-t)/CLOCKS_PER_SEC;
        printf("  N=%d: %.4f ms (space=%d KB)\\n",N,ms,(N*N*3*(int)sizeof(int))/1024);
        for(int i=0;i<N;i++){free(A[i]);free(B[i]);free(C[i]);}free(A);free(B);free(C);
    }

    printf("\n--- Space Complexity Summary ---\n");
    printf("  L (log n):    addition, parity, palindrome, nth prime\n");
    printf("  NL (log n):   PATH (nondeterministic guess)\n");
    printf("  PSPACE (n^k): TQBF, QSAT, game playing\n");
    printf("  EXP (2^n):    all PSPACE + time-intensive problems\n");
    printf("\nProvable: L != PSPACE, P != EXP. Open: L vs NL, P vs PSPACE.\n");
}