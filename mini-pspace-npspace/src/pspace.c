/* pspace.c — PSPACE Theory: module initializer and glue logic
 *
 * Core functions (qbf_evaluate, savitch_reachability, space_complexity_summary)
 * are implemented in their dedicated files:
 *   qbf_evaluate         → qbf_solver.c
 *   savitch_reachability  → savitch.c
 *   space_complexity_summary → space_classes.c
 *
 * This file provides module-level initialization and cross-cutting utilities. */
#include "pspace.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* ─── Module-level state ─────────────────────────────────────── */

static int module_initialized = 0;

void pspace_module_init(void) {
    if (module_initialized) return;
    module_initialized = 1;
    /* Any global initialization would go here */
}

/* ─── PSPACE class membership test ────────────────────────────── */

/* Test if a problem characterized by time/space bounds belongs to a given
 * complexity class. Uses asymptotic reasoning with configurable thresholds. */
int tm_in_class(int time_bound, int space_bound, ComplexityClass c) {
    switch (c) {
        case CLASS_L:
            return (space_bound <= 20);  /* log-space for demo purposes */
        case CLASS_NL:
            return (space_bound <= 20);
        case CLASS_P:
            return (time_bound <= 100000);
        case CLASS_NP:
            return (time_bound <= 100000);
        case CLASS_PSPACE:
            return (space_bound <= 500);
        case CLASS_NPSPACE:
            return (space_bound <= 500);
        case CLASS_EXP:
            return (time_bound <= 100000000);
        case CLASS_NEXP:
            return (time_bound <= 100000000);
        case CLASS_2EXP:
            return 1;
        default:
            return 0;
    }
}

/* pspace_summary_demo() is defined in pspace_summary.c */
