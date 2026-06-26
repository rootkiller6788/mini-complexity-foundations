/* space_universal_tm.c -- Universal Space-Bounded TM
 * U(M,x) simulates M on x in O(s_M(|x|)) space.
 * Key lemma for Space Hierarchy Theorem.
 * NO log factor overhead (unlike time hierarchy).
 * Reference: Hartmanis-Lewis-Stearns (1965); Arora & Barak 4.1
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/spaceh.h"
#include "../include/space_tm.h"

typedef struct {
    int n_states, n_input_syms, n_work_syms;
    int q_start, q_accept, q_reject;
    int* delta_flat;
    int stride1, stride2;
} TMEncoding;

static TMEncoding* tm_decode(const char* desc, int desc_len) {
    (void)desc_len;
    TMEncoding* enc = malloc(sizeof(TMEncoding));
    enc->n_states = 3; enc->n_input_syms = 2; enc->n_work_syms = 3;
    enc->q_start = 0; enc->q_accept = 1; enc->q_reject = 2;
    const char* p = desc;
    while (*p) {
        if (strncmp(p, "S:", 2) == 0) enc->n_states = atoi(p + 2);
        else if (strncmp(p, "I:", 2) == 0) enc->n_input_syms = atoi(p + 2);
        else if (strncmp(p, "W:", 2) == 0) enc->n_work_syms = atoi(p + 2);
        else if (strncmp(p, "Q:", 2) == 0) {
            char* qs = strdup(p + 2);
            char* tok = strtok(qs, ",");
            if (tok) enc->q_start = atoi(tok);
            tok = strtok(NULL, ",");
            if (tok) enc->q_accept = atoi(tok);
            tok = strtok(NULL, ",");
            if (tok) enc->q_reject = atoi(tok);
            free(qs);
        }
        p++;
    }
    enc->stride1 = enc->n_input_syms * enc->n_work_syms;
    enc->stride2 = enc->n_work_syms;
    size_t ds = (size_t)enc->n_states * (size_t)enc->stride1;
    enc->delta_flat = calloc(ds * 4, sizeof(int));
    return enc;
}

static void tm_encoding_free(TMEncoding* enc) {
    if (enc) { free(enc->delta_flat); free(enc); }
}

static int utm_simulate(TMEncoding* enc, const int* input, int input_len,
                         int space_bound, int max_steps) {
    int* work_tape = calloc((size_t)space_bound, sizeof(int));
    int state = enc->q_start, input_head = 0, work_head = 0, steps = 0;
    while (steps < max_steps) {
        if (state == enc->q_accept) { free(work_tape); return 1; }
        if (state == enc->q_reject) { free(work_tape); return 0; }
        int input_sym = (input_head < input_len) ? input[input_head] : 0;
        int work_sym = work_tape[work_head];
        int idx = (state * enc->stride1
                   + input_sym * enc->stride2 + work_sym) * 4;
        int ns = enc->delta_flat[idx];
        int ws = enc->delta_flat[idx + 1];
        int im = enc->delta_flat[idx + 2];
        int wm = enc->delta_flat[idx + 3];
        work_tape[work_head] = ws; state = ns;
        input_head += im;
        if (input_head < 0) input_head = 0;
        if (input_head > input_len) input_head = input_len;
        work_head += wm;
        if (work_head < 0 || work_head >= space_bound)
            { free(work_tape); return -1; }
        steps++;
    }
    free(work_tape); return -1;
}

/* Diagonal language L_D = {1^n | M_n(1^n) rejects in g(n) space} */
static int diagonal_language_utm(int n, int (*f_bound)(int),
                                  int (*g_bound)(int)) {
    (void)f_bound;
    int space_limit = g_bound(n);
    if (space_limit <= 0) space_limit = 10;
    char desc[128];
    snprintf(desc, 127, "S:4 I:2 W:3 Q:0,1,2 n:%d", n);
    TMEncoding* enc = tm_decode(desc, (int)strlen(desc));
    for (int s = 0; s < enc->n_states; s++)
        for (int is_ = 0; is_ < enc->n_input_syms; is_++)
            for (int ws = 0; ws < enc->n_work_syms; ws++) {
                int idx = (s * enc->stride1
                           + is_ * enc->stride2 + ws) * 4;
                int hash = (n * 7919 + s * 6271 + ws * 4093) % 16;
                enc->delta_flat[idx] = hash % enc->n_states;
                enc->delta_flat[idx + 1] = hash % enc->n_work_syms;
                enc->delta_flat[idx + 2] = (hash % 3) - 1;
                enc->delta_flat[idx + 3] = (hash / 3) % 3 - 1;
            }
    int* input = malloc((size_t)n * sizeof(int));
    for (int i = 0; i < n; i++) input[i] = 1;
    int result = utm_simulate(enc, input, n, space_limit, 5000);
    free(input); tm_encoding_free(enc);
    if (result == 1) return 0;
    if (result == 0) return 1;
    return -1;
}

void diag_relativization_demo(void) {
    printf("\n===== Space Hierarchy Relativization =====\n\n");
    printf("Space Hierarchy RELATIVIZES: for ANY oracle A,\n");
    printf("  SPACE^A(f) != SPACE^A(g) when f=o(g).\n\n");
    printf("Unlike P vs NP (Baker-Gill-Solovay 1975):\n");
    printf("  Contradictory relativizations exist for P vs NP.\n");
    printf("  Space hierarchy has NO contradictory relativizations.\n");
    printf("  Diagonalization resolves space but NOT P vs NP.\n\n");
    printf("Reason: space-bounded machines have FEWER configs.\n");
    printf("  Can enumerate ALL oracle queries within space bound.\n");
}

static int utm_f_bound(int n) { return n; }
static int utm_g_bound(int n) { return n * n; }

void space_universal_tm_demo(void) {
    printf("\n===== Universal Space-Bounded TM =====\n\n");
    printf("Key Lemma: U(M,x) simulates M on x in O(s_M(|x|)) space.\n\n");
    printf("--- Diagonal Language Demo ---\n");
    printf("  f(n)=n, g(n)=n^2\n");
    printf("  L_D = {1^n | M_n(1^n) rejects in n^2 space}\n");
    for (int n = 1; n <= 6; n++) {
        int r = diagonal_language_utm(n, utm_f_bound, utm_g_bound);
        printf("  n=%d: L_D(1^%d) = %d\n", n, n, r);
    }
    printf("\n--- Why No Log Factor Overhead ---\n");
    printf("  Time hierarchy: O(T log T) overhead for step count.\n");
    printf("  Space hierarchy: Reuse same tape cells. O(s(n)) space!\n\n");
    diag_relativization_demo();
}
