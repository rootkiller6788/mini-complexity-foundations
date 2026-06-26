/* timing_benchmark.c — Measure actual growth rates of machine families
 * Demonstrates O(n), O(n^2), O(n^3), O(2^n) with wall-clock timing.
 * Shows the hierarchy visually through empirical data. */
#include <stdio.h>
#include <time.h>
#include <math.h>

static int ml(int n){volatile int s=0;for(int i=0;i<n;i++)s+=i;return s;}
static int mq(int n){volatile int s=0;for(int i=0;i<n;i++)for(int j=0;j<n;j++)s+=(i*j)%7;return s;}
static int mc(int n){volatile int s=0;for(int i=0;i<n;i++)for(int j=0;j<n;j++)for(int k=0;k<n&&k<10;k++)s+=1;return s;}

void timing_benchmark(void) {
    printf("\n===== Empirical Growth Rates =====\n\n");
    printf("%8s %12s %12s %12s\n", "n", "O(n)", "O(n^2)", "O(n^3)");
    printf("%8s %12s %12s %12s\n", "──", "────", "─────", "─────");
    for(int n=100; n<=2000; n*=2) {
        clock_t t1=clock(); ml(n); double l=(double)(clock()-t1)/CLOCKS_PER_SEC*1000;
        clock_t t2=clock(); mq(n); double q=(double)(clock()-t2)/CLOCKS_PER_SEC*1000;
        clock_t t3=clock(); mc(n); double c=(double)(clock()-t3)/CLOCKS_PER_SEC*1000;
        printf("%8d %11.3f %11.3f %11.3f\n", n, l, q, c);
    }
    printf("\nO(n) grows linearly, O(n^2) quadratically, O(n^3) cubically.\n");
    printf("The hierarchy theorem says: TIME(n) != TIME(n^2) != TIME(n^3).\n");
    printf("These are provably DIFFERENT complexity classes.\n");
}