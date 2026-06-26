#include "pspace.h"
#include <stdio.h>
#include <stdlib.h>

/* External demo function declarations (non-header prototypes) */
void alt_pspace_demo(void);
void logspace_reductions_demo(void);
void l_vs_nl_demo(void);
void pspace_complete_demo(void);
void pspace_summary_demo(void);
void pspace_vs_exp_demo(void);
void qbf_to_sat_demo(void);
void alternating_tm_demo(void);
void symbolic_qbf_demo(void);
void config_graph_demo(void);
void nl_reachability_demo(void);
void space_bounded_demo(void);
void pebble_game_demo(void);
void bf_game_demo(void);
void game_solver_demo(void);
void game_tree_demo(void);
void nl_algorithms_demo(void);
void graph_nl_demo(void);
void qbf_dpll_demo(void);
void qbf_preprocessor_demo(void);
void qbf_resolution_demo(void);
void qbf_certificate_demo(void);
void qbf_cegar_demo(void);
void qbf_optimization_demo(void);
void qdimacs_demo(void);
void connect4_demo(void);
void succinct_graphs_demo(void);
void model_checking_demo(void);
void space_benchmark(void);
void geography_full_demo(void);

/* New demo functions from added modules */
void savitch_full_demo(void);
void immerman_full_demo(void);
void space_hierarchy_demo(void);
void pspace_oracles_demo(void);

/* Assertion helper macros */
#define ASSERT_TRUE(cond, msg) do { \
    if (!(cond)) { printf("FAIL: %s (%s:%d)\n", msg, __FILE__, __LINE__); exit(1); } \
    else { printf("  PASS: %s\n", msg); } \
} while(0)

#define ASSERT_EQ_INT(expected, actual, msg) do { \
    if ((expected) != (actual)) { \
        printf("FAIL: %s — expected %d, got %d (%s:%d)\n", \
               msg, expected, actual, __FILE__, __LINE__); \
        exit(1); \
    } else { printf("  PASS: %s (got %d)\n", msg, actual); } \
} while(0)

int main(void) {
    setbuf(stdout, NULL);
    printf("\n===== PSPACE Full Test Suite =====\n\n");

    /* L2: Core Concepts */
    printf("--- L2: Core Concepts ---\n");
    space_complexity_summary();
    pspace_summary_demo();
    pspace_vs_exp_demo();
    printf("\n");

    /* L4: Fundamental Theorems */
    printf("--- L4: Fundamental Theorems ---\n");
    savitch_demo();
    immerman_demo();
    alt_pspace_demo();
    alternating_tm_demo();
    space_hierarchy_demo();
    printf("\n");

    /* L5: Algorithms — QBF */
    printf("--- L5: QBF Algorithms ---\n");
    qbf_demo();
    qbf_dpll_demo();
    qbf_preprocessor_demo();
    qbf_resolution_demo();
    qbf_certificate_demo();
    qbf_cegar_demo();
    qbf_optimization_demo();
    qbf_to_sat_demo();
    symbolic_qbf_demo();
    qdimacs_demo();
    printf("\n");

    /* L5: Algorithms — NL & Logspace */
    printf("--- L5: NL & Logspace ---\n");
    nl_reachability_demo();
    nl_algorithms_demo();
    graph_nl_demo();
    l_vs_nl_demo();
    logspace_reductions_demo();
    printf("\n");

    /* L5: Algorithms — Space & Config */
    printf("--- L5: Space Computation ---\n");
    space_bounded_demo();
    config_graph_demo();
    succinct_graphs_demo();
    printf("\n");

    /* L6: PSPACE-Complete Problems */
    printf("--- L6: PSPACE-Complete Problems ---\n");
    pspace_complete_demo();
    geography_demo();
    geography_full_demo();
    bf_game_demo();
    printf("\n");

    /* L6: PSPACE-Complete Games */
    printf("--- L6: PSPACE-Complete Games ---\n");
    game_solver_demo();
    game_tree_demo();
    checkers_demo();
    hex_demo();
    rush_hour_demo();
    sokoban_demo();
    sliding_puzzle_demo();
    connect4_demo();
    pebble_game_demo();
    printf("\n");

    /* L7: Applications */
    printf("--- L7: Applications ---\n");
    model_checking_demo();
    printf("\n");

    /* L8: Advanced */
    printf("--- L8: Advanced Topics ---\n");
    savitch_full_demo();
    immerman_full_demo();
    pspace_oracles_demo();
    space_benchmark();
    printf("\n");

    /* L1 Definitions — Typedef Verification */
    printf("--- L1: Definition Verification ---\n");
    ASSERT_TRUE(sizeof(ComplexityClass) > 0, "ComplexityClass enum defined");
    ASSERT_TRUE(sizeof(SpaceTM) > 0, "SpaceTM struct defined");
    ASSERT_TRUE(sizeof(TMConfiguration) > 0, "TMConfiguration struct defined");
    ASSERT_TRUE(sizeof(QBFInstance) > 0, "QBFInstance struct defined");
    ASSERT_TRUE(sizeof(ConfigGraph) > 0, "ConfigGraph struct defined");
    ASSERT_TRUE(sizeof(AlternatingTM) > 0, "AlternatingTM struct defined");
    ASSERT_TRUE(sizeof(Digraph) > 0, "Digraph struct defined");
    ASSERT_TRUE(sizeof(SuccinctGraph) > 0, "SuccinctGraph struct defined");
    ASSERT_TRUE(sizeof(QBFSolverStats) > 0, "QBFSolverStats struct defined");

    /* L4 Theorem Verification */
    printf("\n--- L4: Theorem Verification ---\n");
    ASSERT_EQ_INT(SPACE_CLASS_RELATION_KNOWN_SEPARATION,
                  complexity_compare(CLASS_L, CLASS_PSPACE),
                  "L ⊊ PSPACE (proven separation)");
    ASSERT_EQ_INT(SPACE_CLASS_RELATION_KNOWN_SEPARATION,
                  complexity_compare(CLASS_P, CLASS_EXP),
                  "P ⊊ EXP (proven separation)");
    ASSERT_EQ_INT(SPACE_CLASS_RELATION_KNOWN_SUBSET,
                  complexity_compare(CLASS_NL, CLASS_P),
                  "NL ⊆ P (proven)");
    ASSERT_EQ_INT(SPACE_CLASS_RELATION_KNOWN_SUBSET,
                  complexity_compare(CLASS_PSPACE, CLASS_NPSPACE),
                  "PSPACE ⊆ NPSPACE (trivial)");
    ASSERT_EQ_INT(SPACE_CLASS_RELATION_KNOWN_SUBSET,
                  complexity_compare(CLASS_NPSPACE, CLASS_PSPACE),
                  "NPSPACE ⊆ PSPACE (Savitch)");
    ASSERT_EQ_INT(SPACE_CLASS_RELATION_KNOWN_SUBSET,
                  complexity_compare(CLASS_PSPACE, CLASS_EXP),
                  "PSPACE ⊆ EXP (proven)");
    ASSERT_EQ_INT(SPACE_CLASS_RELATION_OPEN,
                  complexity_compare(CLASS_P, CLASS_PSPACE),
                  "P ⊆ PSPACE (open)");

    /* L5 Algorithm Verification */
    printf("\n--- L5: Algorithm Assertions ---\n");
    {
        int cls1[2][3] = {{QBF_LIT(0,0), QBF_LIT(1,0), QBF_PAD},
                          {QBF_LIT(0,1), QBF_LIT(1,1), QBF_PAD}};
        /* (x0|x1)&(!x0|!x1) ≡ x0 XOR x1.
         * E x0 A x1: need x0 s.t. formula holds for both x1=0 AND x1=1.
         *   x0=0: x1 AND true = x1 — fails when x1=0
         *   x0=1: true AND !x1 = !x1 — fails when x1=1
         *   → FALSE
         * A x0 E x1: for each x0, need some x1.
         *   x0=0: pick x1=1 → true
         *   x0=1: pick x1=0 → true
         *   → TRUE */
        int r = qbf_evaluate(2, "EA", cls1, 2);
        ASSERT_EQ_INT(0, r, "QBF E x0 A x1: (x0⊕x1) = FALSE");
        r = qbf_evaluate(2, "AE", cls1, 2);
        ASSERT_EQ_INT(1, r, "QBF A x0 E x1: (x0⊕x1) = TRUE");
    }
    {
        int cls2[1][3] = {{QBF_LIT(0,0), QBF_LIT(1,0), QBF_LIT(2,0)}};
        int r = qbf_evaluate(3, "EEE", cls2, 1);
        ASSERT_EQ_INT(1, r, "QBF exists x0,x1,x2: (x0|x1|x2) = TRUE");
    }
    {
        /* Test: Savitch reachability on a simple path graph */
        int* edges[2];
        int e0[] = {0, 1}; int e1[] = {1, 2};
        edges[0] = e0; edges[1] = e1;
        int r = savitch_reachability(0, 1, 1, 3, edges, 2);
        ASSERT_EQ_INT(1, r, "PATH(0,1) direct edge");
        r = savitch_reachability(1, 0, 1, 3, edges, 2);
        ASSERT_EQ_INT(0, r, "PATH(1,0) no reverse edge");
    }
    {
        double configs = count_tm_configs(5, 10, 3);
        ASSERT_TRUE(configs > 0, "count_tm_configs returns positive");
    }
    {
        int result = space_hierarchy_separates(1.0, 100.0);
        ASSERT_EQ_INT(1, result, "Space hierarchy: f=o(g) separates");
    }

    printf("\n===== ALL TESTS PASSED =====\n");
    return 0;
}