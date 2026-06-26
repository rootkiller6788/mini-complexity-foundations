#include "pspace.h"
#include <stdio.h>
#include <time.h>
int main(void){setbuf(stdout,NULL);
printf("\n===== QBF Quick Benchmark =====\n\n");
for(int n=4;n<=10;n++){
 char quants[20]; for(int i=0;i<n;i++) quants[i]=(i%2)?'A':'E';quants[n]=0;
 int cls[5][3]; for(int i=0;i<3;i++)for(int j=0;j<3;j++)cls[i][j]=((rand()%n)<<1)|(rand()&1);
 clock_t t0=clock(); qbf_evaluate(n,quants,cls,3);
 printf("n=%2d  %.3f ms\n", n, (double)(clock()-t0)/CLOCKS_PER_SEC*1000);
}
return 0;}