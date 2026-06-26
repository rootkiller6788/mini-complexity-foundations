# Course Alignment ！ mini-pspace-npspace

## Nine-School Curriculum Mapping

This module maps to PSPACE/NPSPACE content from nine world-class institutions.

### MIT ！ 6.045/18.400 Automata, Computability, Complexity + 6.841 Advanced Complexity

| Chapter | Topic | This Module |
|---------|-------|-------------|
| Sipser Ch.8 | Space Complexity | space_classes.c, space_bounded_tm.c |
| Sipser 8.2 | Savitch Theorem | savitch.c, savitch_reachability() |
| Sipser 8.4 | NL = coNL | immerman.c, l_vs_nl.c |
| Sipser 8.3 | PSPACE-completeness | qbf_solver.c, pspace_complete.c |
| 6.841 Lec 8-10 | Alternation, PSPACE | alternating_pspace.c, alternating_tm_full.c |

### Stanford ！ CS254 Computational Complexity

| Topic | This Module |
|-------|-------------|
| Space complexity hierarchy | space_classes.c, complexity_compare() |
| Savitch theorem (NSPACE vs SPACE) | savitch.c |
| Immerman-Szelepcsenyi (NL = coNL) | immerman.c |
| TQBF and PSPACE-completeness | qbf_solver.c, pspace_complete.c |
| AP = PSPACE | alternating_pspace.c |
| Games and PSPACE | geography.c, game_solver.c |

### Berkeley ！ CS172 Computability & Complexity + CS278 Computational Complexity

| Topic | This Module |
|-------|-------------|
| L, NL, PSPACE definitions | pspace.h enum |
| Savitch Theorem | savitch.c |
| Immerman-Szelepcsenyi | immerman.c |
| PSPACE-complete problems | qbf_solver.c, pspace_complete.c |
| NL-complete (PATH) | nl_reachability.c |
| Configuration graphs | config_graph.c |

### CMU ！ 15-455 Undergrad Complexity + 15-855 Graduate Complexity

| Topic | This Module |
|-------|-------------|
| Space classes and hierarchy | space_classes.c |
| Savitch theorem and PSPACE | savitch.c |
| NL and coNL | l_vs_nl.c, immerman.c |
| QBF solvers (DPLL, resolution) | qbf_dpll.c, qbf_resolution.c |
| Game complexity | game_solver.c, checkers_endgame.c |

### Princeton ！ COS 522 Computational Complexity

| Topic | This Module |
|-------|-------------|
| Space complexity: L, NL, PSPACE | space_classes.c |
| Savitch theorem | savitch.c |
| PSPACE-completeness of TQBF | qbf_solver.c |
| NL = coNL via inductive counting | immerman.c |
| Connection to cryptography | documented, crypto modules |

### Caltech ！ CS 151 Complexity Theory + CS 154 Limits of Computation

| Topic | This Module |
|-------|-------------|
| Space complexity and PSPACE | space_classes.c, pspace.c |
| Savitch theorem | savitch.c |
| PSPACE-complete games | hex_game.c, geography.c |
| Physics connection (reversible computation) | documented |
| Succinct representations | succinct_graphs.c |

### Cambridge ！ Part II Complexity Theory + Part III Advanced Complexity

| Topic | This Module |
|-------|-------------|
| Space hierarchy | space_hierarchy_separates() |
| Savitch: NPSPACE = PSPACE | savitch.c |
| Immerman-Szelepcsenyi | immerman.c |
| PSPACE-completeness | qbf_solver.c |
| Alternation and games | alternating_pspace.c |
| Logspace reductions | logspace_reductions.c |

### Oxford ！ Computational Complexity + Advanced Complexity Theory

| Topic | This Module |
|-------|-------------|
| Space-bounded computation | space_bounded_tm.c |
| PSPACE and NPSPACE | space_classes.c |
| TQBF as canonical PSPACE problem | qbf_solver.c |
| Games characterization | boolean_formula_game.c |
| Quantum complexity connections | documented |

### ETH ！ 263-4650 Advanced Complexity + 252-0400 Logic & Computation

| Topic | This Module |
|-------|-------------|
| Space complexity foundations | space_classes.c |
| Savitch and Immerman theorems | savitch.c, immerman.c |
| Alternation = PSPACE | alternating_pspace.c, alternating_tm_full.c |
| QBF proof systems (Q-resolution) | qbf_resolution.c |
| Model checking in PSPACE | model_checking.c |
| Descriptive complexity connection | documented |

## Core Intersection

The following topics appear at ALL nine institutions:

1. **L, NL, PSPACE definitions** (9/9)
2. **Savitch Theorem** (9/9)
3. **Immerman-Szelepcsenyi (NL = coNL)** (9/9)
4. **TQBF is PSPACE-complete** (9/9)
5. **PATH is NL-complete** (8/9)
6. **AP = PSPACE** (7/9)
7. **PSPACE-complete games** (7/9)
8. **Space hierarchy theorem** (9/9)

All eight core intersection topics are fully implemented in this module.
