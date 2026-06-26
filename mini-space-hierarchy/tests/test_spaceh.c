/* test_spaceh.c — Comprehensive test suite for mini-space-hierarchy
 * Uses standard assert() for mathematical correctness checks.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "../include/spaceh.h"
#include "../include/space_tm.h"

static int tests_run = 0;
static int tests_passed = 0;

/* Standalone space bound function (C99: no nested functions) */
static size_t test_bound_fn(size_t n) { return (size_t)n + 2; }

#define TEST(name) do { tests_run++; printf("  TEST %s... ", name); } while(0)
#define PASS() do { printf("PASS\n"); tests_passed++; } while(0)
#define FAIL(msg) do { printf("FAIL: %s\n", msg); } while(0)

/* Test 1: Space constructibility */
static void test_space_constructible(void) {
    TEST("space-constructible functions");
    assert(is_space_constructible_fn(sc_log_n, 100) == 1);
    assert(is_space_constructible_fn(sc_log_n, 100) == 1);
    assert(sc_log_n(1.0) == 1.0);
    assert(sc_log_n(2.0) == 1.0);
    assert(sc_log_n(3.0) == 2.0);
    assert(sc_log_n(4.0) == 2.0);
    assert(sc_log_n(8.0) == 3.0);
    assert(sc_log_n(16.0) == 4.0);
    assert(sc_n_to_k(2.0, 3) == 8.0);
    assert(sc_n_to_k(10.0, 2) == 100.0);
    PASS();
}

/* Test 2: Logspace addition correctness */
static void test_logspace_addition(void) {
    TEST("logspace binary addition");
    char result[8];
    int carry;

    carry = logspace_add_binary("0000", "0000", result, 4);
    assert(carry == 0);
    assert(strcmp(result, "0000") == 0);

    carry = logspace_add_binary("0101", "0011", result, 4);
    assert(carry == 0);
    assert(strcmp(result, "1000") == 0); /* 5+3=8 */

    carry = logspace_add_binary("1111", "0001", result, 4);
    assert(carry == 1);
    assert(strcmp(result, "0000") == 0); /* 15+1=16 with carry */

    carry = logspace_add_binary("1010", "0101", result, 4);
    assert(carry == 0);
    assert(strcmp(result, "1111") == 0); /* 10+5=15 */

    PASS();
}

/* Test 3: Palindrome correctness */
static void test_palindrome(void) {
    TEST("logspace palindrome check");
    assert(logspace_palindrome_check("racecar", 7) == 1);
    assert(logspace_palindrome_check("abba", 4) == 1);
    assert(logspace_palindrome_check("abc", 3) == 0);
    assert(logspace_palindrome_check("a", 1) == 1);
    assert(logspace_palindrome_check("aa", 2) == 1);
    assert(logspace_palindrome_check("ab", 2) == 0);
    PASS();
}

/* Test 4: Diagonal language properties */
static void test_diagonal_language(void) {
    TEST("space hierarchy diagonal language");
    /* L_D should be deterministic based on its input */
    int r1 = space_hierarchy_diagonal_language(5);
    int r2 = space_hierarchy_diagonal_language(5);
    assert(r1 == r2); /* deterministic */

    int r3 = space_hierarchy_diagonal_language(3);
    int r4 = space_hierarchy_diagonal_language(3);
    assert(r3 == r4);

    /* L_D(n) should be 0 or 1 */
    for (int n = 0; n <= 10; n++) {
        int r = space_hierarchy_diagonal_language(n);
        assert(r == 0 || r == 1);
    }
    PASS();
}

/* Test 5: Space-bounded TM configuration graph */
static void test_config_graph(void) {
    TEST("configuration graph acceptance");
    SpaceBoundedTM* tm = sptm_create("Test", 4, 2, 3, test_bound_fn);
    sptm_set_accept_reject(tm, 1, 2, 0);
    sptm_set_transition(tm, 0, 0, 0, 1, 1, 1, 1);

    int input[] = {1, 0};
    ConfigGraphSpace* cg = cfg_graph_build(tm, input, 2, 6);
    assert(cg != NULL);
    assert(cg->n_vertices > 0);
    /* The TM should produce a non-empty config graph */
    cfg_graph_free(cg);
    sptm_free(tm);
    PASS();
}

/* Test 6: Savitch theorem — recursive path finding */
static void test_savitch_path(void) {
    TEST("Savitch path finding");
    int n = 4;
    int** edges = malloc((size_t)(n - 1) * sizeof(int*));
    for (int i = 0; i < n - 1; i++) {
        edges[i] = malloc(2 * sizeof(int));
        edges[i][0] = i;
        edges[i][1] = i + 1;
    }
    int result = savitch_path(edges, n - 1, 0, n - 1, n - 1, n);
    assert(result == 1); /* 0->1->2->3 reachable */

    /* Reverse direction: 3->0 not reachable */
    for (int i = 0; i < n - 1; i++) {
        int tmp = edges[i][0];
        edges[i][0] = edges[i][1];
        edges[i][1] = tmp;
    }
    result = savitch_path(edges, n - 1, 0, n - 1, n - 1, n);
    assert(result == 0); /* 0 should not be reachable from start=3? */
    /* Actually with edges reversed, 3->2->1->0 IS reachable from 0.
       Let's test 3->0 instead */
    result = savitch_path(edges, n - 1, n - 1, 0, n - 1, n);
    assert(result == 1); /* 3->2->1->0 reachable */

    for (int i = 0; i < n - 1; i++) free(edges[i]);
    free(edges);
    PASS();
}

/* Test 7: Logspace multiplication */
static void test_logspace_multiplication(void) {
    TEST("logspace multiplication");
    int r1 = logspace_multiply(7, 8, 8);
    assert(r1 == 56);
    int r2 = logspace_multiply(0, 100, 8);
    assert(r2 == 0);
    int r3 = logspace_multiply(5, 5, 4);
    assert(r3 == 25);
    PASS();
}

int main(void) {
    setbuf(stdout, NULL);
    printf("\n========================================\n");
    printf("  mini-space-hierarchy Test Suite\n");
    printf("========================================\n\n");

    test_space_constructible();
    test_logspace_addition();
    test_palindrome();
    test_diagonal_language();
    test_config_graph();
    test_savitch_path();
    test_logspace_multiplication();

    printf("\n========================================\n");
    printf("  Results: %d/%d tests passed\n", tests_passed, tests_run);
    printf("========================================\n");

    if (tests_passed == tests_run) {
        printf("ALL TESTS PASSED\n");
        return 0;
    } else {
        printf("SOME TESTS FAILED\n");
        return 1;
    }
}
