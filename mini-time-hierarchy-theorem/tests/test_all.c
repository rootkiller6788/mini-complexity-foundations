#include "tht.h"
#include <stdio.h>
void timing_benchmark(void);void diagonal_computation(void);void oracle_simulator_demo(void);
int main(void){setbuf(stdout,NULL);
printf("===== Time Hierarchy: Full Test =====\n");
time_hierarchy_demo(2,8);bgs_limits_demo();
timing_benchmark();diagonal_computation();oracle_simulator_demo();
printf("\nALL PASSED\n");return 0;}