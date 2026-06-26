#ifndef SPACEH_H
#define SPACEH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/* ================================================================
 * spaceh.h — Space Hierarchy Theorem: Core API
 *
 * SPACE(f) ⊊ SPACE(g) when f = o(g) and g is space-constructible.
 * Corollary: L ⊊ PSPACE (unconditional, unlike P vs NP).
 *
 * Key insight: Space hierarchy is stronger than Time hierarchy
 * because space is reusable — no log-factor overhead needed.
 *
 * Reference: Arora & Barak §4.1—§4.2; Sipser §8
 * ================================================================ */

/*------------------------------------------------------------------
 * L1: Core Definitions
 *------------------------------------------------------------------*/

/* Space-bounded computation: a TM using ≤ s(n) tape cells on
 * inputs of length n. The space bound s(n) must be space-constructible
 * (computable by an O(s(n))-space machine). */

typedef enum {
    SPACE_CLASS_L,          /* SPACE(log n) — Deterministic Logspace     */
    SPACE_CLASS_NL,         /* NSPACE(log n) — Nondeterministic Logspace */
    SPACE_CLASS_RL,         /* RL: Randomized Logspace (BPL subset)      */
    SPACE_CLASS_POLYLOG,    /* SPACE(log^k n) — Polylog Space            */
    SPACE_CLASS_PSPACE,     /* SPACE(poly(n)) — Polynomial Space         */
    SPACE_CLASS_NPSPACE,    /* NSPACE(poly(n)) — Nondeterministic PS     */
    SPACE_CLASS_EXPSPACE,   /* SPACE(2^poly(n)) — Exponential Space      */
    SPACE_CLASS_DSPACE,     /* DSPACE(s(n)) — Generic deterministic bound */
    SPACE_CLASS_NSPACE      /* NSPACE(s(n)) — Generic nondeterministic   */
} SpaceClass;

/* Space bound descriptor */
typedef struct {
    SpaceClass class_id;
    const char* name;           /* "L", "NL", "PSPACE", etc. */
    const char* canonical_fn;   /* "log n", "poly(n)", "2^poly(n)", etc. */
    double (*bound_fn)(double n); /* actual space bound function s(n) */
    int proven_separated_from_l;  /* 1 if known L != this class  */
    int proven_separated_from_nl; /* 1 if known NL != this class */
} SpaceClassDef;

/*------------------------------------------------------------------
 * L2: Core Concepts
 *------------------------------------------------------------------*/

/* Space constructibility: f is space-constructible if there exists
 * a TM that on input 1^n computes bin(f(n)) using O(f(n)) space.
 * All "natural" functions (log n, n^k, 2^n) are space-constructible. */
int is_space_constructible(double (*f)(double));
double space_construct_log_n(double n);
double space_construct_poly_n(double n, int k);
double space_construct_exp_n(double n, double base);

/* Configuration graph of a space-bounded TM:
 * Vertices = configurations (state, head pos, tape contents)
 * Edges = legal transitions.
 * With s(n) space: ≤ |Q|·s(n)·|Γ|^{s(n)} configurations. */
typedef struct {
    int n_configs;         /* number of configurations */
    int n_states;          /* |Q| */
    int alphabet_size;     /* |Γ| */
    int space_bound;       /* s(|input|) */
    int* config_states;
    int* config_head_pos;
    int** config_tapes;
    char** adjacency;       /* adjacency matrix of config graph */
} ConfigGraph;

/*------------------------------------------------------------------
 * L3: Mathematical Structures
 *------------------------------------------------------------------*/

/* Space-bounded Turing Machine (offline model with read-only input tape,
 * read-write work tape, write-only output tape). Only work tape counts. */
#define SPTM_MAX_STATES  16
#define SPTM_MAX_SYMBOLS  8
#define SPTM_MAX_CONFIGS  8192

typedef struct {
    int state;            /* current state q ∈ Q */
    int work_head;         /* work tape head position */
    int work_tape_len;     /* currently used work tape length */
    int work_tape[SPTM_MAX_CONFIGS]; /* work tape contents */
} SPTMConfig;

typedef struct {
    int n_states;          /* |Q| */
    int n_syms;            /* |Γ| (work tape alphabet) */
    int n_input_syms;      /* |Σ| (input tape alphabet) */
    int q0;                /* start state */
    int q_accept;          /* accepting state */
    int q_reject;          /* rejecting state */
    /* transition: (state, input_sym, work_sym) -> (new_state, new_work_sym, work_move, input_move) */
    int delta[SPTM_MAX_STATES][SPTM_MAX_SYMBOLS][SPTM_MAX_SYMBOLS][4];
    /* config graph built during simulation */
    int n_configs;
    SPTMConfig configs[SPTM_MAX_CONFIGS];
    char edges[SPTM_MAX_CONFIGS][SPTM_MAX_CONFIGS / 8 + 1]; /* bit-packed */
} SpaceTM;

/*------------------------------------------------------------------
 * L4: Fundamental Laws — API
 *------------------------------------------------------------------*/

/* Space Hierarchy Theorem */
void space_hierarchy_demo(void);
int space_hierarchy_diagonal_language(int n);

/* Savitch Theorem: NSPACE(s) ⊆ DSPACE(s²) */
void savitch_full_demo(void);
int savitch_path(int** edges, int m, int s, int t, int steps, int n_vertices);

/* Immerman—Szelepcsényi: NL = coNL */
void immerman_szelepcsenyi_demo(void);

/* Space-bounded TMs */
void space_tm_demo(void);

/* Space Diagonalization */
void space_diagonal_demo(void);
int space_bounded_simulate(int machine_id, int input, int space_bound);

/*------------------------------------------------------------------
 * L5: Algorithms/Methods
 *------------------------------------------------------------------*/

/* Logspace algorithms */
void logspace_algos_demo(void);
int logspace_add_binary(const char* a, const char* b, char* result, int len);
int logspace_palindrome_check(const char* s, int len);
void logspace_counter_demo(int n);
int logspace_multiply(int a, int b, int bits);

/* Logspace problems */
void logspace_problems_demo(void);

/* NL algorithms */
void nl_algorithms_demo(void);
int nl_path(int** adj, int n, int s, int t, int max_steps);
int nl_non_path(int** adj, int n, int s, int t);

/* NL-Completeness */
void nl_complete_demo(void);

/* PSPACE algorithms */
void pspace_algorithms_demo(void);
int tqbf_evaluate(const char* formula, int len);

/* Logspace reduction: A ≤_L B */
void logspace_reduction_demo(void);

/*------------------------------------------------------------------
 * L6: Canonical Problems
 *------------------------------------------------------------------*/

/* PATH: Directed s-t connectivity — NL-complete */
int path_is_nl_complete(void);

/* TQBF: True Quantified Boolean Formulas — PSPACE-complete */
void tqbf_demo(void);

/*------------------------------------------------------------------
 * L7: Applications
 *------------------------------------------------------------------*/

/* 2-SAT solver (NL application to VLSI CAD) */
void two_sat_application_demo(void);

/* Game playing in PSPACE */
void game_playing_demo(void);

/* Model checking in PSPACE */
void model_checking_demo(void);

/*------------------------------------------------------------------
 * L8: Advanced Topics
 *------------------------------------------------------------------*/

/* Alternating Logspace and the Chandra-Kozen-Stockmeyer theorem */
void alternating_logspace_demo(void);

/* Randomized Logspace: RL, BPL */
void randomized_logspace_demo(void);

/* Reingold's Theorem (2008): Undirected ST-Connectivity ∈ L */
void reingold_theorem_demo(void);

/*------------------------------------------------------------------
 * L9: Research Frontiers
 *------------------------------------------------------------------*/

/* Space-bounded derandomization */
void space_derandomization_summary(void);

/*------------------------------------------------------------------
 * Utilities
 *------------------------------------------------------------------*/

void space_classes_demo(void);
void space_bench(void);

#endif /* SPACEH_H */
