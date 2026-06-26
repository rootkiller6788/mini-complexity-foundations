# Prerequisite Dependency Tree — mini-time-hierarchy-theorem

## Upstream Dependencies (What you need to know first)

```
mini-time-hierarchy-theorem
├── Computability Theory
│   ├── Turing machines (definition, configuration, computation)
│   │   └── Ref: Sipser §3.1, AB §1.2
│   ├── Universal Turing machine
│   │   └── Ref: Sipser §3.2, AB §1.4
│   ├── Godel numbering / TM enumeration
│   │   └── Ref: Sipser §4.2
│   └── Undecidability (Halting Problem)
│       └── Ref: Sipser §4.2, AB §1.5
│
├── Asymptotic Analysis
│   ├── Big-O, Little-o notation
│   │   └── Ref: CLRS §3, AB §1.3
│   ├── Polynomial vs Exponential growth
│   │   └── Ref: AB §1.3
│   └── Time-constructible functions
│       └── Ref: AB §3.1
│
├── Complexity Classes (Basic)
│   ├── TIME(f(n)), NTIME(f(n))
│   │   └── Ref: Sipser §7.1, AB §1.2
│   ├── P, NP, EXP, NEXP
│   │   └── Ref: Sipser §7.2-7.3, AB §1-2
│   └── PSPACE
│       └── Ref: Sipser §8, AB §4
│
└── Proof Techniques
    ├── Diagonalization (Cantor)
    │   └── Ref: Sipser §4.2, AB §3.1
    ├── Proof by contradiction
    └── Simulation arguments
```

## Downstream Dependencies (What depends on this)

```
mini-time-hierarchy-theorem
├── mini-p-np-np-completeness
│   └── Depends on: diagonalization, class structure, relativization
│
├── mini-space-hierarchy
│   └── Depends on: hierarchy method, constructibility, padding
│
├── mini-ladner-theorem
│   └── Depends on: diagonalization technique, complexity classes
│
├── mini-polynomial-hierarchy
│   └── Depends on: alternation, oracle machines, hierarchy structure
│
├── mini-pspace-npspace
│   └── Depends on: time/space relationship, relativization
│
├── mini-boolean-circuits-model (circuit complexity)
│   └── Depends on: hierarchy separation, barrier awareness
│
├── mini-natural-proofs-barrier
│   └── Depends on: limits.c barrier discussion
│
├── mini-ip-pspace
│   └── Depends on: non-relativizing techniques
│
└── mini-pcp-theorem
    └── Depends on: complexity class landscape
```

## Internal Module Dependency Graph

```
tht.h (main API)
├── tm_types.h (TM definitions)
├── diagonal_language.h (diagonal construction)
├── constructible_funcs.h (time-constructible functions)
└── complexity_classes.h (class relations)

src/tht.c (core: P ≠ EXP demo)
├── src/tm_simulator.c (full TM simulation)
│   └── src/tm_enumerator.c (Godel numbering)
├── src/diagonal.c (diagonalization proof)
│   └── src/diagonal_computation.c (L_D computation)
├── src/constructible.c (constructible verification)
│   └── src/gap_theorem.c (Gap theorem)
├── src/oracle_machines.c (BGS theorem)
│   └── src/oracle_simulator.c (oracle TM)
├── src/poly_vs_exp.c (P ⊊ EXP)
│   └── src/exponential_hierarchy.c (tower of EXP)
├── src/nondeterministic_hierarchy.c (NTIME hierarchy)
├── src/alternation.c (alternating hierarchy)
├── src/speedup.c (Blum speedup)
├── src/padding.c (padding argument)
├── src/limits.c (3 barriers)
├── src/hierarchy_table.c (class zoo)
├── src/complexity_tester.c (empirical tester)
├── src/benchmark_tht.c (benchmarks)
├── src/timing_benchmark.c (timing)
├── src/exponent_fitter.c (regression)
├── src/fuzz_tht.c (diagonal fuzz test)
├── src/utm.c (UTM demo)
│
├── [L7 Applications]
│   ├── src/crypto_timing.c (cryptography)
│   ├── src/model_checking.c (formal verification)
│   └── src/circuit_verification.c (hardware)
│
└── [L8 Advanced]
    ├── src/monte_carlo_complexity.c (Monte Carlo, MCMC, Lyapunov)
    └── src/bayesian_timing.c (Bayesian, Markov blanket, fuzzy)
```

## Learning Path

### Beginner Track (Undergraduate)
1. `src/tht.c` — core hierarchy demo (simplest)
2. `examples/tht_demo.c` — P ≠ EXP explanation
3. `src/tm_simulator.c` — TM simulation with timing
4. `src/diagonal.c` — Cantor-style diagonalization

### Intermediate Track (Graduate)
5. `src/constructible.c` — why constructibility matters
6. `src/poly_vs_exp.c` — full P ⊊ EXP proof details
7. `src/oracle_machines.c` — BGS relativization barrier
8. `src/nondeterministic_hierarchy.c` — NTIME Cook/Zak

### Advanced Track (Research)
9. `src/limits.c` — all three barriers
10. `src/exponential_hierarchy.c` — tower of EXP classes
11. `src/gap_theorem.c` — Borodin gap, pathologic f
12. `src/speedup.c` — Blum axioms
13. `src/alternation.c` — Chandra-Kozen-Stockmeyer
14. `src/monte_carlo_complexity.c` — L8: empirical detection
15. `src/bayesian_timing.c` — L8: Bayesian/fuzzy extensions
16. `src/tht.lean` — formal theorem statements
