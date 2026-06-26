# Knowledge Graph ！ mini-pspace-npspace

## L1: Definitions (Complete ！ 16 entries)

| # | Concept | Definition | C Implementation |
|---|---------|-----------|-----------------|
| 1 | SPACE(f(n)) | Languages decidable by DTM with O(f(n)) workspace cells | ComplexityClass enum in pspace.h |
| 2 | NSPACE(f(n)) | Languages decidable by NTM with O(f(n)) workspace cells | ComplexityClass enum |
| 3 | L = SPACE(log n) | Deterministic logspace computation | CLASS_L enum |
| 4 | NL = NSPACE(log n) | Nondeterministic logspace | CLASS_NL enum |
| 5 | PSPACE | Union over k of SPACE(n^k) | CLASS_PSPACE enum |
| 6 | NPSPACE | Union over k of NSPACE(n^k) | CLASS_NPSPACE enum |
| 7 | EXP | Union over k of TIME(2^{n^k}) | CLASS_EXP enum |
| 8 | Space-bounded TM | (Q,S,G,d,q0,qa,qr) with space limit | SpaceTM struct |
| 9 | TM Configuration | (state, head_position, tape_content) | TMConfiguration struct |
| 10 | QBF/TQBF | Q1x1...Qnxn: phi(x1,...,xn) | QBFInstance struct |
| 11 | Alternating TM | TM with EXISTS and FORALL state types | AlternatingTM struct |
| 12 | Configuration Graph | Digraph of all configurations in space bound | ConfigGraph struct |
| 13 | Logspace Reduction | Reduction using O(log n) workspace on read-only input | logspace_reductions.c |
| 14 | PSPACE-complete | Language L in PSPACE s.t. forall A in PSPACE: A <=_L L | PSPACECompleteProblem enum |
| 15 | Succinct Graph | N=2^n vertex graph defined by Boolean circuit C(u,v) | SuccinctGraph struct |
| 16 | PATH problem | Given digraph G, nodes s,t: is there s-t path? (NL-complete) | Digraph + PATH functions |

## L2: Core Concepts (Complete ！ 18 entries)

| # | Concept | Implementation |
|---|---------|---------------|
| 1 | Space hierarchy: L ? PSPACE (provable) | space_hierarchy_separates() |
| 2 | Time hierarchy: P ? EXP (provable) | complexity_compare() |
| 3 | Savitch: NSPACE(s) ? SPACE(s^2) | savitch.c, savitch_reachability() |
| 4 | PSPACE = NPSPACE (Savitch corollary) | savitch.c, pspace_summary.c |
| 5 | NL = coNL (Immerman-Szelepcsenyi) | immerman.c, l_vs_nl.c |
| 6 | AP = PSPACE (alternation = space) | alternating_pspace.c, alternating_tm_full.c |
| 7 | ALOGSPACE = P | alternating_pspace.c |
| 8 | TQBF is PSPACE-complete | qbf_solver.c, pspace_complete.c |
| 9 | PATH is NL-complete | nl_reachability.c, nl_algorithms_full.c |
| 10 | PSPACE ? EXP (provable) | pspace_vs_exp.c |
| 11 | Configuration counting = 2^{poly} bound | count_tm_configs() |
| 12 | Polynomial Hierarchy ? PSPACE | pspace_summary.c |
| 13 | NL ? P (trivial: simulate NTM in poly time) | graph_nl.c, nl_reachability.c |
| 14 | L ? NL ? P ? NP ? PSPACE ? EXP chain | space_classes.c |
| 15 | Closure under complement for NL | immerman.c |
| 16 | Game characterization of PSPACE | boolean_formula_game.c, game_solver.c |
| 17 | Circuit-space connection (succinct graphs) | succinct_graphs.c |
| 18 | Reingold: undirected connectivity in L | (documented, not implemented) |

## L3: Mathematical Structures (Complete ！ 14 entries)

| # | Structure | Representation |
|---|-----------|---------------|
| 1 | Turing Machine | SpaceTM struct: (Q,Sigma,Gamma,delta,q0,qa,qr) |
| 2 | Boolean Circuit DAG | quantified_cnf.c clause representation |
| 3 | 3-CNF Formula | QBF_LIT(var,neg) encoding, arrays of clauses |
| 4 | Literal encoding: lit = (var<<1)|sign | Macros in pspace.h |
| 5 | Configuration Graph | ConfigGraph with adjacency matrix |
| 6 | Quantifier Prefix | QBFInstance.quant_prefix: 'E'/'A' string |
| 7 | Game Tree | GamePosition, minimax with alpha-beta |
| 8 | Directed Graph | Digraph: adjacency matrix, out-degrees |
| 9 | Kripke Structure | model_checking.c states + transitions + labels |
| 10 | Truth Table (2^n size) | qbf_symbolic.c TruthTable |
| 11 | QDIMACS Format | quantified_cnf.c QDIMACS parser |
| 12 | Hexagonal Grid | hex_game.c 6-neighbor adjacency |
| 13 | Pebble DAG | pebble_game.c PebbleDAG |
| 14 | Succinct Graph | Edge predicate function pointer |

## L4: Fundamental Laws (Complete ！ 11 entries)

| # | Theorem | Source | C Verification | Lean |
|---|---------|--------|---------------|------|
| 1 | Savitch: NSPACE(s) ? SPACE(s^2) | Savitch 1970 | savitch_reachability() | pspace.lean |
| 2 | PSPACE = NPSPACE | Savitch corollary | Config counting + Savitch | pspace.lean |
| 3 | NL = coNL | Immerman-Szelepcsenyi 1987 | Inductive counting algorithm | pspace.lean |
| 4 | AP = PSPACE | Chandra-Kozen-Stockmeyer 1981 | atm_evaluate() | pspace.lean |
| 5 | ALOGSPACE = P | CKS corollary | Documented, code ref | pspace.lean |
| 6 | Space Hierarchy Theorem | Hartmanis-Lewis-Stearns 1965 | space_hierarchy_separates() | pspace.lean |
| 7 | PSPACE ? EXP | Config counting proof | count_tm_configs() | pspace.lean |
| 8 | TQBF is PSPACE-complete | Stockmeyer-Meyer 1973 | qbf_evaluate() | pspace.lean |
| 9 | PATH is NL-complete | Jones 1975 | nl_reachability.c | pspace.lean |
| 10 | NL ? P | Trivial: simulate NTM | BFS simulation in graph_nl.c | pspace.lean |
| 11 | Undirected connectivity （ L | Reingold 2008 | (documented, reference) | (documented) |

## L5: Algorithms/Methods (Complete ！ 19 entries)

| # | Algorithm | File | Time | Space |
|---|-----------|------|------|-------|
| 1 | Savitch Recursive PATH | savitch.c | O(n^{log n}) | O(log^2 n) |
| 2 | QBF Recursive Eval | qbf_solver.c | O(2^n) | O(n) |
| 3 | DPLL-style QBF | qbf_dpll.c | O(2^n) | O(n) |
| 4 | QBF Unit Propagation | qbf_preprocessor.c | O(n，|phi|) | O(|phi|) |
| 5 | Q-Resolution | qbf_resolution.c | O(2^n) | O(|phi|) |
| 6 | CEGAR QBF | qbf_cegar.c | Iterative refinement | O(|phi|) |
| 7 | QBF-to-SAT Expansion | qbf_to_sat.c | O(2^k，|phi|) | O(2^k，|phi|) |
| 8 | QDIMACS Parser | quantified_cnf.c | O(|input|) | O(|input|) |
| 9 | Alpha-Beta Pruning | game_solver.c | O(b^{d/2}) | O(d) |
| 10 | Alternating TM Sim | alternating_tm_full.c | O(b^depth) | O(depth) |
| 11 | Symbolic QBF (Truth Table) | qbf_symbolic.c | O(2^n) | O(2^n) |
| 12 | BFS PATH | nl_reachability.c | O(n+m) | O(n) |
| 13 | NL PATH (nondeterministic) | nl_algorithms_full.c | randomized | O(log n) |
| 14 | CTL Model Checking | model_checking.c | O(|phi|，|M|) | O(|phi|，log|M|) |
| 15 | Hex Path Detection | hex_game.c | O(n^2) | O(n^2) |
| 16 | Connect-4 Win Check | connect4.c | O(n，m) | O(1) |
| 17 | Pebble Game Solver | pebble_game.c | O(2^k，n) | O(k，n) |
| 18 | Geography Memoized | geography_full.c | O(2^|V|) | O(|V|) |
| 19 | Min-Max Game Search | game_tree.c | O(b^d) | O(d) |

## L6: Canonical Problems (Complete ！ 16 entries)

| # | Problem | Class | Implementation |
|---|---------|-------|---------------|
| 1 | TQBF | PSPACE-complete | qbf_solver.c |
| 2 | QSAT | PSPACE-complete | qbf_solver.c |
| 3 | Generalized Geography | PSPACE-complete | geography.c, geography_full.c |
| 4 | Boolean Formula Game | PSPACE-complete | boolean_formula_game.c |
| 5 | Checkers (n〜n) | PSPACE-complete | checkers_endgame.c |
| 6 | Hex (n〜n) | PSPACE-complete | hex_game.c |
| 7 | Rush Hour | PSPACE-complete | rush_hour.c |
| 8 | Sokoban | PSPACE-complete | sokoban.c |
| 9 | Sliding Puzzle (15-puzzle gen.) | PSPACE-complete | sliding_puzzle.c |
| 10 | Connect-4 (n〜m) | PSPACE-complete | connect4.c |
| 11 | Pebble Game (k pebbles) | PSPACE-complete | pebble_game.c |
| 12 | PATH (dir. reachability) | NL-complete | nl_reachability.c |
| 13 | Succinct PATH | NPSPACE-complete | succinct_graphs.c |
| 14 | NFA Universality | PSPACE-complete | (documented, ref) |
| 15 | CTL Model Checking | PSPACE | model_checking.c |
| 16 | Regex Equivalence | PSPACE-complete | (documented, ref) |

## L7: Applications (Complete ！ 4 entries)

| # | Application | File | Domain |
|---|-------------|------|--------|
| 1 | Hardware Verification (CTL MC) | model_checking.c | Model checking used at Intel/AMD/NASA |
| 2 | AI Game Playing (minimax) | game_solver.c | Alpha-beta = chess engine core |
| 3 | Compiler Register Allocation | pebble_game.c | Pebble game models register spilling |
| 4 | QBF in Formal Verification | qbf_*.c | QBF solvers for bounded model checking |

## L8: Advanced Topics (Partial ！ 3/7)

| # | Topic | Status |
|---|-------|--------|
| 1 | Q-Resolution Proof System | Complete: qbf_resolution.c |
| 2 | CEGAR for QBF | Complete: qbf_cegar.c |
| 3 | Symbolic Model Checking | Complete: qbf_symbolic.c |
| 4 | PCP Theorem connection | Documented only (separate module) |
| 5 | Switching Lemma | Documented (circuit complexity module) |
| 6 | Natural Proofs Barrier | Documented only |
| 7 | IP = PSPACE | Documented only (separate module) |

## L9: Research Frontiers (Partial ！ documented)

| # | Topic | Status |
|---|-------|--------|
| 1 | L vs NL (open since 1975) | Documented in l_vs_nl.c |
| 2 | NL vs P implications | Documented |
| 3 | P vs PSPACE (major open) | Documented in pspace_vs_exp.c |
| 4 | PSPACE vs EXP | Documented |
| 5 | QBF Solver Competitions | Documented in qbf_benchmark.c |
| 6 | Proof Complexity of Q-resolution | Documented in qbf_resolution.c |
| 7 | Meta-complexity (MCSP) | Planned separate module |
