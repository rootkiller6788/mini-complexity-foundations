# Course Tree — mini-p-np-np-completeness

## Prerequisite Dependency Tree

This module depends on concepts from the following modules and topics.
Arrows indicate "must understand X before Y."

```
                    ┌─────────────────────────┐
                    │  mini-p-np-np-completeness │
                    └────────────┬────────────┘
                                 │
           ┌─────────────────────┼─────────────────────┐
           │                     │                     │
    ┌──────▼──────┐      ┌──────▼──────┐      ┌──────▼──────┐
    │ Cook-Levin  │      │  Reductions │      │ SAT Solvers │
    │   Theorem   │      │  (Karp 72)  │      │ (DPLL/CDCL) │
    └──────┬──────┘      └──────┬──────┘      └──────┬──────┘
           │                    │                    │
    ┌──────▼──────┐      ┌──────▼──────┐      ┌──────▼──────┐
    │   Turing    │      │   Problem   │      │   Search &  │
    │  Machines   │      │  Encoding   │      │ Backtracking│
    └─────────────┘      └─────────────┘      └─────────────┘
```

## Internal Dependency Graph

```
types.h/types.c (CNF data structure)
    ├── sat.h/sat.c (SAT solver interface)
    │   ├── dpll.h/dpll.c (DPLL backtracking)
    │   ├── cdcl.h/cdcl.c (CDCL with learning)
    │   └── stochastic.h/stochastic.c (WalkSAT, GSAT, SA)
    ├── cook_levin.h/cook_levin.c (Cook-Levin theorem)
    ├── reduction.h/reduction.c (NP reductions)
    ├── resolution.h/resolution.c (Resolution proof system)
    │   └── resolution_tree.c (DAG proofs)
    ├── circuit.h/circuit.c (Boolean circuits)
    ├── heuristics.h/heuristics.c (Branching heuristics)
    ├── horn_sat.h/horn_sat.c (Horn SAT, P-complete)
    ├── two_sat.h/two_sat.c (2-SAT, O(V+E))
    ├── dimacs.h/dimacs.c (DIMACS I/O)
    ├── np_catalog.h/np_catalog.c (Problem catalog)
    ├── graph_np.h/graph_np.c (NP graph problems)
    ├── phase_analysis.h/phase_analysis.c (Phase transition)
    ├── time_classes.h/time_classes.c (Complexity classes)
    ├── padding.c (Padding arguments)
    └── formalization.lean (Lean 4 formalization)
```

## Concept Dependency (What to know before this module)

### Required Prerequisites

| Concept | Where to Learn | Why Needed |
|---------|---------------|------------|
| Big-O notation | Any algorithms course | To define polynomial time |
| Turing Machines | Sipser Ch.3, AB Ch.1 | Cook-Levin tableau construction |
| Boolean logic (AND/OR/NOT) | Digital logic basics | CNF formulas, circuits |
| Graph theory basics | Discrete math | NP-complete graph problems |
| Basic C programming | Any | Implementation language |
| Proof by induction | Discrete math | Lean formalization |

### Recommended Co-requisites

| Concept | Relationship |
|---------|-------------|
| First-order logic | Resolution proof system |
| Linear algebra | Circuit complexity (matrix representations) |
| Probability theory | Stochastic SAT solvers, phase transitions |
| Algorithm design | DPLL/CDCL are sophisticated backtracking |

## Downstream Modules (What depends on this module)

```
mini-p-np-np-completeness
    ├── mini-time-hierarchy-theorem
    ├── mini-space-hierarchy
    ├── mini-ladner-theorem
    ├── mini-polynomial-hierarchy
    ├── mini-pspace-npspace
    ├── mini-reductions-completeness
    ├── mini-circuit-complexity/
    │   ├── mini-boolean-circuits-model
    │   ├── mini-circuit-sat-complexity
    │   └── mini-ac0-nc-poly-hierarchy
    └── mini-algorithmic-complexity/
        ├── mini-approximation-algorithms
        ├── mini-pcp-theorem
        └── mini-ip-pspace
```
