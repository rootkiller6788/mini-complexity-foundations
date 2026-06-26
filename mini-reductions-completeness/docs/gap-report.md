# Gap Report: mini-reductions-completeness

## Missing Knowledge Items

### L4: Additional Theorems (Priority: MEDIUM)
- Ladner's Theorem (NP-intermediate if P!=NP) — connection only, full proof belongs in mini-ladner-theorem
- Savitch's Theorem (PSPACE = NPSPACE) — mentioned in completeness hierarchy

### L5: Additional Algorithms (Priority: LOW)
- Full gadget builder implementation (gadget_reductions.h declared, not compiled)
- CLIQUE -> INDEPENDENT SET (trivial, in example)
- 3SAT -> 3DM reduction (complex, could be future work)

### L7: Applications (Priority: HIGH)
- Only 2 applications touched; need concrete application code
- SAT-based planning: encode STRIPS planning as SAT
- Formal verification: bounded model checking via SAT
- Crypto: one-way function from average-case NPC

### L8: Advanced Topics (Priority: MEDIUM)
- PCP Theorem connection (belongs in mini-pcp-inapproximability)
- Fine-grained SETH-based reductions
- Hardness of approximation via gap reductions
- Interactive proofs (IP = PSPACE)

### L9: Research Frontiers (Priority: LOW)
- MCSP (Minimum Circuit Size Problem)
- GCT (Geometric Complexity Theory)
- Meta-complexity research program

## Line Count Status
Current: 4183 lines (include/ + src/)  ✓
Required: >= 3000 lines  ✓

## Recommendations
1. Add one concrete L7 application (SAT-based planning) - 200 lines
2. Expand gadget_reductions.h implementation - 150 lines
3. These would push score to 16/18 = COMPLETE

## Dependency Gaps
- Ladner's theorem: full in mini-ladner-theorem ✓
- Cook-Levin details: full in mini-cook-levin-theorem ✓
- PCP theorem: will be in mini-pcp-inapproximability ✓
