# Resolution Proof System: Completeness for Refutation

**Stanford CS254 · Davis-Putnam (JACM 1960) · Robinson (JACM 1965)**

## The Resolution Rule
Given clauses C₁ = (A ∨ x) and C₂ = (B ∨ ¬x), derive C = (A ∨ B).

Fundamental property: if C₁ and C₂ are true, then C is true (soundness).

## Theorem: Resolution is Refutation-Complete
If a CNF formula φ is unsatisfiable, then there exists a resolution derivation
of the empty clause □ from φ.

### Proof (Tree Resolution via Semantic Trees)
1. Consider the complete binary tree of all 2ⁿ partial assignments (depth n)
2. For each leaf, check if it falsifies some clause of φ  
3. Mark each node as "failing" if both children are failing
4. A failing node corresponds to a clause: the set of assignments that reach it
   and fail—this clause is a resolvent of the children's clauses
5. The root is failing (since φ is unsatisfiable) → empty clause derived

### The DPLL-Resolution Connection
Every run of DPLL on an unsatisfiable formula corresponds to a tree resolution
refutation. The size of the DPLL search tree equals the resolution proof size.

This is why DPLL takes exponential time on unsatisfiable formulas:
Resolution proofs for pigeonhole principle require exponential size (Haken 1985).

## Resolution Lower Bounds

| Formula | Resolution Size | Proven By |
|---------|----------------|-----------|
| Pigeonhole Principle (PHP^{n+1}_n) | exp(Ω(n)) | Haken (1985) |
| Tseitin formulas (odd charge) | exp(Ω(n)) | Tseitin (1968) |
| Random 3-CNF (α>4.26) | exp(Ω(n)) | Chvátal-Szemerédi (1988) |
| Clique-Coloring | exp(Ω(n)) | — |

## Why Resolution Matters for P vs NP
If we could prove superpolynomial lower bounds for resolution on all proof
systems (not just resolution), we would prove NP ≠ co-NP (and thus P ≠ NP).

Resolution is the weakest interesting proof system. Proving lower bounds
for stronger systems (Cutting Planes, Frege, Extended Frege) is a major
open problem in proof complexity.

## Implementation
Our `src/resolution.c` implements exhaustive resolution search up to a
clause limit. The `mini-proof-complexity` module (#5) covers lower bounds.

## References
Davis, Putnam. "A Computing Procedure for Quantification Theory." JACM 1960.
Robinson. "A Machine-Oriented Logic Based on the Resolution Principle." JACM 1965.
Haken. "The Intractability of Resolution." TCS 1985.
