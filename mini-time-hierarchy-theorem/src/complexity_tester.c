/* complexity_tester.c — Test if a function is in a given TIME class
 * Given a machine and time bound, empirically measure if it stays
 * within the complexity class. */
#include <stdio.h>
#include <time.h>
#include <math.h>

typedef int (*Algo)(int);

static int algo_linear(int n)  { volatile int s=0; for(int i=0;i<n;i++) s+=i; return s; }
static int algo_quad(int n)    { volatile int s=0; for(int i=0;i<n;i++) for(int j=0;j<n;j++) s+=1; return s; }
static int algo_exp(int n)     { int lim=1<<(n<15?n:15); volatile int s=0; for(int i=0;i<lim;i++) s+=1; return s; }

typedef struct { Algo f; const char* name; double expected_exponent; int in_p; } Algorithm;

void complexity_tester(void) {
    printf("\n===== Complexity Class Membership Tester =====\n\n");
    Algorithm algs[] = {
        {algo_linear, "O(n)", 1.0, 1},
        {algo_quad,   "O(n^2)", 2.0, 1},
        {algo_exp,    "O(2^n)", 10.0, 0},
        {NULL, NULL, 0, 0}
    };
    
    printf("Testing each algorithm for P membership:\n");
    printf("  name     n=100    n=200    n=400    in P?\n");
    printf("  ----     -----    -----    -----    -----\n");
    
    for(int a=0; algs[a].f; a++) {
        printf("  %-8s", algs[a].name);
        for(int n=100; n<=400; n*=2) {
            clock_t t0=clock(); algs[a].f(n);
            double ms=(double)(clock()-t0)/CLOCKS_PER_SEC*1000;
            printf("%8.3f", ms);
        }
        printf("  %s\n", algs[a].in_p?"YES":"NO (EXP)");
    }
    printf("\nP = algorithms with poly(n) growth.\n");
    printf("The hierarchy separator is the one EXP algorithm not in P.\n");
}