# Ladner's Theorem

**Ladner (JACM 1975) · MIT 6.840 §5 · UT Austin CS388T**

## Theorem
If P ≠ NP, there exist languages in NP that are:
- NOT in P (not polynomial-time decidable)
- NOT NP-complete (not hardest in NP)

## Construction
L = {x in SAT | f(|x|) is even} where f grows very slowly.
- f unbounded → L differs from every P-machine → L ∉ P
- f grows slowly → L is sparse → L not NP-complete (by Mahaney)

## Related Results
- Mahaney (1982): no sparse NP-complete set unless P=NP
- Berman-Hartmanis (1977): all NPC sets are p-isomorphic?
- Valiant-Vazirani (1986): Unique-SAT randomized reduction