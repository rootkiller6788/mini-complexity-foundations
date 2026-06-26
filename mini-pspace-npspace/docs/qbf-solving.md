# QBF Solving Techniques

## Standard algorithms for QBF (PSPACE-complete):
1. DPLL-style: extend SAT DPLL with quantifier scoping
2. Symbolic (BDD): represent formula as OBDD, quantifiers as projection
3. Expansion: forall x:phi = phi[0] AND phi[1] (ungrounded)
4. Q-resolution: refutationally complete proof system
5. CEGAR-based: counterexample guided abstraction refinement

## Research frontier
- Preprocessing: QBF-specific simplification rules
- Certificate generation: Skolem/Herbrand functions
- Incremental solving: reuse learned clauses across quantifier levels