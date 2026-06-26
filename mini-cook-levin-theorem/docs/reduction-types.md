# Polynomial-Time Reductions

## Cook Reduction (Turing)
A ≤_T B: A can be decided in polynomial time with oracle for B.
- Multiple queries allowed
- Used in Cook's original proof

## Karp Reduction (Many-One)
A ≤_m B: ∃ poly-time f s.t. x ∈ A ⇔ f(x) ∈ B.
- Single instance transformation
- Standard for NP-completeness proofs

## Properties
- Transitive: A≤B and B≤C ⇒ A≤C
- If B ∈ P and A≤B ⇒ A ∈ P
- If A is NP-hard and A≤B ⇒ B is NP-hard

## Cook-Levin uses Karp reduction
f(w) = CNF formula φ encoding M's computation on w
f runs in poly time, preserves membership.