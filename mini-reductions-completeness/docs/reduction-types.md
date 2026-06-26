# Reduction Types

## Cook (Turing) Reduction
A <=_T B: poly-time TM with oracle B solves A.

## Karp (Many-One) Reduction  
A <=_m B: poly-time f s.t. x in A iff f(x) in B.

## Logspace Reduction
A <=_L B: f computable in O(log n) space.

## Properties
- Karp < Cook (strict)
- Logspace < Poly-time
- NP-completeness: Karp reductions
- PH: Cook reductions (oracle hierarchy)
