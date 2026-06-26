/* promise_problems.c -- Promise Problems and NP Structure
 *
 * A promise problem is (YES, NO, PI): only decide correctly on
 * instances satisfying the promise PI. Many NP-intermediate candidates
 * are naturally promise problems (QR, SQR-free, factoring).
 *
 * However, Ladner's construction yields a LANGUAGE (defined on ALL
 * inputs), proving NP-intermediate LANGUAGES exist (if P != NP).
 *
 * Promise problems studied:
 *   - Square-Free numbers: is n divisible by any square?
 *   - Quadratic Residuosity (QR): does a have a sqrt mod n?
 *   - Factoring with composite promise
 *   - Graph invariant: degree sequence match (necessary for GI)
 *
 * The gap between PROMISE problems (which are easier to find) and
 * LANGUAGES (which Ladner proves exist) is a subtle but important
 * distinction in complexity theory.
 */

#include "ladner.h"

/* ---- Square-Free Numbers ---- */

int is_square_free(int n) {
    if (n <= 1) return 1;
    for (int d = 2; d * d <= n; d++) {
        if (d * d > n) break;
        if (n % (d * d) == 0) return 0;
    }
    return 1;
}

/* Count square-free numbers up to N.
 * Asymptotically, 6/pi^2 ~ 60.8% of numbers are square-free. */
static int count_square_free(int N) {
    int cnt = 0;
    for (int n = 1; n <= N; n++)
        if (is_square_free(n)) cnt++;
    return cnt;
}

/* ---- Quadratic Residuosity ---- */

int is_quadratic_residue(int n, int a) {
    if (n <= 1) return 0;
    a = a % n;
    if (a < 0) a += n;
    for (int x = 1; x < n; x++) {
        if ((x * x) % n == a) return 1;
    }
    return 0;
}

/* Count QR modulo n */
static int count_qr(int n) {
    int cnt = 0;
    for (int a = 1; a < n; a++)
        if (is_quadratic_residue(n, a)) cnt++;
    return cnt;
}

/* ---- Factoring with Promise ---- */

/* Check if n has a small factor (<= 100) */
static int has_small_factor(int n) {
    if (n <= 1) return 0;
    for (int d = 2; d <= 100 && d < n; d++)
        if (n % d == 0) return 1;
    return 0;
}

/* Promise: n is composite. Find smallest factor. */
static int smallest_factor(int n) {
    for (int d = 2; d * d <= n; d++)
        if (n % d == 0) return d;
    return n;  /* prime (promise violated) */
}

/* ---- Graph Degree Sequence Match ---- */

/* deg_sequence_match is defined in graph_isomorphism.c */

/* ---- Demo ---- */

void promise_problems_demo(void) {
    printf("\n===== Promise Problems =====\n\n");

    printf("Promise problem = (YES, NO, PI): decide correctly only\n");
    printf("on instances satisfying promise PI.\n\n");

    /* Square-free */
    printf("--- Square-Free Numbers ---\n");
    printf("n is square-free if no d^2 divides n.\n");
    printf("Square-free <= 30: ");
    for (int n = 2; n <= 30; n++)
        if (is_square_free(n)) printf("%d ", n);
    printf("\n");
    int sf100 = count_square_free(100);
    printf("Count <= 100: %d (%.0f%%)\n", sf100, 100.0 * sf100 / 100);
    printf("(Expect ~61%% asymptotically: 6/pi^2)\n\n");

    /* Quadratic Residuosity */
    printf("--- Quadratic Residuosity ---\n");
    printf("QR mod n: a is QR if exists x with x^2 = a (mod n).\n");
    printf("QR mod 11: ");
    for (int a = 1; a < 11; a++)
        if (is_quadratic_residue(11, a)) printf("%d ", a);
    printf("\n");
    printf("Count QR mod 11: %d (expect (11-1)/2 = 5)\n\n", count_qr(11));

    /* Factoring promise */
    printf("--- Factoring (Promise: n is composite) ---\n");
    printf("Composites <= 200 with factor < 100: ");
    for (int n = 101; n <= 200; n += 10)
        if (has_small_factor(n)) printf("%d ", n);
    printf("\n");
    printf("Smallest factor of 221: %d (expect 13)\n", smallest_factor(221));
    printf("Promise violated (n=prime): smallest factor of 97 = %d (97)\n\n",
           smallest_factor(97));

    /* Graph invariant */
    printf("--- Graph Invariant: Degree Sequence ---\n");
    int d_cycle4[] = {2, 2, 2, 2};       /* 4-cycle */
    int d_path4[]  = {2, 1, 2, 1};       /* 4-path */
    int d_star4[]  = {3, 1, 1, 1};       /* K_{1,3} star */
    printf("deg(4-cycle) == deg(4-cycle): %s\n",
           deg_sequence_match(d_cycle4, d_cycle4, 4) ? "YES" : "NO");
    printf("deg(4-cycle) == deg(4-path):  %s\n",
           deg_sequence_match(d_cycle4, d_path4, 4) ? "YES" : "NO");
    printf("deg(4-cycle) == deg(star):    %s\n",
           deg_sequence_match(d_cycle4, d_star4, 4) ? "YES" : "NO");
    printf("(Necessary but NOT sufficient for isomorphism)\n\n");

    /* Ladner connection */
    printf("--- Ladner Connection ---\n");
    printf("Many NP-intermediate candidates are promise problems.\n");
    printf("But Ladner's construction yields a LANGUAGE defined on\n");
    printf("ALL inputs. This is a stronger result: NP-intermediate\n");
    printf("LANGUAGES exist (if P != NP), not just promise problems.\n");
    printf("The gap between languages and promise problems is subtle\n");
    printf("but important: languages have a decision on EVERY input.\n");
}
