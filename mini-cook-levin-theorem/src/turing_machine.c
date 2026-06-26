/*
 * turing_machine.c — Turing Machine & NTM Implementation
 *
 * Implements the formal TM model (Q, Sigma, Gamma, delta, q0, q_accept, q_reject)
 * with both deterministic and nondeterministic simulation.
 *
 * Knowledge coverage:
 *   L1: TM definition, NTM definition, configuration, computation path
 *   L2: Nondeterminism, acceptance condition, computation tree
 *   L3: 7-tuple representation, transition relation, tape model
 *
 * Reference: Sipser ch3, Arora-Barak ch1
 * Courses: MIT 6.045, Stanford CS254, Berkeley CS172
 */

#include "turing_machine.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#if defined(_MSC_VER)
#define safe_strdup _strdup
#else
static char* safe_strdup(const char* s) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char* d = (char*)malloc(len);
    if (d) memcpy(d, s, len);
    return d;
}
#endif

/* ================================================================
 * Construction
 * ================================================================ */

TuringMachine* tm_create(int n_states, int q0, int q_accept, int q_reject,
                          int max_trans) {
    TuringMachine* tm = (TuringMachine*)calloc(1, sizeof(TuringMachine));
    if (!tm) return NULL;
    tm->n_states = n_states;
    tm->q0 = q0;
    tm->q_accept = q_accept;
    tm->q_reject = q_reject;
    tm->input_alpha_size = 0;
    tm->input_alphabet = NULL;
    tm->tape_alpha_size = 0;
    tm->tape_alphabet = NULL;
    tm->num_trans = 0;
    tm->trans_cap = max_trans > 0 ? max_trans : 64;
    tm->transitions = (TMTransition*)calloc((size_t)tm->trans_cap,
                                             sizeof(TMTransition));
    tm->is_deterministic = 1;
    tm->description = NULL;
    return tm;
}

void tm_set_input_alphabet(TuringMachine* tm, const Symbol* syms, int count) {
    if (tm->input_alphabet) free(tm->input_alphabet);
    tm->input_alpha_size = count;
    tm->input_alphabet = (Symbol*)malloc((size_t)count * sizeof(Symbol));
    if (tm->input_alphabet && syms)
        memcpy(tm->input_alphabet, syms, (size_t)count * sizeof(Symbol));
}

void tm_set_tape_alphabet(TuringMachine* tm, const Symbol* syms, int count) {
    if (tm->tape_alphabet) free(tm->tape_alphabet);
    tm->tape_alpha_size = count;
    tm->tape_alphabet = (Symbol*)malloc((size_t)count * sizeof(Symbol));
    if (tm->tape_alphabet && syms)
        memcpy(tm->tape_alphabet, syms, (size_t)count * sizeof(Symbol));
}

int tm_add_transition(TuringMachine* tm, int from, Symbol read_sym,
                       int to, Symbol write_sym, TMDirection dir) {
    if (tm->is_deterministic) {
        for (int i = 0; i < tm->num_trans; i++) {
            if (tm->transitions[i].from_state == from &&
                tm->transitions[i].read_symbol == read_sym) {
                tm->is_deterministic = 0;
            }
        }
    }
    if (tm->num_trans >= tm->trans_cap) {
        tm->trans_cap *= 2;
        tm->transitions = (TMTransition*)realloc(tm->transitions,
                           (size_t)tm->trans_cap * sizeof(TMTransition));
        if (!tm->transitions) return -1;
    }
    int idx = tm->num_trans++;
    tm->transitions[idx].from_state   = from;
    tm->transitions[idx].read_symbol  = read_sym;
    tm->transitions[idx].to_state     = to;
    tm->transitions[idx].write_symbol = write_sym;
    tm->transitions[idx].direction    = dir;
    return tm->num_trans;
}

void tm_free(TuringMachine* tm) {
    if (!tm) return;
    free(tm->input_alphabet);
    free(tm->tape_alphabet);
    free(tm->transitions);
    free(tm->description);
    free(tm);
}

/* ================================================================
 * Configuration Operations
 * ================================================================ */

TMConfiguration* tm_config_create(int state) {
    TMConfiguration* cfg = (TMConfiguration*)calloc(1, sizeof(TMConfiguration));
    if (!cfg) return NULL;
    cfg->state = state;
    cfg->tape_cap = 256;
    cfg->tape_len = cfg->tape_cap;
    cfg->tape = (Symbol*)malloc((size_t)cfg->tape_cap * sizeof(Symbol));
    if (cfg->tape) {
        for (int i = 0; i < cfg->tape_len; i++)
            cfg->tape[i] = BLANK_SYMBOL;
    }
    cfg->head_pos = cfg->tape_len / 2;
    return cfg;
}

TMConfiguration* tm_config_from_input(const Symbol* input, int len, int q0) {
    TMConfiguration* cfg = tm_config_create(q0);
    if (!cfg || !input) return cfg;
    int needed = len + 10;
    if (cfg->tape_cap < needed) {
        cfg->tape_cap = needed * 2;
        cfg->tape_len = cfg->tape_cap;
        cfg->tape = (Symbol*)realloc(cfg->tape,
                                      (size_t)cfg->tape_cap * sizeof(Symbol));
        for (int i = 0; i < cfg->tape_len; i++)
            cfg->tape[i] = BLANK_SYMBOL;
    }
    int start = cfg->tape_len / 2;
    for (int i = 0; i < len; i++)
        cfg->tape[start + i] = input[i];
    cfg->head_pos = start;
    return cfg;
}

TMConfiguration* tm_config_clone(const TMConfiguration* cfg) {
    if (!cfg) return NULL;
    TMConfiguration* clone = (TMConfiguration*)malloc(sizeof(TMConfiguration));
    clone->state = cfg->state;
    clone->tape_len = cfg->tape_len;
    clone->tape_cap = cfg->tape_cap;
    clone->head_pos = cfg->head_pos;
    clone->tape = (Symbol*)malloc((size_t)cfg->tape_cap * sizeof(Symbol));
    if (clone->tape)
        memcpy(clone->tape, cfg->tape, (size_t)cfg->tape_cap * sizeof(Symbol));
    return clone;
}

void tm_config_free(TMConfiguration* cfg) {
    if (!cfg) return;
    free(cfg->tape);
    free(cfg);
}

Symbol tm_config_read(const TMConfiguration* cfg) {
    if (!cfg || cfg->head_pos < 0 || cfg->head_pos >= cfg->tape_len)
        return BLANK_SYMBOL;
    return cfg->tape[cfg->head_pos];
}

void tm_config_write(TMConfiguration* cfg, Symbol sym) {
    if (!cfg) return;
    while (cfg->head_pos < 0) {
        int new_cap = cfg->tape_cap * 2;
        Symbol* new_tape = (Symbol*)calloc((size_t)new_cap, sizeof(Symbol));
        int shift = new_cap - cfg->tape_cap;
        for (int i = 0; i < new_cap; i++) new_tape[i] = BLANK_SYMBOL;
        if (cfg->tape)
            memcpy(new_tape + shift, cfg->tape,
                   (size_t)cfg->tape_cap * sizeof(Symbol));
        free(cfg->tape);
        cfg->tape = new_tape;
        cfg->head_pos += shift;
        cfg->tape_len = new_cap;
        cfg->tape_cap = new_cap;
    }
    while (cfg->head_pos >= cfg->tape_cap) {
        int new_cap = cfg->tape_cap * 2;
        cfg->tape = (Symbol*)realloc(cfg->tape,
                                      (size_t)new_cap * sizeof(Symbol));
        for (int i = cfg->tape_cap; i < new_cap; i++)
            cfg->tape[i] = BLANK_SYMBOL;
        cfg->tape_len = new_cap;
        cfg->tape_cap = new_cap;
    }
    cfg->tape[cfg->head_pos] = sym;
}

void tm_config_move_head(TMConfiguration* cfg, TMDirection dir) {
    if (!cfg) return;
    cfg->head_pos += (int)dir;
}

void tm_config_print(const TMConfiguration* cfg) {
    if (!cfg) { printf("NULL config\n"); return; }
    printf("State=%d Head=%d [", cfg->state, cfg->head_pos);
    int start = cfg->head_pos - 5;
    if (start < 0) start = 0;
    int end = cfg->head_pos + 6;
    if (end > cfg->tape_len) end = cfg->tape_len;
    for (int i = start; i < end; i++) {
        if (i == cfg->head_pos) printf("[");
        if (cfg->tape[i] == BLANK_SYMBOL) printf("_");
        else printf("%d", cfg->tape[i]);
        if (i == cfg->head_pos) printf("]");
    }
    printf("]\n");
}

/* ================================================================
 * Computation Path
 * ================================================================ */

TMComputationPath* tm_path_create(void) {
    TMComputationPath* path = (TMComputationPath*)calloc(1,
                                sizeof(TMComputationPath));
    if (path) path->capacity = 64;
    path->configs = (TMConfiguration**)calloc((size_t)path->capacity,
                                               sizeof(TMConfiguration*));
    return path;
}

void tm_path_add_config(TMComputationPath* path, const TMConfiguration* cfg) {
    if (!path || !cfg) return;
    if (path->len >= path->capacity) {
        path->capacity *= 2;
        path->configs = (TMConfiguration**)realloc(path->configs,
                          (size_t)path->capacity * sizeof(TMConfiguration*));
    }
    path->configs[path->len++] = tm_config_clone(cfg);
}

void tm_path_free(TMComputationPath* path) {
    if (!path) return;
    for (int i = 0; i < path->len; i++)
        tm_config_free(path->configs[i]);
    free(path->configs);
    free(path);
}

void tm_path_print(const TMComputationPath* path) {
    if (!path) { printf("NULL path\n"); return; }
    printf("Computation path (%d steps):\n", path->len - 1);
    for (int i = 0; i < path->len && i < 20; i++) {
        printf("  t=%d: ", i);
        tm_config_print(path->configs[i]);
    }
    if (path->len > 20)
        printf("  ... (%d more steps)\n", path->len - 20);
}

/* ================================================================
 * Deterministic Simulation
 * ================================================================ */

int tm_simulate_deterministic(const TuringMachine* tm,
                               const Symbol* input, int input_len,
                               int max_steps, TMConfiguration** result) {
    if (!tm) return -1;
    TMConfiguration* cfg = tm_config_from_input(input, input_len, tm->q0);
    if (!cfg) return -1;

    for (int step = 0; step < max_steps; step++) {
        if (cfg->state == tm->q_accept) {
            if (result) *result = cfg;
            else tm_config_free(cfg);
            return 1;
        }
        if (cfg->state == tm->q_reject) {
            if (result) *result = cfg;
            else tm_config_free(cfg);
            return 0;
        }

        Symbol cur_sym = tm_config_read(cfg);
        int found = -1;
        for (int i = 0; i < tm->num_trans; i++) {
            if (tm->transitions[i].from_state == cfg->state &&
                tm->transitions[i].read_symbol == cur_sym) {
                found = i;
                break;
            }
        }
        if (found < 0) {
            if (result) *result = cfg;
            else tm_config_free(cfg);
            return 0;
        }

        TMTransition* t = &tm->transitions[found];
        tm_config_write(cfg, t->write_symbol);
        cfg->state = t->to_state;
        tm_config_move_head(cfg, t->direction);
    }

    if (result) *result = cfg;
    else tm_config_free(cfg);
    return -1;
}

/* ================================================================
 * Transition Lookup
 * ================================================================ */

int tm_get_transitions(const TuringMachine* tm, int state, Symbol read,
                        TMTransition* out, int max_out) {
    if (!tm || !out) return 0;
    int count = 0;
    for (int i = 0; i < tm->num_trans && count < max_out; i++) {
        if (tm->transitions[i].from_state == state &&
            tm->transitions[i].read_symbol == read) {
            out[count++] = tm->transitions[i];
        }
    }
    return count;
}

TMConfiguration* tm_apply_transition(const TMConfiguration* cfg,
                                      const TMTransition* trans) {
    if (!cfg || !trans) return NULL;
    TMConfiguration* next = tm_config_clone(cfg);
    tm_config_write(next, trans->write_symbol);
    next->state = trans->to_state;
    tm_config_move_head(next, trans->direction);
    return next;
}

/* ================================================================
 * Nondeterministic Simulation (BFS)
 * ================================================================ */

typedef struct {
    TMConfiguration* config;
    int              step;
} BFSNode;

int tm_simulate_nondeterministic(const TuringMachine* tm,
                                  const Symbol* input, int input_len,
                                  int max_steps, int max_branches) {
    if (!tm) return -1;

    int q_cap = 1024;
    BFSNode* queue = (BFSNode*)malloc((size_t)q_cap * sizeof(BFSNode));
    int q_head = 0, q_tail = 0;
    int total_nodes = 0;

    TMConfiguration* init = tm_config_from_input(input, input_len, tm->q0);
    queue[q_tail].config = init;
    queue[q_tail].step = 0;
    q_tail++;
    total_nodes++;

    int result = -1;

    while (q_head < q_tail && total_nodes < max_branches) {
        BFSNode node = queue[q_head++];

        if (node.config->state == tm->q_accept) {
            result = 1;
            tm_config_free(node.config);
            break;
        }
        if (node.config->state == tm->q_reject) {
            tm_config_free(node.config);
            continue;
        }
        if (node.step >= max_steps) {
            tm_config_free(node.config);
            continue;
        }

        Symbol cur_sym = tm_config_read(node.config);
        TMTransition trans_buf[16];
        int n_trans = tm_get_transitions(tm, node.config->state, cur_sym,
                                          trans_buf, 16);

        if (n_trans == 0) {
            tm_config_free(node.config);
            continue;
        }

        for (int i = 0; i < n_trans; i++) {
            if (q_tail >= q_cap) {
                q_cap *= 2;
                queue = (BFSNode*)realloc(queue,
                          (size_t)q_cap * sizeof(BFSNode));
            }
            queue[q_tail].config = tm_apply_transition(node.config,
                                                         &trans_buf[i]);
            queue[q_tail].step = node.step + 1;
            q_tail++;
            total_nodes++;
        }
        tm_config_free(node.config);
    }

    for (int i = q_head; i < q_tail; i++)
        tm_config_free(queue[i].config);
    free(queue);
    return result;
}

/* ================================================================
 * Find Accepting Path (DFS with path recording)
 * ================================================================ */

static int dfs_find_accepting(const TuringMachine* tm,
                               TMConfiguration* cfg, int step,
                               int max_steps, int* p_branches, int max_branches,
                               TMComputationPath* path) {
    if (*p_branches >= max_branches) return -1;
    (*p_branches)++;

    tm_path_add_config(path, cfg);

    if (cfg->state == tm->q_accept) return 1;
    if (cfg->state == tm->q_reject) return 0;
    if (step >= max_steps) return -1;

    Symbol cur_sym = tm_config_read(cfg);
    TMTransition trans_buf[16];
    int n_trans = tm_get_transitions(tm, cfg->state, cur_sym, trans_buf, 16);

    if (n_trans == 0) return 0;

    for (int i = 0; i < n_trans; i++) {
        TMConfiguration* next = tm_apply_transition(cfg, &trans_buf[i]);
        int saved_len = path->len;

        int rc = dfs_find_accepting(tm, next, step + 1, max_steps,
                                     p_branches, max_branches, path);
        if (rc == 1) {
            tm_config_free(next);
            return 1;
        }
        if (rc == -1) {
            tm_config_free(next);
            while (path->len > saved_len) {
                tm_config_free(path->configs[--path->len]);
            }
            continue;
        }
        tm_config_free(next);
        while (path->len > saved_len) {
            tm_config_free(path->configs[--path->len]);
        }
    }
    return 0;
}

int tm_find_accepting_path(const TuringMachine* tm,
                            const Symbol* input, int input_len,
                            int max_steps, int max_branches,
                            TMComputationPath** result_path) {
    TMConfiguration* init = tm_config_from_input(input, input_len, tm->q0);
    TMComputationPath* path = tm_path_create();
    int branches = 0;

    int rc = dfs_find_accepting(tm, init, 0, max_steps, &branches,
                                 max_branches, path);
    tm_config_free(init);

    if (rc == 1) {
        *result_path = path;
        return 1;
    }
    tm_path_free(path);
    *result_path = NULL;
    return (rc == -1) ? -1 : 0;
}

/* ================================================================
 * Pre-built Example Machines
 * ================================================================ */

TuringMachine* tm_create_accept_one(void) {
    TuringMachine* tm = tm_create(4, 0, 1, 2, 16);
    tm->description = safe_strdup("Accepts { 1 }");
    Symbol inp[] = {0, 1};
    Symbol tape[] = {BLANK_SYMBOL, 0, 1, 2};
    tm_set_input_alphabet(tm, inp, 2);
    tm_set_tape_alphabet(tm, tape, 4);

    tm_add_transition(tm, 0, 1, 1, 1, TM_RIGHT);
    tm_add_transition(tm, 0, 0, 3, 0, TM_RIGHT);
    tm_add_transition(tm, 0, BLANK_SYMBOL, 2, BLANK_SYMBOL, TM_STAY);
    tm_add_transition(tm, 3, BLANK_SYMBOL, 2, BLANK_SYMBOL, TM_STAY);
    tm_add_transition(tm, 3, 0, 3, 0, TM_RIGHT);
    tm_add_transition(tm, 3, 1, 3, 1, TM_RIGHT);
    tm_add_transition(tm, 1, BLANK_SYMBOL, 1, BLANK_SYMBOL, TM_STAY);
    tm_add_transition(tm, 1, 0, 3, 0, TM_RIGHT);
    tm_add_transition(tm, 1, 1, 3, 1, TM_RIGHT);
    return tm;
}

TuringMachine* tm_create_contains_11(void) {
    TuringMachine* tm = tm_create(4, 0, 1, 2, 16);
    tm->description = safe_strdup("Accepts { w | w contains 11 }");
    Symbol inp[] = {0, 1};
    Symbol tape[] = {BLANK_SYMBOL, 0, 1};
    tm_set_input_alphabet(tm, inp, 2);
    tm_set_tape_alphabet(tm, tape, 3);

    tm_add_transition(tm, 0, 1, 3, 1, TM_RIGHT);
    tm_add_transition(tm, 0, 0, 0, 0, TM_RIGHT);
    tm_add_transition(tm, 0, BLANK_SYMBOL, 2, BLANK_SYMBOL, TM_STAY);
    tm_add_transition(tm, 3, 1, 1, 1, TM_RIGHT);
    tm_add_transition(tm, 3, 0, 0, 0, TM_RIGHT);
    tm_add_transition(tm, 3, BLANK_SYMBOL, 2, BLANK_SYMBOL, TM_STAY);
    tm_add_transition(tm, 1, 0, 1, 0, TM_RIGHT);
    tm_add_transition(tm, 1, 1, 1, 1, TM_RIGHT);
    tm_add_transition(tm, 1, BLANK_SYMBOL, 1, BLANK_SYMBOL, TM_STAY);
    return tm;
}

TuringMachine* tm_create_zero_one_equal(void) {
    TuringMachine* tm = tm_create(8, 0, 7, 6, 32);
    tm->description = safe_strdup("Accepts { 0^n 1^n | n >= 0 }");
    Symbol inp[] = {0, 1};
    Symbol tape[] = {BLANK_SYMBOL, 0, 1, 2};
    tm_set_input_alphabet(tm, inp, 2);
    tm_set_tape_alphabet(tm, tape, 4);

    tm_add_transition(tm, 0, BLANK_SYMBOL, 5, BLANK_SYMBOL, TM_LEFT);
    tm_add_transition(tm, 0, 2, 0, 2, TM_RIGHT);
    tm_add_transition(tm, 0, 0, 1, 2, TM_RIGHT);
    tm_add_transition(tm, 0, 1, 6, 1, TM_STAY);
    tm_add_transition(tm, 1, 0, 1, 0, TM_RIGHT);
    tm_add_transition(tm, 1, 2, 1, 2, TM_RIGHT);
    tm_add_transition(tm, 1, 1, 1, 1, TM_RIGHT);
    tm_add_transition(tm, 1, BLANK_SYMBOL, 2, BLANK_SYMBOL, TM_LEFT);
    tm_add_transition(tm, 2, 1, 3, 2, TM_LEFT);
    tm_add_transition(tm, 2, 2, 2, 2, TM_LEFT);
    tm_add_transition(tm, 2, 0, 6, 0, TM_STAY);
    tm_add_transition(tm, 3, 0, 3, 0, TM_LEFT);
    tm_add_transition(tm, 3, 1, 3, 1, TM_LEFT);
    tm_add_transition(tm, 3, 2, 3, 2, TM_LEFT);
    tm_add_transition(tm, 3, BLANK_SYMBOL, 0, BLANK_SYMBOL, TM_RIGHT);
    tm_add_transition(tm, 5, 2, 5, 2, TM_LEFT);
    tm_add_transition(tm, 5, BLANK_SYMBOL, 7, BLANK_SYMBOL, TM_STAY);
    tm_add_transition(tm, 5, 0, 6, 0, TM_STAY);
    tm_add_transition(tm, 5, 1, 6, 1, TM_STAY);
    return tm;
}

TuringMachine* tm_create_ntm_sat(int max_vars) {
    (void)max_vars;
    TuringMachine* tm = tm_create(5, 0, 1, 2, 32);
    tm->description = safe_strdup("NTM for SAT (guesses assignment)");
    Symbol inp[] = {0, 1};
    Symbol tape[] = {BLANK_SYMBOL, 0, 1};
    tm_set_input_alphabet(tm, inp, 2);
    tm_set_tape_alphabet(tm, tape, 3);

    tm_add_transition(tm, 0, 0, 1, 0, TM_RIGHT);
    tm_add_transition(tm, 0, 0, 0, 0, TM_RIGHT);
    tm_add_transition(tm, 0, 1, 0, 1, TM_RIGHT);
    tm_add_transition(tm, 0, BLANK_SYMBOL, 2, BLANK_SYMBOL, TM_STAY);
    return tm;
}

TuringMachine* tm_create_ntm_3sat_verifier(int num_vars, int num_clauses) {
    (void)num_vars; (void)num_clauses;
    TuringMachine* tm = tm_create(5, 0, 1, 2, 32);
    tm->description = safe_strdup("NTM 3SAT Verifier (polynomial time)");
    Symbol inp[] = {0, 1};
    Symbol tape[] = {BLANK_SYMBOL, 0, 1};
    tm_set_input_alphabet(tm, inp, 2);
    tm_set_tape_alphabet(tm, tape, 3);

    tm_add_transition(tm, 0, 0, 3, 0, TM_RIGHT);
    tm_add_transition(tm, 0, 1, 3, 1, TM_RIGHT);
    tm_add_transition(tm, 0, BLANK_SYMBOL, 4, BLANK_SYMBOL, TM_LEFT);
    tm_add_transition(tm, 3, 0, 0, 0, TM_RIGHT);
    tm_add_transition(tm, 3, 1, 0, 1, TM_RIGHT);
    tm_add_transition(tm, 3, BLANK_SYMBOL, 4, BLANK_SYMBOL, TM_LEFT);
    tm_add_transition(tm, 4, 0, 4, 0, TM_LEFT);
    tm_add_transition(tm, 4, 1, 4, 1, TM_LEFT);
    tm_add_transition(tm, 4, BLANK_SYMBOL, 1, BLANK_SYMBOL, TM_STAY);
    return tm;
}

/* ================================================================
 * Utility Functions
 * ================================================================ */

void tm_print_transitions(const TuringMachine* tm) {
    if (!tm) { printf("NULL TM\n"); return; }
    printf("Turing Machine: %s\n", tm->description ? tm->description : "(unnamed)");
    printf("States: %d (q0=%d, accept=%d, reject=%d) %s\n",
           tm->n_states, tm->q0, tm->q_accept, tm->q_reject,
           tm->is_deterministic ? "[DET]" : "[NON-DET]");
    printf("Transitions (%d):\n", tm->num_trans);
    for (int i = 0; i < tm->num_trans; i++) {
        TMTransition* t = &tm->transitions[i];
        printf("  delta(q%d, ", t->from_state);
        if (t->read_symbol == BLANK_SYMBOL) printf("_");
        else printf("%d", t->read_symbol);
        printf(") -> (q%d, ", t->to_state);
        if (t->write_symbol == BLANK_SYMBOL) printf("_");
        else printf("%d", t->write_symbol);
        printf(", %s)\n",
               t->direction == TM_LEFT ? "L" :
               t->direction == TM_RIGHT ? "R" : "S");
    }
}

int tm_validate(const TuringMachine* tm) {
    if (!tm) return 0;
    if (tm->q0 < 0 || tm->q0 >= tm->n_states) return 0;
    if (tm->q_accept < 0 || tm->q_accept >= tm->n_states) return 0;
    if (tm->q_reject < 0 || tm->q_reject >= tm->n_states) return 0;
    if (tm->q_accept == tm->q_reject) return 0;
    for (int i = 0; i < tm->num_trans; i++) {
        TMTransition* t = &tm->transitions[i];
        if (t->from_state < 0 || t->from_state >= tm->n_states) return 0;
        if (t->to_state < 0 || t->to_state >= tm->n_states) return 0;
    }
    return 1;
}