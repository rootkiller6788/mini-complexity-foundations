# Cook-Levin Theorem

**MIT 6.840 §5 · Sipser §7.4 · Cook (STOC 1971) · AB §2.3**

## Theorem
SAT is NP-complete. Every language in NP polynomial-time reduces to SAT.

## Proof (Tableau Method)
1. Given NTM M deciding L in time p(n), build CNF φ
2. Variables encode: cell[t,p,s], head[t,p], state[t,q]
3. Clauses: start, cell-uniqueness, head-uniqueness, transitions, accept
4. Size: O(poly(n)) = polynomial

## Implementation
- `ntm.c`: NTM simulator (accepts {"1"})
- `tableau.c`: Full tableau construction with all clause groups
- `verifier.c`: Systematic correctness verification
- `ntm_variants.c`: Multiple NP language verifiers
- `benchmark.c`, `clause_analyzer.c`, `encoding_bench.c`: Empirical analysis