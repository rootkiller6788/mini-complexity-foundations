# Knowledge Graph: Polynomial Hierarchy

## L1: Definitions (Complete)

| ID | Term | Definition | Code Location |
|----|------|-----------|---------------|
| L1.1 | PH_CNF | 3-CNF formula with literal encoding | include/ph.h:PH_CNF |
| L1.2 | PH_Clause | Clause of 3 literals | include/ph.h:PH_Clause |
| L1.3 | PH_QBF | Quantified Boolean Formula | include/ph.h:PH_QBF |
| L1.4 | PH_QuantBlock | Quantifier block (type + variables) | include/ph.h:PH_QuantBlock |
| L1.5 | PH_QuantType | EXISTS / FORALL quantifier | include/ph.h:PH_QuantType |
| L1.6 | PH_BoolCircuit | Boolean circuit (DAG of gates) | include/ph.h:PH_BoolCircuit |
| L1.7 | PH_Gate | Circuit gate (AND/OR/NOT/INPUT/CONST) | include/ph.h:PH_Gate |
| L1.8 | PH_OracleMachine | NP oracle machine | include/ph.h:PH_OracleMachine |
| L1.9 | PH_ATMConfig | Alternating TM configuration | include/ph.h:PH_ATMConfig |
| L1.10 | PH_ATMState | ATM state type (EXISTS/FORALL/ACCEPT/REJECT) | include/ph.h:PH_ATMState |

## L2: Core Concepts (Complete)

| ID | Concept | Description | Implementation |
|----|---------|-------------|----------------|
| L2.1 | Quantifier Alternation | ∃∀∃... pattern defines PH levels | src/ph_qbf_solver.c |
| L2.2 | Oracle Hierarchy | Σ_{k+1} = NP^{Σ_k} | src/ph_oracle.c |
| L2.3 | PH Collapse | Σ_k = Π_k ⇒ PH = Σ_k | src/ph_core.c:ph_collapse_check |
| L2.4 | Alternating Computation | ATM with EXISTS/FORALL states | src/ph_atm.c |
| L2.5 | Polynomial Simulation | BPP amplification in poly time | src/ph_bpp.c |
| L2.6 | Counting Complexity | #P characterizes PH power | src/ph_toda.c |
| L2.7 | Non-uniform Computation | P/poly and circuit advice | src/ph_core.c (circuits) |

## L3: Mathematical Structures (Complete)

| ID | Structure | Description | Implementation |
|----|-----------|-------------|----------------|
| L3.1 | CNF Formula | Conjunctive Normal Form over {0,1}^n | ph_cnf_* functions |
| L3.2 | QBF Structure | Quantifier prefix + matrix formula | PH_QBF struct |
| L3.3 | Boolean Circuit DAG | Directed acyclic graph of logic gates | PH_BoolCircuit |
| L3.4 | Literal Encoding | (var << 1) | sign — space-efficient | ph_cnf_eval |
| L3.5 | Assignment Vector | Truth assignment as int array | Used throughout |
| L3.6 | Quantifier Block | Contiguous variables with same quantifier | PH_QuantBlock |

## L4: Fundamental Theorems (Complete)

| ID | Theorem | Statement | Year | Implementation |
|----|---------|-----------|------|----------------|
| L4.1 | Sigma_k-Completeness | Σ_k-SAT is Σ_k^p-complete | 1976 | ph_sigma_k_sat |
| L4.2 | PH ⊆ PSPACE | TQBF solves all PH levels | 1973 | ph_tqbf_solve |
| L4.3 | Collapse Theorem | Σ_k=Π_k ⇒ PH collapses | — | ph_collapse_check |
| L4.4 | Karp-Lipton | NP⊆P/poly ⇒ PH=Σ₂ | 1982 | ph_circuit_eval |
| L4.5 | Toda's Theorem | PH ⊆ P^{#P} | 1991 | ph_sigma2_via_counting |
| L4.6 | SGL Theorem | BPP ⊆ Σ₂ ∩ Π₂ | 1983 | ph_bpp_to_sigma2 |
| L4.7 | CKS Characterization | Σ_k = ATIME(poly, k alts) | 1981 | ph_atm_simulate |
| L4.8 | IP = PSPACE | Interactive proofs = PSPACE | 1992 | ph_ip_verify_tqbf |

## L5: Algorithms (Complete)

| ID | Algorithm | Description | Complexity |
|----|-----------|-------------|------------|
| L5.1 | DPLL SAT | Davis-Putnam-Logemann-Loveland | Exp worst-case |
| L5.2 | Recursive QBF | Quantifier elimination by enumeration | O(2^n), O(n) space |
| L5.3 | #SAT Counting | Brute-force solution counter | O(2^n) |
| L5.4 | SAT→CLIQUE | Karp reduction with literal graph | O(n³) |
| L5.5 | SAT→VERTEX-COVER | Clause-triangle gadget reduction | O(n³) |
| L5.6 | SAT→3-COLOR | Base triangle + variable/clause gadgets | O(n+m) |
| L5.7 | SAT→SUBSET-SUM | Decimal-encoding reduction | O(n+m) |
| L5.8 | SAT→HAM-PATH | Diamond-gadget reduction | O(n+m) |
| L5.9 | BPP Amplification | Chernoff-bound majority voting | O(trials) |

## L6: Canonical Problems (Complete)

| ID | Problem | Complexity | Implementation |
|----|---------|------------|----------------|
| L6.1 | QSAT_k | Σ_k-complete | ph_qsat_benchmark |
| L6.2 | TQBF | PSPACE-complete | ph_tqbf_solve |
| L6.3 | Circuit Value | P-complete | ph_circuit_value |
| L6.4 | SAT/3SAT | NP-complete | ph_dpll_sat |
| L6.5 | CLIQUE | NP-complete | ph_sat_to_clique |
| L6.6 | VERTEX-COVER | NP-complete | ph_sat_to_vertexcover |
| L6.7 | 3-COLOR | NP-complete | ph_sat_to_3color |

## L7: Applications (Complete — 3 applications)

| ID | Application | Description | Implementation |
|----|-------------|-------------|----------------|
| L7.1 | Bounded Model Checking | State reachability as QBF | ph_bounded_model_check |
| L7.2 | Game Solving | 2-player perfect-information game as QBF | ph_game_solver |
| L7.3 | Circuit Minimization | Is there a smaller equivalent circuit? (Σ₂) | ph_circuit_minimization_check |

## L8: Advanced Topics (Partial+ — 3 topics)

| ID | Topic | Description | Implementation |
|----|-------|-------------|----------------|
| L8.1 | PCP Theorem | Probabilistically checkable proofs | ph_pcp_verify |
| L8.2 | Valiant-Vazirani | SAT to Unique-SAT reduction | ph_valiant_vazirani |
| L8.3 | IP = PSPACE | Interactive proofs for TQBF | ph_ip_verify_tqbf |

## L9: Research Frontiers (Partial+ — documented)

| ID | Frontier | Description | Location |
|----|----------|-------------|----------|
| L9.1 | PH infinite? | Is hierarchy strict at all levels? | ph_research_summary |
| L9.2 | Meta-Complexity | MIN-CIRCUIT is Σ₂-complete | ph_research_summary |
| L9.3 | GCT | Geometric approach to P vs NP | ph_research_summary |
| L9.4 | Quantum vs PH | BQP vs PH relationship | ph_research_summary |
| L9.5 | Hardness-Randomness | Derandomization from circuit lower bounds | ph_research_summary |
