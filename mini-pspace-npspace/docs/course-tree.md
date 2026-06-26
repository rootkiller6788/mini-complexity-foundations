# Course Tree ¡ª mini-pspace-npspace

## Prerequisites (modules this one depends on)

| Dependency | What We Use |
|-----------|-------------|
| mini-cook-levin-theorem | SAT/NP-completeness concepts to contrast with QBF/PSPACE |
| mini-p-np-np-completeness | NP, NP-completeness, polynomial-time TM definition |
| mini-time-hierarchy-theorem | Time hierarchy: TIME(f) ? TIME(g^{O(1)}) |
| mini-space-hierarchy | Space hierarchy analogue |
| mini-reductions-completeness | Logspace reductions (<=_L) for PSPACE-completeness |

## Knowledge Flow

```
Theory of Computation Basics
  |
  +-> NP/Completeness (mini-p-np-np-completeness)
  |     |
  |     +-> SAT, 3SAT reduction chain
  |     +-> NP-complete = under <=_P reductions
  |            |
  |            v
  +-> PSPACE/NPSPACE (THIS MODULE)
        |
        +-> L, NL, PSPACE definitions
        +-> Savitch: NPSPACE = PSPACE
        +-> Immerman: NL = coNL
        +-> AP = PSPACE
        +-> TQBF is PSPACE-complete
        +-> PSPACE-complete games
        +-> Logspace reductions
        |
        +---> Circuit Complexity (mini-circuit-complexity/*)
        |       Uses: Succinct graph representations
        |
        +---> IP = PSPACE (mini-ip-pspace)
        |       Uses: PSPACE characterization
        |
        +---> PCP Theorem (mini-pcp-theorem)
        |       Uses: PSPACE vs NP context
        |
        +---> Crypto Foundations (mini-crypto-foundations/*)
                Uses: One-way functions relative to PSPACE
```

## Within-Module Dependency Graph

```
pspace.h (definitions)
  |
  +-> space_classes.c (hierarchy)
  +-> space_bounded_tm.c (TM simulation)
  |
  +-> savitch.c (first theorem)
  |     +-> config_graph.c
  |
  +-> qbf_solver.c (TQBF evaluation)
  |     +-> qbf_dpll.c (DPLL-style)
  |     +-> qbf_preprocessor.c (unit propagation)
  |     +-> qbf_resolution.c (proof system)
  |     +-> qbf_to_sat.c (SAT reduction)
  |     +-> qbf_symbolic.c (truth table)
  |     +-> qbf_cegar.c (abstraction)
  |     +-> qbf_certificate.c (certificates)
  |     +-> qbf_optimizer.c (optimizations)
  |     +-> qbf_fuzz.c (testing)
  |     +-> qbf_benchmark.c (performance)
  |     +-> quantified_cnf.c (QDIMACS parsing)
  |
  +-> alternating_pspace.c (AP=PSPACE)
  |     +-> alternating_tm_full.c
  |
  +-> pspace_complete.c (completeness)
  |     +-> boolean_formula_game.c
  |
  +-> nl_reachability.c (PATH, NL)
  |     +-> nl_algorithms_full.c
  |     +-> graph_nl.c
  |     +-> immerman.c
  |     +-> l_vs_nl.c
  |
  +-> logspace_reductions.c
  |
  +-> geography.c + geography_full.c
  +-> game_solver.c + game_tree.c
  +-> checkers_endgame.c
  +-> hex_game.c
  +-> connect4.c
  +-> rush_hour.c
  +-> sokoban.c
  +-> sliding_puzzle.c
  +-> pebble_game.c
  +-> succinct_graphs.c
  +-> model_checking.c
  +-> pspace_vs_exp.c
  +-> space_benchmark.c
```

## Postrequisites (modules that need this one)

| Consumer | What They Need |
|----------|---------------|
| mini-circuit-complexity/* | Succinct graphs, PSPACE completeness machinery |
| mini-ip-pspace | PSPACE definition, TQBF |
| mini-pcp-theorem | PSPACE context vs NP |
| mini-crypto-foundations/* | OWF relative to PSPACE |
| mini-quantum-computation/* | BQP ? PSPACE relationship |
| mini-descriptive-complexity | SO logic captures PSPACE (Fagin's theorem for NP, SO(TC) for PSPACE) |
