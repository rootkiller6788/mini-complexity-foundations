#ifndef DIAGONAL_LANGUAGE_H
#define DIAGONAL_LANGUAGE_H

/* ── diagonal_language.h — Diagonal Language Construction ──
 * The diagonal language L_D is the heart of the Time Hierarchy Theorem.
 * L_D = {n | M_n rejects n within g(n) steps}
 *
 * L2 Core Concepts: diagonalization, universal simulation
 * L4 Fundamental Laws: Time Hierarchy Theorem
 * L6 Canonical Problems: L_D separation problem
 *
 * Ref: Hartmanis-Stearns (1965), AB §3.1-3.2 */

#include "tht.h"

/* ══════════════════════════════════════════
 * L_D: The Diagonal Language */

/* Compute L_D(n) using time bound function g.
 * L_D ∈ TIME(g) but L_D ∉ TIME(f) when f·log f = o(g). */
int diag_language(int n, TimeBoundFn g_bound);

/* Verify diagonal property: L_D(n) != M_n(n) for all n.
 * Returns number of violations found (should be 0). */
int diag_verify_property(int n_start, int n_end, TimeBoundFn g_bound);

/* ══════════════════════════════════════════
 * Diagonalization Against TM Families */

/* Build a language that separates class C1 from C2.
 * C1 = TIME(f), C2 = TIME(g), f·log f = o(g).
 * Returns: language L ∈ C2 \ C1. */
Language diag_separate_classes(TimeBoundFn f_bound, TimeBoundFn g_bound);

/* Demonstrate that NO machine in family can decide L_D.
 * For each M_i, find n where M_i(n) != L_D(n). */
void diag_find_counterexample(int n_machines, TimeBoundFn g_bound,
                              int *counterexample_n, int *counterexample_i);

/* ══════════════════════════════════════════
 * Cantor's Diagonal Argument (General) */

/* General diagonal function: given any enumeration of functions
 * f_0, f_1, f_2, ..., produce a new function not in the list.
 * f_diag(n) = 1 - f_n(n). */
int cantor_diagonal(int n, int (*f_table)(int index, int input));

/* Verify Cantor: f_diag differs from every f_i on input i. */
int cantor_verify(int n_funcs, int (*f_table)(int index, int input));

/* ══════════════════════════════════════════
 * Baker-Gill-Solovay Diagonalization */

/* Oracle A making P^A = NP^A (TQBF-complete).
 * P^A = NP^A = PSPACE. */
int bgs_oracle_A(int query);

/* Oracle B making P^B != NP^B (sparse diagonalized set).
 * Constructed via delayed diagonalization. */
int bgs_oracle_B(int query);

/* Test: does relativization hold for a given technique?
 * If technique T proves P != NP, then T^O should prove P^O != NP^O for all O.
 * BGS shows diagonalization fails this test. */
int bgs_test_relativization(int (*proof_technique)(Oracle));

#endif
