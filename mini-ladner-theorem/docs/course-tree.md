# Course Tree: mini-ladner-theorem

## Prerequisite Dependency Graph

```
mini-p-np-np-completeness
  ??? mini-ladner-theorem  (needs P, NP, NPC definitions)
       ??? mini-time-hierarchy-theorem  (uses diagonalization)
       ??? mini-circuit-complexity/     (natural proofs connection)
       ??? mini-polynomial-hierarchy    (Low/High hierarchy)
       ??? mini-reductions-completeness (many-one reductions)
```

## Knowledge Dependency Tree

```
L1: Definitions (P, NP, NPC)
 ??? L2: Core Concepts (delayed diag, Ladner construction)
 ?    ??? L3: Math Structures (slow-growing f, sparse sets, degrees)
 ?    ?    ??? L4: Theorems (Ladner, Mahaney, Schoning, B-H, V-V, BGS, RR)
 ?    ?    ?    ??? L5: Algorithms (popcount, pruning, WL, Pollard rho, V-V reduce)
 ?    ?    ?    ?    ??? L6: Problems (SAT, GI, Factoring, US, QR, SQR-free)
 ?    ?    ?    ?    ?    ??? L7: Applications (RSA, DH, Lattice crypto, Natural proofs)
 ?    ?    ?    ?    ?    ?    ??? L8: Advanced (Schoning hierarchy, Oracle worlds, RR barrier)
 ?    ?    ?    ?    ?    ?    ?    ??? L9: Frontiers (Meta-complexity, GI quasi-poly, Quantum, GCT)
 ?    ?    ?    ?    ?    ?
 ?    ?    ?    ?    ?    ??? mini-pcp-theorem, mini-approximation-algorithms
 ?    ?    ?    ?    ??? mini-algorithmic-complexity/
 ?    ?    ?    ??? mini-time-hierarchy-theorem
 ?    ?    ??? mini-boolean-circuits-model
 ?    ??? mini-reductions-completeness
 ??? mini-p-np-np-completeness
```

## Module Internal Dependencies

```
include/ladner.h (all type definitions + API)
 ??? src/ladner.c (slow-growing functions, construction demo)
 ??? src/delayed_diag.c (delayed diagonalization)
 ??? src/ladner_language.c (full L construction + verification)
 ??? src/mahaney.c (Mahaney pruning + theorem)
 ??? src/sparse_sets.c (sparse set operations)
 ??? src/sparse_operations.c (advanced sparse operations)
 ??? src/degree_structure.c (NP degree lattice)
 ??? src/berman_hartmanis.c (p-isomorphism)
 ??? src/factoring.c (factoring algorithms)
 ??? src/graph_isomorphism.c (GI algorithms)
 ??? src/padded_sat_full.c (padded SAT construction)
 ??? src/uniqueness_np.c (V-V theorem)
 ??? src/promise_problems.c (promise problems)
 ??? src/np_intermediate_candidates.c (NPI candidates)
 ??? src/oracle_construction.c (oracle worlds)
 ??? src/relativization_ladner.c (relativized Ladner)
 ??? src/schoning_theorem.c (infinite hierarchy)
 ??? src/natural_proofs.c (RR barrier)
 ??? src/ladner_bench.c (benchmarks)
 ??? src/ladner.lean (Lean 4 formalization)
```

## Forward Dependencies (modules depending on this)

- mini-pcp-theorem (uses NP-intermediate structure)
- mini-approximation-algorithms (NPI candidates)
- mini-circuit-complexity (natural proofs)
- mini-polynomial-hierarchy (Low/High connection)
- mini-parameterized-complexity (degree structure)
