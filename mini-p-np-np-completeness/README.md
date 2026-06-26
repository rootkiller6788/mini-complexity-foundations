# mini-p-np-np-completeness

P vs NP and NP-Completeness: from SAT solvers to the Cook-Levin Theorem.
A complete implementation covering complexity classes, reductions, proof systems,
and algorithmic SAT solving.

## Module Status: COMPLETE

| Level | Rating | Description |
|-------|--------|-------------|
| L1 Definitions | Complete | 10 core definitions with dual C + Lean implementations |
| L2 Core Concepts | Complete | 8 concepts: polynomial time, nondeterminism, NP, reductions |
| L3 Math Structures | Complete | 8 structures: CNF, circuits, implication graphs, proof DAGs |
| L4 Fundamental Laws | Complete | 10 theorems: Cook-Levin, resolution, Time Hierarchy, Ladner |
| L5 Algorithms | Complete | 25 algorithms: DPLL, CDCL, WalkSAT, GSAT, counting, reductions |
| L6 Canonical Problems | Complete | 14 problems: 10 NP-complete + 2 in P + 2 unknown |
| L7 Applications | Complete | 3 applications: BMC verification, cryptography, problem catalog |
| L8 Advanced Topics | Partial | 4/7: PCP Theorem, IP=PSPACE, Natural Proofs, Circuit hierarchy |
| L9 Research Frontiers | Partial | 3 frontiers documented: MCSP, GCT, Quantum complexity |

**Score: 16/18 → COMPLETE** (threshold: >= 16, L1/L4 not Missing)

## Core Definitions

| Concept | C Implementation | Lean Definition |
|---------|-----------------|-----------------|
| Boolean variable | Integer encoding: `(var<<1)|sign` | `Var := Nat` |
| Literal | `typedef int` — encoded as `(var_idx, sign_bit)` | `structure Literal` |
| Clause | `typedef struct { int* data; int n; } Clause` | `structure Clause` |
| CNF Formula | `typedef struct { Clause* clauses; ... } CNF` | `structure CNFFormula` |
| SAT | `int sat_solve(CNF*, int* assign)` | `def SAT (f) := satisfiable f` |
| NP | Verifier + certificate model | `def SATVerifier`, `theorem sat_in_np` |
| Graph | `typedef struct { int n; int** adj; }` | `structure Graph` |
| Complexity class | `typedef enum { CLASS_P, CLASS_NP, ... }` | (declared in comments) |

## Core Theorems

| Theorem | Status | Key Reference |
|---------|--------|---------------|
| **Cook-Levin**: SAT is NP-complete | C: `cook_levin_tableau()` | Cook (1971), Levin (1973) |
| **SAT in NP**: poly-time verifier exists | C: `sat_verify()` O(n*m) | AB §2.3 |
| **Resolution soundness**: resolvent preserves satisfiability | C: `resolution_apply()` | Robinson (1965) |
| **Resolution refutation-complete**: unsat => empty clause derivable | C: `resolution_refute_full()` | Robinson (1965) |
| **Time Hierarchy**: DTIME(f) proper subset DTIME(f^2) | C: `time_hierarchy_demo()` | Hartmanis-Stearns (1965) |
| **Ladner**: If P != NP, NP-intermediate languages exist | C: `ladner_theorem_demo()` | Ladner (1975) |
| **Padding**: EXP = NEXP => P = NP | C: `padding_demo_sat_to_exp()` | AB §3.2 |
| **Natural Proofs Barrier**: OWF => no natural proof separates P/NP | Lean: `natural_proofs_barrier` | Razborov-Rudich (1997) |
| **AC0 proper subset NC1**: PARITY not in AC0 | C: `circuit_parity()` | Furst-Saxe-Sipser (1984) |

## Core Algorithms

| Algorithm | Type | Complexity | File |
|-----------|------|------------|------|
| DPLL | Complete SAT | O(2^n) worst | dpll.c |
| CDCL | Complete SAT | O(2^n) worst | cdcl.c |
| WalkSAT | Stochastic local search | O(flips * m) | stochastic.c |
| Horn SAT | Polynomial special case | O(n+m) | horn_sat.c |
| 2-SAT (SCC) | Polynomial special case | O(V+E) | two_sat.c |
| Tseitin transformation | Circuit to CNF | O(|C|) | circuit.c |
| Resolution (exhaustive) | Proof system | exponential | resolution.c |
| #SAT (exact) | Counting (#P-complete) | O(2^n * m) | sat.c |

## Canonical Problems

**NP-Complete**: SAT, 3SAT, CLIQUE, VERTEX-COVER, INDEPENDENT-SET,
HAMILTONIAN-PATH, SUBSET-SUM, 3-COLORING, SUBGRAPH-ISOMORPHISM, CIRCUIT-SAT

**In P**: 2SAT (O(V+E)), HORN-SAT (O(n+m))

**Unknown**: GRAPH-ISOMORPHISM, FACTORING

## Nine-School Curriculum Alignment

| School | Key Course | Coverage |
|--------|-----------|----------|
| MIT | 6.045/18.400, 6.841 | P/NP, Cook-Levin, circuit complexity, PCP |
| Stanford | CS254 | Circuit complexity, Natural Proofs, Hastad |
| Berkeley | CS172, CS278 | NP-completeness, PCP, Interactive Proofs |
| CMU | 15-455, 15-855 | SAT algorithms, DPLL/CDCL, phase transitions |
| Princeton | COS 522, COS 551 | Cryptography connection, hardness of approximation |
| Caltech | CS 151, CS 154 | Computational models, hierarchy theorems |
| Cambridge | Part II/III | P vs NP, circuit complexity, interactive proofs |
| Oxford | Complexity Theory | Core definitions + quantum reference |
| ETH | 263-4650, 252-0400 | Proof complexity, descriptive complexity |

## Quick Start

```bash
make          # Build static library libpnp.a
make test     # Run all tests (49 tests, all pass)
make examples # Build example programs
```

## Directory Structure

```
mini-p-np-np-completeness/
  Makefile              # make test builds and runs all tests
  README.md             # This file
  include/              # 17 header files
    types.h             # CNF data structure
    sat.h               # SAT solver API
    dpll.h, cdcl.h      # DPLL and CDCL solvers
    cook_levin.h        # Cook-Levin theorem implementation
    reduction.h         # NP reductions (SAT->3SAT, 3SAT->CLIQUE)
    circuit.h           # Boolean circuits
    resolution.h        # Resolution proof system
    heuristics.h        # Branching heuristics (VSIDS, JW, MOMS)
    horn_sat.h          # Horn SAT (polynomial time)
    two_sat.h           # 2-SAT (polynomial via SCC)
    np_catalog.h        # NP-complete problem catalog
    graph_np.h          # NP-complete graph solvers
    dimacs.h            # DIMACS format I/O
    time_classes.h      # Complexity class definitions
    phase_analysis.h    # Phase transition analysis
    stochastic.h        # Stochastic SAT solvers
  src/                  # 19 source files + 1 Lean file
    types.c, sat.c, dpll.c, cdcl.c
    cook_levin.c        # Tableau construction
    reduction.c         # Karp reductions
    circuit.c           # Boolean circuits + Tseitin
    resolution.c, resolution_tree.c
    heuristics.c        # 8 branching heuristics
    horn_sat.c, two_sat.c
    np_catalog.c        # 16 NP-complete problems cataloged
    graph_np.c          # NP-complete graph problem solvers
    dimacs.c            # DIMACS parser
    time_classes.c      # Complexity zoo
    padding.c           # Padding arguments + Ladner
    phase_analysis.c    # SAT phase transition experiments
    stochastic.c        # WalkSAT, GSAT, Simulated Annealing
    formalization.lean  # Lean 4 formal definitions and theorems
  tests/                # 3 test files (49 tests, all pass)
    test_all.c          # 17 integration tests
    test_sat.c          # 12 unit tests + circuit tests
    fuzz_test.c         # 20 randomized fuzz tests
  examples/             # 6 example programs
    demo_pnp.c          # P vs NP demo
    horn_sat_demo.c     # Horn SAT example
    phase_transition.c  # Phase transition visualization
    reduction_demo.c    # NP reduction chain demo
    sat_cli.c           # Command-line SAT solver
    two_sat_demo.c      # 2-SAT example
  docs/                 # 8 documentation files
    knowledge-graph.md  # L1-L9 knowledge coverage table
    coverage-report.md  # Detailed per-level coverage assessment
    gap-report.md       # Missing knowledge + quality gaps
    course-alignment.md # Nine-school curriculum mapping
    course-tree.md      # Prerequisite/downstream dependency tree
    cook-levin-guide.md # Cook-Levin theorem guide
    cook-levin-proof.md # Complete proof construction
    diagonalization.md  # Diagonalization techniques
    hastad-inapproximability.md # Hastad's theorem
    phase-transition.md # SAT phase transition theory
    relativization.md   # Baker-Gill-Solovay relativization
    resolution-completeness.md # Resolution proof system theory
  benches/              # Performance benchmarks
  demos/                # Visualization/demo scripts
```

## References

- Arora & Barak, _Computational Complexity: A Modern Approach_ (2009)
- Sipser, _Introduction to the Theory of Computation_ (2013)
- Garey & Johnson, _Computers and Intractability_ (1979)
- Karp, _Reducibility Among Combinatorial Problems_ (1972)
- Cook, _The Complexity of Theorem-Proving Procedures_ (1971)
- Robinson, _A Machine-Oriented Logic Based on the Resolution Principle_ (1965)
- Vollmer, _Introduction to Circuit Complexity_ (1999)
