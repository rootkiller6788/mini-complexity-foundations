/* exponent_fitter.c — Fits empirical timing data to find exponent
 * T(n) = c * n^k. Take logs: log T = log c + k*log n.
 * Linear regression gives k (the polynomial degree).
 * k > any fixed constant => not in P. */
#include <stdio.h>
#include <time.h>
#include <math.h>

static double fit_exponent(double* ns, double* ts, int pts) {
    double sx=0,sy=0,sxx=0,sxy=0;
    for(int i=0;i<pts;i++){
        double x=log(ns[i]), y=log(ts[i]);
        sx+=x; sy+=y; sxx+=x*x; sxy+=x*y;
    }
    return (pts*sxy-sx*sy)/(pts*sxx-sx*sx);
}

void exponent_fitter_demo(void) {
    printf("\n===== Empirical Complexity Fitter =====\n\n");
    printf("Fitting T(n) = c * n^k via log-log regression.\n");
    printf("If k is polynomial -> in P. If k ~ exponential -> EXP.\n\n");
    
    printf("Fitting O(n^2) algorithm:\n");
    double ns[]={100,200,400,800,1600};
    double ts[5];
    for(int i=0;i<5;i++) {
        clock_t t0=clock();
        volatile int x;
        for(int j=0;j<ns[i]*ns[i];j++) x=j;
        ts[i]=(double)(clock()-t0)/CLOCKS_PER_SEC;
    }
    double k = fit_exponent(ns, ts, 5);
    printf("  Measured exponent: %.2f (expected 2.0)\n", k);
    printf("  k is constant -> polynomial time -> in P.\n");
    
    printf("\nFitting O(2^n) algorithm:\n");
    double ns2[]={10,12,14,16,18};
    double ts2[5];
    for(int i=0;i<5;i++) {
        int lim=1<<(int)ns2[i];
        clock_t t0=clock();
        volatile int x;
        for(int j=0;j<lim;j++) x=j;
        ts2[i]=(double)(clock()-t0)/CLOCKS_PER_SEC;
    }
    double k2 = fit_exponent(ns2, ts2, 5);
    printf("  Measured exponent: %.2f (expected >> constant)\n", k2);
    printf("  k grows with n -> exponential -> not in P.\n");
}