# PSPACE-Complete Problems

## Canonical: TQBF
True Quantified Boolean Formula.
- Input: Q1x1...Qnxn. φ(x)
- Question: Is the formula true?
- PSPACE-complete (Stockmeyer-Meyer 1973)

## Other PSPACE-complete problems:
- QSAT (Quantified SAT)
- Generalized Geography
- First-order logic over finite structures
- Regular expression equivalence
- Context-free grammar universality
- Lambda calculus typeability (undecidable in general, PSPACE for simple types)
- Many board games (Hex, Checkers, Go endgame)

## Reductions
All PSPACE-complete problems reduce to each other via
polynomial-time (in fact, logspace) reductions.