/* gap_theorem.c */
#include <stdio.h>
#include <math.h>
#include "tht.h"
static double gap_func(double n){double r=n;for(int i=0;i<8;i++){r=pow(2.0,r);if(r>1e50)break;}return r;}
void gap_theorem_demo(void){
  printf("\n===== Borodin Gap Theorem (1972) =====\n\n");
  printf("For any computable r, exists f s.t. TIME(f)=TIME(r o f).\n");
  printf("Example: exists f where TIME(f)=TIME(2^f).\n\n");
  printf("Pathological f(n) growth: ");
  for(int n=1;n<=4;n++)printf("f(%d)~1e%.0f ",n,log10(gap_func((double)n)));
  printf("\nYou CANNOT compute f(n) in O(f(n)) time. NOT constructible!\n\n");
  printf("vs time-constructible n^2: computable within O(n^2).\n");
  printf("Without constructibility: TIME(f)=TIME(2^f) for pathological f.\n");
  printf("With constructibility: TIME(g)!=TIME(g^2) for all constructible g.\n");
  printf("The gap theorem PROVES constructibility is ESSENTIAL.\n");
}
