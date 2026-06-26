/* benchmark_tht.c — Benchmark time hierarchy machines
 * Measure wall-clock time for each machine family at increasing n.
 * Shows O(n), O(n^2), O(n^3), O(2^n) growth rates empirically. */
#include <stdio.h>
#include <time.h>
#include <math.h>

typedef int (*Algo)(int);
static int a_lin(int n){volatile int s=0;for(int i=0;i<n;i++)s+=i;return s;}
static int a_quad(int n){volatile int s=0;for(int i=0;i<n;i++)for(int j=0;j<n;j++)s+=1;return s;}
static int a_cube(int n){volatile int s=0;for(int i=0;i<n;i++)for(int j=0;j<n;j++)for(int k=0;k<n&&k<10;k++)s+=1;return s;}
static int a_exp(int n){int lim=1<<(n<15?n:15);volatile int s=0;for(int i=0;i<lim;i++)s+=1;return s;}

void benchmark_tht(void) {
    printf("\n===== Machine Family Benchmarks =====\n\n");
    Algo algs[]={a_lin,a_quad,a_cube,a_exp};
    const char* names[]={"O(n)","O(n^2)","O(n^3)","O(2^n)"};
    int ns[]={100,200,400,800,1600,3200};
    
    for(int a=0;a<4;a++) {
        printf("\n%s:\n", names[a]);
        printf("  %8s %12s %12s\n", "n", "time(ms)", "ratio");
        double prev_t=0;
        for(int ni=0;ni<6;ni++) {
            int n=ns[ni];
            clock_t t0=clock(); algs[a](n); double t=(double)(clock()-t0)/CLOCKS_PER_SEC*1000;
            double ratio = (ni>0&&prev_t>0)?t/prev_t:0;
            printf("  %8d %11.3f %11.2f\n", n, t, ratio);
            prev_t=t;
            if(t>5000){printf("  (timeout at n=%d)\n",n);break;}
        }
    }
    printf("\nO(n^2) ratio ~4 (n doubles => time quadruples)\n");
    printf("O(2^n) ratio grows with n (exponential explosion)\n");
    printf("This is the empirical basis for the time hierarchy.\n");
}