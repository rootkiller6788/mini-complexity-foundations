/* ph_qbf_solver.c - QBF Solver: Sigma_k / Pi_k / TQBF */
#include "ph.h"

static int qbf_rec(const PH_QBF* qbf, int block_idx,
                   int* assign, long long* nodes) {
    int bs = qbf->blocks[block_idx].n_vars;
    int offset = qbf->blocks[block_idx].start_var;
    int is_last = (block_idx == qbf->n_blocks - 1);
    int is_exist = (qbf->blocks[block_idx].type == PH_QUANT_EXISTS);
    long long n_total = 1LL << bs;
    long long max_enum = (is_last && bs <= 20) ? n_total : (bs > 10 ? 1024 : n_total);
    if (n_total > max_enum) n_total = max_enum;
    long long step = (n_total >= (1LL << bs)) ? 1 : (1LL << bs) / n_total;
    if (is_exist) {
        for (long long m = 0; m < n_total; m++) {
            long long mask = (step > 1) ? (m * step) : m;
            for (int j = 0; j < bs; j++)
                assign[offset + j] = (int)((mask >> j) & 1);
            (*nodes)++;
            int r = is_last ? ph_cnf_eval(&qbf->matrix, assign)
                            : qbf_rec(qbf, block_idx + 1, assign, nodes);
            if (r) return 1;
        }
        return 0;
    } else {
        for (long long m = 0; m < n_total; m++) {
            long long mask = (step > 1) ? (m * step) : m;
            for (int j = 0; j < bs; j++)
                assign[offset + j] = (int)((mask >> j) & 1);
            (*nodes)++;
            int r = is_last ? ph_cnf_eval(&qbf->matrix, assign)
                            : qbf_rec(qbf, block_idx + 1, assign, nodes);
            if (!r) return 0;
        }
        return 1;
    }
}

static int plain_sat(const PH_CNF* f, int nv) {
    if (nv <= 0) return 1;
    if (nv > PH_MAX_VARS) nv = PH_MAX_VARS;
    long long total = 1LL << nv;
    if (total > (1LL << 20)) total = 1LL << 20;
    int* a = calloc((size_t)nv, sizeof(int));
    if (!a) return 0;
    for (long long m = 0; m < total; m++) {
        for (int i = 0; i < nv; i++) a[i] = (int)((m >> i) & 1);
        if (ph_cnf_eval(f, a)) { free(a); return 1; }
    }
    free(a);
    return 0;
}

int ph_sigma_k_sat(const PH_QBF* qbf) {
    if (!qbf) return 0;
    if (qbf->n_blocks == 0) return plain_sat(&qbf->matrix, qbf->total_vars);
    int* a = calloc((size_t)qbf->total_vars, sizeof(int));
    if (!a) return 0;
    long long nodes = 0;
    int r = qbf_rec(qbf, 0, a, &nodes);
    free(a);
    return r;
}

int ph_pi_k_sat(const PH_QBF* qbf) {
    if (!qbf) return 0;
    if (qbf->n_blocks == 0) return plain_sat(&qbf->matrix, qbf->total_vars);
    int* a = calloc((size_t)qbf->total_vars, sizeof(int));
    if (!a) return 0;
    long long nodes = 0;
    int r = qbf_rec(qbf, 0, a, &nodes);
    free(a);
    return r;
}

int ph_qbf_solve(const PH_QBF* qbf) {
    if (!qbf) return 1;
    if (qbf->n_blocks == 0) return plain_sat(&qbf->matrix, qbf->total_vars);
    int* a = calloc((size_t)qbf->total_vars, sizeof(int));
    if (!a) return 0;
    long long nodes = 0;
    int r = qbf_rec(qbf, 0, a, &nodes);
    free(a);
    return r;
}

int ph_tqbf_solve(const PH_QBF* qbf) {
    return ph_qbf_solve(qbf);
}
