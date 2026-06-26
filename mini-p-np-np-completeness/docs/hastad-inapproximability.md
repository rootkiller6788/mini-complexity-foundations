# Håstad's 3SAT Inapproximability

**Stanford CS254 Week 22 · AB §22 · Håstad (JACM 2001)**

## The Result
For any ε > 0, it is NP-hard to approximate MAX-3SAT within a factor of 7/8 + ε.
Equivalently: distinguishing between (1-ε)-satisfiable and (7/8+ε)-satisfiable
3CNF formulas is NP-hard.

## Why 7/8?
- Random assignment satisfies 7/8 of clauses in expectation
- Håstad proves you cannot do better than random, unless P=NP
- This is an **optimal inapproximability** result: the threshold is tight

## Proof Chain
```
Label Cover (NP-hard to approximate)
    ↓ (parallel repetition)
Gap Label Cover with large gap
    ↓ (long-code + Fourier analysis)
3SAT is NP-hard to approximate within 7/8+ε
```

## Key Techniques
1. **Long Code**: Encode an assignment a ∈ {0,1}^n as the truth table of a function f:{0,1}^n→{0,1} where f(x)=x_a
2. **Fourier Analysis over {0,1}^n**: Decompose boolean functions into Fourier coefficients, identify heavy characters
3. **Dictatorship Testing**: Verify f is close to a dictator (long code of some coordinate)
4. **Soundness via Friedgut's Theorem**: Functions far from dictators have influential variables

## Connection to PCP Theorem
- Håstad builds on the PCP Theorem (NP = PCP[O(log n), O(1)])
- His result shows the PCP query complexity cannot be reduced to 3 bits with
  better than 7/8 completeness (tight)
- Optimal PCP construction for MAX-3SAT

## Practical Implication
Our SAT solver cannot be "improved" to find solutions within 7/8+ε of optimal
in polynomial time, unless P=NP. The exponential behavior is not a failure of
our algorithm—it's a fundamental property of the problem.

## Reference
Håstad, J. "Some Optimal Inapproximability Results." JACM 48(4):798-859, 2001.
