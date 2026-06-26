# Course Alignment — mini-time-hierarchy-theorem

## Nine-School Curriculum Mapping

### MIT
- **6.045/18.400 Automata, Computability, Complexity**
  - §9: Time Complexity → `src/tht.c`, `src/diagonal.c`
  - §10: NP-Completeness → `src/poly_vs_exp.c`
- **6.841 Advanced Complexity Theory**
  - §3: Diagonalization → `src/diagonal.c`
  - §4: Time Hierarchy → `src/tht.c`, `src/exponential_hierarchy.c`
  - §5: Space Complexity → (future: mini-space-hierarchy)
  - §6: Relativization → `src/oracle_machines.c`, `src/limits.c`

### Stanford
- **CS254 Computational Complexity**
  - Lecture 3-4: Time Hierarchy, P ≠ EXP → `src/tht.c`, `src/poly_vs_exp.c`
  - Lecture 5: Oracle machines, relativization → `src/oracle_machines.c`
  - Lecture 9: NTIME hierarchy → `src/nondeterministic_hierarchy.c`
  - Lecture 22: PCP Theorem → `src/limits.c` (barrier discussion)

### Berkeley
- **CS172 Computability and Complexity**
  - Chapter 7: Time Complexity → `src/tm_simulator.c`, `src/constructible.c`
  - Chapter 9: Intractability → `src/poly_vs_exp.c`
- **CS278 Computational Complexity**
  - Diagonalization → `src/diagonal.c`
  - Relativization → `src/oracle_machines.c`

### CMU
- **15-455 Undergraduate Complexity Theory**
  - Time Hierarchy Theorem → `src/tht.c`, `src/diagonal.c`
  - P and EXP → `src/poly_vs_exp.c`
- **15-855 Graduate Complexity Theory**
  - Barriers: Relativization, Natural Proofs, Algebrization → `src/limits.c`
  - NTIME hierarchy → `src/nondeterministic_hierarchy.c`

### Princeton
- **COS 522 Computational Complexity**
  - §3: Diagonalization → `src/diagonal.c`
  - §4: Time Hierarchy → `src/tht.c`
  - §20: Cryptography and Complexity → `src/crypto_timing.c`

### Caltech
- **CS 151 Complexity Theory**
  - Time/Space Hierarchy → `src/tht.c`, `src/exponential_hierarchy.c`
  - Relativization → `src/oracle_machines.c`
- **CS 154 Limits of Computation**
  - Godel numbering → `src/tm_enumerator.c`
  - Universal TM → `src/utm.c`

### Cambridge
- **Part II Complexity Theory**
  - Time Hierarchy, P ≠ EXP → `src/tht.c`
  - Relativization → `src/oracle_machines.c`
  - Gap Theorem → `src/gap_theorem.c`
  - Speedup Theorem → `src/speedup.c`
- **Part III Advanced Complexity**
  - Barriers survey → `src/limits.c`
  - Circuit complexity preview → `src/circuit_verification.c`

### Oxford
- **Computational Complexity**
  - Hierarchy theorems → `src/tht.c`, `src/exponential_hierarchy.c`
  - Oracle computations → `src/oracle_machines.c`
- **Advanced Complexity Theory**
  - Quantum complexity → (documented in L9)
  - Boolean function analysis → (future: mini-circuit-complexity)

### ETH
- **263-4650 Advanced Complexity**
  - Constructible functions → `src/constructible.c`
  - Gap/Speedup theorems → `src/gap_theorem.c`, `src/speedup.c`
  - Alternating hierarchy → `src/alternation.c`
- **252-0400 Logic and Computation**
  - Godel numbering → `src/tm_enumerator.c`
  - Undecidability and complexity → `src/limits.c`

## Module Mapping Summary

| Module | Primary School | Key Files |
|--------|---------------|-----------|
| Time Hierarchy | MIT, Stanford, Cambridge | `tht.c`, `diagonal.c` |
| P ⊊ EXP | All 9 | `poly_vs_exp.c` |
| Constructibility | ETH, Cambridge | `constructible.c` |
| Oracles & BGS | Berkeley, Princeton | `oracle_machines.c` |
| Barriers | CMU, MIT, Stanford | `limits.c` |
| NTIME Hierarchy | Stanford, CMU | `nondeterministic_hierarchy.c` |
| Exponential Hierarchy | All | `exponential_hierarchy.c` |
| Applications | CMU, Princeton | `crypto_timing.c`, `model_checking.c`, `circuit_verification.c` |
| Advanced Topics | Stanford, CMU | `monte_carlo_complexity.c`, `bayesian_timing.c` |
