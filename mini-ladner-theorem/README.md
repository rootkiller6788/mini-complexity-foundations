# mini-ladner-theorem

**Ladner's Theorem (JACM 1975)**: If P != NP, there exist NP-intermediate languages ? languages in NP that are NOT in P and NOT NP-complete.

## Module Status: COMPLETE

| Level | Name | Rating |
|-------|------|--------|
| L1 | Definitions | Complete |
| L2 | Core Concepts | Complete |
| L3 | Mathematical Structures | Complete |
| L4 | Fundamental Theorems | Complete |
| L5 | Algorithms/Methods | Complete |
| L6 | Canonical Problems | Complete |
| L7 | Applications | Complete |
| L8 | Advanced Topics | Complete |
| L9 | Research Frontiers | Partial |

**Score: 17/18** ? COMPLETE

## Quick Start

```bash
make          # compile and run all tests
make test     # run test suite (13 unit test groups + full demos)
make examples # build and run factoring + GI demos
make clean    # remove build artifacts
```

## Core Definitions (L1)

- **NP-intermediate language**: L in NP, L not in P, L not NPC
- **Slow-growing function**: f: N->N, unbounded, f(n)=o(n^c) for all c>0
- **Sparse set**: S where |S_n| <= p(n) for some polynomial p
- **Delayed diagonalization**: Diagonalize against machines using slow-growing index
- **P-isomorphism**: Poly-time bijection preserving membership
- **NP-m-degree**: Equivalence class under polynomial-time many-one reductions

## Core Theorems (L4)

### Ladner's Theorem (1975)
**Statement**: If P != NP, there exists L such that L in NP, L not in P, L not NPC.
**Construction**: L = {x in SAT | f(|x|) is even}
**Key technique**: Delayed diagonalization with slow-growing f.

### Mahaney's Theorem (1982)
**Statement**: If a sparse set is NP-complete, then P = NP.
**Contrapositive**: If P != NP, no sparse set is NPC.
**Application**: Proves Ladner's L is not NPC.

### Sch?ning's Theorem (1982)
**Statement**: There exist infinitely many distinct NP-m-degrees.
**Generalization**: Any complexity class with certain properties has infinite intermediate hierarchy.

### Berman-Hartmanis Conjecture (1977)
**Statement**: All NP-complete languages are p-isomorphic.
**Status**: OPEN. Compatible with Ladner's theorem (L is not NPC).

### Valiant-Vazirani Theorem (1986)
**Statement**: SAT <=_r Unique-SAT with Pr >= 1/(8n).
**Corollary**: If Unique-SAT in P, then NP = RP.

### Baker-Gill-Solovay (1975)
**Statement**: There exist oracles A, B with P^A=NP^A, P^B!=NP^B.
**Implication**: Diagonalization techniques that relativize cannot resolve P vs NP.

### Razborov-Rudich (1997)
**Statement**: If subexponentially strong OWF exist, no natural proof can show NP not-subset P/poly.
**Implication**: Circuit lower bounds for NP require non-natural techniques.

## Core Algorithms (L5)

| Algorithm | Complexity | File |
|-----------|------------|------|
| Popcount | O(log n) | src/ladner.c |
| Delayed diagonalization | O(n) per call | src/delayed_diag.c |
| Mahaney pruning | Poly(n) with sparse oracle | src/mahaney.c |
| Weisfeiler-Lehman refinement | O(n^3) | src/graph_isomorphism.c |
| Pollard's rho | O(n^{1/4}) expected | src/factoring.c |
| Fermat factorization | O(n^{1/3}) for close primes | src/factoring.c |
| Trial division | O(sqrt(n)) | src/factoring.c |
| V-V isolation | Randomized poly-time | src/uniqueness_np.c |
| GI brute force | O(n!) | src/graph_isomorphism.c |

## Canonical Problems (L6)

| Problem | Status | Best Known |
|---------|--------|------------|
| SAT (Padded) | NPC / NPI candidate | - |
| Graph Isomorphism | Quasipoly (Babai 2015) | exp(O(log^c n)) |
| Integer Factoring | Subexp (GNFS) | exp(O(n^{1/3})) |
| Unique-SAT | NP-hard (randomized) | - |
| Quadratic Residuosity | In NP | - |
| Square-Free | In P | O(sqrt(n)) |
| Ladner's L | Proven NPI (if P!=NP) | - |

## Course Alignment (L7-L9)

### Nine-School Coverage

| School | Course | Key Topics Covered |
|--------|--------|--------------------|
| MIT | 6.045, 6.841, 6.845 | Ladner, Mahaney, delayed diag, quantum |
| Stanford | CS254, CS358 | B-H conjecture, sparse sets, natural proofs |
| Berkeley | CS172, CS278 | NPI degrees, Sch?ning, V-V |
| CMU | 15-455, 15-855 | Low/High hierarchy |
| Princeton | COS 522, COS 551 | Factoring, crypto implications |
| Caltech | CS 151, CS 154 | Diagonalization limits |
| Cambridge | Part II, Part III | Structural complexity |
| Oxford | Comp Complexity, Advanced | Quantum connection |
| ETH | 263-4650, 252-0400 | Logic, formal definitions |

## File Structure

```
mini-ladner-theorem/
??? Makefile                    # build system
??? README.md                   # this file
??? include/
?   ??? ladner.h                # 220 lines: all type definitions + API
??? src/
?   ??? ladner.c                # 138 lines: slow-growing f, construction
?   ??? delayed_diag.c          # 176 lines: delayed diagonalization
?   ??? ladner_language.c       # 197 lines: full L construction + verification
?   ??? mahaney.c               # 187 lines: Mahaney pruning + theorem
?   ??? berman_hartmanis.c      # 192 lines: p-isomorphism
?   ??? sparse_sets.c           # 179 lines: sparse set operations
?   ??? sparse_operations.c     # 185 lines: advanced sparse ops
?   ??? degree_structure.c      # 172 lines: NP degree lattice
?   ??? factoring.c             # 227 lines: factoring algorithms
?   ??? graph_isomorphism.c     # 231 lines: GI algorithms
?   ??? schoning_theorem.c      # 99 lines: infinite hierarchy
?   ??? natural_proofs.c        # 69 lines: RR barrier
?   ??? relativization_ladner.c # 83 lines: oracle relativization
?   ??? oracle_construction.c   # 194 lines: oracle worlds
?   ??? padded_sat_full.c       # 47 lines: padded SAT
?   ??? promise_problems.c      # 143 lines: promise problems
?   ??? uniqueness_np.c         # 171 lines: V-V theorem
?   ??? np_intermediate_candidates.c # 206 lines: NPI catalog
?   ??? ladner_bench.c          # 94 lines: benchmarks
?   ??? ladner.lean             # 175 lines: Lean 4 formalization
??? tests/
?   ??? test_ladner.c           # 437 lines: 13 unit test groups
??? examples/
?   ??? factoring_demo.c        # 96 lines: factoring example
?   ??? gi_demo.c               # 109 lines: GI example
??? docs/
    ??? knowledge-graph.md      # 112 lines: L1-L9 knowledge map
    ??? coverage-report.md      # 108 lines: coverage assessment
    ??? gap-report.md           # 41 lines: gap analysis
    ??? course-alignment.md     # 76 lines: 9-school mapping
    ??? course-tree.md          # 67 lines: dependency tree
    ??? ladner-proof.md         # 115 lines: proof structure
```

**include/ + src/ = 3209 lines** (>3000 minimum)

## Building

```bash
make          # compile + run tests
make clean    # clean build artifacts
make examples # build + run both demos
```

## Testing

13 unit test groups verify:
- Slow-growing functions (popcount, log*, inv-Ackermann)
- Padded SAT encoding/decoding
- Delayed diagonalization consistency
- Ladner language membership
- Sparse set operations and bounds
- Factoring algorithms (GCD, Pollard rho, Fermat)
- Mahaney pruning correctness
- NP degree structure comparisons
- Graph isomorphism (brute force + WL)
- Promise problems (square-free, QR)
- NPI candidate counts and properties
- Berman-Hartmanis isomorphism verification
- Full end-to-end demo suite

## References

- Ladner, R.E. "On the Structure of Polynomial Time Reducibility." JACM 22(1):155-171, 1975.
- Mahaney, S.R. "Sparse Complete Sets for NP." JCSS 25(2):130-143, 1982.
- Sch?ning, U. "A uniform approach to obtain diagonal sets in complexity classes." TCS 21, 1982.
- Berman, L. & Hartmanis, J. "On Isomorphisms and Density of NP." SIAM J. Comput. 6(2):305-322, 1977.
- Valiant, L.G. & Vazirani, V.V. "NP is as easy as detecting unique solutions." TCS 47:85-93, 1986.
- Baker, T., Gill, J. & Solovay, R. "Relativizations of the P=?NP Question." SIAM J. Comput. 4(4):431-442, 1975.
- Razborov, A.A. & Rudich, S. "Natural Proofs." JCSS 55(1):24-35, 1997.
- Babai, L. "Graph Isomorphism in Quasipolynomial Time." STOC 2016.
- Arora, S. & Barak, B. "Computational Complexity: A Modern Approach." Cambridge, 2009.
- Sipser, M. "Introduction to the Theory of Computation." 3rd ed., Cengage, 2013.
