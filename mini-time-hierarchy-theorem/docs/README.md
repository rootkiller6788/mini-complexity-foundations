# Time Hierarchy Theorem

**MIT 6.840 §3 · AB §3 · Sipser §9 · Hartmanis-Stearns (1965)**

TIME(f)!=TIME(g) when f·log f = o(g) and f,g time-constructible.

## Proof (Diagonalization)
1. Enumerate TMs: M_1, M_2, ...
2. Universal TM U simulates M_i on x with O(T log T) overhead
3. Define L_D = {n | U(n,n) rejects within g(n) steps}
4. L_D in TIME(g), L_D not in TIME(f) (by contradiction)
5. If M_i decides L_D in f(n) time: M_i(i)=L_D(i)=!M_i(i) → contradiction

## Key Results
- P subsetneq EXP (TIME(n^k) subsetneq TIME(2^n))
- EXPTIME subsetneq 2-EXPTIME
- Can separate any two "sufficiently far apart" functions

## Limitations
- Proof relativizes → cannot resolve P vs NP (Baker-Gill-Solovay 1975)
- Need circuit lower bounds or non-relativizing techniques
