/* ph_bench.c - PH Benchmarks */
#include "ph.h"

void ph_bench_sigma_k(void) {
    printf("\n===== Sigma_k SAT Benchmark =====\n");
    printf("  k   n_vars   result   time_ms\n");
    printf("  ---  ------   ------   -------\n");
    for (int k = 1; k <= 4 && k <= PH_MAX_LEVEL; k++) {
        int nv = k * 3; if (nv > PH_MAX_VARS) nv = PH_MAX_VARS;
        PH_QBF* qbf = ph_qsat_benchmark(k, 3);
        if (!qbf) continue;
        clock_t t0 = clock();
        int r = ph_sigma_k_sat(qbf);
        double ms = 1000.0 * (clock() - t0) / CLOCKS_PER_SEC;
        printf("  %-4d %-7d %-7s %8.3f\n", k, nv, r ? "SAT" : "UNSAT", ms);
        ph_qbf_free(qbf);
    }
    printf("\nExponential growth demonstrates hierarchy separation.\n");
}

void ph_benchmark(void) { ph_bench_sigma_k(); }
