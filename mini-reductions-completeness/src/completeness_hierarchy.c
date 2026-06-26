/* completeness_hierarchy.c — Completeness Beyond NP (L6, L7, L8)
 *
 * Completeness is a GENERAL concept: for ANY complexity class C,
 * a problem L is C-complete if L in C AND L is C-hard (all A in C reduce to L).
 *
 * Completeness hierarchy:
 *   L-complete:     Balanced parentheses? (actually in TC0)
 *   NL-complete:    ST-CONN (graph reachability)
 *   P-complete:     Circuit Value Problem (CVP)
 *   NP-complete:    SAT, 3SAT, CLIQUE, ...
 *   PSPACE-complete: TQBF, Generalized Geography
 *   EXP-complete:    Succinct Circuit Value
 *   NEXP-complete:   Succinct 3SAT
 *   EXPSPACE-complete: EQ-REGEX
 *
 * Each completeness result requires two proofs:
 *   1. Membership: L in C (usually easy)
 *   2. Hardness: for all A in C, A <=_p L (the hard part)
 *
 * Reference: Arora & Barak §3.3 (NL), §3.4 (PSPACE), §4.1 (PH), §4.2 (EXP)
 * Reference: Sipser §8.2 (PSPACE-completeness), §9.1 (Hierarchy Theorems) */

#include "redcomp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ===== NL-Completeness: ST-CONN (L6) ===== */

/* ST-CONN (also called PATH): Given directed graph G and vertices s,t,
 * is there a path from s to t? NL-complete under <=_L.
 * Immerman-Szelepcsenyi (1987): ST-CONN in NL (obvious) and NL-hard.
 * Also: NON-ST-CONN in NL (coNL = NL!). */

static int stconn_bfs(int n, int **adj, int s, int t) {
    int *visited = calloc((size_t)n, sizeof(int));
    int *queue = malloc((size_t)n * sizeof(int));
    int head = 0, tail = 0;

    queue[tail++] = s;
    visited[s] = 1;

    while (head < tail) {
        int u = queue[head++];
        if (u == t) { free(visited); free(queue); return 1; }
        for (int v = 0; v < n; v++) {
            if (adj[u][v] && !visited[v]) {
                visited[v] = 1;
                queue[tail++] = v;
            }
        }
    }
    free(visited); free(queue);
    return 0;
}

/* ===== P-Completeness: Circuit Value Problem (CVP) (L6) ===== */

/* A Boolean circuit is a DAG of gates.
 * CVP: Given a Boolean circuit C and input x, does C(x) = 1?
 * CVP is P-complete under <=_L (Ladner, 1975).
 * If CVP in L, then P = L. */

typedef enum { GATE_INPUT, GATE_AND, GATE_OR, GATE_NOT } GateType;

typedef struct {
    int id;
    GateType type;
    int input1, input2;  /* Gate IDs for inputs; for INPUT: input1 = value */
    int value;            /* Computed value */
} CircuitGate;

/* Evaluate a circuit with topological ordering (assumed given).
 * This is the canonical P-complete problem. */
static int cvp_evaluate(CircuitGate *gates, int n_gates) {
    /* Evaluate gates in order (inputs first, then topological).
     * For demo, we assume gates are already topologically sorted. */
    for (int i = 0; i < n_gates; i++) {
        switch (gates[i].type) {
        case GATE_INPUT:
            gates[i].value = gates[i].input1;  /* input1 holds the bit value */
            break;
        case GATE_AND:
            gates[i].value = gates[gates[i].input1].value
                           && gates[gates[i].input2].value;
            break;
        case GATE_OR:
            gates[i].value = gates[gates[i].input1].value
                          || gates[gates[i].input2].value;
            break;
        case GATE_NOT:
            gates[i].value = !gates[gates[i].input1].value;
            break;
        }
    }
    return gates[n_gates - 1].value;  /* Output = last gate */
}

/* ===== PSPACE-Completeness: TQBF (L6) ===== */

/* TQBF (True Quantified Boolean Formula):
 * Given phi = Q1 x1 Q2 x2 ... Qn xn psi(x1,...,xn)
 * where Qi in {exists, forall} and psi is a 3-CNF,
 * is phi TRUE?
 *
 * TQBF is PSPACE-complete (Stockmeyer & Meyer, 1973).
 * Key: PSPACE = NPSPACE (Savitch's Theorem). */

/* Recursive TQBF evaluator: polynomial space, exponential time. */
static int tqbf_eval(int n_vars, int level, int *quantifiers,
                      int clauses[][3], int n_clauses, int *assignment) {
    if (level == n_vars) {
        /* Evaluate the matrix (3-CNF) */
        for (int ci = 0; ci < n_clauses; ci++) {
            int clause_sat = 0;
            for (int j = 0; j < 3; j++) {
                int lit = clauses[ci][j];
                int var = lit >> 1, is_neg = lit & 1;
                if (assignment[var] != is_neg) { clause_sat = 1; break; }
            }
            if (!clause_sat) return 0;
        }
        return 1;
    }

    if (quantifiers[level] == 0) {
        /* FORALL: both values must yield TRUE */
        assignment[level] = 0;
        if (!tqbf_eval(n_vars, level+1, quantifiers, clauses, n_clauses, assignment))
            return 0;
        assignment[level] = 1;
        return tqbf_eval(n_vars, level+1, quantifiers, clauses, n_clauses, assignment);
    } else {
        /* EXISTS: at least one value yields TRUE */
        assignment[level] = 0;
        if (tqbf_eval(n_vars, level+1, quantifiers, clauses, n_clauses, assignment))
            return 1;
        assignment[level] = 1;
        return tqbf_eval(n_vars, level+1, quantifiers, clauses, n_clauses, assignment);
    }
}

/* ===== EXP-Completeness: Succinct CVP (L6) ===== */

/* A "succinct" circuit: the circuit is described by a small Boolean formula
 * that defines each gate's type and connections. Evaluating it takes
 * exponential time because the circuit itself is exponentially large. */

/* For demo: simulate a double-exponential-style computation.
 * EXP = DTIME(2^{n^c}). */
static int exp_simulate(int n) {
    int limit = 1;
    int exp_n = n < 6 ? n : 6;
    for (int i = 0; i < exp_n; i++) limit *= 2;
    /* 2^n iterations */
    int count = 0;
    for (int i = 0; i < limit; i++) {
        for (int j = 0; j < limit; j++) {
            count++;
        }
    }
    return count % 2;
}

/* ===== Polynomial Hierarchy (L8) ===== */

/* The Polynomial Hierarchy: PH = Union_{k} Sigma_k^P
 * Sigma_0^P = P
 * Sigma_{k+1}^P = NP^{Sigma_k^P}  (NP with oracle for Sigma_k^P)
 * Pi_k^P = co-Sigma_k^P
 *
 * If PH collapses (Sigma_k = Sigma_{k+1}), then PH = Sigma_k.
 * It is widely believed PH does NOT collapse.
 * If P = NP, then PH = P (collapse to level 0). */

/* Simulate Sigma_2^P: NP with NP oracle.
 * Problem: MIN-DNF = {phi | phi is a minimal DNF formula}.
 * "There exists an equivalent DNF of smaller size" = Sigma_2^P-complete. */

static int sigma_2_simulate(int n) {
    /* Exists x forall y P(x,y) pattern. Demo with small domain. */
    int domain = n < 4 ? n : 4;
    for (int x = 0; x < (1 << domain); x++) {
        int all_y_ok = 1;
        for (int y = 0; y < (1 << domain) && all_y_ok; y++) {
            /* P(x,y) = (x AND y) > 0  (simple predicate) */
            if (!(x & y)) all_y_ok = 0;
        }
        if (all_y_ok) return 1;  /* Exists x: forall y: P(x,y) */
    }
    return 0;
}

/* ===== Demonstration ===== */

void completeness_hierarchy_demo(void) {
    printf("\n===== Completeness Beyond NP =====\n\n");

    /* Completeness concept */
    printf("--- The Completeness Spectrum ---\n\n");
    printf("For ANY complexity class C, a problem L is C-complete if:\n");
    printf("  1. L in C (membership).\n");
    printf("  2. For all A in C: A <=_L (hardness).\n\n");

    printf("Class       Complete Problem    Reduction Type\n");
    printf("--------    -----------------   --------------\n");
    printf("L           ???                  <=_L\n");
    printf("NL          ST-CONN (PATH)      <=_L\n");
    printf("P           CVP                 <=_L\n");
    printf("NP          SAT                 <=_m (Karp)\n");
    printf("coNP        TAUTOLOGY           <=_m\n");
    printf("PH          QSAT_k              <=_m\n");
    printf("PSPACE      TQBF                <=_m\n");
    printf("EXP         Succinct CVP        <=_m\n");
    printf("NEXP        Succinct 3SAT       <=_m\n");
    printf("EXPSPACE    EQ-REGEX            <=_m\n\n");

    /* Demo: NL-complete */
    printf("--- NL-Completeness: ST-CONN ---\n");
    int n_test = 4;
    int **adj_test = malloc((size_t)n_test * sizeof(int*));
    for (int i = 0; i < n_test; i++)
        adj_test[i] = calloc((size_t)n_test, sizeof(int));
    adj_test[0][1] = adj_test[1][2] = adj_test[2][3] = 1;
    int reachable = stconn_bfs(n_test, adj_test, 0, 3);
    printf("  Graph: 0->1->2->3, s=0, t=3, reachable? %s\n",
           reachable ? "YES" : "NO");
    for (int i = 0; i < n_test; i++) free(adj_test[i]);
    free(adj_test);
    printf("  ST-CONN is NL-complete under <=_L.\n");
    printf("  coNL = NL (Immerman-Szelepcsenyi 1987).\n\n");

    /* Demo: P-complete */
    printf("--- P-Completeness: Circuit Value Problem ---\n");
    CircuitGate gates[] = {
        {0, GATE_INPUT, 1, 0, 0},     /* Input = 1 */
        {1, GATE_INPUT, 0, 0, 0},     /* Input = 0 */
        {2, GATE_INPUT, 1, 0, 0},     /* Input = 1 */
        {3, GATE_AND,   0, 1, 0},     /* gate0 AND gate1 = 0 */
        {4, GATE_OR,    2, 3, 0},     /* gate2 OR gate3 = 1 */
        {5, GATE_NOT,   4, 0, 0},     /* NOT gate4 = 0 */
    };
    int cvp_out = cvp_evaluate(gates, 6);
    printf("  Circuit: (1 AND 0) OR 1 = 1, NOT(1) = 0 -> Output = %d\n", cvp_out);
    printf("  CVP is P-complete under <=_L (Ladner 1975).\n");
    printf("  If CVP in L, then P = L.\n\n");

    /* Demo: PSPACE-complete */
    printf("--- PSPACE-Completeness: TQBF ---\n");
    int quant[] = {1, 0, 1};  /* exists x0 forall x1 exists x2 */
    int matrix[2][3] = {{0, 2, 0}, {1, 3, 0}};
    int assign[8] = {0};
    int tqbf_result = tqbf_eval(3, 0, quant, matrix, 2, assign);
    printf("  TQBF: exists x0 forall x1 exists x2 (clauses)\n");
    printf("    Result: %s\n", tqbf_result ? "TRUE" : "FALSE");
    printf("  TQBF is PSPACE-complete (Stockmeyer & Meyer 1973).\n\n");

    /* Demo: EXP */
    printf("--- EXP-Completeness ---\n");
    printf("  EXP = DTIME(2^{poly(n)}).\n");
    printf("  Succinct CVP is EXP-complete.\n");
    printf("  EXP != P (by Time Hierarchy Theorem).\n");
    printf("  Simulated 2^%d ops gives parity result: %d\n\n",
           3, exp_simulate(3));

    /* Demo: Polynomial Hierarchy */
    printf("--- Polynomial Hierarchy ---\n");
    printf("  PH = Union_k Sigma_k^P.\n");
    printf("  Sigma_2^P = NP^{NP} (NP with NP oracle).\n");
    int sigma2 = sigma_2_simulate(3);
    printf("  Sigma_2 simulation: %s\n",
           sigma2 ? "exists x forall y P(x,y) = TRUE" : "FALSE");
    printf("  If P = NP, then PH collapses to P.\n");
    printf("  If PH collapses to level k, then PH = Sigma_k^P.\n");
    printf("  Most believe PH is infinite (does not collapse).\n");
}

void sat_logspace_completeness(void) {
    printf("\n===== SAT NP-Completeness Under <=_L =====\n\n");
    printf("Theorem: SAT is NP-complete under LOGSPACE reductions.\n\n");
    printf("This is STRONGER than <=_p completeness:\n");
    printf("  <=_L implies <=_p (logspace => poly-time).\n");
    printf("  So if SAT in P, then NP = P (standard).\n");
    printf("  Also: SAT in L would imply NP = L (stronger!).\n\n");
    printf("Proof sketch (Cook-Levin via logspace):\n");
    printf("  1. Given NP machine M and input x, construct formula phi.\n");
    printf("  2. phi has variables for each (cell, time, state).\n");
    printf("  3. Each variable: (tape position, time step, symbol).\n");
    printf("  4. O(log n) space to output each clause (just indices).\n");
    printf("  5. Total: O(log n) work space, poly output.\n");
    printf("  => SAT is NP-complete under <=_L.\n\n");
    printf("Most NPC reductions in Karp's 21 are <=_L.\n");
    printf("This means NP-completeness is ROBUST even with respect\n");
    printf("to the finest reduction type.\n");
}
