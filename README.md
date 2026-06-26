# Mini Complexity Foundations

A collection of **from-scratch, zero-dependency C implementations** of computational complexity theory. Each module maps to MIT (and other top-tier university) courses, bridging theory and practice by translating fundamental complexity results—hierarchy theorems, NP-completeness, polynomial hierarchy, and PSPACE—into runnable C code.

## Sub-Modules

| Sub-Module | Topics | Key Courses |
|-----------|--------|-------------|
| [mini-cook-levin-theorem](mini-cook-levin-theorem/) | Cook-Levin theorem, SAT is NP-complete, tableau construction, NTM → CNF reduction | MIT 6.045J/6.841, Stanford CS254 |
| [mini-ladner-theorem](mini-ladner-theorem/) | NP-intermediate languages, delayed diagonalization, slow-growing constructive functions | MIT 6.841, Stanford CS254 |
| [mini-p-np-np-completeness](mini-p-np-np-completeness/) | P vs NP, DPLL/CDCL SAT solvers, Boolean circuits, Karp's 21 NP-complete reductions, resolution, phase transitions | MIT 6.045J/6.841, Berkeley CS172 |
| [mini-polynomial-hierarchy](mini-polynomial-hierarchy/) | Σ_k^p / Π_k^p classes, oracle machines, PH collapse, Karp-Lipton theorem, Toda's theorem, BPP bounds | MIT 6.841, Stanford CS254 |
| [mini-pspace-npspace](mini-pspace-npspace/) | PSPACE & NPSPACE, Savitch's theorem, TQBF completeness, AP=PSPACE, Immerman-Szelepcsényi (NL=coNL) | MIT 6.841, Berkeley CS172 |
| [mini-reductions-completeness](mini-reductions-completeness/) | Karp/Cook/Turing/logspace reductions, gadget constructions, reduction verification & metrics | MIT 6.045J, CMU 15-855 |
| [mini-space-hierarchy](mini-space-hierarchy/) | SPACE(f) ⊊ SPACE(g) diagonalization, reachability, configuration graphs, L ⊊ PSPACE separation | MIT 6.841, Berkeley CS172 |
| [mini-time-hierarchy-theorem](mini-time-hierarchy-theorem/) | TIME(f) ⊊ TIME(g) diagonalization, diagonal language L_D, time-constructible functions, gap theorem | MIT 6.045J/6.841, Stanford CS254 |

## Design Philosophy

- **Zero external dependencies** — pure C (C99/C11), only `libc` and `libm`
- **Self-contained modules** — each directory has its own `Makefile`, `include/`, `src/`, `examples/`, `demos/`, `tests/`
- **Theory-to-code mapping** — every module includes `docs/` with course-alignment notes
- **Proof-as-code** — hierarchy separations, completeness reductions, and oracle constructions are executable and verifiable

## Building

Each module is standalone. Navigate to a module directory and run:

```bash
cd mini-cook-levin-theorem
make all    # build everything
make test   # run tests
```

Requires **GCC** and **GNU Make**.

## Project Structure

```
mini-complexity-foundations/
├── mini-cook-levin-theorem/       # Cook-Levin theorem, SAT NP-completeness proof
├── mini-ladner-theorem/           # Ladner's theorem, NP-intermediate languages
├── mini-p-np-np-completeness/     # P vs NP, SAT solvers, Karp reductions
├── mini-polynomial-hierarchy/     # Polynomial hierarchy, oracle machines, Karp-Lipton
├── mini-pspace-npspace/           # PSPACE, Savitch, TQBF, Immerman-Szelepcsényi
├── mini-reductions-completeness/  # Reduction taxonomy, gadgets, verification
├── mini-space-hierarchy/          # Space hierarchy, diagonalization, reachability
└── mini-time-hierarchy-theorem/   # Time hierarchy, diagonal language, constructibility
```

## License

MIT
