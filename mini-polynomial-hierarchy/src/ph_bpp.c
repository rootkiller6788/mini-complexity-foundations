/* ph_bpp.c - BPP subseteq Sigma_2 (Sipser-Gacs-Lautemann 1983) */
#include "ph.h"

int ph_bpp_simulate(PH_BPPPredict pred, int input,
                    int n_random, int n_trials, double* error_bound) {
    if (n_trials <= 0) n_trials = 1;
    if (n_trials > 1000) n_trials = 1000;
    srand((unsigned int)(time(NULL) + input));
    int yes_votes = 0;
    for (int t = 0; t < n_trials; t++) {
        int r = rand() & ((1 << n_random) - 1);
        if (pred(input, r)) yes_votes++;
    }
    int result = (yes_votes > n_trials / 2) ? 1 : 0;
    if (error_bound) *error_bound = exp(-(double)n_trials / 18.0);
    return result;
}

int ph_bpp_to_sigma2(PH_BPPPredict pred, int input, int n_witness) {
    if (n_witness <= 0) n_witness = 1;
    if (n_witness > 50) n_witness = 50;
    int n_random = 8;
    srand((unsigned int)(time(NULL) + input * 31337));
    long long max_tries = 500;
    for (long long attempt = 0; attempt < max_tries; attempt++) {
        int witnesses[50];
        for (int i = 0; i < n_witness; i++)
            witnesses[i] = rand() & ((1 << n_random) - 1);
        int forall_ok = 1;
        long long max_z = 1LL << n_random;
        if (max_z > 256) max_z = 256;
        for (long long z = 0; z < max_z && forall_ok; z++) {
            int yes = 0, total = 0;
            for (int i = 0; i < n_witness; i++) {
                int r = witnesses[i] ^ (int)z;
                if (pred(input, r)) yes++;
                total++;
            }
            if (yes <= total / 2) forall_ok = 0;
        }
        if (forall_ok) return 1;
    }
    return 0;
}

static int example_bpp_pred(int input, int r) {
    if (r % 3 == 0) return r & 1;
    return (input % 2 == 0) ? 1 : 0;
}

void ph_demo_bpp(void) {
    printf("\n===== BPP subseteq Sigma_2 (Sipser-Gacs-Lautemann) =====\n\n");
    printf("Theorem (SGL 1983): BPP is in Sigma_2 cap Pi_2.\n");
    printf("Randomness gives at most 2 quantifier alternations.\n\n");
    printf("--- BPP Language Demo ---\n");
    printf("L = { x | x is even }. BPP: 2/3 correct via P(x,r).\n\n");
    printf("BPP simulation with amplification (m=99 trials):\n");
    for (int x = 0; x < 8; x++) {
        double err;
        int result = ph_bpp_simulate(example_bpp_pred, x, 8, 99, &err);
        int correct = (x % 2 == 0) ? 1 : 0;
        printf("  x=%d: BPP=%d (correct=%d), err_bound=%.6f\n",
               x, result, correct, err);
    }
    printf("\n--- SGL Sigma_2 Construction ---\n");
    for (int x = 0; x < 6; x++) {
        int sr = ph_bpp_to_sigma2(example_bpp_pred, x, 10);
        printf("  x=%d: Sigma_2=%d, actual=%d\n",
               x, sr, (x%2==0)?1:0);
    }
    printf("\nImplications: If P=NP then BPP=P. BPP=P may hold even if P!=NP.\n");
}
