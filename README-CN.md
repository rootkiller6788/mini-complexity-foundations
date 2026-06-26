# Mini Complexity Foundations（迷你复杂度基础）

**从零开始、零依赖的 C 语言实现**，涵盖计算复杂度理论的核心成果。每个模块对应 MIT（及其他顶尖大学）的一门或多门课程，将层次定理、NP 完全性、多项式谱系和 PSPACE 等基础理论转化为可运行的 C 代码，实现理论与实践的桥接。

## 子模块总览

| 子模块 | 主题 | 参考课程 |
|-----------|--------|-------------|
| [mini-cook-levin-theorem](mini-cook-levin-theorem/) | Cook-Levin 定理、SAT 是 NP 完全的、画面法构造、NTM → CNF 归约 | MIT 6.045J/6.841, Stanford CS254 |
| [mini-ladner-theorem](mini-ladner-theorem/) | NP 中间语言、延迟对角化、缓增长可构造函数 | MIT 6.841, Stanford CS254 |
| [mini-p-np-np-completeness](mini-p-np-np-completeness/) | P vs NP、DPLL/CDCL SAT 求解器、布尔电路、Karp 21 个 NP 完全问题归约、归结证明、相变分析 | MIT 6.045J/6.841, Berkeley CS172 |
| [mini-polynomial-hierarchy](mini-polynomial-hierarchy/) | Σ_k^p / Π_k^p 类、谕示机（oracle machines）、PH 崩塌、Karp-Lipton 定理、户田定理、BPP 上界 | MIT 6.841, Stanford CS254 |
| [mini-pspace-npspace](mini-pspace-npspace/) | PSPACE 与 NPSPACE、Savitch 定理、TQBF 完全性、AP=PSPACE、Immerman-Szelepcsényi 定理（NL=coNL） | MIT 6.841, Berkeley CS172 |
| [mini-reductions-completeness](mini-reductions-completeness/) | Karp/Cook/Turing/logspace 归约、小工具（gadget）构造、归约验证与度量 | MIT 6.045J, CMU 15-855 |
| [mini-space-hierarchy](mini-space-hierarchy/) | SPACE(f) ⊊ SPACE(g) 对角化、可达性问题、配置图、L ⊊ PSPACE 分离 | MIT 6.841, Berkeley CS172 |
| [mini-time-hierarchy-theorem](mini-time-hierarchy-theorem/) | TIME(f) ⊊ TIME(g) 对角化、对角线语言 L_D、时间可构造函数、间隙定理 | MIT 6.045J/6.841, Stanford CS254 |

## 设计理念

- **零外部依赖** — 纯 C（C99/C11），仅使用 `libc` 和 `libm`
- **模块自包含** — 每个目录自带 `Makefile`、`include/`、`src/`、`examples/`、`demos/`、`tests/`
- **理论到代码的映射** — 每个模块包含 `docs/` 目录，内有课程对齐说明
- **证明即代码** — 层次分离、完全性归约和谕示机构造均为可执行、可验证的代码实现

## 构建方式

每个模块相互独立。进入模块目录后运行：

```bash
cd mini-cook-levin-theorem
make all    # 构建全部
make test   # 运行测试
```

需要 **GCC** 和 **GNU Make**。

## 项目结构

```
mini-complexity-foundations/
├── mini-cook-levin-theorem/       # Cook-Levin 定理，SAT NP 完全性证明
├── mini-ladner-theorem/           # Ladner 定理，NP 中间语言
├── mini-p-np-np-completeness/     # P vs NP，SAT 求解器，Karp 归约
├── mini-polynomial-hierarchy/     # 多项式谱系，谕示机，Karp-Lipton 定理
├── mini-pspace-npspace/           # PSPACE，Savitch 定理，TQBF，Immerman-Szelepcsényi
├── mini-reductions-completeness/  # 归约分类学，小工具，归约验证
├── mini-space-hierarchy/          # 空间层次定理，对角化，可达性
└── mini-time-hierarchy-theorem/   # 时间层次定理，对角线语言，可构造性
```

## 许可证

MIT
