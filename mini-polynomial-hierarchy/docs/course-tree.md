# Course Tree: Polynomial Hierarchy

## Prerequisites
```
mini-p-np-np-completeness
    P, NP, coNP definitions
    SAT, 3SAT
    NP-completeness
    Polynomial-time reductions
        ↓
mini-polynomial-hierarchy  ← YOU ARE HERE
    Sigma_k, Pi_k, PH
    Sigma_k-SAT completeness
    Oracle characterization
    Collapse theorems
    Karp-Lipton theorem
    Toda theorem
    SGL theorem
    CKS characterization
    IP = PSPACE
        ↓
mini-circuit-complexity (future)
    Boolean circuits
    AC0, NC, P/poly
    Circuit lower bounds
    Natural proofs barrier
```

## Internal Dependencies
```
ph.h (types + API)
  ↓
ph_core.c (CNF ops, DPLL, circuits)
  ↓
ph_qbf_solver.c (Sigma_k, Pi_k, TQBF)
  ↓
ph_toda.c + ph_oracle.c + ph_bpp.c
  ↓
ph_atm.c + ph_ip_pspace.c
  ↓
ph_reductions.c + ph_apps.c
  ↓
ph_demo.c + ph_bench.c + ph_advanced.c
```

## Knowledge Dependency Graph
```
L1 (Definitions) → L2 (Concepts) → L3 (Structures)
                                      ↓
                                 L4 (Theorems)
                                      ↓
                    L5 (Algorithms) ←┘
                          ↓
                    L6 (Problems)
                          ↓
          L7 (Applications) → L8 (Advanced) → L9 (Research)
```
