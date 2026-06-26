# Baker-Gill-Solovay: Why We Can't Prove P≠NP (Yet)

**UT Austin CS388T · Baker-Gill-Solovay (SICOMP 1975)**

## The Question
Why, after 50+ years, have we not resolved P vs NP?

The answer, in one line: **all known proof techniques relativize, but P vs NP does not.**

## What is Relativization?
Adding an oracle to a Turing machine: the machine can query a black-box language A
in O(1) time. Define P^A and NP^A as the relativized complexity classes.

A proof technique "relativizes" if it would hold identically for any oracle A.

## Theorem (Baker-Gill-Solovay)
There exists oracle A with P^A = NP^A.  
There exists oracle B with P^B ≠ NP^B.

## Construction Sketch

**Oracle A where P^A = NP^A:**
Let A be PSPACE-complete. Then P^A = NP^A = PSPACE^A = PSPACE.

**Oracle B where P^B ≠ NP^B:**
Let B be a language constructed via diagonalization...
- Enumerate all polynomial-time oracle machines M₁, M₂, ...
- For each M_i running in time n^i, construct B so that L(B) = {1ⁿ | ∃y∈B, |y|=n} is in NP^B but not decided by M_i^B
- At stage i: choose a large n, if M_i^B(1ⁿ) accepts, make no string of length n be in B (so 1ⁿ ∉ L(B)); if M_i^B rejects, add some string of length n to B (so 1ⁿ ∈ L(B))
- The construction avoids breaking earlier stages by working at sufficiently large n

## Implications for P vs NP

| Technique | Relativizes? | Can prove P≠NP? |
|-----------|-------------|-----------------|
| Diagonalization | ✅ Yes | ❌ No (BGS) |
| Simulation | ✅ Yes | ❌ No |
| Combinatorial counting | ✅ Yes | ❌ No |
| Circuit lower bounds (AC⁰) | ❌ No! | Maybe... |
| Interactive proofs (IP=PSPACE) | ❌ No! | Maybe... |
| PCP theorem | ❌ No! | Maybe... |
| Algebrization barrier | — | Partial obstacle |

## The Path Forward
To prove P≠NP, we need a technique that does NOT relativize.
The most promising candidates:
1. Circuit lower bounds (proving explicit functions require superpolynomial circuits)
2. Algebraic methods (permanent vs determinant over finite fields)
3. Geometric complexity theory (representation theory approach)

## Our Implementation
Our SAT solver demonstrates the practical consequence: we have empirical evidence
that solving SAT takes exponential time, but we cannot prove it mathematically.
The Baker-Gill-Solovay theorem explains why this proof is so elusive.

## Reference
Baker, T., Gill, J., Solovay, R. "Relativizations of the P=?NP Question."
SIAM Journal on Computing 4(4):431-442, 1975.
