/*
 * padding_argument.c - Padding Arguments for Hierarchy Theorems
 *
 * L4 Fundamental Law:
 *   Padding arguments translate separations between complexity classes
 *   upward using the technique of "padding" inputs with extra symbols.
 *
 *   If DTIME(f(n)) != DTIME(g(n)) for some f,g, then padding can lift
 *   this separation to larger time bounds.
 *
 * Key results enabled by padding:
 *   1. Time Hierarchy Theorem: DTIME(f(n)) != DTIME(f(n)^2 + n)
 *   2. P != EXP (exponential-time analog)
 *   3. NP != NEXP (nondeterministic analog)
 *   4. PSPACE != EXPSPACE (space analog)
 *
 * Padding lemma: If L in DTIME(t(n)), then
 *   pad(L) = { x#^k : x in L, k >= 0 } in DTIME(t(|x|)).
 *   The padding symbol # allows us to "dilate" the input.
 *
 * This module implements:
 *   - Padding function: pad input with delimiter symbols
 *   - Unpadding function: extract original input
 *   - Padding-based separation proofs
 *   - Budget amplification via padding
 *
 * Reference: Sipser 9.1, Arora-Barak 3.1-3.2, Hartmanis-Stearns (1965)
 * Courses: MIT 6.045, Stanford CS254, Berkeley CS172, CMU 15-855
 */

#include "turing_machine.h"
#include "complexity.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/* ---- Padding Operations ---- */

#define PADDING_DELIMITER (-2)  /* # symbol for padding */

/*
 * Pad an input string with k delimiters.
 * padded[i] = input[i] for i < input_len, = PADDING_DELIMITER otherwise.
 * Caller must free the returned array.
 */
Symbol* pad_input(const Symbol* input, int input_len, int k, int* out_len) {
    *out_len = input_len + k;
    Symbol* padded = (Symbol*)malloc((size_t)(*out_len) * sizeof(Symbol));
    if (!padded) return NULL;
    for (int i = 0; i < input_len; i++)
        padded[i] = input[i];
    for (int i = input_len; i < *out_len; i++)
        padded[i] = PADDING_DELIMITER;
    return padded;
}

/*
 * Extract the original input from a padded string.
 * Scans from the start until the first delimiter.
 * Caller must free the returned array.
 */
Symbol* unpad_input(const Symbol* padded, int padded_len, int* out_len) {
    int orig_len = 0;
    while (orig_len < padded_len && padded[orig_len] != PADDING_DELIMITER)
        orig_len++;
    *out_len = orig_len;
    Symbol* orig = (Symbol*)malloc((size_t)orig_len * sizeof(Symbol));
    if (!orig) return NULL;
    memcpy(orig, padded, (size_t)orig_len * sizeof(Symbol));
    return orig;
}

/* Count padding delimiters in a string */
int count_padding(const Symbol* str, int len) {
    int count = 0;
    for (int i = 0; i < len; i++)
        if (str[i] == PADDING_DELIMITER) count++;
    return count;
}

/* ---- Padding Language Construction ---- */

/*
 * Given language L, construct pad(L):
 *   pad(L) = { x#^k : x in L, k >= 0 }
 *
 * We represent a padded language implicitly via a membership oracle.
 * The padding lemma states: if L in DTIME(t(n)), then pad(L) in DTIME(t(n)).
 */
typedef struct {
    int   (*membership)(const Symbol*, int);  /* decides L */
    const char* name;
} Language;

/* Membership test for a padded language */
int padded_membership(const Language* L, const Symbol* w, int w_len) {
    if (!L || !w) return 0;
    /* Count delimiters from the end */
    int orig_len = w_len;
    while (orig_len > 0 && w[orig_len - 1] == PADDING_DELIMITER)
        orig_len--;
    /* Remove trailing delimiters, call original membership */
    return L->membership(w, orig_len);
}

/* ---- Budget Amplification via Padding ---- */

/*
 * Padding can "amplify" computational budgets:
 * Given a TM M running in time t(n) deciding L, we can construct
 * a TM M' running in time O(t(n)) deciding pad(L), but M'
 * has budget amplification: on input of length m, the effective
 * "real" input length is n <= m.
 *
 * This trick is used to prove P != EXP:
 *   - Assume P = EXP (for contradiction)
 *   - Then DTIME(2^n) subseteq DTIME(n^k) for some k
 *   - Apply padding to get DTIME(2^{2^n}) subseteq DTIME(2^{nk})
 *   - Diagonalize against this �� contradiction by Time Hierarchy
 */

typedef struct {
    double  original_budget;    /* t(|x|) for original input */
    double  padded_budget;      /* t(|x#^k|) = t(|x| + k) */
    double  effective_budget;   /* t(|x|) �� the budget on real work */
    int     input_len;
    int     padding_amount;
} BudgetAmplification;

BudgetAmplification compute_budget_amplification(int input_len, int pad_amount,
                                                   double (*time_bound)(int)) {
    BudgetAmplification ba;
    ba.input_len = input_len;
    ba.padding_amount = pad_amount;
    ba.original_budget = time_bound(input_len);
    ba.padded_budget = time_bound(input_len + pad_amount);
    ba.effective_budget = time_bound(input_len);
    return ba;
}

/* Print a budget amplification analysis */
void budget_amplification_print(const BudgetAmplification* ba) {
    if (!ba) return;
    printf("Budget Amplification (padding=%d):\n", ba->padding_amount);
    printf("  Input: %d symbols + %d padding = %d total\n",
           ba->input_len, ba->padding_amount,
           ba->input_len + ba->padding_amount);
    printf("  Original budget: %.1f  |  Padded budget: %.1f  |  Ratio: %.2fx\n",
           ba->original_budget, ba->padded_budget,
           ba->original_budget > 0 ?
           ba->padded_budget / ba->original_budget : 0.0);
}

/* ---- Separation via Padding: P != EXP ---- */

/*
 * Proof sketch that P != EXP (via padding + diagonalization):
 *
 * 1. By Time Hierarchy Theorem, DTIME(2^n) != DTIME(2^{2n}).
 * 2. Assume P = EXP for contradiction.
 *    Then every language in EXP has a polynomial-time algorithm.
 * 3. Specifically, some L in DTIME(2^n) is in P, so L in DTIME(n^k).
 * 4. Apply padding argument: pad(L) in DTIME(n^k) also.
 * 5. But pad(L) can encode DTIME(2^{2n}) computations in DTIME(2^n)
 *    (by using padding symbols as "time budget").
 * 6. Contradiction with Time Hierarchy Theorem.
 * 7. Therefore P != EXP.
 */

/*
 * Demonstrate the padding argument for P != EXP.
 * (We cannot prove it in code �� this illustrates the proof structure.)
 */
void padding_argument_demo_p_vs_exp(void) {
    printf("\n=== Padding Argument: P != EXP ===\n\n");
    printf("Theorem: P is a proper subset of EXP.\n");
    printf("  (P != EXP, and in fact EXP is much larger than P.)\n\n");
    printf("Proof sketch:\n");
    printf("  1. Time Hierarchy Theorem says:\n");
    printf("     DTIME(2^n) is NOT subset of DTIME(2^{n/2})\n\n");
    printf("  2. Assume P = EXP (for contradiction)\n");
    printf("     => DTIME(2^n) subseteq P subseteq DTIME(n^k)\n\n");
    printf("  3. Take L in DTIME(2^n) but L not in DTIME(2^{n/2})\n");
    printf("     (Such L exists by Time Hierarchy)\n\n");
    printf("  4. By assumption, L in DTIME(n^k)\n");
    printf("     Let M be a TM deciding L in time n^k\n\n");
    printf("  5. Construct padded language pad(L):\n");
    printf("     pad(L) = { x#^t : M accepts x within t steps }\n\n");
    printf("  6. pad(L) in DTIME(n^k) (same machine, pad is cheap)\n");
    printf("     But pad(L) encodes DTIME(2^{n^k})!\n\n");
    printf("  7. This contradicts Time Hierarchy Theorem.\n");
    printf("     Hence our assumption must be false: P != EXP.\n");
    printf("     QED\n");
}

/* ---- Explicit Padding TM Construction ---- */

/*
 * Build a Turing machine that decides a padded version of a language.
 * The padded TM first strips padding, then simulates the original TM.
 */
TuringMachine* tm_create_padded_decider(const TuringMachine* base_tm) {
    if (!base_tm) return NULL;
    /* Create a TM with extra states for padding stripping */
    int total_states = base_tm->n_states + 3;
    TuringMachine* tm = tm_create(total_states, 0,
                                    base_tm->q_accept + 3,
                                    base_tm->q_reject + 3,
                                    base_tm->num_trans + 32);

    /* State 0: scan right to find end of non-padding */
    tm_add_transition(tm, 0, 0, 0, 0, TM_RIGHT);
    tm_add_transition(tm, 0, 1, 0, 1, TM_RIGHT);
    tm_add_transition(tm, 0, PADDING_DELIMITER, 1, PADDING_DELIMITER, TM_LEFT);
    tm_add_transition(tm, 0, BLANK_SYMBOL, 1, BLANK_SYMBOL, TM_LEFT);

    /* State 1: replace delimiters with blanks and rewind */
    tm_add_transition(tm, 1, PADDING_DELIMITER, 1, BLANK_SYMBOL, TM_LEFT);
    tm_add_transition(tm, 1, 0, 2, 0, TM_LEFT);
    tm_add_transition(tm, 1, 1, 2, 1, TM_LEFT);
    tm_add_transition(tm, 1, BLANK_SYMBOL, 2, BLANK_SYMBOL, TM_LEFT);

    /* State 2: rewind to start */
    tm_add_transition(tm, 2, 0, 2, 0, TM_LEFT);
    tm_add_transition(tm, 2, 1, 2, 1, TM_LEFT);
    tm_add_transition(tm, 2, BLANK_SYMBOL, 3, BLANK_SYMBOL, TM_RIGHT);

    /* State 3+: offset simulation of base TM states */
    for (int i = 0; i < base_tm->num_trans; i++) {
        TMTransition* t = &base_tm->transitions[i];
        int from = t->from_state + 3;
        int to   = t->to_state + 3;
        /* Remap accept/reject */
        if (t->to_state == base_tm->q_accept)
            to = total_states - 3;  /* new accept */
        else if (t->to_state == base_tm->q_reject)
            to = total_states - 2;  /* new reject */
        tm_add_transition(tm, from, t->read_symbol, to,
                          t->write_symbol, t->direction);
    }

    return tm;
}

/* ---- Padding and Hierarchy: Quantitative Analysis ---- */

/*
 * Given a time bound t(n) and padding amount p(n), compute the
 * largest class that can be "absorbed" via padding into P.
 *
 * If t(n) = 2^{n^k} (exponential) and we pad by 2^{n^k}, then
 * the padded language runs in poly(n) �� effectively putting
 * EXP problems into P via padding.
 *
 * This is of course impossible, hence P != EXP.
 */

typedef struct {
    double original_degree;     /* k where t(n) ~ n^k or 2^{n^k} */
    double padded_degree;       /* k where padded version runs in n^k */
    int    is_hierarchy_violation; /* 1 if padding creates a hierarchy violation */
} PaddingAnalysis;

PaddingAnalysis analyze_padding_effect(double time_exponent,
                                         int pad_exponent) {
    PaddingAnalysis pa;
    pa.original_degree = time_exponent;
    /* After padding input length n -> n + n^{pad_exponent},
       the effective time becomes t(n) on padded input of size ~n^{pad_exponent}.
       In terms of padded input size m = n^{pad_exponent}:
       t(n) = 2^{n^k} = 2^{m^{k/pad_exponent}} */
    pa.padded_degree = time_exponent / (double)pad_exponent;
    /* Hierarchy violation if padded is polynomial (degree <= constant)
       but original is super-polynomial */
    pa.is_hierarchy_violation = (time_exponent > 1.0 && pa.padded_degree <= 10.0);
    return pa;
}

void padding_analysis_print(const PaddingAnalysis* pa) {
    if (!pa) return;
    printf("Padding Analysis:\n");
    printf("  Original time: O(n^%.2f)\n", pa->original_degree);
    printf("  After padding: O(m^%.2f) where m = padded length\n",
           pa->padded_degree);
    printf("  Hierarchy violation? %s\n",
           pa->is_hierarchy_violation ? "YES" : "no");
}

/* Time bound function: quadratic */
static double quadratic_bound(int n) {
    return (double)n * (double)n;
}

/* ---- Complete Padding Argument Demo ---- */

void padding_argument_demo(void) {
    printf("\n============================================\n");
    printf("  Padding Arguments in Complexity Theory\n");
    printf("============================================\n");

    padding_argument_demo_p_vs_exp();

    printf("\n--- Budget Amplification Example ---\n");
    BudgetAmplification ba = compute_budget_amplification(10, 90, quadratic_bound);
    budget_amplification_print(&ba);

    printf("\n--- Padding Effect Analysis ---\n");
    PaddingAnalysis pa = analyze_padding_effect(2.0, 2);
    padding_analysis_print(&pa);
}
