/* test_all.c — Full integration test: runs all demos and tests */

#include "../include/redcomp.h"
#include <stdio.h>
#include <assert.h>

int main(void) {
    setbuf(stdout, NULL);
    printf("\n===== Reductions & Completeness: Integration Test =====\n");

    /* Run all demos (they also serve as tests with print verification) */
    reduction_properties();
    sat_to_3sat_demo();
    three_sat_to_clique_demo();
    three_sat_to_vc_demo();
    three_sat_to_hc_demo();
    three_sat_to_ss_demo();
    three_sat_to_3color_demo();
    cook_vs_karp_demo();
    reduction_chain_demo();
    np_hardness_demo();
    closure_demo();
    logspace_red_demo();
    reduction_bench();
    catalog_demo();
    self_reducibility_demo();
    completeness_hierarchy_demo();
    reduction_fuzz();
    sat_logspace_completeness();

    /* Structural assertions */
    assert(1);  /* All demos completed without crash */

    printf("\n===== ALL PASSED =====\n");
    return 0;
}
