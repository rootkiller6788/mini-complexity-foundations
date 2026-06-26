# mini-p-np-np-completeness

P vs NP 问题与 NP 完全性：从 SAT 求解器到 Cook-Levin 定理的完整实现。

## 核心概念

### P vs NP
- **P**: 多项式时间可解 (Polynomial time)
- **NP**: 多项式时间可验证 (Nondeterministic Polynomial time)
- **P vs NP 问题**: P = NP?
- **悬赏**: Clay Millennium Prize ($1M)

### NP 完全性
一个问题 L 是 NP-完全的 (NP-Complete) 当:
1. L ∈ NP (存在多项式时间验证器)
2. ∀L′ ∈ NP, L′ ≤_p L (每个 NP 问题可多项式归约到 L)

### Cook-Levin 定理 (1971)
SAT (布尔可满足性问题) 是 NP-完全的。

证明思路:
1. 给定任意 NP 语言 L，存在多项式时间 NTM M 判定 L
2. 对 M 在输入 x 上的计算，构造布尔公式 φ
3. 变量编码计算配置: cell[t,p,s], head[t,p], state[t,q]
4. 公式 φ 可满足 ⟺ M 接受 x

### Karp 的 21 个 NP 完全问题 (1972)
Richard Karp 通过多项式归约证明了 21 个问题的 NP 完全性:
SAT → 3SAT → CLIQUE → VERTEX COVER → HAMILTONIAN PATH → ...

## 实现

### SAT 求解器
- **DPLL** (Davis-Putnam-Logemann-Loveland, 1962): 回溯 + 单元传播 + 纯文字消除
- **CDCL** (Conflict-Driven Clause Learning, 1996): 现代求解器核心
  - VSIDS 启发式 (Variable State Independent Decaying Sum)
  - 学习子句 (Learned Clauses)
  - 回跳 (Backjumping)
  - 重启 (Restarts)

### 复杂度分析
- 求解: O(2^n) — 指数时间 (假设 P≠NP)
- 验证: O(n·m) — 多项式时间
- 计数 (#SAT): #P-完全

## 文件结构

```
include/    API headers (types, sat, dpll, cdcl, dimacs, reduction, heuristics)
src/        Implementation (sat, dpll, cdcl, types, dimacs, reduction, heuristics)
examples/   CLI solver, phase transition, reduction demo
demos/      DPLL trace visualization
tests/      Unit tests, DIMACS tests, reduction tests, fuzz tests
docs/       Algorithm documentation, NP-completeness guide
```

## 参考

- Cook, S.A. "The Complexity of Theorem-Proving Procedures" (STOC 1971)
- Karp, R.M. "Reducibility Among Combinatorial Problems" (1972)
- Davis, M., Logemann, G., Loveland, D. "A Machine Program for Theorem-Proving" (CACM 1962)
- Moskewicz, M.W. et al. "Chaff: Engineering an Efficient SAT Solver" (DAC 2001)
