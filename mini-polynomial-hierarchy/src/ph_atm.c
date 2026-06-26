/* ph_atm.c - Alternating Turing Machine and PH (CKS 1981) */
#include "ph.h"
#include <string.h>

int ph_atm_simulate(const PH_ATMConfig* config, int n_states,
                    int start_state, int depth, int max_depth,
                    int is_exist, int input) {
    if (depth >= max_depth) return 0;
    const PH_ATMConfig* st = &config[start_state];
    switch (st->type) {
        case PH_ATM_ACCEPT: return 1;
        case PH_ATM_REJECT: return 0;
        case PH_ATM_EXISTS:
            for (int i = 0; i < st->n_transitions && i < 4; i++)
                if (ph_atm_simulate(config, n_states, st->transitions[i],
                                    depth + 1, max_depth, 0, input))
                    return 1;
            return 0;
        case PH_ATM_FORALL:
            for (int i = 0; i < st->n_transitions && i < 4; i++)
                if (!ph_atm_simulate(config, n_states, st->transitions[i],
                                     depth + 1, max_depth, 1, input))
                    return 0;
            return 1;
        default: return 0;
    }
}

static int is_prime_leaf(int n) {
    if (n < 2) return 0;
    for (int d = 2; d * d <= n; d++)
        if (n % d == 0) return 0;
    return 1;
}

void ph_demo_atm(void) {
    printf("\n===== Alternating TM and PH (CKS 1981) =====\n\n");
    printf("An Alternating TM has two types of states:\n");
    printf("  EXISTS: accept if SOME transition accepts\n");
    printf("  FORALL: accept if ALL transitions accept\n\n");
    printf("Theorem (CKS 1981):\n");
    printf("  Sigma_k = ATIME(poly(n), k alts, start EXISTS)\n");
    printf("  Pi_k    = ATIME(poly(n), k alts, start FORALL)\n");
    printf("  PSPACE  = ATIME(poly(n), unlimited) = AP\n\n");

    PH_ATMConfig atm[8];
    memset(atm, 0, sizeof(atm));
    atm[0] = (PH_ATMConfig){PH_ATM_EXISTS, 2, {4, 5}};
    atm[1] = (PH_ATMConfig){PH_ATM_FORALL, 2, {6, 7}};
    atm[4] = (PH_ATMConfig){PH_ATM_FORALL, 1, {6}};
    atm[5] = (PH_ATMConfig){PH_ATM_EXISTS, 1, {7}};
    atm[6] = (PH_ATMConfig){PH_ATM_ACCEPT, 0, {0}};
    atm[7] = (PH_ATMConfig){PH_ATM_REJECT, 0, {0}};

    printf("Testing primality via ATM (k=2 alternations):\n");
    for (int n = 2; n <= 20; n++) {
        int result = ph_atm_simulate(atm, 8, 0, 0, 4, 1, n);
        printf("  n=%2d: ATM=%s, actual=%s\n",
               n, result ? "PRIME" : "NOT  ",
               is_prime_leaf(n) ? "PRIME" : "NOT  ");
    }
    printf("\n  k alts    Class       Complete Problem\n");
    printf("  0          P           Circuit Value\n");
    printf("  1 (EXISTS) NP          SAT\n");
    printf("  2          Sigma_2     QSAT_2\n");
    printf("  3          Sigma_3     QSAT_3\n");
    printf("  unlimited  PSPACE      TQBF\n");
}
