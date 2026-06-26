/* diagonal_computation.c — Compute the diagonal language L_D
 * L_D = {n | M_n(n) halts and rejects within 2^n steps}
 * We actually compute this for small n using our machine families.
 * Shows L_D is NOT in any of the machine families (diagonalization works). */
#include <stdio.h>
#include <time.h>

typedef int (*M)(int);
static int m0(int n){return n%2;}
static int m1(int n){int s=0;for(int i=0;i<n;i++)s+=i;return s%2;}
static int m2(int n){int s=0;for(int i=0;i<n;i++)for(int j=0;j<n;j++)s+=i*j;return s%2;}
static M ms[]={m0,m1,m2};

void diagonal_computation(void) {
    printf("\n===== Diagonal Language Computation =====\n\n");
    printf("L_D = {n | M_{n%%3}(n) rejects}\n\n");
    printf("  n   Machine   Output   L_D(n)\n");
    printf("  ---  -------   ------   ------\n");
    for(int n=0;n<10;n++) {
        int idx=n%3, out=ms[idx](n)?1:0;
        int ld = !out;
        printf("  %-4d M%d       %-7d %d\n", n, idx, out, ld);
    }
    printf("\nKey check: does any M_i decide L_D correctly?\n");
    printf("  M0(0)=%d L_D(0)=%d -> %s\n", ms[0](0)?1:0, !ms[0](0)?1:0, ms[0](0)==(!ms[0](0)?1:0)?"AGREE":"DIFFER");
    printf("  M1(1)=%d L_D(1)=%d -> %s\n", ms[1](1)?1:0, !ms[1](1)?1:0, ms[1](1)==(!ms[1](1)?1:0)?"AGREE":"DIFFER");
    printf("  M2(2)=%d L_D(2)=%d -> %s\n", ms[2](2)?1:0, !ms[2](2)?1:0, ms[2](2)==(!ms[2](2)?1:0)?"AGREE":"DIFFER");
    printf("\nIf ANY M_i agreed on input i, diagonalization would fail.\n");
    printf("But it always DIFFERS: L_D(i) = !M_i(i) by definition.\n");
}