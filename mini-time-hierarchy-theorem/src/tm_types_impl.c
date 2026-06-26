/* tm_types.c — Turing Machine Operations Implementation
 * Implements the API declared in include/tm_types.h
 * L1 Definitions, L3 Math Structures */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "tht.h"
#include "tm_types.h"

/* ── TM Construction ── */
TM *tm_create(int num_states, int num_symbols,
              int start, int accept, int reject) {
    if (num_states < 2 || num_states > TM_Q_MAX) return NULL;
    if (num_symbols < 2 || num_symbols > TM_GAMMA_MAX) return NULL;
    if (start >= num_states || accept >= num_states || reject >= num_states) return NULL;
    TM *m = calloc(1, sizeof(TM));
    if (!m) return NULL;
    m->num_states = num_states;
    m->num_symbols = num_symbols;
    m->start_state = start;
    m->accept_state = accept;
    m->reject_state = reject;
    m->is_deterministic = 1;
    m->input_alphabet_size = num_symbols - 1;
    snprintf(m->name, TM_NAME_LEN, "TM_%dstates_%dsyms", num_states, num_symbols);
    return m;
}

void tm_set_transition(TM *m, int q, int a, int r, int b, TMDirection D) {
    if (!m || q < 0 || q >= m->num_states || a < 0 || a >= m->num_symbols) return;
    m->delta[q][a].new_state = r;
    m->delta[q][a].new_symbol = b;
    m->delta[q][a].move = D;
}

TM *tm_clone(const TM *src) {
    if (!src) return NULL;
    TM *m = malloc(sizeof(TM));
    if (!m) return NULL;
    memcpy(m, src, sizeof(TM));
    return m;
}

void tm_destroy(TM *m) { free(m); }

/* ── TM Simulation ── */
int tm_simulate(const TM *m, const int *input, int input_len,
                int max_steps, int *steps_out, int *space_out) {
    if (!m) { *steps_out = 0; *space_out = 0; return -1; }
    int tape[TM_TAPE_SIZE], head = TM_TAPE_SIZE / 2;
    int min_h = head, max_h = head;
    memset(tape, 0, sizeof(tape));
    for (int i = 0; i < input_len && (head + i) < TM_TAPE_SIZE; i++)
        tape[head + i] = (input[i] != 0) ? 1 : 0;
    int state = m->start_state, step = 0;
    while (step < max_steps) {
        if (state == m->accept_state) { *steps_out = step; *space_out = max_h - min_h + 1; return 1; }
        if (state == m->reject_state) { *steps_out = step; *space_out = max_h - min_h + 1; return 0; }
        int sym = tape[head]; if (sym >= m->num_symbols) sym = 0;
        TMTransition tr = m->delta[state][sym];
        tape[head] = tr.new_symbol;
        state = tr.new_state;
        head += (int)tr.move;
        step++;
        if (head < 0 || head >= TM_TAPE_SIZE) { *steps_out = step; *space_out = max_h - min_h + 1; return 0; }
        if (head < min_h) min_h = head;
        if (head > max_h) max_h = head;
    }
    *steps_out = step; *space_out = max_h - min_h + 1; return -1;
}

int tm_trace(const TM *m, const int *input, int input_len,
             int max_steps, TMConfig **trace_out) {
    if (!m) { *trace_out = NULL; return -1; }
    TMConfig *trace = malloc((size_t)(max_steps + 2) * sizeof(TMConfig));
    if (!trace) { *trace_out = NULL; return -1; }
    int tape[TM_TAPE_SIZE], head = TM_TAPE_SIZE / 2;
    memset(tape, 0, sizeof(tape));
    for (int i = 0; i < input_len && (head + i) < TM_TAPE_SIZE; i++)
        tape[head + i] = (input[i] != 0) ? 1 : 0;
    int state = m->start_state, step = 0, trace_len = 0;
    while (step <= max_steps) {
        if (state == m->accept_state) {
            trace[trace_len].state = state; trace[trace_len].step_number = step;
            trace[trace_len].accepted = 1; *trace_out = trace; return trace_len + 1;
        }
        if (state == m->reject_state) {
            trace[trace_len].state = state; trace[trace_len].step_number = step;
            trace[trace_len].accepted = 0; *trace_out = trace; return trace_len + 1;
        }
        trace[trace_len].state = state; trace[trace_len].step_number = step;
        trace[trace_len].accepted = -1;
        for (int i = 0; i < TM_TAPE_SIZE; i++) trace[trace_len].tape.tape[i] = tape[i];
        trace[trace_len].tape.head_pos = head;
        trace_len++;
        if (step == max_steps) break;
        int sym = tape[head]; if (sym >= m->num_symbols) sym = 0;
        TMTransition tr = m->delta[state][sym];
        tape[head] = tr.new_symbol; state = tr.new_state;
        head += (int)tr.move; step++;
        if (head < 0 || head >= TM_TAPE_SIZE) break;
    }
    free(trace); *trace_out = NULL; return -1;
}

void tm_config_trace_free(TMConfig *trace, int length) { free(trace); (void)length; }

/* ── Godel Numbering ── */
int tm_godel_encode(const TM *m) {
    if (!m) return 0;
    int code = m->num_states * 1000000 + m->num_symbols * 100000 +
               m->start_state * 10000 + m->accept_state * 1000 + m->reject_state * 100;
    for (int q = 0; q < m->num_states && q < 4; q++)
        for (int a = 0; a < m->num_symbols && a < 3; a++)
            code += (int)(m->delta[q][a].new_state * 10 + m->delta[q][a].new_symbol);
    return code;
}

TM *tm_godel_decode(int index) {
    if (index <= 0) return NULL;
    int ns = 2 + (index % 4);
    int nsym = 2;
    TM *m = tm_create(ns, nsym, 0, ns - 1, ns - 2);
    if (!m) return NULL;
    m->godel_number = index;
    for (int q = 0; q < ns; q++)
        for (int a = 0; a < nsym; a++)
            tm_set_transition(m, q, a, (index + q * 7 + a * 13) % ns,
                              (index + a) % nsym, TM_RIGHT);
    return m;
}

TM *tm_get_enumerated(int index) {
    return tm_godel_decode(index);
}

int tm_enumeration_count(void) { return 100; }

/* ── TM Classification ── */
double tm_empirical_exponent(TM *m, int max_n) {
    if (!m || max_n < 2) return 0;
    double sx = 0, sy = 0, sxx = 0, sxy = 0;
    for (int i = 0; i < 10; i++) {
        int n = 10 + (i * max_n) / 10; if (n < 1) n = 1;
        int input[] = {0};
        clock_t t0 = clock();
        int steps, space;
        tm_simulate(m, input, 1, n * 100, &steps, &space);
        double t_ms = 1000.0 * (clock() - t0) / CLOCKS_PER_SEC;
        if (t_ms < 0.0001) t_ms = 0.0001;
        double x = log((double)n), y = log(t_ms);
        sx += x; sy += y; sxx += x * x; sxy += x * y;
    }
    double denom = 10 * sxx - sx * sx;
    if (fabs(denom) < 1e-12) return 0;
    return (10 * sxy - sx * sy) / denom;
}

int tm_is_decider(TM *m, int max_input, int time_bound) {
    if (!m) return 0;
    for (int n = 0; n <= max_input; n++) {
        int input[] = {n};
        int steps, space;
        int r = tm_simulate(m, input, 1, time_bound, &steps, &space);
        if (r == -1) return 0;
    }
    return 1;
}

int tm_recognizes_language(TM *m, int (*reference)(int),
                           int n_tests, int time_bound) {
    if (!m || !reference) return 0;
    for (int i = 0; i < n_tests; i++) {
        int n = i + 1;
        int input[] = {n};
        int steps, space;
        int r = tm_simulate(m, input, 1, time_bound, &steps, &space);
        int expected = reference(n);
        if ((r > 0 ? 1 : (r == 0 ? 0 : -1)) != expected) return 0;
    }
    return 1;
}
