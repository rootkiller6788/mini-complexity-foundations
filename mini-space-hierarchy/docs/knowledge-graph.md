# Knowledge Graph -- mini-space-hierarchy

## L1: Definitions
| Term | C Implementation | Lean |
|------|-----------------|------|
| SPACE(s(n)) | SpaceClass enum | SpaceClass inductive |
| L (Logspace) | SPACE_CLASS_L | .L |
| NL | SPACE_CLASS_NL | .NL |
| PSPACE | SPACE_CLASS_PSPACE | .PSPACE |
| EXPSPACE | SPACE_CLASS_EXPSPACE | .EXPSPACE |
| Space-constructible | is_space_constructible_fn() | SpaceConstructible |
| Configuration | TMConfig | Config |
| Configuration Graph | ConfigGraphSpace | -- |
| PATH | nl_path() | -- |
| Logspace Reduction | LogspaceTransducer | -- |

## L2: Core Concepts
| Concept | Implementation |
|---------|---------------|
| Space hierarchy separation | space_hierarchy_diagonal_language() |
| Universal space-bounded TM | utm_simulate() |
| Config graph reachability | cfg_graph_accepts() |
| Inductive counting | reach_immerman_count() |
| Relativization | diag_relativization_demo() |
| Random walk in logspace | ustcon_random_walk() |

## L3: Math Structures
| Structure | C Type |
|-----------|--------|
| Space-bounded TM | SpaceBoundedTM |
| TM Transition | TMTransition |
| Configuration | TMConfig |
| Config Graph | ConfigGraphSpace |
| Directed Graph | DirectedGraph / DiGraph |
| Undirected Graph | UndirectedGraph / UGraph |
| Logspace Transducer | LogspaceTransducer |
| QSAT Formula | QSATFormula |
| 2-SAT Instance | TwoSATInstance |
| NFA | NFA |

## L4: Fundamental Laws
| Theorem | Implementation |
|---------|---------------|
| Space Hierarchy | space_hierarchy_diagonal_language() |
| Savitch Theorem | savitch_path() |
| Immerman-Szelepcsenyi | non_path_immerman() |
| Reingold Theorem | reingold_theorem_demo() |
| CKS Theorem (AL=P) | cks_theorem_demo() |
| NPSPACE=PSPACE | savitch_full_demo() |

## L5: Algorithms
| Algorithm | Space | File |
|-----------|-------|------|
| Binary addition | O(log n) | logspace_algos.c |
| Palindrome | O(log n) | logspace_algos.c |
| GCD | O(log n) | logspace_problems.c |
| Modular exp | O(log n) | logspace_problems.c |
| NL PATH | O(log n) | nl_algorithms.c |
| Savitch PATH | O(log^2 n) | savitch_full.c |
| Inductive counting | O(log n) | immerman_szelepcsenyi.c |
| Diagonalization | O(g(n)) | space_diagonal.c |
| TQBF | O(poly n) | pspace_algorithms.c |
| 2-SAT | O(log n) NL | space_nl_reachability.c |

## L6: Canonical Problems
| Problem | Class | Completeness |
|---------|-------|-------------|
| PATH | NL-complete | Jones 1975 |
| TQBF | PSPACE-complete | Stockmeyer-Meyer 1973 |
| QSAT | PSPACE-complete | Meyer-Stockmeyer 1972 |
| Gen Geography | PSPACE-complete | Schaefer 1978 |
| 2-SAT | NL-complete | Papadimitriou |
| NFA Universality | PSPACE-complete | Meyer-Stockmeyer |

## L7: Applications
| Application | Domain | Function |
|-------------|--------|----------|
| Model Checking | Formal Verification | model_checking_demo() |
| Game Playing | AI | game_playing_demo() |
| VLSI Placement | EDA/CAD | two_sat_application_demo() |

## L8: Advanced Topics
| Topic | Implementation |
|-------|---------------|
| Alternating Logspace (AL=P) | alternating_logspace.c |
| Randomized Logspace (RL/BPL) | rl_space.c |
| Reingold (SL=L) | space_reingold.c |
| Space Hierarchy Relativizes | space_universal_tm.c |
| Nisan PRG | rl_space.c |

## L9: Research Frontiers
| Topic | Status |
|-------|--------|
| BPL vs L | Open, documented |
| Space derandomization | Active area |
| Quantum space complexity | Mentioned in Lean |
