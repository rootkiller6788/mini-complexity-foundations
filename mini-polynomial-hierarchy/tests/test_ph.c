/* test_ph.c - PH Module Test Suite */
#include "ph.h"
#include <assert.h>
#include <string.h>

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) do { printf("  TEST: %s ... ", name); } while(0)
#define PASS() do { printf("PASS\n"); tests_passed++; } while(0)
#define FAIL(msg) do { printf("FAIL: %s\n", msg); tests_failed++; } while(0)
#define CHECK(cond, msg) do { if (cond) PASS(); else FAIL(msg); } while(0)

int main(void) {
    setbuf(stdout, NULL);
    printf("\n========== PH Module Test Suite ==========\n\n");

    /* ---- L2: CNF Operations ---- */
    printf("--- L2: CNF Operations ---\n");

    TEST("cnf_eval on (x0|x1|x2), assign 111");
    {
        PH_CNF* f = ph_cnf_build_single(0,0,1,0,2,0);
        int a[] = {1,1,1};
        CHECK(ph_cnf_eval(f,a) == 1, "expected SAT");
        free(f);
    }

    TEST("cnf_eval on (x0|x1|x2), assign 000");
    {
        PH_CNF* f = ph_cnf_build_single(0,0,1,0,2,0);
        int a[] = {0,0,0};
        CHECK(ph_cnf_eval(f,a) == 0, "expected UNSAT");
        free(f);
    }

    TEST("cnf_parse simple");
    {
        PH_CNF* f = ph_cnf_parse("3 1; 0 0, 1 0, 2 0");
        CHECK(f != NULL && f->n_vars == 3 && f->n_clauses == 1, "parse failed");
        if (f) free(f);
    }

    TEST("cnf_clone equality");
    {
        PH_CNF* f = ph_cnf_build_single(0,0,1,1,2,0);
        PH_CNF* g = ph_cnf_clone(f);
        int a[] = {1,0,0};
        CHECK(f && g && ph_cnf_eval(f,a) == ph_cnf_eval(g,a), "clone mismatch");
        free(f); free(g);
    }

    /* ---- L3: QBF Construction ---- */
    printf("--- L3: QBF Operations ---\n");

    TEST("qbf_create basic");
    {
        PH_CNF* f = ph_cnf_build_single(0,0,1,0,-1,-1);
        PH_QuantType t[2] = {PH_QUANT_EXISTS, PH_QUANT_FORALL};
        int s[2] = {1, 1};
        PH_QBF* q = ph_qbf_create(2, t, s, f);
        CHECK(q && q->n_blocks == 2 && q->total_vars == 2, "qbf create failed");
        ph_qbf_free(q);
    }

    TEST("qbf_restrict");
    {
        PH_CNF* f = ph_cnf_build_single(0,0,1,0,-1,-1);
        PH_QuantType t[2] = {PH_QUANT_EXISTS, PH_QUANT_FORALL};
        int s[2] = {1, 1};
        PH_QBF* q = ph_qbf_create(2, t, s, f);
        int assign[] = {1};  /* fix first var to 1 */
        PH_QBF* r = ph_qbf_restrict(q, assign);
        CHECK(r && r->n_blocks == 1, "restrict should have 1 block");
        ph_qbf_free(r);
        ph_qbf_free(q);
    }

    /* ---- L4: Theorems ---- */
    printf("--- L4: Key Theorems ---\n");

    TEST("Sigma_1 SAT (NP): (x0)&(!x0) should be UNSAT");
    {
        PH_CNF* f = ph_cnf_build_single(0,0,-1,-1,-1,-1);
        f->n_clauses = 2;
        f->clauses[0] = (PH_Clause){{0,0,0}};
        f->clauses[1] = (PH_Clause){{1,1,1}};
        PH_QuantType t[1] = {PH_QUANT_EXISTS};
        int s[1] = {1};
        PH_QBF* q = ph_qbf_create(1, t, s, f);
        int r = ph_sigma_k_sat(q);
        CHECK(r == 0, "contradiction should be UNSAT");
        ph_qbf_free(q);
    }

    TEST("Sigma_1 SAT (NP): (x0) should be SAT");
    {
        PH_CNF* f = ph_cnf_build_single(0,0,0,0,0,0);
        PH_QuantType t[1] = {PH_QUANT_EXISTS};
        int s[1] = {1};
        PH_QBF* q = ph_qbf_create(1, t, s, f);
        int r = ph_sigma_k_sat(q);
        CHECK(r == 1, "x0 should be SAT");
        ph_qbf_free(q);
    }

    TEST("Sigma_2 SAT: exists x0 forall x1: x0 should be SAT");
    {
        PH_CNF* f = ph_cnf_build_single(0,0,0,0,-1,-1);
        PH_QuantType t[2] = {PH_QUANT_EXISTS, PH_QUANT_FORALL};
        int s[2] = {1, 1};
        PH_QBF* q = ph_qbf_create(2, t, s, f);
        int r = ph_sigma_k_sat(q);
        CHECK(r == 1, "E x A y: x should be SAT");
        ph_qbf_free(q);
    }

    TEST("Pi_2 SAT: forall x0 exist x1: NOT x0 OR x1 should be TRUE");
    {
        PH_CNF* f = ph_cnf_build_single(0,1,1,0,-1,-1);
        PH_QuantType t[2] = {PH_QUANT_FORALL, PH_QUANT_EXISTS};
        int s[2] = {1, 1};
        PH_QBF* q = ph_qbf_create(2, t, s, f);
        int r = ph_pi_k_sat(q);
        CHECK(r == 1, "A x E y: !x|y should be TRUE");
        ph_qbf_free(q);
    }

    TEST("Collapse check runs");
    {
        ph_collapse_check(2);
        PASS();
    }

    TEST("#SAT counter: (x0|x1|x2) has 7 solutions");
    {
        PH_CNF* f = ph_cnf_build_single(0,0,1,0,2,0);
        f->n_vars = 3;
        long long cnt = ph_count_sat(f);
        CHECK(cnt == 7, "expected 7 out of 8");
        free(f);
    }

    /* ---- L5: Algorithms ---- */
    printf("--- L5: Algorithms ---\n");

    TEST("DPLL SAT: (x0) should be SAT");
    {
        PH_CNF* f = ph_cnf_build_single(0,0,0,0,0,0);
        int r = ph_dpll_sat(f);
        CHECK(r == 1, "DPLL should find SAT");
        free(f);
    }

    TEST("DPLL SAT: (x0)&(!x0) should be UNSAT");
    {
        PH_CNF* f = ph_cnf_build_single(0,0,-1,-1,-1,-1);
        f->n_clauses = 2;
        f->clauses[0] = (PH_Clause){{0,0,0}};
        f->clauses[1] = (PH_Clause){{1,1,1}};
        int r = ph_dpll_sat(f);
        CHECK(r == 0, "DPLL should find UNSAT");
        free(f);
    }

    TEST("SAT-to-CLIQUE reduction");
    {
        PH_CNF* f = ph_cnf_build_single(0,0,1,0,2,0);
        f->n_vars = 3; f->n_clauses = 1;
        int graph[9] = {0};
        int n, k;
        ph_sat_to_clique(f, graph, &n, &k);
        CHECK(n == 3 && k == 1, "reduction output wrong");
        free(f);
    }

    /* ---- L6: Complete Problems ---- */
    printf("--- L6: Canonical Problems ---\n");

    TEST("QSAT benchmark k=2");
    {
        PH_QBF* q = ph_qsat_benchmark(2, 2);
        CHECK(q && q->n_blocks == 2, "QSAT benchmark failed");
        ph_qbf_free(q);
    }

    TEST("Circuit value problem");
    {
        PH_BoolCircuit* c = ph_circuit_build_and(2);
        int in00[] = {0,0}, in11[] = {1,1};
        CHECK(c && ph_circuit_value(c, in00) == 0, "AND(0,0) should be 0");
        if (c) { CHECK(ph_circuit_value(c, in11) == 1, "AND(1,1) should be 1"); }
        free(c);
    }

    /* ---- L7: Applications ---- */
    printf("--- L7: Applications ---\n");

    TEST("Bounded model check (safe system)");
    {
        int trans[4] = {1,0,0,1}; /* 0->0, 1->1 only */
        int err[2] = {0,0};
        int r = ph_bounded_model_check(2, 2, trans, err);
        CHECK(r == 0, "no error should be reachable");
    }

    /* ---- L8: Advanced Topics ---- */
    printf("--- L8: Advanced Topics ---\n");

    TEST("Valiant-Vazirani isolation");
    {
        PH_CNF* f = ph_cnf_build_single(0,0,1,0,2,0);
        f->n_vars = 3;
        PH_CNF iso;
        int r = ph_valiant_vazirani(f, &iso);
        CHECK(r == 1, "isolation should succeed");
        free(f);
    }

    /* ---- Structural Demos (verify no crashes) ---- */
    printf("--- Structural Demos ---\n");

    TEST("ph_print_hierarchy");
    ph_print_hierarchy();
    PASS();

    TEST("ph_summary");
    ph_summary();
    PASS();

    TEST("ph_demo_stockmeyer");
    ph_demo_stockmeyer();
    PASS();

    TEST("ph_demo_toda");
    ph_demo_toda();
    PASS();

    TEST("ph_demo_karp_lipton");
    ph_demo_karp_lipton();
    PASS();

    TEST("ph_demo_bpp");
    ph_demo_bpp();
    PASS();

    TEST("ph_demo_collapse");
    ph_demo_collapse();
    PASS();

    TEST("ph_demo_oracle");
    ph_demo_oracle();
    PASS();

    TEST("ph_demo_atm");
    ph_demo_atm();
    PASS();

    TEST("ph_demo_complete");
    ph_demo_complete();
    PASS();

    TEST("ph_demo_qbf");
    ph_demo_qbf();
    PASS();

    TEST("ph_benchmark");
    ph_benchmark();
    PASS();

    TEST("ph_research_summary");
    ph_research_summary();
    PASS();

    /* ---- Results ---- */
    printf("\n===== Test Results =====\n");
    printf("  Passed: %d\n", tests_passed);
    printf("  Failed: %d\n", tests_failed);
    printf("  Total:  %d\n", tests_passed + tests_failed);
    printf("==========================\n");

    return tests_failed > 0 ? 1 : 0;
}
