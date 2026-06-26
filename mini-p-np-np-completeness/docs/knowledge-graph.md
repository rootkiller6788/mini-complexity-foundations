# Knowledge Graph — mini-p-np-np-completeness

## L1: Definitions (Complete)

| # | Concept | C Implementation | Lean Definition |
|---|---------|-----------------|-----------------|
| 1 | Boolean variable | Encoded as (var<<1)|sign | Var := Nat |
| 2 | Literal | Encoded integer: (var_index, sign) | structure Literal |
| 3 | Clause | typedef struct { int* data; int n; } | structure Clause |
| 4 | CNF Formula | typedef struct { Clause* clauses; ... } | structure CNFFormula |
| 5 | Boolean Assignment | int* assign (0/1/-1) | Assignment := Var -> Bool |
| 6 | SAT decision problem | int sat_solve(CNF*, int*) | def SAT (f) := satisfiable f |
| 7 | Certificate (witness) | Assignment passed to verifier | def Certificate := Assignment |
| 8 | Poly-time verifier | int sat_verify(CNF*, const int*) | def SATVerifier |
| 9 | Graph (NP reductions) | typedef struct { int n; int** adj; } | structure Graph |
| 10 | Complexity class enum | typedef enum { CLASS_P, CLASS_NP, ... } | (comments) |

## L2: Core Concepts (Complete)

| # | Concept | C Implementation | File |
|---|---------|-----------------|------|
| 1 | Polynomial time | double time_decider() | time_classes.c |
| 2 | Nondeterminism | NTM simulation | cook_levin.c |
| 3 | NP membership test | int np_membership_test() | time_classes.c |
| 4 | Class inclusion (P in NP in EXP) | int class_is_subset() | time_classes.c |
| 5 | Polynomial reduction | reduce_sat_to_3sat() | reduction.c |
| 6 | NP-completeness catalog | npc_catalog_print() | np_catalog.c |
| 7 | coNP | CLASS_CONP handling | time_classes.c |
| 8 | Search-to-decision | clique_solve() returns witness | graph_np.c |

## L3: Mathematical Structures (Complete)

| # | Structure | C Data Type | File |
|---|-----------|------------|------|
| 1 | CNF formula (full API) | CNF + cnf_new/add/free/copy/print | types.c |
| 2 | Boolean circuit (DAG) | Circuit + Gate (AND/OR/NOT/INPUT) | circuit.c |
| 3 | Implication graph (2-SAT) | Adjacency matrix + Kosaraju SCC | two_sat.c |
| 4 | Directed hypergraph (Horn) | Implication list + antecedent tracking | horn_sat.c |
| 5 | Resolution proof DAG | ResClause + parent links | resolution_tree.c |
| 6 | Graph (adjacency matrix) | Graph + g_new/g_free/g_edge | np_catalog.c |
| 7 | NTM tableau encoding | Variable encoding scheme | cook_levin.c |
| 8 | DIMACS CNF format | Parser + writer | dimacs.c |

## L4: Fundamental Theorems (Complete)

| # | Theorem | C Test | Lean |
|---|---------|--------|------|
| 1 | Cook-Levin (SAT is NP-complete) | cook_levin_tableau() | theorem cook_levin |
| 2 | SAT in NP | sat_verify() polynomial time | theorem sat_has_poly_verifier |
| 3 | Resolution soundness | resolution_apply() | theorem resolution_soundness |
| 4 | Empty clause unsatisfiable | resolution_refute() detects empty | theorem empty_clause_unsatisfiable |
| 5 | Resolution refutation-complete | resolution_refute_full() | theorem resolution_refutation_complete |
| 6 | Time Hierarchy (P != EXP) | time_hierarchy_demo() | (Lean stated) |
| 7 | Ladner Theorem | ladner_theorem_demo() | (Lean stated) |
| 8 | Padding argument | padding_demo_sat_to_exp() | (Lean stated) |
| 9 | Natural Proofs Barrier | Documented | theorem natural_proofs_barrier |
| 10 | AC0 proper subset NC1 | circuit_parity() | (Lean stated) |

## L5: Algorithms (Complete — 25 algorithms)

| # | Algorithm | Function | Complexity |
|---|-----------|----------|------------|
| 1 | DPLL | dpll_solve() | O(2^n) worst |
| 2 | CDCL | cdcl_solve() | O(2^n) worst |
| 3 | Unit propagation | dpll_unit_propagate() | O(n*m) |
| 4 | Pure literal elimination | dpll_pure_literal() | O(n*m) |
| 5 | VSIDS branching | branch_vsids() | O(n) per branch |
| 6 | Jeroslow-Wang | branch_jeroslow_wang() | O(n*m) |
| 7 | MOMS heuristic | branch_moms() | O(n*m) |
| 8 | DLIS (most frequent) | branch_most_frequent() | O(n*m) |
| 9 | Luby restarts | luby_restart_limit() | O(1) |
| 10 | Phase saving | phase_saving_record/get() | O(1) |
| 11 | Horn SAT (linear) | horn_sat_solve() | O(n+m) |
| 12 | 2-SAT via SCC | two_sat_solve() | O(V+E) |
| 13 | WalkSAT | walksat_solve() | O(flips*m) |
| 14 | GSAT | gsat_solve() | O(flips*n*m) |
| 15 | Simulated Annealing SAT | sa_sat_solve() | O(steps*n*m) |
| 16 | #SAT exact count | sat_count_exact() | O(2^n*m) |
| 17 | #SAT approx (sampling) | sat_count_approx() | O(samples*m) |
| 18 | Exhaustive resolution | resolution_refute() | O(2^clauses) |
| 19 | Tseitin transformation | circuit_to_cnf() | O(|C|) |
| 20 | Clique branch-and-bound | clique_search() | O(n^k) |
| 21 | Graph coloring | coloring_search() | O(k^n) |
| 22 | Subgraph isomorphism | iso_search() | O(n!/(n-k)!) |
| 23 | SAT to 3SAT reduction | reduce_sat_to_3sat() | O(total lits) |
| 24 | 3SAT to CLIQUE reduction | reduce_3sat_to_clique_graph() | O(n_clauses^2) |
| 25 | 3SAT to SUBSET SUM | reduce_3sat_to_subset_sum() | O(n_vars*n_clauses) |

## L6: Canonical Problems (Complete — 14 problems)

| # | Problem | Status | Implementation |
|---|---------|--------|---------------|
| 1 | SAT | NP-complete (Cook 1971) | sat_solve(), sat_verify() |
| 2 | 3SAT | NP-complete (Karp 1972) | reduce_sat_to_3sat() |
| 3 | CLIQUE | NP-complete (Karp 1972) | clique_solve() |
| 4 | VERTEX COVER | NP-complete (Karp 1972) | vertex_cover_via_clique() |
| 5 | INDEPENDENT SET | NP-complete (Karp 1972) | independent_set_solve() |
| 6 | HAMILTONIAN PATH | NP-complete (Karp 1972) | reduce_3sat_to_hamiltonian() |
| 7 | SUBSET SUM | NP-complete (Karp 1972) | subset_sum_solve() |
| 8 | 3-COLORING | NP-complete (Karp 1972) | graph_k_colorable() |
| 9 | SUBGRAPH ISOMORPHISM | NP-complete | subgraph_isomorphism() |
| 10 | 2SAT | P (polynomial) | two_sat_solve() O(V+E) |
| 11 | HORN SAT | P-complete | horn_sat_solve() O(n+m) |
| 12 | CIRCUIT SAT | NP-complete | circuit_sat() via Tseitin |
| 13 | GRAPH ISOMORPHISM | Unknown (NP, not NPC or P) | Cataloged |
| 14 | FACTORING | Unknown | Cataloged |

## L7: Applications (Complete — 3 apps)

| # | Application | Implementation |
|---|-------------|---------------|
| 1 | Bounded Model Checking (BMC) | BMCInstance structure; reduction to SAT |
| 2 | Cryptography (OWF -> P!=NP) | OneWayFunction + owf_implies_p_neq_np |
| 3 | NP-complete problem catalog | npc_catalog_print() — 16 problems |

## L8: Advanced Topics (Partial — 4/7)

| # | Topic | File |
|---|-------|------|
| 1 | PCP Theorem | docs/hastad-inapproximability.md, Lean |
| 2 | IP = PSPACE | Lean comment |
| 3 | Natural Proofs Barrier | Lean theorem |
| 4 | Circuit hierarchy (AC0 subset NC1) | circuit_parity(), docs/ |
| 5 | Phase transition | phase_transition_experiment() |
| 6 | Baker-Gill-Solovay relativization | docs/relativization.md |
| 7 | Hastad inapproximability | docs/hastad-inapproximability.md |

## L9: Research Frontiers (Partial — documented)

| # | Topic | Status |
|---|-------|--------|
| 1 | Meta-complexity (MCSP) | Documented in Lean |
| 2 | GCT (Geometric Complexity Theory) | Documented in Lean |
| 3 | Quantum complexity (BQP) | Referenced in course alignment |

## Summary

| Level | Rating | Score |
|-------|--------|-------|
| L1 | Complete | 2 |
| L2 | Complete | 2 |
| L3 | Complete | 2 |
| L4 | Complete | 2 |
| L5 | Complete | 2 |
| L6 | Complete | 2 |
| L7 | Complete | 2 |
| L8 | Partial | 1 |
| L9 | Partial | 1 |
| **TOTAL** | | **16/18 -> COMPLETE** |
