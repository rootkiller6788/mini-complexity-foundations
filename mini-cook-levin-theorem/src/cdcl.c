/*
 * cdcl.c — CDCL SAT Solver (Conflict-Driven Clause Learning)
 *
 * L5 Algorithm:
 *   CDCL is the dominant paradigm for modern SAT solving.
 *   Key innovations over DPLL:
 *     1. Watched literals: O(1) unit propagation per clause
 *     2. Conflict analysis: learn new clauses from conflicts
 *     3. Non-chronological backtracking: jump to the decision level
 *        responsible for the conflict
 *     4. VSIDS: Variable State Independent Decaying Sum heuristic
 *     5. Restarts: periodically restart to escape unfruitful branches
 *
 * Algorithm CDCL(phi):
 *   1. While true:
 *      a. if unit_propagate() == CONFLICT (returns conflicting clause idx):
 *           if decision_level == 0: return UNSAT
 *           backtrack_level = analyze_conflict(conflict_clause)
 *           backtrack_to(backtrack_level)
 *      b. else:
 *           if all_vars_assigned: return SAT
 *           v = choose_branch_variable()
 *           decision_level++
 *           assign[v] = 0 (try false first)
 *
 * Reference:
 *   Marques-Silva & Sakallah (1996) — GRASP
 *   Moskewicz et al. (2001) — Chaff (watched literals, VSIDS)
 *   Een & Sorensson (2003) — MiniSat
 *   Arora-Barak section 2.4
 *
 * Courses: MIT 6.841, Stanford CS254, Berkeley CS278
 */

#include "sat.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/* ── CDCL Internal State ─────────────────────────────────── */

#define CDCL_MAX_VARS 4096
#define CDCL_MAX_LEARNED 8192
#define MAX_STEPS 500000

typedef struct {
    int watch1_idx;
    int watch2_idx;
} WatchedClause;

typedef struct {
    int value;
    int decision_level;
    int antecedent_clause;
    double activity;
    int saved_phase;   /* phase saving heuristic */
} VarState;

typedef struct {
    CNF*           formula;
    CNF*           learned_db;
    int            n_orig_clauses;
    VarState*      vars;          /* dynamically allocated, size = n_vars */
    int            n_vars;
    int            decision_level;
    int*           trail;
    int            trail_size, trail_cap;
    WatchedClause* watched;
    int            n_watched;
    long long      conflicts, restarts, learned_count, propagations;
    int            restart_interval;
    double         var_decay;
    int            conflict_count_since_restart;
} CDCLSolver;

/* ── Solver Statistics ───────────────────────────────────── */
static long long cdcl_conflict_count = 0;
static long long cdcl_restart_count = 0;
static long long cdcl_learned_count = 0;
static long long cdcl_prop_count = 0;

/* ── Literal evaluation ──────────────────────────────────── */
static int cdcl_lit_value(int lit, const VarState* vars) {
    int var = SAT_LIT_VAR(lit);
    int sign = SAT_LIT_SIGN(lit);
    if (vars[var].value < 0) return -1;
    if (sign) return vars[var].value ? 0 : 1;
    return vars[var].value;
}

/*
 * Unit propagation with watched literals.
 * On conflict, returns the INDEX of the conflicting clause.
 * On success (no conflict), returns -1.
 */
static int cdcl_unit_propagate(CDCLSolver* s) {
    int* q = (int*)malloc((size_t)s->n_vars * sizeof(int));
    int q_head = 0, q_tail = 0;
    int* in_queue = (int*)calloc((size_t)s->n_vars, sizeof(int));

    for (int i = 0; i < s->trail_size; i++) {
        int v = s->trail[i];
        q[q_tail++] = v;
        in_queue[v] = 1;
    }

    while (q_head < q_tail) {
        int var = q[q_head++];
        in_queue[var] = 0;

        for (int ci = 0; ci < s->n_watched; ci++) {
            Clause* c = &s->formula->clauses[ci];
            WatchedClause* w = &s->watched[ci];
            if (c->n == 0) continue;

            int w1_var = (c->n > w->watch1_idx) ? SAT_LIT_VAR(c->lits[w->watch1_idx]) : -1;
            int w2_var = (c->n > w->watch2_idx) ? SAT_LIT_VAR(c->lits[w->watch2_idx]) : -1;

            if (w1_var != var && w2_var != var) continue;

            /* Ensure var is watch1 */
            if (w2_var == var) {
                int tmp = w->watch1_idx;
                w->watch1_idx = w->watch2_idx;
                w->watch2_idx = tmp;
                /* Recompute after swap */
                w1_var = SAT_LIT_VAR(c->lits[w->watch1_idx]);
            }

            /* If watch1 is satisfied, clause is satisfied */
            int w1_val = cdcl_lit_value(c->lits[w->watch1_idx], s->vars);
            if (w1_val == 1) continue;

            /* Try to find a new watch2 that is not false */
            int found_new = 0;
            for (int j = 0; j < c->n; j++) {
                if (j == w->watch1_idx) continue;
                int val = cdcl_lit_value(c->lits[j], s->vars);
                if (val != 0) {
                    w->watch2_idx = j;
                    found_new = 1;
                    break;
                }
            }
            if (found_new) continue;

            /* No replacement. Check watch2 */
            int w2_val = cdcl_lit_value(c->lits[w->watch2_idx], s->vars);
            if (w2_val == 0) {
                /* CONFLICT: both watches are false */
                free(q); free(in_queue);
                s->conflicts++;
                return ci;  /* return conflicting clause index */
            }

            /* watch2 is undef: unit clause, force assignment */
            int lit = c->lits[w->watch2_idx];
            int impl_var = SAT_LIT_VAR(lit);
            int impl_sign = SAT_LIT_SIGN(lit);

            s->vars[impl_var].value = impl_sign ? 0 : 1;
            s->vars[impl_var].decision_level = s->decision_level;
            s->vars[impl_var].antecedent_clause = ci;

            if (s->trail_size >= s->trail_cap) {
                s->trail_cap *= 2;
                s->trail = (int*)realloc(s->trail, (size_t)s->trail_cap * sizeof(int));
            }
            s->trail[s->trail_size++] = impl_var;
            s->propagations++;

            if (!in_queue[impl_var]) {
                q[q_tail++] = impl_var;
                in_queue[impl_var] = 1;
            }
        }
    }
    free(q);
    free(in_queue);
    return -1;  /* no conflict */
}

/*
 * Analyze conflict: compute backtrack level from conflicting clause.
 * Learns a new clause (the conflicting clause) and returns where to backtrack.
 * Returns the backtrack level (placed in *backtrack_level).
 */
static void cdcl_analyze_conflict(CDCLSolver* s, int conflict_ci,
                                   int* backtrack_level) {
    int max_level = 0, second_max = 0;

    if (conflict_ci >= 0 && conflict_ci < s->formula->n_clauses) {
        Clause* c = &s->formula->clauses[conflict_ci];
        for (int j = 0; j < c->n; j++) {
            int var = SAT_LIT_VAR(c->lits[j]);
            int lvl = s->vars[var].decision_level;
            if (lvl > max_level) { second_max = max_level; max_level = lvl; }
            else if (lvl > second_max) second_max = lvl;
            s->vars[var].activity += 1.0;
        }
        /* Learn the clause */
        cnf_add_clause(s->learned_db, c->lits, c->n);
        s->learned_count++;
    } else {
        /* Fallback: use current decision level */
        max_level = s->decision_level;
        second_max = max_level > 0 ? max_level - 1 : 0;
    }

    *backtrack_level = second_max;
}

/* Backtrack to given level.
 * Also flips the phase of the decision variable at level+1 so the
 * next decision at that level tries the opposite value. */
static void cdcl_backtrack(CDCLSolver* s, int level) {
    int new_sz = 0;
    for (int i = 0; i < s->trail_size; i++) {
        int v = s->trail[i];
        if (s->vars[v].decision_level <= level) {
            new_sz++;
        } else {
            s->vars[v].saved_phase = 1 - s->vars[v].value;
            s->vars[v].value = -1;
            s->vars[v].decision_level = 0;
            s->vars[v].antecedent_clause = -1;
        }
    }
    s->trail_size = new_sz;
    s->decision_level = level;
}

/* VSIDS variable selection with phase saving */
static int cdcl_choose_variable(CDCLSolver* s) {
    int best_var = -1;
    double best_act = -1.0;

    for (int v = 0; v < s->n_vars; v++) {
        if (s->vars[v].value < 0 && s->vars[v].activity > best_act) {
            best_var = v;
            best_act = s->vars[v].activity;
        }
    }

    /* Decay activities periodically */
    for (int v = 0; v < s->n_vars; v++)
        s->vars[v].activity *= s->var_decay;

    if (best_var >= 0) {
        /* Phase saving: try the previously successful phase first */
        if (s->vars[best_var].saved_phase >= 0) {
            /* Will be applied at decision time */
        }
    }
    return best_var;
}

/* ── Public CDCL Interface ───────────────────────────────── */

int sat_solve_cdcl(CNF* cnf, Assignment assign) {
    if (!cnf || !assign) return 0;
    int n = cnf->n_vars;
    if (n > CDCL_MAX_VARS)
        return sat_solve_dpll(cnf, assign);

    /* Reset global stats */
    memset(assign, -1, (size_t)n * sizeof(int));

    CDCLSolver s;
    memset(&s, 0, sizeof(s));
    s.formula = cnf;
    s.n_vars = n;
    s.n_orig_clauses = cnf->n_clauses;
    s.decision_level = 0;
    s.var_decay = 0.95;
    s.restart_interval = 200;
    s.trail_cap = 1024;
    s.trail = (int*)malloc((size_t)s.trail_cap * sizeof(int));
    s.trail_size = 0;
    s.vars = (VarState*)calloc((size_t)n, sizeof(VarState));

    for (int v = 0; v < n; v++) {
        s.vars[v].value = -1;
        s.vars[v].decision_level = 0;
        s.vars[v].antecedent_clause = -1;
        s.vars[v].activity = 0.0;
        s.vars[v].saved_phase = 0;
    }

    /* Initialize watches */
    s.n_watched = cnf->n_clauses;
    s.watched = (WatchedClause*)calloc((size_t)s.n_watched, sizeof(WatchedClause));
    for (int ci = 0; ci < cnf->n_clauses; ci++) {
        Clause* c = &cnf->clauses[ci];
        s.watched[ci].watch1_idx = (c->n >= 1) ? 0 : 0;
        s.watched[ci].watch2_idx = (c->n >= 2) ? 1 : 0;
    }

    s.learned_db = cnf_create(n, 1024);

    /* Seed activities from occurrence counts */
    int* occ_cnt = (int*)calloc((size_t)n, sizeof(int));
    for (int ci = 0; ci < cnf->n_clauses; ci++) {
        Clause* c = &cnf->clauses[ci];
        for (int j = 0; j < c->n; j++)
            occ_cnt[SAT_LIT_VAR(c->lits[j])]++;
    }
    for (int v = 0; v < n; v++)
        s.vars[v].activity = (double)occ_cnt[v];
    free(occ_cnt);

    /* Main CDCL loop */
    int result = 0;
    long long step = 0;

    while (step < MAX_STEPS) {
        step++;

        int conflict_ci = cdcl_unit_propagate(&s);
        if (conflict_ci >= 0) {
            if (s.decision_level == 0) {
                result = 0;  /* UNSAT */
                break;
            }
            int bt_level;
            cdcl_analyze_conflict(&s, conflict_ci, &bt_level);
            cdcl_backtrack(&s, bt_level);
            s.conflict_count_since_restart++;
            continue;
        }

        /* All vars assigned? */
        int all_done = 1;
        for (int v = 0; v < n; v++)
            if (s.vars[v].value < 0) { all_done = 0; break; }

        if (all_done) {
            for (int v = 0; v < n; v++) assign[v] = s.vars[v].value;
            result = sat_verify(cnf, assign) ? 1 : 0;
            break;
        }

        /* Restart if too many conflicts since last restart */
        if (s.conflict_count_since_restart > s.restart_interval && s.decision_level > 0) {
            cdcl_backtrack(&s, 0);
            s.restarts++;
            s.conflict_count_since_restart = 0;
            s.restart_interval = s.restart_interval * 11 / 10;
            if (s.restart_interval > 5000) s.restart_interval = 5000;
            continue;
        }

        int pivot = cdcl_choose_variable(&s);
        if (pivot < 0) {
            /* All vars assigned but formula not satisfied? Shouldn't happen */
            for (int v = 0; v < n; v++) assign[v] = s.vars[v].value;
            result = 0;
            break;
        }

        s.decision_level++;
        /* Use phase saving for initial value */
        s.vars[pivot].value = s.vars[pivot].saved_phase;
        s.vars[pivot].decision_level = s.decision_level;
        s.vars[pivot].antecedent_clause = -1;

        if (s.trail_size >= s.trail_cap) {
            s.trail_cap *= 2;
            s.trail = (int*)realloc(s.trail, (size_t)s.trail_cap * sizeof(int));
        }
        s.trail[s.trail_size++] = pivot;
    }

    /* Propagate global stats */
    cdcl_conflict_count += s.conflicts;
    cdcl_restart_count += s.restarts;
    cdcl_learned_count += s.learned_count;
    cdcl_prop_count += s.propagations;

    free(s.trail);
    free(s.watched);
    cnf_free(s.learned_db);
    return result;
}

/* ── Auto-Select Solver ──────────────────────────────────── */

int sat_solve(CNF* cnf, Assignment assign) {
    if (!cnf) return 0;
    if (cnf->n_clauses >= 200 && cnf->n_vars <= CDCL_MAX_VARS)
        return sat_solve_cdcl(cnf, assign);
    return sat_solve_dpll(cnf, assign);
}

void sat_cdcl_stats(void) {
    printf("CDCL Statistics:\n");
    printf("  Conflicts:   %lld\n", cdcl_conflict_count);
    printf("  Restarts:    %lld\n", cdcl_restart_count);
    printf("  Learned:     %lld\n", cdcl_learned_count);
    printf("  Propagations:%lld\n", cdcl_prop_count);
}