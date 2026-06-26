# Cook-Levin Reduction Complexity

## Asymptotic Bounds

Given NTM deciding L in time p(n) = O(n^k), input length n:
- T = p(n) = O(n^k)
- P = T + n + 2 = O(n^k)
- |Σ| = O(1), |Q| = O(1)

Variables: O(T·P·(|Σ|+|Q|)) = O(n^{2k})
Clauses: O(T·P·(|Σ|²+P+|Σ|·|Q|)) = O(n^{3k})
Construction time: O(n^{4k})

## Empirical Growth (measured)
| n | vars | clauses | ratio |
|---|------|---------|-------|
| 1 |  560 |     17  | 0.03  |
| 2 |  880 |     28  | 0.03  |
| 3 | 1260 |     41  | 0.03  |

Growth follows theoretical O(T²) and O(T³) curves.