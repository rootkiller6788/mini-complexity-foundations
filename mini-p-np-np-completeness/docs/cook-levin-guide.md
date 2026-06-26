# Cook-Levin Theorem Guide

## Theorem (Cook 1971)
SAT (Boolean Satisfiability) is NP-Complete.

## Proof Structure

### 1. SAT ∈ NP
Trivial: given an assignment, verify it satisfies the formula in polynomial time O(n·m).

### 2. ∀L ∈ NP, L ≤_p SAT
Given any NP language L, there exists a polynomial-time NTM M = (Q,Σ,Γ,δ,q₀,q_accept,q_reject)
that decides L in time p(n) for some polynomial p.

Construct a boolean formula φ such that φ is satisfiable iff M accepts input w within p(|w|) steps.

### Variable Encoding
For computation of length T = p(|w|), tape cells indexed -T to T:

- **State variables**: S[t,q] = 1 iff at time t, machine is in state q
- **Head variables**: H[t,i] = 1 iff at time t, head is at position i
- **Tape variables**: C[t,i,s] = 1 iff at time t, cell i contains symbol s

Total variables: O(T²) = polynomial in |w|.

### Clause Groups

1. **Start configuration**: Encode initial tape, head position, state
2. **Unique state/head/tape**: At each time step, exactly one state, one head position, one symbol per cell
3. **Transition**: If at time t machine is (q,i,σ) and δ(q,σ)→(q',σ',d), then at time t+1 state=q', head=i+d, tape[i]=σ'
4. **Accept condition**: At some time t, state = q_accept

Total clauses: O(T³) = polynomial in |w|.

## Significance

Cook's theorem established that NP-complete problems exist. If any NP-complete problem
can be solved in polynomial time, then **all** NP problems can (P = NP).

Conversely, proving a lower bound for any NP-complete problem proves P ≠ NP.

## Reference
Cook, S.A. "The Complexity of Theorem-Proving Procedures." STOC 1971, pp. 151-158.
