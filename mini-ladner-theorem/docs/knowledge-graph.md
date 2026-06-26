# Knowledge Graph: mini-ladner-theorem

## L1: Definitions

| ID | Term | Definition | Code Location |
|----|------|------------|---------------|
| L1.1 | NP-intermediate language | L in NP, L not in P, L not NPC | `include/ladner.h` (NPIntermediate_Language) |
| L1.2 | Slow-growing function | f: N->N, unbounded, f(n)=o(n^c) for all c>0 | `include/ladner.h` (SlowGrowingFunction) |
| L1.3 | Sparse set | S where \|S_n\| <= p(n) for some poly p | `include/ladner.h` (SparseSet) |
| L1.4 | P-machine | Deterministic polynomial-time Turing machine | `include/ladner.h` (P_Machine) |
| L1.5 | NP-machine | Nondeterministic polynomial-time TM | `include/ladner.h` (NP_Machine) |
| L1.6 | Polynomial-time reduction | A <=^p_m B mapping, computable in poly time | `include/ladner.h` (PolyTimeReduction) |
| L1.7 | P-isomorphism | Poly-time computable bijection preserving membership | `include/ladner.h` (PolyIsomorphism) |
| L1.8 | Delayed diagonalization | Diagonalizing against machines using slow-growing index | `src/delayed_diag.c` |
| L1.9 | NP-m-degree | Equivalence class under <=^p_m reductions | `include/ladner.h` (NP_Degree) |
| L1.10 | Oracle relativized P/NP | P^O, NP^O: classes with oracle O | `src/ladner.c`, `src/oracle_construction.c` |

## L2: Core Concepts

| ID | Concept | Description | Code Location |
|----|---------|-------------|---------------|
| L2.1 | Delayed diagonalization technique | Use f(n) to slow down diagonalization | `src/delayed_diag.c` |
| L2.2 | Ladner's construction | L = {x in SAT | f(|x|) is even} | `src/ladner.c`, `src/ladner_language.c` |
| L2.3 | Mahaney's sparse set implication | Sparse NPC => P=NP | `src/mahaney.c` |
| L2.4 | Berman-Hartmanis conjecture | All NPC languages are p-isomorphic | `src/berman_hartmanis.c` |
| L2.5 | Oracle relativization | Ladner's theorem relativizes | `src/oracle_construction.c`, `src/relativization_ladner.c` |
| L2.6 | Schoning infinite hierarchy | Infinite NP-m-degrees exist (if P!=NP) | `src/schoning_theorem.c` |
| L2.7 | Valiant-Vazirani isolation | Randomized reduction SAT -> Unique-SAT | `src/uniqueness_np.c` |
| L2.8 | Natural proofs barrier | No natural proof separates P from NP/poly | `src/natural_proofs.c` |

## L3: Mathematical Structures

| ID | Structure | Formal Definition | Code |
|----|-----------|-------------------|------|
| L3.1 | Slow-growing function types | popcount, log*, inverse Ackermann | `src/ladner.c` |
| L3.2 | Sparse set with polynomial bound | \|S_n\| <= n^k | `src/sparse_sets.c`, `src/sparse_operations.c` |
| L3.3 | NP degree lattice | Poset of NP-m-degrees under <=^p_m | `src/degree_structure.c` |
| L3.4 | Graph adjacency matrix | n x n boolean matrix | `src/graph_isomorphism.c` |
| L3.5 | CNF formula encoding | Clauses of literals | `include/ladner.h` (CNFFormula) |
| L3.6 | Padded SAT encoding | phi # 1^{2^{|phi|}} | `src/padded_sat_full.c` |
| L3.7 | Delayed diagonalization state machine | Track f(n), machine index, progress | `src/delayed_diag.c` |
| L3.8 | Oracle Turing machine simulation | P^O, NP^O wrappers | `src/ladner.c`, `src/oracle_construction.c` |

## L4: Fundamental Theorems

| ID | Theorem | Statement | Code Status |
|----|---------|-----------|-------------|
| L4.1 | Ladner's Theorem (1975) | If P!=NP, exist NP-intermediate languages | Full demo + property verification |
| L4.2 | Mahaney's Theorem (1982) | No sparse NPC set unless P=NP | Pruning algorithm + verification |
| L4.3 | Schoning's Theorem (1982) | Infinite strict NP-m-degree hierarchy | Hierarchy construction demo |
| L4.4 | Valiant-Vazirani (1986) | SAT <=_r Unique-SAT, Pr >= 1/(8n) | Counting + reduction simulation |
| L4.5 | Berman-Hartmanis Conjecture (1977) | All NPC sets p-isomorphic | Isomorphism verification |
| L4.6 | Baker-Gill-Solovay (1975) | Exist oracles A,B with P^A=NP^A, P^B!=NP^B | Oracle world construction |
| L4.7 | Razborov-Rudich (1997) | Natural proofs barrier | Natural proof candidate analysis |
| L4.8 | Ladner property: L in NP | Membership verifiable in polynomial time | Verified in ladner_language_demo |
| L4.9 | Ladner property: L not in P | Differs from every P-machine | Delayed diagonalization verification |
| L4.10 | Ladner property: L not NPC | Sparse-like structure + Mahaney | Sparse analysis verification |

## L5: Algorithms/Methods

| ID | Algorithm | Description | Code |
|----|-----------|-------------|------|
| L5.1 | Popcount | O(log n) bit counting | `src/ladner.c` |
| L5.2 | Delayed diagonalization | L(n) = !M_{f(n)}(n) | `src/delayed_diag.c` |
| L5.3 | Mahaney pruning | SAT tree pruning with sparse oracle | `src/mahaney.c` |
| L5.4 | Weisfeiler-Lehman refinement | Color refinement for GI | `src/graph_isomorphism.c` |
| L5.5 | Pollard's rho | Probabilistic factoring, O(n^{1/4}) | `src/factoring.c` |
| L5.6 | Fermat factorization | a^2 - b^2 = n factorization | `src/factoring.c` |
| L5.7 | Trial division | O(sqrt(n)) factoring | `src/factoring.c` |
| L5.8 | Valiant-Vazirani reduction | XOR constraint isolation | `src/uniqueness_np.c` |
| L5.9 | Graph isomorphism brute force | n! backtracking with pruning | `src/graph_isomorphism.c` |

## L6: Canonical Problems

| ID | Problem | Complexity Status | Code |
|----|---------|-------------------|------|
| L6.1 | SAT / Padded SAT | NPC / NP-intermediate candidate | `src/padded_sat_full.c` |
| L6.2 | Graph Isomorphism | Quasipoly (Babai 2015), not known NPC | `src/graph_isomorphism.c` |
| L6.3 | Integer Factoring | Subexp (GNFS), not known NPC | `src/factoring.c` |
| L6.4 | Unique-SAT | NP-hard (randomized), may be NPC | `src/uniqueness_np.c` |
| L6.5 | Quadratic Residuosity | In NP, not known NPC | `src/promise_problems.c` |
| L6.6 | Square-Free | In P | `src/promise_problems.c` |
| L6.7 | Ladner's Language L | Proven NP-intermediate (if P!=NP) | `src/ladner_language.c` |

## L7: Applications

| ID | Application | Description | Code |
|----|-------------|-------------|------|
| L7.1 | Cryptography (RSA) | Factoring hardness => RSA security | `src/factoring.c` |
| L7.2 | Cryptography (D-H) | Discrete log hardness | `src/np_intermediate_candidates.c` |
| L7.3 | NIST PQC (Lattice) | SVP hardness => lattice crypto | `src/np_intermediate_candidates.c` |
| L7.4 | Natural proofs barrier | Limits on circuit lower bound techniques | `src/natural_proofs.c` |

## L8: Advanced Topics

| ID | Topic | Description | Code |
|----|-------|-------------|------|
| L8.1 | Schoning infinite hierarchy | Infinitely many distinct NP-m-degrees | `src/schoning_theorem.c` |
| L8.2 | Oracle worlds (BGS) | Different oracle structures | `src/oracle_construction.c` |
| L8.3 | Natural proofs (RR) | Razborov-Rudich barrier | `src/natural_proofs.c` |
| L8.4 | Relativized Ladner | Ladner's proof in oracle worlds | `src/relativization_ladner.c` |
| L8.5 | Low/High hierarchy | Oracle-based classification of NP sets | `src/degree_structure.c` |

## L9: Research Frontiers

| ID | Topic | Status |
|----|-------|--------|
| L9.1 | Meta-complexity | Is Ladner's L "natural"? Can we find natural NPI? |
| L9.2 | GI quasipolynomial | Babai 2015 breakthrough. Is GI in P? |
| L9.3 | Quantum complexity | Shor's algorithm. What does BQP mean for NP structure? |
| L9.4 | GCT (Mulmuley-Sohoni) | Algebraic geometry approach to P vs NP |
| L9.5 | Proof complexity | Lower bounds for proof systems |
