#ifndef THT_H
#define THT_H

/* ── tht.h — Time Hierarchy Theorem: Core API ──
 * Hartmanis-Stearns (TAMS 1965), Turing Award 1993.
 * TIME(f) != TIME(g) when f(n)*log f(n) = o(g(n)) and f,g time-constructible.
 *
 * Knowledge levels covered: L1 Definitions, L2 Core Concepts, L4 Theorems */

/* ═══════════════════════════════════════════════════════════════
 * L1: Core Definitions (typedef structs for TM, config, classes)
 * ═══════════════════════════════════════════════════════════════ */

/* Turing Machine: (Q, Σ, Γ, δ, q0, q_accept, q_reject) — Sipser §3.1 */
#define TM_MAX_STATES 32
#define TM_MAX_SYMBOLS 8
#define TM_TAPE_LENGTH 2048
#define TM_MAX_TRANSITIONS 1024

typedef struct {
    int n_states;           /* |Q| */
    int n_symbols;          /* |Γ| */
    int q0;                 /* start state */
    int q_accept;           /* accept state */
    int q_reject;           /* reject state */
    /* δ: Q × Γ → Q × Γ × {L,R,S}. Flattened: [q][sym][new_sym,new_q,move] */
    int delta[TM_MAX_STATES][TM_MAX_SYMBOLS][3];
    int is_deterministic;
    int desc[32];           /* Godel number of this TM */
} TuringMachine;

/* TM Configuration: state, tape, head position — for step-by-step trace */
typedef struct {
    int state;
    int tape[TM_TAPE_LENGTH];
    int head_position;
    int step_count;
    int space_used;         /* max head excursion */
} TMConfiguration;

/* Time-bound function: n → steps. Time-constructible if computable in O(f(n)). */
typedef double (*TimeBoundFn)(double n);

/* Complexity class descriptor */
typedef struct {
    const char *name;       /* e.g., "TIME(n^2)", "EXP" */
    TimeBoundFn bound;      /* resource bound function */
    int deterministic;      /* 1=DET, 0=NDET */
    int alternation_depth;  /* 0=P, 1=NP, ... */
} ComplexityClass;

/* Language: a subset of {0,1}* — represented as decision function */
typedef int (*Language)(int n);

/* Oracle: a set A ⊆ {0,1}*. Query answered in O(1). */
typedef int (*Oracle)(int query);

/* Oracle Turing Machine: M^A — like TM but with oracle query tape */
typedef struct {
    TuringMachine base;
    Oracle oracle;
    int oracle_calls;
} OracleTM;

/* ═══════════════════════════════════════════════════════════════
 * L2-L4: Core Operations & Theorem Implementations
 * ═══════════════════════════════════════════════════════════════ */

/* Universal TM: simulate arbitrary TM on input with step bound.
 * Runtime: O(T log T) where T is steps of simulated TM.
 * Essential for time hierarchy proof (AB Theorem 1.9). */
int universal_simulate(int machine_id, int input, int time_bound);

/* Universal TM (full version): simulate a given TM struct.
 * Returns: 1=accept, 0=reject, -1=timeout. Sets steps_taken. */
int universal_simulate_tm(const TuringMachine *tm, const int *input,
                          int input_len, int time_bound, int *steps_taken,
                          int *space_used);

/* Diagonal language: L_D = {n | M_n rejects n within g(n) steps}.
 * Theorem: L_D ∈ TIME(g) but L_D ∉ TIME(f) when f·log f = o(g).
 * This is the heart of the Time Hierarchy Theorem. */
int diagonal_language(int x);
int diagonal_language_extended(int n, TimeBoundFn g_bound);

/* Time Hierarchy Theorem main demo: P ⊊ EXP */
void time_hierarchy_demo(int max_k, int max_n);

/* BGS Limits: oracle-based barriers */
void bgs_limits_demo(void);

/* TM Simulator with timing analysis */
void tm_simulator_demo(void);

/* Benchmark: measure wall-clock time for machine families */
void benchmark_tht(void);

/* Fuzz test diagonal property */
void fuzz_tht(void);

/* Diagonalization proof demo (Cantor-style) */
void diagonal_demo(void);

/* Universal Turing Machine demo */
void utm_demo(void);

/* Time-constructible functions demo */
void constructible_demo(void);

/* Full complexity hierarchy table */
void hierarchy_table(void);

/* Hartmanis-Stearns full demo */
void hs_demo(void);

/* P vs EXP unconditional separation */
void poly_vs_exp_demo(void);

/* Oracle machines and BGS Theorem */
void oracle_machines_demo(void);

/* Nondeterministic time hierarchy */
void nondet_hierarchy_demo(void);

/* Linear speedup theorem demo */
void speedup_demo(void);

/* Alternating time hierarchy */
void alternation_demo(void);

/* Gap theorem demo */
void gap_theorem_demo(void);

/* Exponential hierarchy demo */
void exponential_hierarchy_demo(void);

/* Padding argument demo */
void padding_demo(void);

/* Limits of proof techniques */
void limits_demo(void);

/* Complexity class membership tester */
void complexity_tester(void);

/* L7: Cryptographic application — OWF timing analysis */
void crypto_timing_demo(void);

/* L7: Model checking complexity application */
void model_checking_demo(void);

/* L7: Circuit verification timing (Tesla, NASA applications) */
void circuit_verification_demo(void);

/* L8: Monte Carlo complexity estimation */
void monte_carlo_complexity_demo(void);

/* L8: Bayesian timing analysis */
void bayesian_timing_demo(void);

/* ═══════════════════════════════════════════════════════════════
 * L4: Fundamental Laws — verified assertions
 * ═══════════════════════════════════════════════════════════════ */

/* Theorem TH1: TIME(n^k) ⊊ TIME(n^{k+1}) for all k ≥ 1.
 * Proved via universal simulation + diagonalization. */
int verify_hierarchy_separation_k(int k, int n_max);

/* Theorem TH2: P ⊊ EXP (corollary of TH1).
 * P = ∪_k TIME(n^k), EXP = ∪_k TIME(2^{n^k}). */
int verify_p_not_in_exp(int n_test);

/* Theorem TH3: NTIME(n^k) ⊊ NTIME(n^{k+1}) for k ≥ 1.
 * Cook (1973) / Zak (1983). Proved via translational diagonalization. */
int verify_nondet_hierarchy(int k, int n_max);

/* Theorem TH4 (Borodin Gap): ∃f computable s.t. TIME(f) = TIME(2^f).
 * Shows time-constructibility is ESSENTIAL for hierarchy theorem. */
int verify_gap_theorem_exists(void);

/* Theorem TH5 (Blum Speedup): ∃L with no optimal algorithm.
 * For every algorithm for L, ∃ one n² times faster. */
int verify_speedup_property(int n_samples);

#endif