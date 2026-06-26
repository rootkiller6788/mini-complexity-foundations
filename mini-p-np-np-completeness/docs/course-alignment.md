# Course Alignment — mini-p-np-np-completeness

## Nine-School Curriculum Mapping

This module covers the common core of P/NP/NP-completeness across all nine target institutions.

### MIT — 6.045/18.400 Automata, Computability, Complexity + 6.841 Advanced Complexity

| MIT Topic | Module Coverage |
|-----------|---------------|
| P, NP definitions | time_classes.h (L1-L2) |
| NP-completeness, SAT | sat.h/sat.c, cook_levin.c (L4) |
| Cook-Levin Theorem | cook_levin.c (tableau construction), formalization.lean |
| Karp's 21 NP-complete problems | np_catalog.c (16 problems), reduction.c, graph_np.c |
| Time Hierarchy Theorem | padding.c (time_hierarchy_demo) |
| Space complexity (PSPACE) | time_classes.h (CLASS_PSPACE) |
| Circuit complexity | circuit.c (Boolean circuits, Tseitin) |
| PCP Theorem (6.841) | formalization.lean (L8 statement), docs/hastad-inapproximability.md |
| Interactive Proofs (6.841) | formalization.lean (L8 IP=PSPACE statement) |

### Stanford — CS254 Computational Complexity

| Stanford Topic | Module Coverage |
|---------------|---------------|
| NP-completeness proofs | cook_levin.c, reduction.c |
| Circuit complexity lower bounds | circuit.c (parity, majority) |
| Natural Proofs barrier | formalization.lean (L8 theorem) |
| Hastad's Switching Lemma | docs/hastad-inapproximability.md |
| Phase transitions | phase_analysis.c |
| PCP Theorem | formalization.lean, docs/ |

### Berkeley — CS172 Computability & Complexity + CS278 Computational Complexity

| Berkeley Topic | Module Coverage |
|---------------|---------------|
| NP and NP-completeness | Full module (L1-L6) |
| Space complexity | time_classes.h |
| Circuit lower bounds | circuit.c |
| Interactive proofs (CS278) | formalization.lean |
| PCP theorem (CS278) | formalization.lean, docs/ |

### CMU — 15-455 Undergrad Complexity + 15-855 Graduate Complexity

| CMU Topic | Module Coverage |
|-----------|---------------|
| P, NP, polynomial time | time_classes.c |
| NP-completeness reductions | reduction.c, np_catalog.c |
| SAT algorithms (DPLL/CDCL) | dpll.c, cdcl.c, heuristics.c |
| Resolution proof system | resolution.c, resolution_tree.c |
| Phase transition | phase_analysis.c (empirical) |
| Circuit complexity | circuit.c |

### Princeton — COS 522 Computational Complexity + COS 551 Advanced Complexity

| Princeton Topic | Module Coverage |
|----------------|---------------|
| NP-completeness | Full (L1-L6) |
| Cryptography connection (OWF => NP != P) | formalization.lean (L7) |
| Interactive proofs | formalization.lean (L8) |
| PCP Theorem + hardness of approximation | docs/hastad-inapproximability.md |

### Caltech — CS 151 Complexity Theory + CS 154 Limits of Computation

| Caltech Topic | Module Coverage |
|--------------|---------------|
| Computational models | NTMs, circuits |
| NP-completeness | Full (Karp's reductions) |
| Hierarchy theorems | padding.c (Time Hierarchy) |
| Randomized computation | stochastic.c (WalkSAT, SA) |

### Cambridge — Part II Complexity Theory + Part III Advanced Complexity

| Cambridge Topic | Module Coverage |
|----------------|---------------|
| P vs NP | Full module |
| NP-completeness | Cook-Levin + Karp reductions |
| Polynomial hierarchy | time_classes.h (enum includes PH reference) |
| Circuit complexity | circuit.c |
| Interactive proofs | formalization.lean |
| PCP Theorem | formalization.lean, docs/ |

### Oxford — Computational Complexity + Advanced Complexity Theory

| Oxford Topic | Module Coverage |
|-------------|---------------|
| P, NP, reductions | Full module |
| Cook-Levin Theorem | cook_levin.c |
| Circuit complexity | circuit.c |
| Quantum complexity (reference) | formalization.lean (L9 BQP reference) |

### ETH — 263-4650 Advanced Complexity + 252-0400 Logic & Computation

| ETH Topic | Module Coverage |
|-----------|---------------|
| Complexity classes | time_classes.c (L1-L2) |
| NP-completeness | Full module |
| Proof complexity | resolution.c (PHP exponential lower bound) |
| Circuit complexity | circuit.c |
| Descriptive complexity | Not yet implemented |

## Textbook Mapping

| Textbook | Coverage in Module |
|----------|-------------------|
| Arora-Barak (2009) | Primary reference: Chapters 1-6, 11, 22 |
| Sipser (2013) | Core definitions: Chapters 3, 7 |
| Papadimitriou (1994) | Karp reductions, logical characterization |
| Goldreich (2008) | Conceptual framework for NP/crypto connection |
| Moore-Mertens (2011) | Phase transition, statistical physics |
| Vollmer (1999) | Circuit complexity definitions |
| Garey-Johnson (1979) | NP-complete problem catalog |
| Nielsen-Chuang (2010) | Quantum complexity reference (L9) |
