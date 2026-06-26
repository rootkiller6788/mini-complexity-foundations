/*
 * turing_machine.h — Turing Machine & Nondeterministic TM
 *
 * Formal Definition (L1, L3):
 *   A TM is a 7-tuple M = (Q, Σ, Γ, δ, q₀, q_accept, q_reject)
 *   Q: finite set of states
 *   Σ: input alphabet (does not contain blank)
 *   Γ: tape alphabet (Σ ∪ {blank} ⊆ Γ)
 *   δ: transition function (deterministic) or relation (nondeterministic)
 *   q₀ ∈ Q: initial state
 *   q_accept ∈ Q: accept state
 *   q_reject ∈ Q: reject state, q_accept ≠ q_reject
 *
 * Nondeterministic TM: δ ⊆ (Q × Γ) × (Q × Γ × {L, R})
 * An NTM accepts if ANY computation branch reaches q_accept.
 *
 * Reference: Sipser §3.1-3.2, Arora-Barak §1.2-1.3
 * Courses: MIT 6.045, Stanford CS254, Berkeley CS172, CMU 15-455
 */

#ifndef TURING_MACHINE_H
#define TURING_MACHINE_H

#include <stdlib.h>
#include <stdint.h>

/* ── Symbol and Direction Types ──────────────────────────── */
typedef int Symbol;
#define BLANK_SYMBOL (-1)

typedef enum { TM_LEFT = -1, TM_STAY = 0, TM_RIGHT = 1 } TMDirection;

/* ── Transition ──────────────────────────────────────────── */
typedef struct {
    int         from_state;
    Symbol      read_symbol;
    int         to_state;
    Symbol      write_symbol;
    TMDirection direction;
} TMTransition;

/* ── Turing Machine ──────────────────────────────────────── */
typedef struct {
    int          n_states;
    int          q0, q_accept, q_reject;
    int          input_alpha_size;
    Symbol*      input_alphabet;
    int          tape_alpha_size;
    Symbol*      tape_alphabet;
    int          num_trans;
    int          trans_cap;
    TMTransition* transitions;
    int          is_deterministic;
    char*        description;
} TuringMachine;

/* ── Configuration / Snapshot ────────────────────────────── */
typedef struct {
    int     state;
    Symbol* tape;
    int     tape_len, tape_cap, head_pos;
} TMConfiguration;

/* ── Computation Path ────────────────────────────────────── */
typedef struct {
    TMConfiguration** configs;
    int              len, capacity;
} TMComputationPath;

/* ── Construction ────────────────────────────────────────── */
TuringMachine* tm_create(int n_states, int q0, int q_accept, int q_reject,
                          int max_trans);
void tm_set_input_alphabet(TuringMachine* tm, const Symbol* syms, int count);
void tm_set_tape_alphabet(TuringMachine* tm, const Symbol* syms, int count);
int  tm_add_transition(TuringMachine* tm, int from, Symbol read_sym,
                        int to, Symbol write_sym, TMDirection dir);
void tm_free(TuringMachine* tm);

/* ── Configuration API ───────────────────────────────────── */
TMConfiguration* tm_config_create(int state);
TMConfiguration* tm_config_from_input(const Symbol* input, int len, int q0);
TMConfiguration* tm_config_clone(const TMConfiguration* cfg);
void tm_config_free(TMConfiguration* cfg);
Symbol tm_config_read(const TMConfiguration* cfg);
void   tm_config_write(TMConfiguration* cfg, Symbol sym);
void   tm_config_move_head(TMConfiguration* cfg, TMDirection dir);
void   tm_config_print(const TMConfiguration* cfg);

/* ── Computation Path API ────────────────────────────────── */
TMComputationPath* tm_path_create(void);
void tm_path_add_config(TMComputationPath* path, const TMConfiguration* cfg);
void tm_path_free(TMComputationPath* path);
void tm_path_print(const TMComputationPath* path);

/* ── Simulation ──────────────────────────────────────────── */
int tm_simulate_deterministic(const TuringMachine* tm, const Symbol* input,
                               int input_len, int max_steps,
                               TMConfiguration** result);
int tm_get_transitions(const TuringMachine* tm, int state, Symbol read,
                        TMTransition* out, int max_out);
TMConfiguration* tm_apply_transition(const TMConfiguration* cfg,
                                      const TMTransition* trans);
int tm_simulate_nondeterministic(const TuringMachine* tm, const Symbol* input,
                                  int input_len, int max_steps, int max_branches);
int tm_find_accepting_path(const TuringMachine* tm, const Symbol* input,
                            int input_len, int max_steps, int max_branches,
                            TMComputationPath** result_path);

/* ── Pre-built Machines ──────────────────────────────────── */
TuringMachine* tm_create_accept_one(void);
TuringMachine* tm_create_contains_11(void);
TuringMachine* tm_create_zero_one_equal(void);
TuringMachine* tm_create_ntm_sat(int max_vars);
TuringMachine* tm_create_ntm_3sat_verifier(int num_vars, int num_clauses);

/* ── Utility ─────────────────────────────────────────────── */
void tm_print_transitions(const TuringMachine* tm);
int  tm_validate(const TuringMachine* tm);

#endif
