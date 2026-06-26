# Knowledge Graph — mini-time-hierarchy-theorem

## L1: Definitions
| # | Term | Definition | C Implementation | Lean |
|---|------|-----------|-----------------|------|
| 1 | Turing Machine | (Q,Σ,Γ,δ,q0,q_accept,q_reject) | `TuringMachine` struct in `tht.h` | `TuringMachine` in `tht.lean` |
| 2 | TM Configuration | (state, tape, head, step) | `TMConfiguration` struct | `TMConfig` in tht.lean |
| 3 | Language | L ⊆ {0,1}* | `typedef int (*Language)(int)` | `Language := Nat → Bool` |
| 4 | Time-Constructible | f(n) computable in O(f(n)) steps | `TimeBoundFn`, `tc_*` functions | `isTimeConstructible` predicate |
| 5 | Complexity Class | TIME(T(n)), NTIME(T(n)) | `ComplexityClass` struct | `TIME(T) : Set Language` |
| 6 | Oracle TM | M^A with oracle query tape | `OracleTM` struct | (documented) |
| 7 | Godel Numbering | TM encoding as integer | `tm_godel_encode/decode` | `godelEncodeTM`, `enumerateTM` |
| 8 | Diagonal Language | L_D = {n | M_n rejects n} | `diagonal_language()` | `cantorDiagonal` |
| 9 | Polynomial Time | P = ∪_k TIME(n^k) | `cc_p_not_exp_demonstrate()` | (documented) |
| 10 | Exponential Time | EXP = ∪_k TIME(2^{n^k}) | `poly_vs_exp.c` | `p_not_exp` theorem |

## L2: Core Concepts
| # | Concept | Implementation |
|---|---------|---------------|
| 1 | Universal Simulation | `utm.c`, `universal_simulate()` |
| 2 | Diagonalization | `diagonal.c`, `diag_language()` |
| 3 | Time Hierarchy | `tht.c`, `time_hierarchy_demo()` |
| 4 | Hierarchies are strict | `exponential_hierarchy.c` |
| 5 | Relativization | `oracle_machines.c`, BGS theorem |
| 6 | Padding | `padding.c` |
| 7 | P vs NP vs EXP landscape | `poly_vs_exp.c`, `complexity_classes.h` |

## L3: Mathematical Structures
| # | Structure | Implementation |
|---|-----------|---------------|
| 1 | TM formal structure | `include/tm_types.h`, `include/tht.h` |
| 2 | Boolean circuit DAG | `src/circuit_verification.c` |
| 3 | Kripke structure | `src/model_checking.c` |
| 4 | CTL formula AST | `src/model_checking.c` |
| 5 | Complexity class lattice | `src/hierarchy_table.c`, `include/complexity_classes.h` |
| 6 | Timing distribution model | `src/monte_carlo_complexity.c` |

## L4: Fundamental Laws
| # | Theorem | Status | Files |
|---|---------|--------|-------|
| 1 | TIME(f) ≠ TIME(g) when f·log f = o(g) | Complete | `tht.c`, `diagonal.c`, Lean |
| 2 | P ⊊ EXP (corollary) | Complete | `poly_vs_exp.c`, Lean |
| 3 | NTIME(f) ≠ NTIME(g) (Cook/Zak) | Complete | `nondeterministic_hierarchy.c` |
| 4 | Borodin Gap Theorem | Partial | `gap_theorem.c` |
| 5 | Blum Speedup Theorem | Partial | `speedup.c` |
| 6 | BGS Relativization Barrier | Complete | `oracle_machines.c`, `limits.c` |
| 7 | Linear Speedup Theorem | Partial | `speedup.c`, Lean |
| 8 | Cantor Diagonal (general) | Complete | `diagonal.c`, Lean |
| 9 | Union Theorem | Partial | Lean statement |
| 10 | Hierarchy extends indefinitely | Complete | `exponential_hierarchy.c` |

## L5: Algorithms/Methods
| # | Algorithm | Implementation |
|---|-----------|---------------|
| 1 | Universal TM simulation | `tm_simulator.c`, `utm.c` |
| 2 | Diagonal language computation | `diagonal_computation.c`, `tht.c` |
| 3 | Time-constructible verification | `constructible.c` |
| 4 | CTL model checking | `model_checking.c` |
| 5 | Monte Carlo complexity estimation | `monte_carlo_complexity.c` |
| 6 | Bayesian complexity inference | `bayesian_timing.c` |
| 7 | Bootstrap confidence intervals | `monte_carlo_complexity.c` |
| 8 | Circuit evaluation | `circuit_verification.c` |

## L6: Canonical Problems
| # | Problem | Examples/Demo |
|---|---------|--------------|
| 1 | P vs EXP separation language | `examples/tht_demo.c`, `src/poly_vs_exp.c` |
| 2 | Diagonal language L_D | `src/diagonal_computation.c`, `src/fuzz_tht.c` |
| 3 | Oracle relativization problem | `examples/bgs_demo.c`, `src/oracle_machines.c` |
| 4 | NTIME hierarchy separation | `src/nondeterministic_hierarchy.c` |
| 5 | Gap theorem pathology | `src/gap_theorem.c` |

## L7: Applications (5 implemented)
| # | Application | Keywords | Implementation |
|---|-------------|----------|---------------|
| 1 | Cryptographic OWF Timing | RSA, AES, SHA-256, lattice, NIST, Bitcoin, TLS | `src/crypto_timing.c` |
| 2 | Model Checking (Boeing, NASA) | NASA, Boeing, Tesla, Toyota, ISO 26262, DO-178C | `src/model_checking.c` |
| 3 | Circuit Verification | Tesla, SpaceX, iPhone, F-35, nuclear, Fukushima | `src/circuit_verification.c` |
| 4 | Benchmark timing analysis | (all families) | `src/benchmark_tht.c` |
| 5 | Complexity class tester | (empirical) | `src/complexity_tester.c` |

## L8: Advanced Topics (3 implemented)
| # | Topic | Implementation |
|---|-------|---------------|
| 1 | Monte Carlo complexity estimation | `src/monte_carlo_complexity.c` (MCMC, Lyapunov) |
| 2 | Bayesian complexity inference | `src/bayesian_timing.c` (Bayesian, Markov blanket) |
| 3 | Adaptive policy / Game of Life | `src/bayesian_timing.c` (fuzzy, adaptive) |

## L9: Research Frontiers (documented)
| # | Frontier | Location |
|---|----------|----------|
| 1 | P vs NP (Clay Millennium) | `limits.c`, README, Lean |
| 2 | Meta-complexity (MCSP) | Lean, `docs/gap-report.md` |
| 3 | Geometric Complexity Theory | Lean, `docs/gap-report.md` |
| 4 | Quantum complexity | (documented in course-alignment.md) |
| 5 | Natural Proofs barrier | `limits.c`, Lean |
