/* space_tm.h — Space-Bounded Turing Machine Structures
 *
 * Defines the mathematical structures for space-bounded computation:
 * - Configuration graph: the central tool converting infinite TM runs
 *   into finite graph reachability problems
 * - Offline TM model: read-only input + read-write work tape
 * - Space-constructible function generator
 *
 * Key theorem: With s(n) space, a TM has at most
 *   |Q| · s(n) · |Γ|^{s(n)} configurations.
 * PATH in this graph ≡ acceptance. The config graph has size
 * O(2^{s(n)}) in general, making reachability a PSPACE-complete
 * problem for polynomial-space bounds.
 *
 * Reference: Arora & Barak §4.1; Sipser §8.1
 */

#ifndef SPACE_TM_H
#define SPACE_TM_H

#include "spaceh.h"

/*------------------------------------------------------------------
 * Turing Machine Configuration (Space-Bounded)
 *------------------------------------------------------------------*/

/* Configuration of a space-bounded TM:
 * state q ∈ Q, head positions for input and work tapes,
 * work tape contents bounded by s(n). */
typedef struct {
    int state;              /* current state */
    int input_head;         /* position on read-only input tape */
    int work_head;          /* position on work tape */
    int work_used;          /* max work tape position used so far */
    int work_tape[256];    /* work tape contents (bounded) */
    int tape_len;           /* logical tape length = min(s(n), 256) */
} TMConfig;

/* Transition: δ(q, σ, γ) = (q', γ', D_{input}, D_{work})
 * where D ∈ {L=-1, S=0, R=+1} */
typedef struct {
    int next_state;
    int write_symbol;
    int input_move;   /* -1, 0, +1 */
    int work_move;    /* -1, 0, +1 */
} TMTransition;

/* Space-Bounded Turing Machine */
typedef struct {
    /* Machine specification */
    char name[64];
    int n_states;              /* |Q| */
    int input_alphabet_size;   /* |Σ| */
    int work_alphabet_size;    /* |Γ| */
    int q_start;
    int q_accept;
    int q_reject;

    /* Transition function: [state][input_sym][work_sym] -> TMTransition */
    /* Stored as flat 3D array for simplicity */
    TMTransition* delta_flat;
    int delta_stride1, delta_stride2;

    /* Space bound function: input length -> space limit */
    size_t (*space_bound)(size_t input_len);
    int is_nondeterministic;

    /* Runtime simulation tracking */
    int steps_executed;
    int space_used_max;
} SpaceBoundedTM;

/*------------------------------------------------------------------
 * Configuration Graph
 *------------------------------------------------------------------*/

/* The config graph converts a space-bounded TM into a finite directed
 * graph: vertices = configurations, edges = valid transitions.
 * PATH(s, t) in this graph = TM accepts.
 * Number of configurations: N = |Q| · s(n) · |Γ|^{s(n)}. */

#define CFG_GRAPH_MAX_V  8192

typedef struct {
    int n_vertices;           /* |V| = number of configs enumerated */
    TMConfig* configs;        /* [n_vertices] */
    char** adjacency;          /* adjacency matrix, bit-packed possible */
    int source_vertex;        /* start config index */
    int* accept_vertices;     /* list of accepting config indices */
    int n_accept_vertices;
} ConfigGraphSpace;

/*------------------------------------------------------------------
 * Space-Constructible Functions
 *------------------------------------------------------------------*/

/* Standard space-constructible functions */
double sc_log_n(double n);           /* ⌈log₂ n⌉ */
double sc_n_to_k(double n, int k);   /* n^k */
double sc_two_to_nk(double n, int k); /* 2^{n^k} */
double sc_klog_n(double n, int k);   /* log^k n */

/* Verify f is space-constructible by building TM that computes f(n) in O(f(n)) space */
int is_space_constructible_fn(double (*f)(double), int max_n);

/*------------------------------------------------------------------
 * Configuration Graph Operations
 *------------------------------------------------------------------*/

/* Initialize a ConfigGraph from a TM and input */
ConfigGraphSpace* cfg_graph_build(SpaceBoundedTM* tm, const int* input,
                                   int input_len, int space_bound);

/* Check if accept is reachable from start via BFS/DFS */
int cfg_graph_accepts(ConfigGraphSpace* cg);

/* Free config graph */
void cfg_graph_free(ConfigGraphSpace* cg);

/* Count reachable configs (logspace possible with Savitch) */
int cfg_graph_reachable_count(ConfigGraphSpace* cg, int from_vertex);

/* Space-bounded TM construction helpers */
SpaceBoundedTM* sptm_create(const char* name, int n_states,
    int input_alpha, int work_alpha, size_t (*bound)(size_t));
void sptm_free(SpaceBoundedTM* tm);
void sptm_set_transition(SpaceBoundedTM* tm, int state, int input_sym,
    int work_sym, int next_state, int write_sym, int input_move, int work_move);
void sptm_set_accept_reject(SpaceBoundedTM* tm, int q_accept, int q_reject, int q_start);
int sptm_step(SpaceBoundedTM* tm, TMConfig* src, TMConfig* dst);

#endif /* SPACE_TM_H */
