# Polynomial-Time Reductions

A <=_p B: exists poly-time f s.t. x in A iff f(x) in B.
- Transitive: if A<=B and B<=C then A<=C
- If B in P and A<=B, then A in P
- If A is NP-hard and A<=B, then B is NP-hard

## Karp's 21 Problems (1972)
SAT -> 3SAT -> CLIQUE -> VC -> HC -> TSP -> ...
Each reduction is poly-time and preserves YES/NO.
