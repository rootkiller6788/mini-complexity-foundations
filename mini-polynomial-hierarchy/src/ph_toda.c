/* ph_toda.c - Toda Theorem: PH subseteq P^{#P} (1991, Godel Prize 1998) */
#include "ph.h"
#include <string.h>

long long ph_count_sat(const PH_CNF* f) {
    long long count = 0;
    int n = f->n_vars;
    if (n > 24) n = 24;
    long long total = 1LL << n;
    if (total > (1LL << 22)) total = 1LL << 22;
    int* assign = calloc((size_t)n, sizeof(int));
    if (!assign) return 0;
    for (long long m = 0; m < total; m++) {
        for (int i = 0; i < n; i++)
            assign[i] = (int)((m >> i) & 1);
        if (ph_cnf_eval(f, assign)) count++;
    }
    free(assign);
    return count;
}

static PH_CNF* restrict_cnf(const PH_CNF* f, const int* x_assign, int nx, int ny) {
    PH_CNF* g = malloc(sizeof(PH_CNF));
    if (!g) return NULL;
    memset(g, 0, sizeof(PH_CNF));
    g->n_vars = ny;
    for (int ci = 0; ci < f->n_clauses && g->n_clauses < PH_MAX_CLAUSES; ci++) {
        PH_Clause orig = f->clauses[ci], simplified;
        int sl = 0, clause_sat = 0;
        for (int j = 0; j < 3; j++) {
            int lit = orig.lits[j];
            if (lit < 0) continue;
            int var = lit >> 1, sign = lit & 1;
            if (var < nx) {
                if ((!sign && x_assign[var]) || (sign && !x_assign[var])) {
                    clause_sat = 1; break;
                }
            } else {
                simplified.lits[sl++] = ((var - nx) << 1) | sign;
            }
        }
        if (!clause_sat && sl > 0) {
            while (sl < 3) simplified.lits[sl++] = -1;
            g->clauses[g->n_clauses++] = simplified;
        }
    }
    return g;
}

int ph_sigma2_via_counting(const PH_QBF* qbf) {
    if (!qbf || qbf->n_blocks < 2) return qbf ? ph_sigma_k_sat(qbf) : 0;
    int nx = qbf->blocks[0].n_vars, ny = 0;
    for (int i = 1; i < qbf->n_blocks; i++) ny += qbf->blocks[i].n_vars;
    int* xa = calloc((size_t)nx, sizeof(int));
    if (!xa) return 0;
    long long mx = 1LL << nx; if (mx > 50000) mx = 50000;
    for (long long xm = 0; xm < mx; xm++) {
        for (int i = 0; i < nx; i++) xa[i] = (int)((xm >> i) & 1);
        PH_CNF* res = restrict_cnf(&qbf->matrix, xa, nx, ny);
        if (!res) continue;
        long long cnt = ph_count_sat(res);
        long long total_y = 1LL << ny;
        if (total_y > (1LL << 20)) total_y = 1LL << 20;
        if (cnt == total_y) { free(xa); free(res); return 1; }
        free(res);
    }
    free(xa); return 0;
}

int ph_valiant_vazirani(const PH_CNF* f, PH_CNF* iso) {
    if (!f || !iso) return 0;
    memcpy(iso, f, sizeof(PH_CNF));
    int n = f->n_vars; if (n > PH_MAX_VARS) n = PH_MAX_VARS;
    srand((unsigned int)time(NULL));
    for (int h = 0; h < n + 2 && iso->n_clauses < PH_MAX_CLAUSES - 4; h++) {
        int v0 = rand() % n, v1 = rand() % n, v2 = rand() % n, b = rand() & 1;
        int cl = iso->n_clauses;
        if (b == 0) {
            iso->clauses[cl+0] = (PH_Clause){{(v0<<1)|0, (v1<<1)|0, (v2<<1)|0}};
            iso->clauses[cl+1] = (PH_Clause){{(v0<<1)|1, (v1<<1)|1, (v2<<1)|0}};
            iso->clauses[cl+2] = (PH_Clause){{(v0<<1)|1, (v1<<1)|0, (v2<<1)|1}};
            iso->clauses[cl+3] = (PH_Clause){{(v0<<1)|0, (v1<<1)|1, (v2<<1)|1}};
        } else {
            iso->clauses[cl+0] = (PH_Clause){{(v0<<1)|1, (v1<<1)|1, (v2<<1)|1}};
            iso->clauses[cl+1] = (PH_Clause){{(v0<<1)|0, (v1<<1)|0, (v2<<1)|1}};
            iso->clauses[cl+2] = (PH_Clause){{(v0<<1)|0, (v1<<1)|1, (v2<<1)|0}};
            iso->clauses[cl+3] = (PH_Clause){{(v0<<1)|1, (v1<<1)|0, (v2<<1)|0}};
        }
        iso->n_clauses += 4;
    }
    return 1;
}
