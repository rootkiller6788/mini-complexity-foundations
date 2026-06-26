# Cook-Levin Theorem: Complete Proof Construction

**MIT 6.840 Lecture 5 · Sipser §7.4 · AB §2.3**

## Theorem
SAT ∈ NP-Complete. Equivalently: ∀L ∈ NP, L ≤_p SAT.

## Proof Construction (Tableau Method)

### 1. NTM as Tableau
Given NTM M deciding L in time n^k, on input w (|w|=n):
- Computation uses ≤ n^k tape cells
- Tableau: (n^k+1) × (n^k+1) grid
- Row t = configuration at time t
- Cell (t,i) = symbol on tape at position i at time t

### 2. Boolean Variables
Let Σ = tape alphabet, Q = states. Encode using O(n^{2k}) boolean variables:
- **cell[t,i,s]**: at time t, position i contains symbol s ∈ Σ
- **head[t,i]**: at time t, head is at position i  
- **state[t,q]**: at time t, machine is in state q ∈ Q

### 3. Clause Groups (φ = φ_cell ∧ φ_start ∧ φ_accept ∧ φ_transition)

**φ_start** (initial configuration):
- state[0, q₀]
- head[0, 0]
- For i=1..n: cell[0, i, w_i]
- For i=n+1..n^k: cell[0, i, □] (blank)

**φ_cell** (well-formedness):  
- Exactly one symbol per cell per time: ∧_{t,i} (∨_{s} cell[t,i,s]) ∧ (∧_{s≠s'} ¬cell[t,i,s] ∨ ¬cell[t,i,s'])
- Exactly one state per time: ∧_{t} (∨_{q} state[t,q]) ∧ (∧_{q≠q'} ¬state[t,q] ∨ ¬state[t,q'])  
- Exactly one head position: ∧_{t} (∨_{i} head[t,i]) ∧ (∧_{i≠j} ¬head[t,i] ∨ ¬head[t,j])

**φ_transition** (legal moves):
For each time t, each position i, each (state q, symbol σ):
- cell[t,i,σ] ∧ head[t,i] ∧ state[t,q] → 
  ∨_{(q',σ',d) ∈ δ(q,σ)} (state[t+1,q'] ∧ cell[t+1,i,σ'] ∧ head[t+1,i+d])

**φ_accept**:
- ∨_{t=0..n^k} state[t, q_accept]

### 4. Correctness
1. If M accepts w in ≤ n^k steps → tableau yields satisfying assignment for φ
2. If φ satisfiable → assignment defines accepting tableau

### 5. Size Analysis
- Variables: O(poly(n)) = O(n^{3k}) 
- Clauses: O(poly(n)) = O(n^{3k})
- Reduction runs in polynomial time

## Implementation Note

Our `src/cook_levin.c` implements the reduction for a simplified 2-state NTM 
accepting strings with a specific pattern (demonstration). The tableau size 
grows polynomially with input length, proving the reduction is ≤_p.

## Reference
Cook, S.A. "The Complexity of Theorem-Proving Procedures." STOC 1971.
Sipser, M. "Introduction to the Theory of Computation." §7.4.
