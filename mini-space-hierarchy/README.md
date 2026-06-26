# mini-space-hierarchy -- Space Hierarchy Theorem

**SPACE(f) != SPACE(g) when f = o(g). Corollary: L != PSPACE.**

## Module Status: COMPLETE

- L1 Definitions: Complete (SpaceClass, ConfigGraph, TMConfig, SPTMConfig)
- L2 Core Concepts: Complete (space-constructible, config graph, reachability, relativization)
- L3 Math Structures: Complete (SpaceTM, ConfigGraphSpace, TMEncoding, DiGraph, UGraph)
- L4 Fundamental Laws: Complete (Space Hierarchy, Savitch, Immerman-Szelepcsenyi, Reingold)
- L5 Algorithms/Methods: Complete (logspace algo, NL PATH, Savitch recursion, diagonalization)
- L6 Canonical Problems: Complete (PATH, TQBF, Generalized Geography, 2-SAT, QSAT)
- L7 Applications: Complete (model checking, game playing, VLSI CAD 2-SAT)
- L8 Advanced Topics: Partial+ (Alternating Logspace, RL/BPL, Reingold, relativization)
- L9 Research Frontiers: Partial (BPL vs L, space derandomization, documented)

## Nine-Level Knowledge Coverage Summary

| Level | Name | Coverage | Items |
|-------|------|----------|-------|
| L1 | Definitions | Complete | SpaceClass, SpaceClassDef, ConfigGraph, TMConfig, SpaceTM |
| L2 | Core Concepts | Complete | Space-constructibility, config graph, logspace reduction, diagonalization |
| L3 | Math Structures | Complete | TM, ConfigGraphSpace, DirectedGraph, UndirectedGraph, LogspaceTransducer |
| L4 | Fundamental Laws | Complete | Space Hierarchy, Savitch, Immerman-Szelepcsenyi, Reingold |
| L5 | Algorithms/Methods | Complete | Logspace add/palindrome/GCD, NL PATH, Savitch recursion, diagonalization |
| L6 | Canonical Problems | Complete | PATH (NL-complete), TQBF/QSAT (PSPACE-complete), Gen Geography, NFA Univ |
| L7 | Applications | Complete | Model checking (PSPACE), Game playing (PSPACE), VLSI placement (2-SAT in NL) |
| L8 | Advanced Topics | Partial+ | Alternating Logspace (AL=P), RL/BPL, Reingold USTCON in L, Relativization |
| L9 | Research Frontiers | Partial | BPL vs L conjecture, space-bounded derandomization (documented) |

## Core Definitions

- **SPACE(s(n))**: Languages decidable by deterministic TM using O(s(n)) work tape
- **NSPACE(s(n))**: Nondeterministic version
- **L** = SPACE(log n) -- Deterministic Logspace
- **NL** = NSPACE(log n) -- Nondeterministic Logspace
- **PSPACE** = U_k SPACE(n^k) -- Polynomial Space
- **EXPSPACE** = U_k SPACE(2^{n^k}) -- Exponential Space

## Core Theorems

### Space Hierarchy Theorem (Hartmanis, Lewis & Stearns 1965)
SPACE(f) is a strict subset of SPACE(g) when f = o(g) and g is space-constructible.
Corollary: L != PSPACE (unconditional separation!).

### Savitch Theorem (Savitch 1970)
NSPACE(s(n)) is contained in DSPACE(s(n)^2).
Corollary: PSPACE = NPSPACE (proven collapse, unlike P vs NP).

### Immerman-Szelepcsenyi Theorem (1987)
NL = coNL. Nondeterministic logspace is closed under complement. Godel Prize 1995.

### Reingold Theorem (2008)
Undirected s-t connectivity is in L (deterministic logspace!). SL = L.

## Core Algorithms

1. **Logspace Addition**: O(log n) space via carry bit + index counter
2. **Logspace Palindrome**: O(log n) space via two-pointer comparison
3. **NL PATH**: Nondeterministically guess next vertex, O(log n) space
4. **Savitch PATH**: Recursive divide-and-conquer, O(log^2 n) space
5. **Immerman-Szelepcsenyi Counting**: Inductive counting, O(log n) space
6. **TQBF Evaluation**: Recursive PSPACE algorithm, O(poly n) space
7. **2-SAT via Implication Graph**: NL reduction to PATH

## Canonical Problems

| Problem | Complexity | Completeness |
|---------|-----------|-------------|
| PATH (directed s-t connectivity) | NL-complete | Jones 1975 |
| TQBF (True Quantified Boolean Formula) | PSPACE-complete | Stockmeyer & Meyer 1973 |
| Generalized Geography | PSPACE-complete | Schaefer 1978 |
| 2-SAT | NL-complete | Papadimitriou 1994 |
| NFA Universality | PSPACE-complete | Meyer & Stockmeyer 1972 |

## Nine-School Course Mapping

| School | Course | Relevance |
|--------|--------|-----------|
| MIT | 6.840/18.400 | Space hierarchy (L1-L6) |
| MIT | 6.841 | Savitch, Immerman (L4, L8) |
| Stanford | CS254 | Arora-Barak 4.1-4.2 |
| Berkeley | CS172/CS278 | Space complexity, diagonalization |
| CMU | 15-455/15-855 | Hierarchy theorems, reachability |
| Princeton | COS 522 | Space complexity, PSPACE |
| Caltech | CS 151 | Space hierarchy (Sipser 8-9) |
| Cambridge | Part II | Space classes, Savitch |
| Oxford | Comp Complexity | Quantum aspects (L9) |
| ETH | 263-4650 | Alternating logspace |

## Build & Test

```
make        # Build all examples
make test   # Run test suite
make clean  # Clean build artifacts
```

## Line Count

- include/*.h + src/*.c: 3275 lines (threshold: 3000)
- Headers: 4 (threshold: 4)
- Source files: 19 (threshold: 4)

## References

- Arora & Barak. *Computational Complexity* (2009). 4.1-4.2
- Sipser. *Introduction to the Theory of Computation* (3rd ed, 2013). 8-9
- Hartmanis, Lewis & Stearns. *Hierarchies of memory limited computations* (IEEE 1965)
- Savitch. *Relationships between nondeterministic and deterministic tape complexities* (JCSS 1970)
- Immerman. *Nondeterministic space is closed under complementation* (SICOMP 1988)
- Szelepcsenyi. *The method of forced enumeration* (Acta Inf. 1988)
- Reingold. *Undirected connectivity in log-space* (JACM 2008)
