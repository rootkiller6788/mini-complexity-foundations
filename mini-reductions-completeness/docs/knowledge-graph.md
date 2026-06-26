# Knowledge Graph: Reductions & NP-Completeness

## L1: Definitions
- Polynomial-time reduction (<=_p)
- Many-one / Karp reduction (<=_m)
- Turing / Cook reduction (<=_T)
- Logspace reduction (<=_L)
- Truth-table reduction (<=_tt)
- NP-hardness
- NP-completeness
- C-completeness for class C
- Karp's 21 problems
- Gadget (reduction building block)
- Levin reduction (witness-preserving)
- Parsimonious reduction

## L2: Core Concepts
- Reduction transitivity
- Closure under reductions
- Completeness notion
- Self-reducibility of SAT
- Witness-preserving reductions
- Reduction chain / DAG
- NP membership verification
- Hardness propagation
- Reduction composition
- Completeness beyond NP

## L3: Mathematical Structures
- Reduction as function on strings/languages
- Graph gadgets (variable, clause, diamond)
- Reduction chains as directed graphs
- CNF formulas, 3-CNF, literal encoding
- Graph representations of formulas
- Subset sum numerical encoding
- Circuit DAG structure (CVP)
- Quantified Boolean formulas (TQBF)

## L4: Fundamental Laws
- Cook-Levin Theorem (SAT is NP-complete)
- Karp's Theorem (21 problems NP-complete)
- Closure: P,NP,coNP,PSPACE,EXP under <=_p
- NP != coNP => <=_m != <=_T
- SAT is self-reducible
- NL = coNL (Immerman-Szelepcsenyi)
- CVP is P-complete under <=_L (Ladner)
- TQBF is PSPACE-complete (Stockmeyer-Meyer)

## L5: Algorithms/Methods
- SAT -> 3SAT reduction (auxiliary variables)
- 3SAT -> CLIQUE reduction
- 3SAT -> VERTEX COVER reduction
- 3SAT -> HAMILTONIAN CYCLE reduction
- 3SAT -> SUBSET SUM reduction
- 3SAT -> 3-COLORING reduction
- CLIQUE -> VERTEX COVER (complement)
- Gadget construction methodology
- Reduction composition algorithm
- Logspace reduction simulation

## L6: Canonical Problems
- SAT, 3SAT
- CLIQUE, INDEPENDENT SET
- VERTEX COVER, SET COVER
- HAMILTONIAN CYCLE, TSP
- SUBSET SUM, KNAPSACK, PARTITION
- 3-COLORING
- 3-DIMENSIONAL MATCHING
- Circuit Value Problem (CVP)
- ST-CONN (graph reachability)
- TQBF

## L7: Applications
- Formal verification via SAT reductions
- Planning as SAT
- Constraint satisfaction (3-COL -> CSPs)
- Approximation hardness via gap reductions
- Cryptography: OWF from NPC assumptions

## L8: Advanced Topics
- PCP Theorem connection to hardness of approx
- Fine-grained reductions
- Immerman-Szelepcsenyi (NL = coNL)
- Polynomial Hierarchy completeness
- Schaefer's dichotomy theorem
- Natural proofs barrier

## L9: Research Frontiers
- Meta-complexity and MCSP
- GCT (Geometric Complexity Theory)
- Quantum NP-completeness
- Fine-grained complexity (SETH-based)
- Hardness of approximation tight bounds
