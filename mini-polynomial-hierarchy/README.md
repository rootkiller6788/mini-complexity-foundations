# mini-polynomial-hierarchy

**Module Status: COMPLETE ✅**

Sigma_0 = P, Sigma_{k+1} = NP^{Sigma_k}, Pi_k = co-Sigma_k.
PH = U_k Sigma_k = U_k Pi_k.

## Nine-Level Knowledge Coverage

| Level | Name | Status | Key Items |
|-------|------|--------|-----------|
| **L1** | Definitions | **Complete** | PH_CNF, PH_QBF, PH_QuantBlock, PH_BoolCircuit, PH_OracleMachine, PH_ATMConfig (6+ structs) |
| **L2** | Core Concepts | **Complete** | quantifier alternation, hierarchy structure, oracle access, collapse, alternating computation |
| **L3** | Math Structures | **Complete** | CNF/DNF formulas, QBF with quantifier blocks, Boolean circuits (DAG), alternating TM states |
| **L4** | Fundamental Laws | **Complete** | Sigma_k-SAT completeness, collapse theorem, Karp-Lipton, Toda (PH in P^{#P}), BPP in Sigma_2, CKS (ATM=PH) |
| **L5** | Algorithms | **Complete** | DPLL SAT, recursive Sigma_k-SAT, #SAT counting, SAT-to-CLIQUE/VC/3COLOR/SUBSET-SUM/HAM-PATH |
| **L6** | Canonical Problems | **Complete** | QSAT_k, TQBF, Circuit Value, CLIQUE, VERTEX-COVER, 3-COLOR, SUBSET-SUM |
| **L7** | Applications | **Complete** | Bounded Model Checking (3), Game Solving (2-player), Circuit Minimization |
| **L8** | Advanced Topics | **Partial+** | PCP verification, Valiant-Vazirani isolation, IP=PSPACE, interactive proofs |
| **L9** | Research Frontiers | **Partial+** | PH infinite? (documented), meta-complexity, GCT, quantum complexity, hardness-randomness |

## Core Definitions

- **Sigma_k^p**: Languages L where exists poly-time predicate R s.t. x∈L iff ∃y₁∀y₂...Q_k y_k: R(x,y₁,...,y_k)
- **Pi_k^p**: co-Sigma_k^p; first quantifier is ∀
- **PH**: ∪_{k≥0} Σ_k^p = ∪_{k≥0} Π_k^p
- **QSAT_k**: Σ_k-complete problem (quantified SAT with k alternations)
- **TQBF**: PSPACE-complete (fully quantified Boolean formulas)
- **PH_CNF**: 3-CNF formula with literal encoding (var<<1)|sign
- **PH_QBF**: Quantified Boolean Formula with block structure

## Core Theorems

1. **Meyer-Stockmeyer (1972/1976)**: Σ_k-SAT is Σ_k^p-complete
2. **Collapse**: If Σ_k = Π_k then PH = Σ_k (domino effect)
3. **Karp-Lipton (1982)**: NP ⊆ P/poly ⇒ PH = Σ₂
4. **Toda (1991)**: PH ⊆ P^{#P} (Gödel Prize 1998)
5. **Sipser-Gács-Lautemann (1983)**: BPP ⊆ Σ₂ ∩ Π₂
6. **Chandra-Kozen-Stockmeyer (1981)**: Σ_k = ATIME(poly(n), k alternations)
7. **Shamir (1992)**: IP = PSPACE

## Core Algorithms

- **Recursive QBF solver**: O(2^{total_vars}) time, O(k) space
- **DPLL SAT solver**: Unit propagation + pure literal elimination
- **#SAT counter**: Brute-force enumeration (#P-complete)
- **Karp reductions**: SAT → CLIQUE, VERTEX-COVER, 3-COLOR, SUBSET-SUM, HAM-PATH
- **BPP amplification**: Chernoff-bound error reduction
- **Oracle machine simulator**: NP^{SAT}, NP^{Sigma_2}

## Nine-School Course Mapping

| School | Course | Key Topic |
|--------|--------|-----------|
| MIT | 6.840/18.404 | PH definition, Sigma_k-SAT completeness |
| Stanford | CS254 | Karp-Lipton, circuit lower bounds |
| Berkeley | CS278 | Toda's theorem, counting complexity |
| CMU | 15-855 | PH collapse, oracle hierarchy |
| Princeton | COS 522 | BPP in PH, derandomization |
| Caltech | CS 151 | Alternating TM, CKS characterization |
| Cambridge | Part III | Interactive proofs, IP=PSPACE |
| Oxford | Adv Complexity | Quantum vs PH, BQP |
| ETH | 263-4650 | Structural complexity, meta-complexity |

## Build & Test

`ash
make        # compile
make test   # run tests (33 tests, 0 failures)
`

## References

- Stockmeyer, L. "The Polynomial-Time Hierarchy." TCS 3(1):1-22, 1976.
- Arora, S. and Barak, B. "Computational Complexity: A Modern Approach." 2009.
- Sipser, M. "Introduction to the Theory of Computation." 3rd ed. 2013.
- Toda, S. "PP is as Hard as the Polynomial-Time Hierarchy." SIAM J. Comput. 1991.
- Karp, R.M. and Lipton, R.J. "Some Connections Between Nonuniform and Uniform Complexity Classes." STOC 1980.
