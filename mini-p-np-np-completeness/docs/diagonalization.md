# Diagonalization: Why Time Hierarchy Matters

**CMU 15-251 Great Ideas · AB §3 · Sipser §9**

## The Core Idea
Cantor proved |ℝ| > |ℕ| by diagonalization: given any enumeration r₁,r₂,... of
real numbers, construct a real d that differs from every r_i at digit i.

We can do the same for languages: given any enumeration of polynomial-time
Turing machines, construct a language L that no polynomial-time machine decides.

## Time Hierarchy Theorem (Hartmanis-Stearns 1965)
For any time-constructible functions f,g with f(n)·log f(n) = o(g(n)):
  TIME(f(n)) ⊊ TIME(g(n))

**Consequence**: P ⊊ EXP  
Because: TIME(n^k) ⊊ TIME(2^n) for any fixed k.

## Why THIS Matters for P vs NP
1. Time hierarchy gives us P ⊊ EXP unconditionally
2. NP sits between P and EXP: P ⊆ NP ⊆ EXP
3. We know P ≠ EXP (by hierarchy), so at least one of P≠NP or NP≠EXP is true
4. But diagonalization can't tell us which!

## Baker-Gill-Solovay (1975): The Limits of Diagonalization
There exist oracles A,B such that P^A = NP^A and P^B ≠ NP^B.

This means: **no proof technique that "relativizes" can resolve P vs NP**.

Diagonalization relativizes (it works identically with any oracle). 
Therefore: diagonalization alone cannot prove P≠NP.

This is why we need circuit lower bounds, interactive proofs, and other
non-relativizing techniques.

## Phase Transition Theory

### The Empirical Phenomenon
For random 3-SAT at clause-to-variable ratio α = m/n:
- α < 4.26: almost certainly satisfiable (underconstrained)
- α > 4.26: almost certainly unsatisfiable (overconstrained)  
- α ≈ 4.26: hardest instances (50% SAT, exponential median runtime)

### Why 4.26?
The threshold comes from the **method of moments** on the 2-SAT subproblem:
- Unit clause propagation cascades at α > 1
- Pure literal rule stops working around α ≈ 2.5
- The 2-SAT component of a reduced 3-SAT formula becomes unsatisfiable
  at the threshold, which can be computed via branching process analysis

### Theoretical Significance
This is an example of **self-organized criticality**: the hardest instances
occur at the critical boundary between two phases. This is analogous to
physical phase transitions (water ↔ ice) and is a universal property of
constraint satisfaction problems.

## References
Hartmanis, Stearns. "On the Computational Complexity of Algorithms." TAMS 1965.
Baker, Gill, Solovay. "Relativizations of the P=?NP Question." SICOMP 1975.
Cheeseman, Kanefsky, Taylor. "Where the Really Hard Problems Are." IJCAI 1991.
Mitchell, Selman, Levesque. "Hard and Easy Distributions of SAT Problems." AAAI 1992.
