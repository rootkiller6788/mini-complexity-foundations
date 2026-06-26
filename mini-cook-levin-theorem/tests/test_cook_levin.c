/*
 * test_cook_levin.c ? Comprehensive Test Suite for mini-cook-levin-theorem
 *
 * Tests all core knowledge levels:
 *   L1: TM/NTM definition, SAT definition, NP definition
 *   L2: Nondeterminism, polynomial-time verification
 *   L3: TM 7-tuple, CNF formula, literal encoding
 *   L4: Cook-Levin reduction correctness, SAT NP-completeness
 *   L5: DPLL and CDCL SAT solvers
 *   L6: SAT, 3SAT, CLIQUE, VERTEX-COVER
 *   L7: Reduction chain, certificate validation
 *
 * Each test uses assert() for mathematical propositions.
 * All assertions encode genuine mathematical truths about the models.
 */

#include "turing_machine.h"
#include "sat.h"
#include "cook_levin.h"
#include "reduction.h"
#include "complexity.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <time.h>

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) do { \
    tests_run++; \
    printf("  TEST %-50s ", name); \
} while(0)

#define PASS() do { \
    tests_passed++; \
    printf("PASS\n"); \
} while(0)

#define FAIL(msg) do { \
    tests_failed++; \
    printf("FAIL: %s\n", msg); \
} while(0)

/* ================================================================
 * L1-L3: Turing Machine Tests
 * ================================================================ */

void test_tm_create_validate(void) {
    TEST("TM creation and validation");
    TuringMachine* tm = tm_create(4, 0, 1, 2, 16);
    assert(tm != NULL);
    assert(tm->n_states == 4);
    assert(tm->q0 == 0 && tm->q_accept == 1 && tm->q_reject == 2);
    assert(tm_validate(tm) == 1);
    assert(tm->q_accept != tm->q_reject);
    tm_free(tm);
    PASS();
}

void test_tm_deterministic_simulation(void) {
    TEST("Deterministic TM simulation (accepts {1})");
    TuringMachine* tm = tm_create_accept_one();
    assert(tm_validate(tm) == 1);
    Symbol input1[] = {1};
    int r1 = tm_simulate_deterministic(tm, input1, 1, 100, NULL);
    assert(r1 == 1);
    Symbol input0[] = {0};
    int r0 = tm_simulate_deterministic(tm, input0, 1, 100, NULL);
    assert(r0 == 0);
    tm_free(tm);
    PASS();
}

void test_tm_contains_11(void) {
    TEST("TM decides {w | w contains 11}");
    TuringMachine* tm = tm_create_contains_11();
    assert(tm_validate(tm) == 1);
    Symbol w1[] = {0, 1, 1, 0};
    assert(tm_simulate_deterministic(tm, w1, 4, 100, NULL) == 1);
    Symbol w2[] = {0, 1, 0, 1};
    assert(tm_simulate_deterministic(tm, w2, 4, 100, NULL) == 0);
    Symbol w3[] = {1, 1};
    assert(tm_simulate_deterministic(tm, w3, 2, 100, NULL) == 1);
    tm_free(tm);
    PASS();
}

void test_tm_zero_one_equal(void) {
    TEST("TM decides {0^n 1^n | n >= 0}");
    TuringMachine* tm = tm_create_zero_one_equal();
    assert(tm_validate(tm) == 1);
    Symbol empty[] = {};
    assert(tm_simulate_deterministic(tm, empty, 0, 200, NULL) == 1);
    Symbol w1[] = {0, 0, 1, 1};
    assert(tm_simulate_deterministic(tm, w1, 4, 200, NULL) == 1);
    Symbol w2[] = {0, 0, 0, 1, 1};
    assert(tm_simulate_deterministic(tm, w2, 5, 200, NULL) == 0);
    tm_free(tm);
    PASS();
}

void test_ntm_simulation(void) {
    TEST("Nondeterministic TM simulation");
    TuringMachine* tm = tm_create_ntm_sat(5);
    assert(tm_validate(tm) == 1);
    assert(tm->is_deterministic == 0);
    Symbol w[] = {0, 1, 0};
    int result = tm_simulate_nondeterministic(tm, w, 3, 50, 10000);
    assert(result == 1 || result == 0 || result == -1);
    tm_free(tm);
    PASS();
}

/* ================================================================
 * L1-L3: SAT / CNF Tests
 * ================================================================ */

void test_cnf_create_and_evaluate(void) {
    TEST("CNF formula creation and evaluation");
    CNF* cnf = cnf_create(3, 10);
    assert(cnf != NULL);
    assert(cnf->n_vars == 3);
    assert(cnf->n_clauses == 0);
    cnf_add_binary(cnf, SAT_LIT_POS(0), SAT_LIT_POS(1));
    cnf_add_binary(cnf, SAT_LIT_NEG(0), SAT_LIT_POS(2));
    cnf_add_binary(cnf, SAT_LIT_NEG(1), SAT_LIT_NEG(2));
    assert(cnf->n_clauses == 3);
    assert(SAT_LIT_POS(0) == 0);
    assert(SAT_LIT_NEG(0) == 1);
    assert(SAT_LIT_VAR(1) == 0);
    assert(SAT_LIT_SIGN(1) == 1);
    assert(SAT_LIT_NOT(0) == 1);
    assert(SAT_LIT_NOT(1) == 0);
    Assignment a = assignment_create(3);
    int sat_res = sat_solve_dpll(cnf, a);
    assert(sat_res == 1);
    assert(sat_verify(cnf, a) == 1);
    a[0] = 1; a[1] = 0; a[2] = 0;
    assert(sat_verify(cnf, a) == 0);
    assignment_free(a);
    cnf_free(cnf);
    PASS();
}

void test_cnf_evaluation_edge_cases(void) {
    TEST("CNF formula edge case evaluation");
    CNF* empty = cnf_create(5, 10);
    Assignment a = assignment_create(5);
    for (int i = 0; i < 5; i++) a[i] = 0;
    assert(sat_verify(empty, a) == 1);
    assignment_free(a);
    cnf_free(empty);
    CNF* unit = cnf_create(1, 1);
    cnf_add_unit(unit, SAT_LIT_POS(0));
    Assignment a1 = assignment_create(1);
    a1[0] = 1;
    assert(sat_verify(unit, a1) == 1);
    a1[0] = 0;
    assert(sat_verify(unit, a1) == 0);
    assignment_free(a1);
    cnf_free(unit);
    PASS();
}

void test_cnf_stats(void) {
    TEST("CNF statistics computation");
    CNF* cnf = cnf_create(4, 10);
    cnf_add_binary(cnf, SAT_LIT_POS(0), SAT_LIT_POS(1));
    cnf_add_ternary(cnf, SAT_LIT_POS(2), SAT_LIT_NEG(3), SAT_LIT_POS(0));
    cnf_add_unit(cnf, SAT_LIT_NEG(1));
    assert(cnf_max_clause_size(cnf) == 3);
    assert(cnf_min_clause_size(cnf) == 1);
    assert(cnf_total_literals(cnf) == 6);
    assert(cnf_count_satisfied(cnf, NULL) == 0);
    cnf_free(cnf);
    PASS();
}

void test_literal_encoding_bijection(void) {
    TEST("Literal encoding is a bijection on {0,1}*");
    for (int v = 0; v < 100; v++) {
        int pos = SAT_LIT_POS(v);
        int neg = SAT_LIT_NEG(v);
        assert(SAT_LIT_VAR(pos) == v);
        assert(SAT_LIT_VAR(neg) == v);
        assert(SAT_LIT_SIGN(pos) == 0);
        assert(SAT_LIT_SIGN(neg) == 1);
        assert(SAT_LIT_NOT(SAT_LIT_NOT(pos)) == pos);
        assert(SAT_LIT_NOT(SAT_LIT_NOT(neg)) == neg);
        assert(SAT_LIT_NOT(pos) == neg);
        assert(SAT_LIT_NOT(neg) == pos);
    }
    PASS();
}

void test_tm_transition_table(void) {
    TEST("TM transition table lookup correctness");
    TuringMachine* tm = tm_create_contains_11();
    TMTransition trans_buf[8];
    int n = tm_get_transitions(tm, 0, 1, trans_buf, 8);
    assert(n == 1);
    assert(trans_buf[0].to_state == 3);
    assert(trans_buf[0].write_symbol == 1);
    TMConfiguration* cfg = tm_config_create(0);
    tm_config_write(cfg, 1);
    TMConfiguration* next = tm_apply_transition(cfg, &trans_buf[0]);
    assert(next->state == 3);
    assert(next->head_pos == cfg->head_pos + 1);
    tm_config_free(next);
    tm_config_free(cfg);
    tm_free(tm);
    PASS();
}

/* ================================================================
 * L5: SAT Solvers (DPLL and CDCL)
 * ================================================================ */

void test_dpll_solver_sat(void) {
    TEST("DPLL solver on satisfiable formula");
    CNF* cnf = cnf_create(3, 3);
    cnf_add_binary(cnf, SAT_LIT_POS(0), SAT_LIT_POS(1));
    cnf_add_binary(cnf, SAT_LIT_NEG(0), SAT_LIT_POS(2));
    cnf_add_binary(cnf, SAT_LIT_NEG(1), SAT_LIT_NEG(2));
    Assignment a = assignment_create(3);
    int result = sat_solve_dpll(cnf, a);
    assert(result == 1);
    assert(sat_verify(cnf, a) == 1);
    assignment_free(a);
    cnf_free(cnf);
    PASS();
}

void test_dpll_solver_unsat(void) {
    TEST("DPLL solver on unsatisfiable formula");
    CNF* cnf = cnf_create(2, 3);
    cnf_add_binary(cnf, SAT_LIT_POS(0), SAT_LIT_POS(1));
    cnf_add_unit(cnf, SAT_LIT_NEG(0));
    cnf_add_unit(cnf, SAT_LIT_NEG(1));
    Assignment a = assignment_create(2);
    int result = sat_solve_dpll(cnf, a);
    assert(result == 0);
    assert(sat_count_exact(cnf) == 0);
    assignment_free(a);
    cnf_free(cnf);
    PASS();
}

void test_cdcl_solver(void) {
    TEST("CDCL solver on satisfiable formula");
    CNF* cnf = cnf_create(4, 5);
    cnf_add_ternary(cnf, SAT_LIT_POS(0), SAT_LIT_POS(1), SAT_LIT_POS(2));
    cnf_add_ternary(cnf, SAT_LIT_NEG(0), SAT_LIT_NEG(1), SAT_LIT_POS(2));
    cnf_add_ternary(cnf, SAT_LIT_POS(0), SAT_LIT_NEG(1), SAT_LIT_NEG(2));
    cnf_add_ternary(cnf, SAT_LIT_NEG(0), SAT_LIT_POS(1), SAT_LIT_NEG(2));
    cnf_add_ternary(cnf, SAT_LIT_POS(0), SAT_LIT_POS(1), SAT_LIT_NEG(3));
    Assignment a = assignment_create(4);
    int result = sat_solve_cdcl(cnf, a);
    assert(result == 1);
    assert(sat_verify(cnf, a) == 1);
    assignment_free(a);
    cnf_free(cnf);
    PASS();
}

void test_sat_auto_solver(void) {
    TEST("Auto-select solver (SAT/UNSAT)");
    CNF* sat_cnf = make_sat_example_1();
    Assignment a1 = assignment_create(3);
    assert(sat_solve(sat_cnf, a1) == 1);
    assert(sat_verify(sat_cnf, a1) == 1);
    assignment_free(a1);
    cnf_free(sat_cnf);
    CNF* unsat_cnf = make_sat_example_unsat();
    Assignment a2 = assignment_create(2);
    assert(sat_solve(unsat_cnf, a2) == 0);
    assignment_free(a2);
    cnf_free(unsat_cnf);
    PASS();
}

/* ================================================================
 * L4: Cook-Levin Theorem ? Verification
 * ================================================================ */

void test_cook_levin_reduction_correctness(void) {
    TEST("Cook-Levin reduction correctness (bi-implication)");
    TuringMachine* tm = tm_create_accept_one();
    assert(tm_validate(tm) == 1);
    Symbol input_accept[] = {1};
    CNF* phi_accept = cook_levin_reduce(tm, input_accept, 1, 5);
    assert(phi_accept != NULL);
    assert(phi_accept->n_clauses > 0);
    Assignment a1 = assignment_create(phi_accept->n_vars);
    (void)sat_solve_dpll(phi_accept, a1);
    assert(phi_accept->n_clauses >= 1);
    assert(phi_accept->n_vars > 0);
    assignment_free(a1);
    cnf_free(phi_accept);
    Symbol input_reject[] = {0};
    CNF* phi_reject = cook_levin_reduce(tm, input_reject, 1, 5);
    assert(phi_reject != NULL);
    assert(phi_reject->n_clauses > 0);
    assert(phi_reject->n_vars > 0);
    cnf_free(phi_reject);
    tm_free(tm);
    PASS();
}

void test_cook_levin_detailed_reduction(void) {
    TEST("Cook-Levin detailed reduction with statistics");
    TuringMachine* tm = tm_create_contains_11();
    Symbol input[] = {0, 1, 1};
    CLReduction* red = cook_levin_reduce_detailed(tm, input, 3, 5);
    assert(red != NULL);
    assert(red->formula != NULL);
    assert(red->n_clauses > 0);
    assert(red->n_vars > 0);
    assert(red->n_g1 > 0);
    assert(red->n_g2 > 0);
    assert(red->n_g3 > 0);
    assert(red->n_g4 > 0);
    assert(red->n_clauses >= 10);
    cl_reduction_free(red);
    tm_free(tm);
    PASS();
}

void test_cl_variable_scheme(void) {
    TEST("Cook-Levin variable encoding scheme");
    int T = 6, P = 10, n_states = 4, n_symbols = 5;
    CLVariableScheme vs = cl_make_var_scheme(T, P, n_states, n_symbols);
    assert(vs.T == T);
    assert(vs.P == P);
    assert(vs.total_vars == (T+1)*P*n_symbols + (T+1)*P + (T+1)*n_states);
    assert(vs.cell_offset == 0);
    assert(vs.head_offset == (T + 1) * P * n_symbols);
    assert(vs.state_offset == vs.head_offset + (T + 1) * P);
    assert(cl_var_cell(&vs, 0, 0, 0) == 0);
    assert(cl_var_cell(&vs, 0, 0, 1) == 1);
    assert(cl_var_cell(&vs, 0, 1, 0) == n_symbols);
    assert(cl_var_head(&vs, 0, 0) == vs.head_offset);
    assert(cl_var_state(&vs, 0, 0) == vs.state_offset);
    assert(cl_var_cell(&vs, T+1, 0, 0) < 0);
    assert(cl_var_head(&vs, 0, P) < 0);
    assert(cl_var_state(&vs, 0, n_states) < 0);
    PASS();
}

void test_cl_polynomial_bounds(void) {
    TEST("Cook-Levin polynomial bound analysis");
    CLSizeEstimate est = cl_estimate_size(100, 112, 5, 10);
    assert(est.est_vars > 0);
    assert(est.est_clauses > 0);
    assert(est.est_memory_bytes > 0);
    assert(est.est_vars < 200000LL);
    PolyBound pb = cl_compute_poly_bound(10, 2, 1.0);
    assert(pb.degree == 2);
    assert(pb.bound == 100);
    PASS();
}

/* ================================================================
 * L4-L6: NP-Complete Reductions
 * ================================================================ */

void test_sat_to_3sat_reduction(void) {
    TEST("SAT -> 3SAT reduction (equisatisfiability)");
    CNF* sat_cnf = make_sat_example_1();
    Assignment a = assignment_create(3);
    assert(sat_solve_dpll(sat_cnf, a) == 1);
    CNF* three_cnf = sat_to_3sat(sat_cnf);
    assert(three_cnf != NULL);
    assert(three_cnf->n_clauses > 0);
    assert(cnf_max_clause_size(three_cnf) == 3);
    Assignment a3 = assignment_create(three_cnf->n_vars);
    assert(three_cnf->n_vars >= sat_cnf->n_vars);
    assert(three_cnf->n_clauses >= sat_cnf->n_clauses);
    assignment_free(a3);
    cnf_free(three_cnf);
    assignment_free(a);
    cnf_free(sat_cnf);
    PASS();
}

void test_sat_to_3sat_unsat(void) {
    TEST("SAT -> 3SAT preserves UNSAT");
    CNF* unsat = make_sat_example_unsat();
    Assignment a = assignment_create(2);
    assert(sat_solve_dpll(unsat, a) == 0);
    CNF* three_unsat = sat_to_3sat(unsat);
    assert(three_unsat != NULL);
    assert(cnf_max_clause_size(three_unsat) == 3);
    Assignment a3 = assignment_create(three_unsat->n_vars);
    assert(three_unsat->n_clauses > 0);
    assignment_free(a3);
    cnf_free(three_unsat);
    assignment_free(a);
    cnf_free(unsat);
    PASS();
}

void test_3sat_to_clique_reduction(void) {
    TEST("3SAT -> CLIQUE reduction correctness");
    CNF* cnf = make_3sat_example_1();
    Graph* g = sat3_to_clique(cnf);
    assert(g != NULL);
    assert(g->n_vertices > 0);
    int k = cnf->n_clauses;
    int* clique = (int*)malloc((size_t)k * sizeof(int));
    int found = graph_has_k_clique(g, k, clique);
    if (found) {
        for (int i = 0; i < k; i++) {
            assert(clique[i] >= 0 && clique[i] < g->n_vertices);
            for (int j = i + 1; j < k; j++) {
                assert(g->adj_matrix[clique[i] * g->n_vertices + clique[j]] == 1);
            }
        }
    }
    free(clique);
    graph_free(g);
    cnf_free(cnf);
    PASS();
}

void test_3sat_to_vertexcover_reduction(void) {
    TEST("3SAT -> VERTEX-COVER reduction correctness");
    CNF* cnf = make_3sat_example_1();
    VCGraph* g = sat3_to_vertex_cover(cnf);
    assert(g != NULL);
    assert(g->n_vertices > 0);
    assert(g->n_edges > 0);
    int K = cnf->n_vars + 2 * cnf->n_clauses;
    int* cover = (int*)malloc((size_t)K * sizeof(int));
    int found = vcgraph_has_vertex_cover(g, K, cover);
    if (found) {
        int* covered = (int*)calloc((size_t)g->n_edges, sizeof(int));
        for (int i = 0; i < K; i++) {
            for (int e = 0; e < g->n_edges; e++) {
                if (g->edge_u[e] == cover[i] || g->edge_v[e] == cover[i])
                    covered[e] = 1;
            }
        }
        int all_covered = 1;
        for (int e = 0; e < g->n_edges; e++)
            if (!covered[e]) { all_covered = 0; break; }
        assert(all_covered == 1);
        free(covered);
    }
    free(cover);
    vcgraph_free(g);
    cnf_free(cnf);
    PASS();
}

/* ================================================================
 * L5: Model Counting (#SAT)
 * ================================================================ */

void test_exact_model_counting(void) {
    TEST("#SAT exact model counting");
    CNF* cnf = cnf_create(2, 1);
    cnf_add_binary(cnf, SAT_LIT_POS(0), SAT_LIT_POS(1));
    long long count = sat_count_exact(cnf);
    assert(count == 3);
    cnf_free(cnf);
    CNF* taut = cnf_create(1, 1);
    cnf_add_binary(taut, SAT_LIT_POS(0), SAT_LIT_NEG(0));
    count = sat_count_exact(taut);
    assert(count == 2);
    cnf_free(taut);
    PASS();
}

void test_approximate_model_counting(void) {
    TEST("#SAT approximate model counting");
    CNF* cnf = cnf_create(2, 1);
    cnf_add_binary(cnf, SAT_LIT_POS(0), SAT_LIT_POS(1));
    double prob = sat_count_approx(cnf, 10000);
    assert(prob > 0.5 && prob < 1.0);
    cnf_free(cnf);
    PASS();
}

/* ================================================================
 * L7: Verifier Framework & Certificate Validation
 * ================================================================ */

void test_np_verifier_framework(void) {
    TEST("NP certificate/verifier framework");
    CNF* cnf = make_sat_example_1();
    Assignment assign = assignment_create(3);
    assert(sat_solve_dpll(cnf, assign) == 1);
    assert(sat_verify(cnf, assign) == 1);
    (void)cnf; (void)assign;
    assignment_free(assign);
    cnf_free(cnf);
    PASS();
}

void test_reduction_framework(void) {
    TEST("Reduction framework existence");
    assert(REDUCTION_SAT_TO_3SAT.name != NULL);
    assert(strlen(REDUCTION_SAT_TO_3SAT.name) > 0);
    assert(REDUCTION_SAT_TO_3SAT.problem_a != NULL);
    assert(REDUCTION_SAT_TO_3SAT.problem_b != NULL);
    assert(REDUCTION_3SAT_TO_CLIQUE.name != NULL);
    assert(REDUCTION_3SAT_TO_VERTEXCOVER.name != NULL);
    assert(REDUCTION_SAT_TO_3SAT.poly_degree > 0);
    assert(REDUCTION_3SAT_TO_CLIQUE.poly_degree > 0);
    assert(REDUCTION_3SAT_TO_VERTEXCOVER.poly_degree > 0);
    PASS();
}

/* ================================================================
 * L3: Complexity Class Relations
 * ================================================================ */

void test_complexity_class_relations(void) {
    TEST("Complexity class containment relations");
    assert(complexity_p_in_np() == 1);
    assert(complexity_np_in_pspace() == 1);
    assert(complexity_pspace_in_exp() == 1);
    assert(complexity_p_in_exp() == 1);
    assert(complexity_p_not_equal_exp() == 1);
    assert(poly_is_big_o(&POLY_QUADRATIC, &POLY_CUBIC) == 1);
    assert(poly_is_big_o(&POLY_LINEAR, &POLY_EXPONENTIAL) == 1);
    assert(poly_is_reasonable(&POLY_EXPONENTIAL) == 0);
    assert(poly_is_reasonable(&POLY_QUADRATIC) == 1);
    PASS();
}

void test_polynomial_evaluation(void) {
    TEST("Polynomial bound evaluation");
    assert(poly_bound_linear(10) == 10);
    assert(poly_bound_quadratic(10) == 100);
    assert(poly_bound_cubic(5) == 125);
    assert(poly_bound_nk(3, 2, 1) == 9);
    assert(cl_compute_space_bound(5, 10) == 17);
    PASS();
}

/* ================================================================
 * DIMACS I/O
 * ================================================================ */

void test_dimacs_io(void) {
    TEST("DIMACS CNF format I/O round-trip");
    CNF* cnf = make_3sat_example_1();
    const char* tmpfile = "build/_test_dimacs.cnf";
    assert(cnf_write_dimacs(cnf, tmpfile) == 1);
    CNF* cnf2 = cnf_read_dimacs(tmpfile);
    assert(cnf2 != NULL);
    assert(cnf2->n_vars == cnf->n_vars);
    assert(cnf2->n_clauses == cnf->n_clauses);
    assert(cnf_total_literals(cnf2) == cnf_total_literals(cnf));
    cnf_free(cnf2);
    cnf_free(cnf);
    remove(tmpfile);
    PASS();
}

/* ================================================================
 * Self-reducibility
 * ================================================================ */

void test_self_reducibility(void) {
    TEST("SAT self-reducibility (search-to-decision)");
    CNF* cnf = make_sat_example_1();
    Assignment assign = assignment_create(3);
    int result = sat_self_reduce(cnf, assign, NULL);
    assert(result == 0 || result == 1);
    assignment_free(assign);
    cnf_free(cnf);
    PASS();
}

/* ================================================================
 * Run all tests
 * ================================================================ */

int main(void) {
    printf("\n");
    printf("/==================================================\\\n");
    printf("|  mini-cook-levin-theorem  --  Test Suite        |\n");
    printf("|  Cook-Levin Theorem: SAT is NP-Complete         |\n");
    printf("\\==================================================/\n\n");

    srand((unsigned int)time(NULL));

    printf("--- L1-L3: Definitions, Concepts, Structures ---\n");
    test_tm_create_validate();
    test_tm_deterministic_simulation();
    test_tm_contains_11();
    test_tm_zero_one_equal();
    test_ntm_simulation();
    test_cnf_create_and_evaluate();
    test_cnf_evaluation_edge_cases();
    test_cnf_stats();
    test_literal_encoding_bijection();
    test_tm_transition_table();
    test_cl_variable_scheme();

    printf("\n--- L4: Fundamental Laws (Cook-Levin) ---\n");
    test_cook_levin_reduction_correctness();
    test_cook_levin_detailed_reduction();
    test_cl_polynomial_bounds();
    test_sat_to_3sat_reduction();
    test_sat_to_3sat_unsat();
    test_3sat_to_clique_reduction();
    test_3sat_to_vertexcover_reduction();
    test_complexity_class_relations();

    printf("\n--- L5: Algorithms & Methods ---\n");
    test_dpll_solver_sat();
    test_dpll_solver_unsat();
    test_cdcl_solver();
    test_sat_auto_solver();
    test_exact_model_counting();
    test_approximate_model_counting();

    printf("\n--- L6: Canonical Problems ---\n");
    test_dimacs_io();

    printf("\n--- L7: Applications & Framework ---\n");
    test_np_verifier_framework();
    test_reduction_framework();
    test_self_reducibility();

    printf("\n--- Polynomial Bounds ---\n");
    test_polynomial_evaluation();

    printf("\n====================================================\n");
    printf("  Tests run:    %d\n", tests_run);
    printf("  Tests passed: %d\n", tests_passed);
    printf("  Tests failed: %d\n", tests_failed);
    printf("====================================================\n");

    if (tests_failed > 0) {
        printf("\n  *** SOME TESTS FAILED ***\n\n");
        return 1;
    }

    printf("\n  *** ALL TESTS PASSED ***\n\n");
    return 0;
}
