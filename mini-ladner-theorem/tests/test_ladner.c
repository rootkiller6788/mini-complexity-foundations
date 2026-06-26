/* test_ladner.c -- Comprehensive Assert-Based Tests for mini-ladner-theorem
 *
 * Tests cover all core functions, theorem properties, and edge cases.
 * Based on assert() from <assert.h>.
 * Run via: make test
 */

#include "ladner.h"
#include <assert.h>
#include <math.h>

/* ---- Test slow-growing functions ---- */

static void test_slow_growing_f(void) {
    printf("TEST: slow_growing_f (popcount)...\n");

    /* Basic values */
    assert(slow_growing_f(0) == 0);
    assert(slow_growing_f(1) == 1);
    assert(slow_growing_f(2) == 1);
    assert(slow_growing_f(3) == 2);
    assert(slow_growing_f(4) == 1);
    assert(slow_growing_f(7) == 3);
    assert(slow_growing_f(8) == 1);
    assert(slow_growing_f(15) == 4);

    /* Monotonic on some ranges */
    assert(slow_growing_f(7) >= slow_growing_f(4));

    /* Unbounded: for powers of 2 minus 1 */
    assert(slow_growing_f(3) == 2);   /* 2^2 - 1 */
    assert(slow_growing_f(7) == 3);   /* 2^3 - 1 */
    assert(slow_growing_f(15) == 4);  /* 2^4 - 1 */
    /* f(2^k - 1) = k */

    printf("  PASS\n");
}

static void test_slow_growing_variants(void) {
    printf("TEST: slow_growing variants...\n");

    /* popcount == slow_growing_f */
    assert(slow_growing_popcount(7) == slow_growing_f(7));
    assert(slow_growing_popcount(0) == 0);
    assert(slow_growing_popcount(255) == 8);

    /* Inverse Ackermann */
    assert(slow_growing_inv_ack(0) == 0 || slow_growing_inv_ack(0) == 0);
    assert(slow_growing_inv_ack(1) == 0);
    assert(slow_growing_inv_ack(2) == 0);
    assert(slow_growing_inv_ack(3) == 1);
    assert(slow_growing_inv_ack(7) == 2);
    assert(slow_growing_inv_ack(63) == 3);
    assert(slow_growing_inv_ack(65536) == 4);
    assert(slow_growing_inv_ack(100000) == 5);

    /* slow_growing_eval */
    assert(slow_growing_eval(7, 0) == slow_growing_f(7));  /* type 0 = popcount */
    assert(slow_growing_eval(7, 2) == slow_growing_inv_ack(7));  /* type 2 = inv ack */
    assert(slow_growing_eval(7, 99) == slow_growing_f(7));  /* default = popcount */

    printf("  PASS\n");
}

/* ---- Test padded SAT decode ---- */

static void test_padded_sat_decode(void) {
    printf("TEST: padded_sat_decode...\n");

    int phi_len, pad_len;

    /* n = k + 2^k. For k=3: n = 3 + 8 = 11 */
    assert(padded_sat_decode(11, &phi_len, &pad_len) == 1);
    assert(phi_len == 3);
    assert(pad_len == 8);

    /* k=4: n = 4 + 16 = 20 */
    assert(padded_sat_decode(20, &phi_len, &pad_len) == 1);
    assert(phi_len == 4);
    assert(pad_len == 16);

    /* k=1: n = 1 + 2 = 3 */
    assert(padded_sat_decode(3, &phi_len, &pad_len) == 1);
    assert(phi_len == 1);
    assert(pad_len == 2);

    /* Invalid encodings */
    assert(padded_sat_decode(0, &phi_len, &pad_len) == 0);
    assert(padded_sat_decode(1, &phi_len, &pad_len) == 0);   /* not k+2^k */
    assert(padded_sat_decode(2, &phi_len, &pad_len) == 0);
    assert(padded_sat_decode(4, &phi_len, &pad_len) == 0);   /* 1+2=3, 2+4=6, ... 4 not of form */

    printf("  PASS\n");
}

/* ---- Test delayed diagonalization ---- */

static void test_delayed_diagonal(void) {
    printf("TEST: delayed_diagonal...\n");

    /* delayed_diagonal should differ from p_machines for most n */
    int n = delayed_diagonal(1);
    assert(n == 0 || n == 1);  /* valid boolean */

    /* For the same n, repeated calls should be consistent */
    int a = delayed_diagonal(42);
    int b = delayed_diagonal(42);
    assert(a == b);

    printf("  PASS\n");
}

/* ---- Test Ladner membership ---- */

static void test_ladner_membership(void) {
    printf("TEST: ladner_membership...\n");

    /* Membership returns 0 or 1 */
    int m1 = ladner_membership(1);
    assert(m1 == 0 || m1 == 1);

    int m2 = ladner_membership(100);
    assert(m2 == 0 || m2 == 1);

    /* ladner_language_member should match */
    assert(ladner_language_member(NULL, 100) == ladner_membership(100));

    /* ladner_f should return small values */
    int f1 = ladner_f(1);
    assert(f1 >= 0 && f1 < 100);
    int f100 = ladner_f(100);
    assert(f100 >= 0 && f100 < 100);

    printf("  PASS\n");
}

/* ---- Test sparse sets ---- */

static void test_sparse_sets(void) {
    printf("TEST: sparse sets...\n");

    SparseSet* s = sparse_new(10);
    assert(s != NULL);
    assert(s->max_n == 10);
    assert(s->poly_deg == 2);

    /* Add elements. At n=1, bound is 1^{poly_deg}=1, so only 1 element fits */
    assert(sparse_add(s, 1, 42) == 1);
    assert(sparse_add(s, 1, 99) == 0);  /* exceeds bound of 1 */

    /* Contains check */
    assert(sparse_contains(s, 1, 42) == 1);
    assert(sparse_contains(s, 1, 99) == 0);  /* not added (bound exceeded) */
    assert(sparse_contains(s, 1, 100) == 0);
    assert(sparse_contains(s, 2, 42) == 0);

    /* Sparse bound check */
    assert(sparse_is_bound(s, 2) == 1);

    /* Add many elements at n=2 (bound = 4) */
    assert(sparse_add(s, 2, 200) == 1);
    assert(sparse_add(s, 2, 201) == 1);
    assert(sparse_add(s, 2, 202) == 1);
    assert(sparse_add(s, 2, 203) == 1);
    assert(sparse_add(s, 2, 204) == 0);  /* exceeds 2^2 = 4 */

    sparse_free(s);
    s = NULL;

    printf("  PASS\n");
}

/* ---- Test factoring ---- */

static void test_factoring(void) {
    printf("TEST: factoring...\n");

    /* GCD */
    assert(gcd_ll(12, 8) == 4);
    assert(gcd_ll(17, 5) == 1);
    assert(gcd_ll(0, 5) == 5);
    assert(gcd_ll(7, 0) == 7);
    assert(gcd_ll(0, 0) == 0);

    /* Factoring known composites */
    long long f1, f2;

    trial_division(91, &f1, &f2);  /* needed for factor_full */
    /* factor_full on known products */
    factor_full(91, &f1, &f2);
    assert(f1 * f2 == 91);
    assert(f1 > 1 && f1 < 91);

    factor_full(143, &f1, &f2);
    assert(f1 * f2 == 143);

    factor_full(121, &f1, &f2);
    assert(f1 * f2 == 121);

    /* Fermat test */
    long long p = 10007, q = 10009;
    long long n = p * q;
    long long ff = fermat_factor(n);
    assert(ff > 1 && ff < n && n % ff == 0);

    /* Pollard rho test */
    long long rho = pollard_rho(8051);
    assert(rho > 1 && rho < 8051 && 8051 % rho == 0);

    printf("  PASS\n");
}

/* Helper for trial_division (declared static in factoring.c)
   We'll test through factor_full instead. */

/* ---- Test Mahaney pruning ---- */

static void test_mahaney(void) {
    printf("TEST: Mahaney pruning...\n");

    /* Pruning for n_vars should give poly-sized result */
    long long pruned_5 = mahaney_prune(5, 2);
    assert(pruned_5 > 0);
    assert(pruned_5 <= (1LL << 5));  /* pruned <= original */

    /* For larger n, pruning should be dramatic */
    long long pruned_20 = mahaney_prune(20, 2);
    long long orig_20 = 1LL << 20;
    assert(pruned_20 < orig_20);  /* pruning happened */

    /* Sparsity check */
    SparseSet* s = sparse_new(5);
    sparse_add(s, 1, 10);
    assert(mahaney_sparse_check(s) == 0);  /* sparse => not NPC */

    sparse_free(s);

    printf("  PASS\n");
}

/* ---- Test degree structure ---- */

static void test_degree_structure(void) {
    printf("TEST: degree structure...\n");

    NP_Degree p_deg = {"P", 0, 1, 0, "P problems", "Null"};
    NP_Degree npc_deg = {"NPC", 100, 0, 1, "SAT", "Complete"};

    assert(deg_cmp(p_deg, npc_deg) == -1);  /* P < NPC */
    assert(deg_cmp(npc_deg, p_deg) == 1);   /* NPC > P */
    assert(deg_cmp(p_deg, p_deg) == 0);     /* P == P */

    /* Interpolation */
    assert(deg_interp(0, 100) == 50);
    assert(deg_interp(0, 50) == 25);
    assert(deg_interp(50, 100) == 75);

    printf("  PASS\n");
}

/* ---- Test graph isomorphism ---- */

static void test_graph_isomorphism(void) {
    printf("TEST: graph isomorphism...\n");

    /* Graph lifecycle */
    Graph* g = g_new(4);
    assert(g != NULL);
    assert(g->n == 4);

    /* Edge operations */
    assert(g_add_edge(g, 0, 1) == 1);
    assert(g_add_edge(g, 1, 2) == 1);
    assert(g_add_edge(g, 2, 0) == 1);

    /* Bad edges */
    assert(g_add_edge(g, -1, 0) == 0);
    assert(g_add_edge(g, 0, 4) == 0);

    /* Equality */
    Graph* h = g_new(4);
    g_add_edge(h, 0, 1); g_add_edge(h, 1, 2); g_add_edge(h, 2, 0);
    assert(g_equal(g, h) == 1);

    /* Modified graph should differ */
    g_add_edge(h, 0, 3);
    assert(g_equal(g, h) == 0);

    /* Permutation */
    int pi[] = {1, 2, 3, 0};
    Graph* g_perm = g_permute(g, pi);
    assert(g_perm != NULL);
    assert(g_perm->n == 4);

    /* Degree sequence on 4-cycle */
    Graph* cycle = g_new(4);
    g_add_edge(cycle, 0, 1); g_add_edge(cycle, 1, 2);
    g_add_edge(cycle, 2, 3); g_add_edge(cycle, 3, 0);
    int degs[4];
    deg_seq(cycle, degs);
    assert(degs[0] == 2); assert(degs[1] == 2);
    assert(degs[2] == 2); assert(degs[3] == 2);

    /* GI brute force on isomorphic graphs */
    Graph* cycle2 = g_new(4);
    g_add_edge(cycle2, 1, 0); g_add_edge(cycle2, 0, 2);
    g_add_edge(cycle2, 2, 3); g_add_edge(cycle2, 3, 1);
    /* cycle2 is same cycle with different vertex ordering */
    int iso_result = gi_brute(cycle, cycle2);
    assert(iso_result == 1);

    /* WL test */
    int wl_result = wl_test(cycle, cycle2, 5);
    assert(wl_result == 1);  /* WL cannot distinguish them either */

    /* Cleanup */
    g_free(g); g_free(h); g_free(g_perm);
    g_free(cycle); g_free(cycle2);

    printf("  PASS\n");
}

/* ---- Test promise problems ---- */

static void test_promise_problems(void) {
    printf("TEST: promise problems...\n");

    /* Square-free */
    assert(is_square_free(1) == 1);
    assert(is_square_free(2) == 1);
    assert(is_square_free(3) == 1);
    assert(is_square_free(4) == 0);    /* 2^2 divides 4 */
    assert(is_square_free(8) == 0);    /* 2^2 divides 8 */
    assert(is_square_free(12) == 0);   /* 2^2 divides 12 */
    assert(is_square_free(30) == 1);   /* 2*3*5, all distinct primes */

    /* Quadratic residue */
    assert(is_quadratic_residue(11, 1) == 1);   /* 1^2 = 1 mod 11 */
    assert(is_quadratic_residue(11, 4) == 1);   /* 2^2 = 4 mod 11 */
    assert(is_quadratic_residue(11, 5) == 1);   /* 4^2 = 16 = 5 mod 11 */
    assert(is_quadratic_residue(7, 3) == 0);    /* no x: x^2 = 3 mod 7 */

    /* Degree sequence match */
    int d1[] = {2, 2, 2, 2};
    int d2[] = {2, 2, 2, 2};
    int d3[] = {3, 1, 1, 1};
    assert(deg_sequence_match(d1, d2, 4) == 1);
    assert(deg_sequence_match(d1, d3, 4) == 0);

    printf("  PASS\n");
}

/* ---- Test NPI candidates ---- */

static void test_npi_candidates(void) {
    printf("TEST: NPI candidates...\n");

    int count = npi_candidate_count();
    assert(count >= 6);  /* at least 6 known candidates */

    printf("  PASS\n");
}

/* ---- Test Berman-Hartmanis ---- */

static void test_berman_hartmanis(void) {
    printf("TEST: Berman-Hartmanis isomorphism...\n");

    int result = berman_hartmanis_check();
    assert(result == 1);

    printf("  PASS\n");
}

/* ---- Full demo run ---- */

static void run_all_demos(void) {
    printf("\n===== Ladner Theorem Tests =====\n");
    ladner_construction_demo();
    delayed_diag_demo();
    mahaney_demo();
    berman_hartmanis_demo();
    sparse_sets_demo();
    degree_structure_demo();
    factoring_demo();
    graph_isomorphism_demo();
    ladner_language_demo();
    promise_problems_demo();
    np_intermediate_candidates_demo();
    oracle_construction_demo();
    uniqueness_demo();
    natural_proofs_demo();
    sparse_operations_demo();
    relativization_demo();
    schoning_demo();
    ladner_bench();
    printf("\n===== ALL DEMOS COMPLETE =====\n");
}

/* ---- Main ---- */

int main(void) {
    setbuf(stdout, NULL);

    printf("\n========================================\n");
    printf("  mini-ladner-theorem: Test Suite\n");
    printf("========================================\n\n");

    /* Unit tests */
    test_slow_growing_f();
    test_slow_growing_variants();
    test_padded_sat_decode();
    test_delayed_diagonal();
    test_ladner_membership();
    test_sparse_sets();
    test_factoring();
    test_mahaney();
    test_degree_structure();
    test_graph_isomorphism();
    test_promise_problems();
    test_npi_candidates();
    test_berman_hartmanis();

    printf("\n===== All unit tests PASSED =====\n\n");

    /* Full demo run */
    run_all_demos();

    return 0;
}
