#ifndef TM_TYPES_H
#define TM_TYPES_H

/* ── tm_types.h — Turing Machine Data Types and Operations ──
 * L1 Definitions: Turing machine formal structures.
 * L3 Math Structures: Q, Σ, Γ, δ as concrete C types.
 *
 * References: Sipser §3.1, AB §1.2, Papadimitriou §2 */

#include <time.h>

/* ══════════════════════════════
 * Turing Machine Formal Definition
 * A 7-tuple: M = (Q, Σ, Γ, δ, q0, q_accept, q_reject) */

#define TM_Q_MAX      32    /* states */
#define TM_GAMMA_MAX   8    /* tape symbols */
#define TM_TAPE_SIZE 4096    /* tape cells */
#define TM_NAME_LEN    64    /* max name length */

/* Transition: δ(q, a) = (r, b, D) where D ∈ {L, R, S} */
typedef enum { TM_LEFT = -1, TM_STAY = 0, TM_RIGHT = 1 } TMDirection;

typedef struct {
    int new_symbol;       /* b ∈ Γ */
    int new_state;        /* r ∈ Q */
    TMDirection move;     /* L, R, or S */
} TMTransition;

/* Turing Machine struct — formal definition */
typedef struct {
    int num_states;                /* |Q| */
    int num_symbols;               /* |Γ| */
    int start_state;               /* q0 */
    int accept_state;              /* q_accept */
    int reject_state;              /* q_reject */
    /* δ: state × symbol → transition. Use TM_MAX indices. */
    TMTransition delta[TM_Q_MAX][TM_GAMMA_MAX];
    int is_deterministic;          /* 1=DFA-like, 0=NTM */
    int input_alphabet_size;       /* |Σ| */
    char name[TM_NAME_LEN];        /* descriptive identifier */
    int godel_number;              /* Godel encoding of this TM */
} TM;

/* Tape configuration snapshot */
typedef struct {
    int tape[TM_TAPE_SIZE];
    int head_pos;
    int min_head;          /* leftmost visited */
    int max_head;          /* rightmost visited */
} TapeSnapshot;

/* Machine configuration at a given step */
typedef struct {
    int state;
    TapeSnapshot tape;
    int step_number;
    int accepted;          /* 1=accept, 0=reject, -1=running */
} TMConfig;

/* ══════════════════════════════
 * TM Construction and Manipulation */

/* Create a blank TM with given parameters.
 * Returns NULL if parameters invalid. */
TM *tm_create(int num_states, int num_symbols,
              int start, int accept, int reject);

/* Set a specific transition δ(q, a) = (r, b, D) */
void tm_set_transition(TM *m, int q, int a, int r, int b, TMDirection D);

/* Deep copy a TM */
TM *tm_clone(const TM *src);

/* Destroy a TM, freeing all memory */
void tm_destroy(TM *m);

/* ══════════════════════════════
 * TM Execution */

/* Simulate TM for at most max_steps. Returns:
 *   1 = accept, 0 = reject, -1 = timeout.
 * Fills *steps_out with actual steps taken.
 * Fills *space_out with tape cells visited. */
int tm_simulate(const TM *m, const int *input, int input_len,
                int max_steps, int *steps_out, int *space_out);

/* Run TM and collect full configuration trace.
 * Returns number of configs (or -1 on timeout).
 * Caller must free trace with tm_config_trace_free(). */
int tm_trace(const TM *m, const int *input, int input_len,
             int max_steps, TMConfig **trace_out);

/* Free config trace array */
void tm_config_trace_free(TMConfig *trace, int length);

/* ══════════════════════════════
 * TM Enumeration (Godel Numbering) */

/* Encode TM as Godel number (product of prime powers).
 * Every TM is assigned a unique integer. */
int tm_godel_encode(const TM *m);

/* Decode index i into a TM (simplified Godel decoder).
 * Returns NULL if i invalid. */
TM *tm_godel_decode(int index);

/* Return the i-th TM in the standard enumeration (M_0, M_1, ...).
 * TMs are COUNTABLY infinite. */
TM *tm_get_enumerated(int index);

/* Number of TMs in the finite enumeration library (for demo). */
int tm_enumeration_count(void);

/* ══════════════════════════════
 * TM Classification */

/* Estimate the asymptotic time complexity of a TM empirically.
 * Returns the measured exponent (e.g., 2.0 = quadratic). */
double tm_empirical_exponent(TM *m, int max_n);

/* Test if TM is a decider (halts on all inputs within bound). */
int tm_is_decider(TM *m, int max_input, int time_bound);

/* Test if TM recognizes language L exactly.
 * Compares with reference decision function on n random inputs. */
int tm_recognizes_language(TM *m, int (*reference)(int),
                           int n_tests, int time_bound);

#endif
