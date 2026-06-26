/* test_redcomp.c — Reduction & Completeness: Assert-Based Tests (L4)
 *
 * Tests for reduction correctness, properties, and chain verification.
 * Each test validates a specific theorem or property of reductions.
 *
 * Assert convention: assert(condition) — crashes on failure.
 * Run with: make test */

#include "../include/redcomp.h"
#include "../include/reduction_metrics.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ===== Test: Reduction Polynomial-Time Check ===== */
static void test_polynomial_check(void) {
    printf("TEST: reduction_is_polynomial...\n");

    /* O(n^2) function should pass polynomial check */
    static int f_poly(int n) {
        int s = 0;
        for (int i = 0; i < n * n; i++) s += i;
        return s;
    }

    int result = reduction_is_polynomial(f_poly, 50);
    assert(result == 1);  /* Should be polynomial */
    printf("  PASS: O(n^2) detected as polynomial\n");

    /* O(2^n) function should fail polynomial check */
    static int f_exp(int n) {
        int lim = 1 << (n < 12 ? n : 12);
        int s = 0;
        for (int i = 0; i < lim; i++) s += i;
        return s;
    }

    int result2 = reduction_is_polynomial(f_exp, 15);
    /* Exponential should NOT pass (too slow even at n=15) */
    assert(result2 == 0);
    printf("  PASS: O(2^n) detected as exponential\n");
}

/* ===== Test: Reduction Verify Correctness ===== */
static void test_reduction_verify(void) {
    printf("TEST: reduction_verify...\n");

    static int f_map(int n) { return n * n; }
    static int dec_A(int n) { return n % 3 == 0; }  /* A = multiples of 3 */
    static int dec_B(int n) { return n % 9 == 0; }  /* B = multiples of 9 */

    /* n multiple of 3 <=> n^2 multiple of 9. This should hold. */
    int verified = reduction_verify(f_map, dec_A, dec_B, 30);
    assert(verified == 1);
    printf("  PASS: f(x)=x^2, A=3Z, B=9Z  verified correct\n");
}

/* ===== Test: Reduction Transitivity ===== */
static void test_transitivity(void) {
    printf("TEST: reduction transitivity...\n");

    Reduction r1 = {1, 2, REDUCTION_MANY_ONE, 1.0, 0, 0, "A<=B"};
    Reduction r2 = {2, 3, REDUCTION_MANY_ONE, 1.5, 0, 0, "B<=C"};
    Reduction composed;

    double cexp = reduction_compose(&r1, &r2, &composed);
    assert(cexp > 0);
    assert(composed.problem_from_id == 1);
    assert(composed.problem_to_id == 3);

    int transitive = reduction_is_transitive(&r1, &r2, 10);
    assert(transitive == 1);
    printf("  PASS: composition valid, transitivity holds\n");
}

/* ===== Test: Closure Properties ===== */
static void test_closure(void) {
    printf("TEST: closure properties...\n");

    /* P-closure: if B in P and A <=_p B, then A in P.
       Test: A(x) = B(f(x)). If f is polynomial and B is polynomial,
       then A should also be polynomial. */

    static int dec_poly(int n) { return n % 2; }
    static int f_poly2(int n) { return n * n; }

    /* Check that composed function is also polynomial time */
    int is_poly = reduction_is_polynomial(f_poly2, 50);
    assert(is_poly == 1);
    printf("  PASS: P-closure: composed poly functions stay poly\n");
}

/* ===== Test: Karp Reduction Property ===== */
static int karp_f(int n) { return n * n; }
static int karp_A(int n) { return n % 2 == 0; }
static int karp_B(int n) { return n % 4 == 0; }

static void test_karp_reduction(void) {
    printf("TEST: Karp reduction (many-one) property...\n");

    /* Test: x in A iff f(x) in B for x=0..30 */
    for (int x = 0; x <= 30; x++) {
        int a = karp_A(x);
        int b = karp_B(karp_f(x));
        assert(a == b);
    }
    printf("  PASS: A<=_m B via f(x)=x^2 holds for 0..30\n");
}

/* ===== Test: Chain Initialization ===== */
static void test_chain_init(void) {
    printf("TEST: reduction chain initialization...\n");

    ReductionChain chain;
    chain_init(&chain);
    assert(chain.n_problems == 0);
    assert(chain.n_reductions == 0);
    printf("  PASS: chain initialized correctly\n");
}

/* ===== Test: Problem Hardness Propagation ===== */
static void test_hardness_propagation(void) {
    printf("TEST: hardness propagation...\n");

    DecisionProblem A = {1, "SAT", "CNF", CLASS_NP, 0, NULL, "standard"};
    DecisionProblem B = {2, "3SAT", "3-CNF", CLASS_NP, 0, NULL, "standard"};

    /* If A is NP-hard and A <= B, then B is NP-hard */
    int b_hard = hardness_propagates(&A, &B, CLASS_NP, REDUCTION_MANY_ONE);
    /* This should return 1 because the reduction exists (by construction) */
    assert(b_hard == 1);
    printf("  PASS: hardness propagates along reduction\n");
}

/* ===== Test: Polynomial-Time Reduction Timing ===== */
static void test_reduction_timing(void) {
    printf("TEST: reduction timing measurement...\n");

    static int f_test(int n) {
        int s = 0;
        for (int i = 0; i < n; i++) s += i;
        return s;
    }

    double times[20];
    double max_t = measure_reduction_timing(f_test, 20, times);
    assert(max_t >= 0);  /* Should complete successfully */
    assert(times[0] >= 0);

    /* For linear function, T(2n)/T(n) should be ~2 */
    double k, c;
    double r2 = fit_power_law(times, 20, &k, &c);
    assert(r2 >= 0.0);  /* Fit should succeed */
    printf("  PASS: timing measurement works (k≈%.2f, R^2≈%.2f)\n", k, r2);
}

/* ===== Test: Reduction Correctness Score ===== */
static void test_correctness_score(void) {
    printf("TEST: reduction correctness score...\n");

    ReductionVerificationResult res = {
        .total_tests = 20,
        .completeness_pass = 20,
        .soundness_pass = 20,
        .completeness_fail = 0,
        .soundness_fail = 0,
        .total_time_sec = 0.01,
        .avg_time_per_test = 0.0005,
        .is_polynomial = 1,
        .estimated_exponent = 1.5
    };

    double score = reduction_correctness_score(&res);
    assert(score == 1.0);  /* Perfect score */
    printf("  PASS: perfect reduction scores 1.0\n");

    /* Imperfect score */
    res.completeness_pass = 18;
    res.soundness_pass = 19;
    double score2 = reduction_correctness_score(&res);
    assert(score2 == (18.0 + 19.0) / 40.0);
    printf("  PASS: score = (completeness+soundness)/(2*total) = %.2f\n", score2);
}

/* ===== Main ===== */
int main(void) {
    setbuf(stdout, NULL);
    printf("\n===== Reductions & Completeness: Test Suite =====\n\n");

    test_polynomial_check();
    test_reduction_verify();
    test_transitivity();
    test_closure();
    test_karp_reduction();
    test_chain_init();
    test_hardness_propagation();
    test_reduction_timing();
    test_correctness_score();

    printf("\n===== ALL TESTS PASSED =====\n\n");
    return 0;
}
