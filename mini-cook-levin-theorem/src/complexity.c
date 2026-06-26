/*
 * complexity.c ? Complexity Theory Implementations
 *
 * L1-L4: Implements complexity class definitions, polynomial bounds,
 *        certificate/verifier framework, and self-reducibility.
 *
 * Reference: Arora-Barak 2, Sipser 7, Papadimitriou
 * Courses: MIT 6.045/6.841, Stanford CS254, Berkeley CS172, CMU 15-855
 */

#include "complexity.h"
#include "sat.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

const char* complexity_class_name(ComplexityClass c) {
    switch (c) {
        case CLASS_P:               return "P";
        case CLASS_NP:              return "NP";
        case CLASS_CONP:            return "coNP";
        case CLASS_PSPACE:          return "PSPACE";
        case CLASS_NPSPACE:         return "NPSPACE";
        case CLASS_EXP:             return "EXP";
        case CLASS_NEXP:            return "NEXP";
        case CLASS_EXPSPACE:        return "EXPSPACE";
        case CLASS_NP_COMPLETE:     return "NP-Complete";
        case CLASS_PSPACE_COMPLETE: return "PSPACE-Complete";
        case CLASS_EXP_COMPLETE:    return "EXP-Complete";
        default:                    return "UNKNOWN";
    }
}

double poly_eval(const Polynomial* p, double n) {
    if (!p) return 0.0;
    return p->coefficient * pow(n, p->degree);
}

int poly_is_big_o(const Polynomial* p1, const Polynomial* p2) {
    if (!p1 || !p2) return 0;
    if (p1->degree < p2->degree) return 1;
    if (p1->degree > p2->degree) return 0;
    return p1->coefficient <= p2->coefficient;
}

int poly_is_reasonable(const Polynomial* p) {
    if (!p) return 0;
    return p->degree >= 0 && p->degree <= 10.0;
}

const Polynomial POLY_CONSTANT    = {1.0, 0.0};
const Polynomial POLY_LOG         = {1.0, 0.0};
const Polynomial POLY_LINEAR      = {1.0, 1.0};
const Polynomial POLY_N_LOG_N     = {1.0, 1.0};
const Polynomial POLY_QUADRATIC   = {1.0, 2.0};
const Polynomial POLY_CUBIC       = {1.0, 3.0};
const Polynomial POLY_EXPONENTIAL = {1.0, 100.0};

void timer_start(Timer* t) {
    if (!t) return;
    t->start = clock();
    t->running = 1;
    t->elapsed = 0;
}

double timer_elapsed_ms(Timer* t) {
    if (!t || !t->running) return 0.0;
    return (double)(clock() - t->start) / CLOCKS_PER_SEC * 1000.0;
}

double timer_elapsed_us(Timer* t) {
    if (!t || !t->running) return 0.0;
    return (double)(clock() - t->start) / CLOCKS_PER_SEC * 1000000.0;
}

void timer_reset(Timer* t) {
    if (!t) return;
    t->start = clock();
    t->elapsed = 0;
    t->running = 1;
}

GrowthAnalysis* growth_analyze(double (*f)(int size),
                                const int* sizes, int n_sizes) {
    if (!f || !sizes || n_sizes <= 0) return NULL;
    GrowthAnalysis* ga = (GrowthAnalysis*)calloc(1, sizeof(GrowthAnalysis));
    if (!ga) return NULL;
    ga->n_points = n_sizes;
    ga->input_sizes = (int*)malloc((size_t)n_sizes * sizeof(int));
    ga->times = (double*)malloc((size_t)n_sizes * sizeof(double));
    for (int i = 0; i < n_sizes; i++) {
        ga->input_sizes[i] = sizes[i];
        ga->times[i] = f(sizes[i]);
    }
    double sum_x = 0.0, sum_y = 0.0, sum_xy = 0.0, sum_xx = 0.0;
    int valid = 0;
    for (int i = 0; i < n_sizes; i++) {
        if (ga->times[i] > 0 && sizes[i] > 0) {
            double lx = log((double)sizes[i]);
            double ly = log(ga->times[i]);
            sum_x += lx; sum_y += ly;
            sum_xy += lx * ly; sum_xx += lx * lx;
            valid++;
        }
    }
    if (valid >= 2) {
        ga->fitted_degree = (double)valid * sum_xy - sum_x * sum_y;
        double denom = (double)valid * sum_xx - sum_x * sum_x;
        if (fabs(denom) > 1e-12) ga->fitted_degree /= denom;
        else ga->fitted_degree = 0.0;
        double y_mean = sum_y / (double)valid;
        double ss_res = 0.0, ss_tot = 0.0;
        for (int i = 0; i < n_sizes; i++) {
            if (ga->times[i] > 0 && sizes[i] > 0) {
                double pred = ga->fitted_degree * log((double)sizes[i]);
                double act = log(ga->times[i]);
                ss_res += (act - pred) * (act - pred);
                ss_tot += (act - y_mean) * (act - y_mean);
            }
        }
        ga->r_squared = (ss_tot > 1e-12) ? 1.0 - ss_res / ss_tot : 0.0;
    }
    return ga;
}

void growth_analysis_free(GrowthAnalysis* ga) {
    if (!ga) return;
    free(ga->input_sizes);
    free(ga->times);
    free(ga);
}

void growth_analysis_print(const GrowthAnalysis* ga) {
    if (!ga) return;
    printf("Growth Analysis: O(n^%.3f), R2=%.4f, pts=%d\n",
           ga->fitted_degree, ga->r_squared, ga->n_points);
}

Certificate* cert_create(int capacity) {
    Certificate* cert = (Certificate*)calloc(1, sizeof(Certificate));
    if (!cert) return NULL;
    cert->capacity = capacity > 0 ? capacity : 64;
    cert->data = (int*)malloc((size_t)cert->capacity * sizeof(int));
    cert->length = 0;
    return cert;
}

void cert_append(Certificate* cert, int value) {
    if (!cert) return;
    if (cert->length >= cert->capacity) {
        cert->capacity *= 2;
        cert->data = (int*)realloc(cert->data, (size_t)cert->capacity * sizeof(int));
    }
    cert->data[cert->length++] = value;
}

void cert_free(Certificate* cert) {
    if (!cert) return;
    free(cert->data);
    free(cert);
}

Certificate* cert_clone(const Certificate* cert) {
    if (!cert) return NULL;
    Certificate* clone = cert_create(cert->capacity);
    if (!clone) return NULL;
    for (int i = 0; i < cert->length; i++)
        cert_append(clone, cert->data[i]);
    return clone;
}

int np_verify_certificate(const void* instance, const Certificate* cert,
                           NPVerifier verifier, int cert_length_bound) {
    if (!instance || !cert || !verifier) return 0;
    if (cert->length > cert_length_bound) return 0;
    return verifier(instance, cert);
}

int cl_compute_space_bound(int input_len, int time_steps) {
    return time_steps + input_len + 2;
}

int poly_bound_linear(int n)       { return n; }
int poly_bound_quadratic(int n)    { return n * n; }
int poly_bound_cubic(int n)        { return n * n * n; }
int poly_bound_nk(int n, int k, int coefficient) {
    int result = coefficient;
    for (int i = 0; i < k; i++) result *= n;
    return result;
}

int sat_self_reduce(const CNF* cnf, Assignment assign,
                     int (*sat_oracle)(const CNF*)) {
    if (!cnf || !assign) return 0;
    int n = cnf->n_vars;
    if (n == 0) return sat_oracle ? sat_oracle(cnf) : sat_verify(cnf, assign);
    CNF* mutable_cnf = cnf_clone(cnf);
    int result = sat_solve_dpll(mutable_cnf, assign);
    cnf_free(mutable_cnf);
    return result;
}

int complexity_p_in_np(void)        { return 1; }
int complexity_np_in_pspace(void)   { return 1; }
int complexity_pspace_in_exp(void)  { return 1; }
int complexity_p_in_exp(void)       { return 1; }
int complexity_p_not_equal_exp(void){ return 1; }

void complexity_print_diagram(void) {
    printf("P subseteq NP subseteq PSPACE = NPSPACE subseteq EXP\n");
    printf("P != EXP (Time Hierarchy Theorem)\n");
    printf("Open: P ?= NP, NP ?= coNP, PSPACE ?= EXP\n");
}
