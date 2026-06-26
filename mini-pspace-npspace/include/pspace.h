/* pspace.h — PSPACE & NPSPACE: Polynomial Space Complexity Theory
 *
 * References:
 *   AB (Arora-Barak, 2009) §4: Space Complexity
 *   Sipser (2013) §8: Space Complexity
 *   Savitch (1970): NSPACE(s) ⊆ SPACE(s²)
 *   Immerman-Szelepcsenyi (1987): NL = coNL
 *   Chandra-Kozen-Stockmeyer (1981): AP = PSPACE
 *   Stockmeyer-Meyer (1973): TQBF is PSPACE-complete
 *
 * Space hierarchy: L ⊆ NL ⊆ P ⊆ NP ⊆ PSPACE = NPSPACE ⊆ EXP
 *
 * This module covers: space classes, Savitch theorem, Immerman theorem,
 * AP=PSPACE, TQBF, PSPACE-complete games, logspace reductions,
 * configuration graphs, succinct representations, QBF solving. */
#ifndef PSPACE_H
#define PSPACE_H

#include <stddef.h>

/* ========================================================================
 * L1: Core Definitions — Data Types
 * ======================================================================== */

/* --- Space Complexity Classes --- */
typedef enum {
    CLASS_L,         /* SPACE(log n) — deterministic logspace */
    CLASS_NL,        /* NSPACE(log n) — nondeterministic logspace */
    CLASS_P,         /* TIME(poly(n)) — polynomial time */
    CLASS_NP,        /* NTIME(poly(n)) — nondeterministic polynomial time */
    CLASS_PSPACE,    /* SPACE(poly(n)) — polynomial space */
    CLASS_NPSPACE,   /* NSPACE(poly(n)) = PSPACE (Savitch) */
    CLASS_EXP,       /* TIME(2^{poly(n)}) — exponential time */
    CLASS_NEXP,      /* NTIME(2^{poly(n)}) — nondeterministic exponential */
    CLASS_2EXP       /* TIME(2^{2^{poly(n)}}) — double exponential */
} ComplexityClass;

/* --- Turing Machine Configuration (space-bounded) --- */
typedef struct {
    int state;           /* current state q ∈ Q */
    int head;            /* tape head position */
    int* tape;           /* tape content: symbols in Γ ∪ {blank} */
    int tape_len;        /* active tape length */
    int space_bound;     /* s(n): max cells used */
} TMConfiguration;

/* --- Space-Bounded Turing Machine Definition --- */
typedef struct {
    int n_states;        /* |Q| */
    int n_symbols;       /* |Γ| (input alphabet is {0,1,blank}) */
    int start_state;     /* q₀ */
    int accept_state;    /* q_acc */
    int reject_state;    /* q_rej */
    /* Transition table: (from_state, from_symbol) → (to_state, write_sym, move_dir) */
    int* trans_from_s;   /* length: n_trans */
    int* trans_from_r;   /* length: n_trans */
    int* trans_to_s;     /* length: n_trans */
    int* trans_to_w;     /* length: n_trans */
    int* trans_to_m;     /* length: n_trans, +1=right, -1=left, 0=stay */
    int n_trans;
} SpaceTM;

/* --- Quantified Boolean Formula (QBF/TQBF) --- */
typedef struct {
    int n_vars;          /* number of variables */
    int n_clauses;       /* number of 3-CNF clauses */
    char* quant_prefix;  /* quantifier prefix: 'E'=exists, 'A'=forall, length n_vars */
    int** clauses;       /* clauses[i][j] = literal: (var<<1)|sign, sign:0=pos,1=neg */
    int* clause_lens;    /* length of each clause (typically 3 for 3-CNF) */
} QBFInstance;

/* --- QBF Literal Encoding ---
 * lit = (var_index << 1) | sign_bit
 * sign_bit = 0: positive literal (x_var)
 * sign_bit = 1: negative literal (¬x_var)
 * lit == -1: padding/end-of-clause marker */
#define QBF_LIT(var, neg) (((var) << 1) | ((neg) ? 1 : 0))
#define QBF_VAR(lit)      ((lit) >> 1)
#define QBF_SIGN(lit)     ((lit) & 1)
#define QBF_PAD           (-1)

/* --- Configuration Graph --- */
typedef struct {
    int n_configs;       /* number of configurations (= vertices) */
    TMConfiguration** configs; /* array of configurations */
    int** adjacency;     /* adjacency matrix: n_configs × n_configs */
    int start_idx;       /* index of start configuration */
    int accept_idx;      /* index of accepting configuration */
} ConfigGraph;

/* --- Alternating Turing Machine --- */
typedef enum {
    ATM_EXISTENTIAL,    /* accept if SOME child accepts (OR-node) */
    ATM_UNIVERSAL,       /* accept if ALL children accept (AND-node) */
    ATM_ACCEPT,         /* terminal: accept */
    ATM_REJECT          /* terminal: reject */
} ATMStateType;

typedef struct {
    int from_state;
    int read_symbol;
    int to_state;
    int write_symbol;
    int move_dir;        /* +1=R, -1=L, 0=S */
} ATMTransition;

typedef struct {
    int n_states;
    ATMStateType* state_types;  /* type of each state */
    int n_trans;
    ATMTransition* transitions;
    int start_state;
} AlternatingTM;

/* --- 2-Player Game State (for PSPACE-complete games) --- */
typedef struct {
    int* board;          /* game board (flattened) */
    int board_size;      /* total cells */
    int num_players;     /* always 2 for PSPACE games */
    int current_player;  /* 0 = Player 1 (Exists), 1 = Player 2 (Forall) */
    int moves_made;      /* current move count */
    int max_moves;       /* polynomial bound on moves */
} GamePosition;

/* --- Directed Graph (for NL reachability / PATH) --- */
typedef struct {
    int n_vertices;
    int** adjacency;     /* adjacency matrix */
    int* out_degree;
} Digraph;

/* --- Succinct Graph Representation ---
 * A graph with N = 2^n vertices described by a Boolean circuit C(u,v)
 * that outputs 1 iff edge (u,v) exists. */
typedef int (*SuccinctEdgePredicate)(int u, int v, void* aux);

typedef struct {
    int n_bits;          /* vertices are n-bit strings */
    int n_vertices;      /* = 2^n_bits */
    SuccinctEdgePredicate edge_fn;  /* C(u,v) */
    void* aux_data;      /* auxiliary data for circuit */
} SuccinctGraph;

/* --- QBF Solver Statistics --- */
typedef struct {
    long long nodes_explored;
    long long branches_pruned;
    long long unit_propagations;
    double time_ms;
    int result;          /* 0=FALSE, 1=TRUE */
} QBFSolverStats;

/* ========================================================================
 * L3: Mathematical Structures — Operations
 * ======================================================================== */

/* --- QBF Evaluation (PSPACE-complete decision problem) --- */

/** qbf_evaluate: Recursively evaluate TQBF instance.
 *  @param n_vars   Number of quantified variables
 *  @param quants   Quantifier prefix string ('E'/'A')
 *  @param clauses  3-CNF clauses [nc][3], literals encoded as above
 *  @param nc       Number of clauses
 *  @return 1 if TRUE, 0 if FALSE
 *  Time: O(2^n), Space: O(n) — polynomial space */
int qbf_evaluate(int n_vars, const char* quants, int clauses[][3], int nc);

/* --- Savitch: Recursive Reachability --- */

/** savitch_reachability: Test if there is a path of length ≤ steps from s to t.
 *  Uses Savitch's divide-and-conquer: PATH(s,t,2^k) = ∃m: PATH(s,m,2^{k-1}) ∧ PATH(m,t,2^{k-1})
 *  @return 1 if path exists, 0 otherwise
 *  Space: O(log²|V| · log steps) */
int savitch_reachability(int s, int t, int steps, int n, int** edges, int m);

/* --- Configuration Graph Construction --- */

/** build_config_graph: From a SpaceTM and input, construct the configuration graph.
 *  Vertices = all configurations using ≤ space_bound cells.
 *  @return ConfigGraph with start and accept marked */
ConfigGraph* build_config_graph(SpaceTM* tm, const int* input, int input_len, int space_bound);

/** config_graph_free: Deallocate configuration graph */
void config_graph_free(ConfigGraph* cg);

/* --- NL=coNL: Inductive Counting (Immerman-Szelepcsenyi) --- */

/** immerman_count_reachable: Count how many vertices are reachable from s
 *  in ≤ k steps. Uses nondeterministic inductive counting (NL algorithm).
 *  This is the core of the NL=coNL proof. */
int immerman_count_reachable(Digraph* g, int s, int max_steps);

/** immerman_non_reachability: Verify s does NOT reach t in ≤ k steps,
 *  using the complementation technique. Runs in NL. */
int immerman_non_reachability(Digraph* g, int s, int t, int max_steps);

/* --- Alternating TM Evaluation --- */

/** atm_evaluate: Simulate an Alternating TM on given input.
 *  Returns 1 if ATM accepts, 0 otherwise.
 *  ATIME(poly) = PSPACE (CKS 1981). */
int atm_evaluate(AlternatingTM* atm, const int* input, int input_len, int max_steps);

/* --- QBF Certificate Verification --- */

/** qbf_verify_certificate: Verify a claimed QBF truth value.
 *  For TRUE: Skolem functions map each ∃-var to value given ∀-var assignments.
 *  For FALSE: Herbrand expansion demonstrates propositional unsatisfiability.
 *  Verifier runs in polynomial space. */
int qbf_verify_certificate(int n_vars, const char* quants, int cls[][3], int nc,
                           int claimed_value, const int* certificate, int cert_len);

/* --- QBF to SAT Reduction (for bounded quantifier depth) --- */

/** qbf_to_sat_reduce: Expand universal quantifiers to produce SAT instance.
 *  Works for Sigma_k/Pi_k with small k. Blowup factor = 2^{#universal vars}.
 *  @param out_clauses  Allocated by function, caller frees
 *  @param out_nc       Number of output clauses
 *  @return 1 on success, 0 if too many universals */
int qbf_to_sat_reduce(int n_vars, const char* quants, int cls[][3], int nc,
                      int*** out_clauses, int* out_nc);

/* --- QBF Solver with DPLL + Optimizations --- */

/** qbf_solve_dpll: DPLL-style QBF solver with unit propagation.
 *  Handles quantifier scope: forall needs both branches TRUE,
 *  exists needs at least one branch TRUE. */
int qbf_solve_dpll(int n_vars, const char* quants, int cls[][3], int nc);

/** qbf_evaluate_opt: Optimized QBF evaluation with early termination.
 *  Forall: return 0 on first FALSE. Exists: return 1 on first TRUE. */
int qbf_evaluate_opt(int n_vars, const char* quants, int cls[][3], int nc, int* stats);

/* --- Space Complexity Predicates --- */

/** complexity_compare: Check known inclusion relationships.
 *  @return 1 if class_a ⊆ class_b is known, -1 if known separation, 0 if open */
int complexity_compare(ComplexityClass class_a, ComplexityClass class_b);

/** space_hierarchy_separates: Verify SPACE(f) ⊊ SPACE(g) when f = o(g).
 *  Returns 1 (proven separation), 0 (not applicable). */
int space_hierarchy_separates(double f_growth, double g_growth);

/** tm_in_class: Test if a given TM operates within the specified complexity class
 *  given its resource bounds. */
int tm_in_class(int time_bound, int space_bound, ComplexityClass class);

/* --- PSPACE-Complete Games: Winning Strategy --- */

/** geography_has_win: Decide if Player 1 has a winning strategy in
 *  Generalized Geography from start node.
 *  PSPACE-complete. Space: O(|V|). */
int geography_has_win(Digraph* g, int start);

/** pebble_game_solvable: Decide if target node can be pebbled with k pebbles.
 *  PSPACE-complete (Gilbert-Lengauer-Tarjan 1980). */
int pebble_game_solvable(int n_nodes, int** predecessors, int* n_pred, int target, int k_pebbles);

/** boolean_formula_game_winner: Two players assign variables.
 *  Player E wins if φ=TRUE, Player A wins if FALSE.
 *  Returns winning player (0=E, 1=A). PSPACE-complete. */
int boolean_formula_game_winner(int n_vars, int (*phi)(int*), int first_player);

/* --- Succinct Graph PATH --- */

/** succinct_path: Test PATH(s,t) on a succinct graph (N=2^n vertices).
 *  Uses Savitch recursion. Space O(n²). NPSPACE-complete. */
int succinct_path(SuccinctGraph* sg, int s, int t, int max_steps_exp);

/* --- Model Checking (CTL) --- */

/** ctl_model_check: Evaluate CTL formula on Kripke structure at given state.
 *  CTL model checking is in PSPACE (Clarke-Emerson 1981).
 *  @param kripke   Kripke structure (state transition system + labels)
 *  @param n_states Number of states
 *  @param formula  CTL formula in prefix notation
 *  @param init_state Starting state
 *  @return 1 if holds, 0 otherwise */
int ctl_model_check(int** transitions, char** labels, int n_states,
                    const char* formula, int init_state);

/* --- Space-Bounded Computation --- */

/** space_tm_simulate: Run a SpaceTM for max_steps, enforcing space_bound.
 *  @return 1=accept, 0=reject, -1=timeout */
int space_tm_simulate(SpaceTM* tm, const int* input, int input_len,
                      int space_bound, int max_steps);

/** count_tm_configs: Count the number of possible configurations
 *  for a TM with given parameters using ≤ s cells of tape.
 *  Formula: |Q| · s · |Γ|^s */
double count_tm_configs(int n_states, int space_bound, int alphabet_size);

/* ========================================================================
 * L6: Canonical Problems — PSPACE-Complete Catalog
 * ======================================================================== */

/* Problem identifiers for all known PSPACE-complete problems */
typedef enum {
    PSPACE_TQBF,                 /* True Quantified Boolean Formula */
    PSPACE_QSAT,                 /* Quantified SAT */
    PSPACE_GEOGRAPHY,            /* Generalized Geography */
    PSPACE_CHECKERS_NXN,         /* n×n Checkers (Fraenkel et al. 1978) */
    PSPACE_HEX_NXN,              /* n×n Hex */
    PSPACE_GO_ENDGAME,           /* Go endgame (Lichtenstein-Sipser 1980) */
    PSPACE_RUSH_HOUR,            /* Rush Hour (Flake-Baum 2002) */
    PSPACE_SOKOBAN,              /* Sokoban (Culberson 1997) */
    PSPACE_SLIDING_PUZZLE,       /* n×n sliding puzzle (15-puzzle gen.) */
    PSPACE_PEBBLE_GAME,          /* Pebble game on DAG (GLT 1980) */
    PSPACE_BOOLEAN_FORMULA_GAME, /* Boolean Formula Game */
    PSPACE_NFA_UNIVERSALITY,     /* NFA accepts Σ*? */
    PSPACE_REGEX_EQUIVALENCE,    /* Are two regexes equivalent? */
    PSPACE_FIRST_ORDER_MC,       /* First-order model checking */
    PSPACE_CTL_MC,               /* CTL model checking */
    PSPACE_SUCCINCT_PATH,        /* PATH on succinct graphs */
    PSPACE_CONNECT4_NXM          /* n×m Connect-4 */
} PSPACECompleteProblem;

/* ========================================================================
 * I/O — Demo Functions
 * ======================================================================== */

/* Core theorems */
void space_complexity_summary(void);
void savitch_demo(void);
void immerman_demo(void);

/* PSPACE-completeness */
void qbf_demo(void);
void pspace_complete_demo(void);
void geography_demo(void);
void geography_full_demo(void);

/* QBF solving */
void qbf_dpll_demo(void);
void qbf_resolution_demo(void);
void qbf_certificate_demo(void);
void qbf_preprocessor_demo(void);
void qbf_to_sat_demo(void);
void symbolic_qbf_demo(void);
void qbf_cegar_demo(void);
void qbf_benchmark(void);
void qbf_fuzz(int rounds);
void qbf_optimization_demo(void);
void qdimacs_demo(void);

/* Alternating TMs */
void alt_pspace_demo(void);
void alternating_tm_demo(void);

/* Space classes */
void pspace_summary(void);
void pspace_vs_exp_demo(void);
void space_bounded_demo(void);

/* NL & logspace */
void nl_reachability_demo(void);
void logspace_reductions_demo(void);
void l_vs_nl_demo(void);
void nl_algorithms_demo(void);
void graph_nl_demo(void);

/* PSPACE games */
void checkers_demo(void);
void hex_demo(void);
void rush_hour_demo(void);
void sokoban_demo(void);
void sliding_puzzle_demo(void);
void connect4_demo(void);
void pebble_game_demo(void);
void bf_game_demo(void);
void game_solver_demo(void);
void game_tree_demo(void);

/* Configuration graphs */
void config_graph_demo(void);
void succinct_graphs_demo(void);
void model_checking_demo(void);
void space_benchmark(void);

/* ========================================================================
 * PSPACE vs Other Classes — Relationship Matrix
 * ======================================================================== */

/** Known relationships:
 *   L ⊂ PSPACE (proven, space hierarchy)
 *   P ⊂ EXP   (proven, time hierarchy)
 *   NL = coNL  (Immerman 1987)
 *   PSPACE = NPSPACE (Savitch 1970)
 *   AP = PSPACE (CKS 1981)
 *
 *   Open:
 *     L ⊂ NL?  L ⊂ P?  NL ⊂ P?
 *     P ⊂ NP?  NP ⊂ PSPACE?  PSPACE ⊂ EXP? */

#define SPACE_CLASS_RELATION_KNOWN_SUBSET     1
#define SPACE_CLASS_RELATION_KNOWN_SEPARATION 2
#define SPACE_CLASS_RELATION_OPEN             0

/* ====================================================================
 * L5: Logspace Algorithms — L-complete and NL-complete problems
 * ==================================================================== */

/** logspace_path_undirected: USTCON — undirected s-t connectivity in L.
 *  Reingold (2008) proved USTCON ∈ L via zig-zag product expander graphs.
 *  This is a simplified version using the Reingold framework.
 *  @return 1 if s and t are connected in the undirected graph */
int logspace_ustcon(int n, const int* edges_u, const int* edges_v, int m, int s, int t);

/** logspace_tree_isomorphism: Tree isomorphism in logspace
 *  Lindell (1992): tree isomorphism ∈ L. Uses recursive centroid
 *  decomposition with logspace bookkeeping. */
int logspace_tree_isomorphic(int* parent1, int n1, int* parent2, int n2);

/** nl_2sat_solver: 2-SAT is NL-complete (Papadimitriou 1994).
 *  Proof: For each clause (x ∨ y) → edges (¬x→y) and (¬y→x).
 *  Formula satisfiable iff no variable x has path x→¬x and ¬x→x.
 *  PATH is in NL, coNL=NL, so 2-SAT ∈ NL. */
int nl_2sat_solve(int n_vars, int n_clauses, int clauses[][2]);

/** nl_2sat_verify: Verify a certificate for reconstructed 2-SAT solution.
 *  Uses the implication graph strongly-connected-component method. */
int nl_2sat_verify(int n_vars, int n_clauses, int clauses[][2], const int* assignment);

/** logspace_universal_tm: A universal Turing machine that uses O(log n)
 *  work space to simulate any logspace machine. Requires read-only input
 *  and read-only encoding of the simulated machine. */
int logspace_universal_tm(const char* machine_desc, const char* input);

/* Demo functions for new additions */
void logspace_hierarchy_demo(void);

/* ====================================================================
 * L6: More PSPACE-Complete Problems — in-depth
 * ==================================================================== */

/** regex_equivalence: Decide if two regular expressions over {0,1}*
 *  denote the same language. PSPACE-complete (Stockmeyer-Meyer 1973).
 *  Algorithm: convert to NFA, do subset construction, check equivalence. */
int regex_equivalence(const char* re1, const char* re2);

/** nfa_universality: Decide if NFA with n states accepts all strings.
 *  PSPACE-complete. Reduces to complement emptiness via powerset. */
int nfa_universality(int n_states, int** transitions, int start, int* accept, int n_accept);

/** context_free_nonemptiness: Is L(G) nonempty for CFG G?
 *  P-complete (Jones-Laaser 1976). Uses bottom-up closure. */
int cfg_nonempty(int n_vars, int n_rules, int rules[][3], int start_var);

/* Demo functions */
void regex_pspace_demo(void);

#endif /* PSPACE_H */
