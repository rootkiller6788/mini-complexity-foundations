# mini-cook-levin-theorem — Knowledge Coverage Report

## Module Status: COMPLETE ✅

- L1-L6: Complete
- L7: Partial+ (3 applications: verifier framework, certificate checking, cryptographic OWF)
- L8: Partial+ (2 advanced topics: Baker-Gill-Solovay, padding arguments)
- L9: Partial (documented, not implemented)

---

## 九层知识覆盖

| Level | 名称 | 覆盖 | 条目 |
|-------|------|:----:|------|
| **L1** | Definitions | Complete | TuringMachine, NTM, CNF, Clause, Symbol, Transition, Circuit, Gate (23 typedef struct) |
| **L2** | Core Concepts | Complete | polynomial-time, nondeterminism, reduction, completeness, SAT, 3SAT |
| **L3** | Mathematical Structures | Complete | TM 7-tuple, CNF formula, Boolean circuit DAG, literal encoding |
| **L4** | Fundamental Theorems | Complete | Cook-Levin (full tableau + polynomial bound), Time Hierarchy |
| **L5** | Algorithms/Methods | Complete | DPLL SAT, CDCL (watched literals + VSIDS + clause learning), Levin universal search |
| **L6** | Canonical Problems | Complete | SAT, 3SAT, CLIQUE, VERTEX-COVER reductions; TM deciders (0^n 1^n, contains 11) |
| **L7** | Applications | Partial+ | SAT verifier, certificate checking, one-way function from SAT hardness |
| **L8** | Advanced Topics | Partial+ | Baker-Gill-Solovay oracles, padding arguments, PCP/natural proofs (documented) |
| **L9** | Research Frontiers | Partial | Documented (meta-complexity, GCT, quantum) |

---

## 核心定义

| 定义 | 类型 | 文件 |
|------|------|------|
| TuringMachine | typedef struct | include/turing_machine.h |
| TMConfiguration | typedef struct | include/turing_machine.h |
| TMTransition | typedef struct | include/turing_machine.h |
| Clause | typedef struct | include/sat.h |
| CNF Formula | typedef struct | include/sat.h |
| CLVariableScheme | typedef struct | include/cook_levin.h |
| Graph (CLIQUE) | typedef struct | include/cook_levin.h |
| VCGraph (VERTEX-COVER) | typedef struct | include/cook_levin.h |
| Reduction | typedef struct | include/reduction.h |
| NPCompleteCertificate | typedef struct | include/reduction.h |
| ComplexityClass | typedef enum | include/complexity.h |
| Certificate | typedef struct | include/complexity.h |

---

## 核心定理

| 定理 | 实现 | 来源 |
|------|:----:|------|
| **Cook-Levin**: SAT is NP-complete | src/cook_levin.c, src/tableau.c | Cook (STOC 1971), Levin (1973) |
| **Tseitin Encoding**: Circuit → CNF | src/cook_levin.c | Tseitin (1968) |
| **DPLL Completeness**: DPLL decides SAT | src/dpll.c | Davis, Putnam (1960); Davis, Logemann, Loveland (1962) |
| **CDCL Soundness**: CDCL learns valid clauses | src/cdcl.c | Marques-Silva, Sakallah (1996) |
| **Levin Universal Search** | src/levin_search.c | Levin (1973) |
| **NP-completeness reductions**: 3SAT→CLIQUE, SAT→3SAT | src/reductions.c | Karp (1972) |
| **Padding argument** | src/padding_argument.c | Hierarchy theorems |
| **Non-relativizing proofs** | src/non_relativizing.c | Baker-Gill-Solovay (1975) |

---

## 核心算法

| 算法 | 复杂度 | 文件 |
|------|--------|------|
| DPLL SAT | O(2^n) worst | src/dpll.c |
| CDCL SAT | O(2^n) worst, practical polynomial | src/cdcl.c |
| Unit Propagation | O(n_clauses × n_vars) | src/dpll.c |
| Pure Literal Elimination | O(n_clauses × n_vars) | src/dpll.c |
| Cook-Levin Reduction | O(T³) polynomial | src/cook_levin.c |
| 3SAT→CLIQUE Reduction | O(n³) | src/reductions.c |
| Levin Universal Search | O(2^n / poly(n)) | src/levin_search.c |
| SAT Verifier | O(n_clauses × n_vars) | src/verifier.c |

---

## 经典问题

| 问题 | 示例文件 |
|------|---------|
| SAT solving with DPLL | examples/example_dpll_solver.c |
| Cook-Levin reduction walkthrough | examples/example_cook_levin_reduction.c |
| NP-completeness reduction chain | examples/example_reduction_chain.c |
| NP verifier framework | examples/example_verifier_framework.c |

---

## 九校课程映射

| 学校 | 课程 | 本文档对应 |
|------|------|-----------|
| MIT | 6.045 Automata, Computability, Complexity | L1-L6 全覆盖 |
| MIT | 6.841 Advanced Complexity | L7-L8 |
| Stanford | CS254 Computational Complexity | L1-L6 |
| Berkeley | CS172/278 Complexity Theory | L1-L5 |
| CMU | 15-455/855 Complexity | L1-L6 |
| Princeton | COS 522 Complexity | L1-L5 |
| Cambridge | Part II Complexity | L1-L5 |
| Oxford | Computational Complexity | L1-L5 |
| ETH | 263-4650 Advanced Complexity | L4-L8 |

---

## 构建

```bash
make          # 编译静态库 libcooklevin.a
make test     # 运行测试
make clean    # 清理
```

## 统计

- `include/` + `src/`: 5,992 行 (5 headers + 14 source files)
- 公共 API: 70+ 导出函数
- 示例: 4 个端到端可运行示例
- 文档: 12 个 docs/*.md (含 5 个 SKILL.md 必需知识文档)
- 测试: 1 个综合测试文件 (30 个测试场景)
- `make` 编译通过，0 errors, 1 warning (pre-existing doc comment)


---

*金标准模板 — 所有其他 mini-theory-of-computation 子模块的参考实现*
