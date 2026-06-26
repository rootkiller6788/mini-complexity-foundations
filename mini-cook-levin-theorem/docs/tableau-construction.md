# Cook-Levin Tableau Construction

## Variable Encoding

For computation of length T, tape size P = T + |w| + 2:

```
cell[t][p][s]:  tape[p] contains symbol s at time t
head[t][p]:     head at position p at time t  
state[t][q]:    machine in state q at time t
```

## Clause Groups

### 1. Start Configuration (O(P) clauses)
- state[0, q0] = 1
- head[0, |w|/2] = 1
- cell[0, i, w_i] = 1 for each input position

### 2. Cell Uniqueness (O(T·P·|Σ|²) clauses)
- At most one symbol per cell per time
- ∨_s cell[t,p,s] (at least one)
- ¬cell[t,p,s] ∨ ¬cell[t,p,s'] for s≠s' (at most one)

### 3. Head Uniqueness (O(T·P²) clauses)  
- At most one head position per time

### 4. Transition (O(T·P·|Σ|·|Q|) clauses)
- If state=q, head at p, cell[p]=s, then next config follows δ(q,s)

### 5. Accept (1 clause)
- ∨_t state[t, q_accept]

## Size Analysis
Variables: O(T·P·max(|Σ|,|Q|)) = O(T²)
Clauses: O(T·P·(|Σ|²+P+|Σ|·|Q|)) = O(T³)
Reduction time: O(T⁴)