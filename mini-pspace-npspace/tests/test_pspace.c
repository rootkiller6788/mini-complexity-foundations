#include "pspace.h"
#include <stdio.h>
int main(void){setbuf(stdout,NULL);
printf("\n===== PSPACE Module Tests =====\n\n");
space_complexity_summary();
qbf_demo();
savitch_demo();
immerman_demo();
geography_demo();
printf("\n===== ALL PASSED =====\n");
return 0;}
