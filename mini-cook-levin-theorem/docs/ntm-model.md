# Nondeterministic Turing Machine Model

## Definition
An NTM is M = (Q, Σ, Γ, δ, q₀, q_accept, q_reject) where:
- Q: finite set of states
- Σ: input alphabet
- Γ: tape alphabet (Σ ∪ {blank})
- δ: Q × Γ → P(Q × Γ × {L,R,S})
- Computation tree: all possible nondeterministic choices

## NP = Nondeterministic Polynomial Time
L ∈ NP iff there exists NTM M s.t.:
- M decides L (accepts iff w ∈ L)
- M runs in time O(n^k) on every computation path

## Certificate View
Equivalent definition: L ∈ NP iff there exists poly-time verifier V s.t.:
- If w ∈ L: ∃c s.t. V(w,c) = 1
- If w ∉ L: ∀c, V(w,c) = 0