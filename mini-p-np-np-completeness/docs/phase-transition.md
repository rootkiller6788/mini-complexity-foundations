# SAT Phase Transition

## The Phenomenon

For random k-SAT, there is a sharp phase transition at a critical
clause-to-variable ratio α_c ≈ 4.26 (for k=3):

- Below α_c: almost all formulas are SAT (under-constrained)
- Above α_c: almost all formulas are UNSAT (over-constrained)
- Near α_c: hardest instances (50% SAT, exponential solve time)

## Theoretical Significance

The phase transition is analogous to physical phase transitions
(water ↔ ice). At the critical point, the problem exhibits:

- **Computational complexity peak**: Instances near α_c take the
  longest to solve
- **Backbone**: The fraction of variables forced to a specific
  value jumps discontinuously at α_c
- **Universality**: The critical behavior is independent of
  microscopic details (universality class)

## Experimental Approach

Our `phase_analysis.c` module:
1. Generates random k-SAT instances at varying ratios
2. Measures solve time and satisfiability fraction
3. Plots the sharp transition curve

## Reference

- Cheeseman, P., Kanefsky, B., Taylor, W.M. "Where the Really Hard Problems Are" (IJCAI 1991)
- Mitchell, D., Selman, B., Levesque, H. "Hard and Easy Distributions of SAT Problems" (AAAI 1992)
