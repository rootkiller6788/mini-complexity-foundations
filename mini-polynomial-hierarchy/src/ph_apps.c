/* ph_apps.c - L7 Applications: Model Checking, Games, Circuit Min */
#include "ph.h"
#include <string.h>

int ph_bounded_model_check(int n_states, int n_steps,
                           const int* trans, const int* err) {
    if (n_steps <= 0 || n_states <= 0) return 0;
    long long mp = 1;
    for (int i = 0; i < n_steps; i++) {
        mp *= n_states;
        if (mp > 1000000) { mp = 1000000; break; }
    }
    int* path = calloc((size_t)(n_steps + 1), sizeof(int));
    if (!path) return 0;
    path[0] = 0;
    for (long long enc = 0; enc < mp; enc++) {
        long long rem = enc; int valid = 1;
        for (int s = 0; s < n_steps && valid; s++) {
            path[s+1] = (int)(rem % n_states); rem /= n_states;
            if (path[s] < 0 || path[s] >= n_states ||
                path[s+1] < 0 || path[s+1] >= n_states) {
                valid = 0; break;
            }
            if (!trans[path[s] * n_states + path[s+1]]) valid = 0;
        }
        if (valid && err[path[n_steps]]) { free(path); return 1; }
    }
    free(path); return 0;
}

int ph_game_solver(const PH_QBF* game, int* strat) {
    int r = ph_qbf_solve(game);
    if (r && strat && game && game->n_blocks > 0) {
        int bs = game->blocks[0].n_vars, off = game->blocks[0].start_var;
        long long na = 1LL << bs; if (na > 1024) na = 1024;
        for (long long m = 0; m < na; m++) {
            int at[PH_MAX_VARS] = {0};
            for (int j = 0; j < bs; j++) at[off+j] = (int)((m>>j)&1);
            PH_QBF* rest = ph_qbf_restrict(game, at);
            if (rest && ph_qbf_solve(rest)) {
                for (int j = 0; j < bs; j++) strat[j] = at[off+j];
                ph_qbf_free(rest); break;
            }
            ph_qbf_free(rest);
        }
    }
    return r;
}

int ph_circuit_minimization_check(const PH_BoolCircuit* c, int tg) {
    if (!c || tg < 0) return 0;
    int ni = c->n_inputs;
    long long nia = 1LL << ni; if (nia > 1024) nia = 1024;
    if (tg <= 1 && ni <= 4) {
        for (int gt = 0; gt < 5; gt++) {
            for (int in1 = 0; in1 < ni + 1; in1++) {
                for (int in2 = 0; in2 < ni + 1; in2++) {
                    PH_BoolCircuit tr; memset(&tr, 0, sizeof(tr));
                    tr.n_inputs = ni; tr.n_gates = 1;
                    tr.gates[0] = (PH_Gate){(PH_GateType)gt, in1, in2, 0};
                    int eq = 1;
                    for (long long x = 0; x < nia && eq; x++) {
                        int inps[PH_MAX_INPUTS] = {0};
                        for (int i = 0; i < ni; i++) inps[i] = (int)((x>>i)&1);
                        if (ph_circuit_eval(c, inps) != ph_circuit_eval(&tr, inps)) eq = 0;
                    }
                    if (eq) return 1;
                }
            }
        }
    }
    if (tg <= 0) {
        int fo = -1, cnst = 1;
        for (long long x = 0; x < nia && cnst; x++) {
            int inps[PH_MAX_INPUTS] = {0};
            for (int i = 0; i < ni; i++) inps[i] = (int)((x>>i)&1);
            int o = ph_circuit_eval(c, inps);
            if (fo < 0) fo = o; else if (o != fo) cnst = 0;
        }
        if (cnst) return 1;
    }
    return 0;
}
