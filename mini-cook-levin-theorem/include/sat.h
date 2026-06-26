/*
 * sat.h — Boolean Satisfiability: Structures, Solvers, & Verification
 *
 * L1 Definitions:
 *   - Boolean variable: x ∈ {0, 1}
 *   - Literal: a variable x or its negation ¬x
 *   - Clause: disjunction of literals (x₁ ∨ ¬x₂ ∨ x₃)
 *   - CNF formula: conjunction of clauses
 *   - SAT: ∃ assignment such that all clauses evaluate to 1
 *   - k-SAT: each clause has exactly k literals
 *   - #SAT: count the number of satisfying assignments
 *
 * L1 Problem Classes:
 *   SAT ∈ NP (polynomial-time verifiable)
 *   3SAT is NP-complete (Cook-Levin, then SAT ≤ₚ 3SAT)
 *
 * L3 Mathematical Structures:
 *   CNF formula represented as list-of-lists of signed integers
 *   Literal encoding: for variable v (0-indexed),
 *     positive literal = (v << 1) | 0
 *     negative literal = (v << 1) | 1
 *
 * Reference: Arora-Barak §2.3-2.4, Sipser §7.4-7.5
 * Courses: All nine institutions (SAT is the canonical NP-complete problem)
 */

#ifndef SAT_H
#define SAT_H

#include <stdlib.h>
#include <stdint.h>

/* ── Literal Encoding ────────────────────────────────────── */

/* Encode variable v as a positive literal */
#define SAT_LIT_POS(v)  (((v) << 1) | 0)

/* Encode variable v as a negative literal */
#define SAT_LIT_NEG(v)  (((v) << 1) | 1)

/* Decode literal to variable index */
#define SAT_LIT_VAR(l)  ((l) >> 1)

/* Decode literal sign: 0 = positive, 1 = negative */
#define SAT_LIT_SIGN(l) ((l) & 1)

/* Negate a literal */
#define SAT_LIT_NOT(l)  ((l) ^ 1)

/* ── Clause ──────────────────────────────────────────────── */
typedef struct {
    int* lits;   /* array of encoded literals */
    int  n;      /* number of literals in this clause */
} Clause;

/* ── CNF Formula ─────────────────────────────────────────── */
typedef struct {
    Clause* clauses;   /* array of clauses */
    int     n_clauses; /* number of clauses */
    int     n_vars;    /* number of variables (0..n_vars-1) */
    int     add_idx;   /* next clause index for cnf_add */
    int     clause_cap;/* capacity of clauses array */
} CNF;

/* ── Assignment ──────────────────────────────────────────── */
/* assign[v] ∈ {0, 1, -1} where -1 = unassigned */
typedef int* Assignment;  /* dynamically allocated array of length n_vars */

/* ── CNF Construction ────────────────────────────────────── */

/* Create empty CNF with given variable count and clause capacity */
CNF* cnf_create(int n_vars, int clause_capacity);

/* Add a clause from an array of literals */
void cnf_add_clause(CNF* cnf, const int* lits, int n_lits);

/* Add a unit clause (single literal) */
void cnf_add_unit(CNF* cnf, int lit);

/* Add a binary clause (two literals) */
void cnf_add_binary(CNF* cnf, int lit1, int lit2);

/* Add a ternary clause (three literals) */
void cnf_add_ternary(CNF* cnf, int lit1, int lit2, int lit3);

/* Create a deep copy of a CNF formula */
CNF* cnf_clone(const CNF* cnf);

/* Free a CNF formula */
void cnf_free(CNF* cnf);

/* ── CNF Inspection ──────────────────────────────────────── */

/* Evaluate a single literal under an assignment */
int lit_value(int lit, const Assignment assign);

/* Evaluate a clause: 1=satisfied, 0=conflict, -1=undetermined */
int clause_value(const Clause* c, const Assignment assign);

/* Evaluate the entire formula: 1=satisfied, 0=unsatisfied, -1=undetermined */
int cnf_value(const CNF* cnf, const Assignment assign);

/* Count how many clauses are satisfied by an assignment */
int cnf_count_satisfied(const CNF* cnf, const Assignment assign);

/* Return 1 if assignment is complete (no -1 entries) */
int assignment_is_complete(const Assignment assign, int n_vars);

/* ── CNF Statistics ──────────────────────────────────────── */
int  cnf_max_clause_size(const CNF* cnf);
int  cnf_min_clause_size(const CNF* cnf);
int  cnf_total_literals(const CNF* cnf);
void cnf_print_stats(const CNF* cnf);

/* ── DIMACS CNF I/O ──────────────────────────────────────── */

/* Write CNF in DIMACS format to file */
int cnf_write_dimacs(const CNF* cnf, const char* filename);

/* Read CNF from DIMACS format file */
CNF* cnf_read_dimacs(const char* filename);

/* Print CNF in DIMACS format to stdout */
void cnf_print_dimacs(const CNF* cnf);

/* ── Assignment Operations ───────────────────────────────── */
Assignment assignment_create(int n_vars);    /* all unassigned (-1) */
Assignment assignment_clone(const Assignment a, int n_vars);
void       assignment_free(Assignment a);
void       assignment_print(const Assignment a, int n_vars);
void       assignment_set_all(Assignment a, int n_vars, int val);

/* ── SAT Solver API ──────────────────────────────────────── */

/*
 * DPLL (Davis-Putnam-Logemann-Loveland) algorithm.
 * Classic backtracking SAT solver with:
 *   - Unit propagation
 *   - Pure literal elimination
 *   - Branching on most frequent variable
 *
 * Returns 1 if SAT (assign contains satisfying assignment),
 *         0 if UNSAT.
 *
 * Complexity: O(2^n) worst case, but often much faster in practice.
 * Reference: Davis, Logemann, Loveland (1962); Arora-Barak §2.4
 */
int sat_solve_dpll(CNF* cnf, Assignment assign);

/*
 * CDCL (Conflict-Driven Clause Learning) solver.
 * Modern SAT solving with:
 *   - Watched literals for fast unit propagation
 *   - Conflict analysis via implication graph
 *   - Learned clause addition
 *   - Activity-based variable selection (VSIDS)
 *   - Periodic restarts
 *
 * Returns 1 if SAT, 0 if UNSAT.
 * Reference: Marques-Silva & Sakallah (1996); Moskewicz et al. (2001)
 */
int sat_solve_cdcl(CNF* cnf, Assignment assign);

/*
 * Auto-select solver: CDCL for large formulas, DPLL for small ones.
 */
int sat_solve(CNF* cnf, Assignment assign);

/*
 * Polynomial-time verification: check that assignment satisfies CNF.
 * O(total_literals) = O(|φ|).
 * This demonstrates that SAT ∈ NP — the core insight of Cook-Levin.
 */
int sat_verify(const CNF* cnf, const Assignment assign);

/*
 * Exact model counting (#SAT, #P-complete).
 * Enumerates all 2^n assignments. Bounded to 2^26 (~67M) for safety.
 * Returns the exact count of satisfying assignments.
 *
 * Complexity: Θ(2^n · |φ|)
 * Reference: Valiant (1979) — #P-completeness of #SAT
 */
long long sat_count_exact(const CNF* cnf);

/*
 * Approximate model counting via random sampling.
 * Returns estimated probability that random assignment satisfies formula.
 *
 * Complexity: O(samples · |φ|)
 * Reference: Stockmeyer (1983) — approximate counting in polynomial hierarchy
 */
double sat_count_approx(const CNF* cnf, int samples);

/*
 * Find ALL satisfying assignments (up to max_solutions).
 * Returns number found. assignments[i][v] contains the i-th solution.
 * Caller must free each row and the outer array.
 */
int sat_enumerate_all(const CNF* cnf, int max_solutions,
                      Assignment** assignments_out);

/* ── Solver Statistics ───────────────────────────────────── */
void sat_dpll_stats(void);   /* nodes explored, propagations */
void sat_cdcl_stats(void);   /* conflicts, restarts, learned clauses */

#endif /* SAT_H */
