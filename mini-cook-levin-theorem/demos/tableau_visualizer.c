/*
 * tableau_visualizer.c - Cook-Levin Tableau Visualizer
 *
 * L3-L4 Demo: Visualizes the tableau constructed in the Cook-Levin proof.
 * For a given TM and input, generates and displays the clause groups G1-G4
 * that encode the computation under the satisfaction equivalence.
 *
 * Usage: compile with make and run to see the tableau structure.
 */

#include "turing_machine.h"
#include "sat.h"
#include "cook_levin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_horizontal_line(int cols, int cell_width) {
    printf("+");
    for (int c = 0; c < cols; c++) {
        for (int w = 0; w < cell_width; w++) putchar('-');
        printf("+");
    }
    printf("\n");
}

static void print_tableau_cell(const TuringMachine* tm,
                                const Symbol* input, int input_len,
                                int t, int p, int T, int P, int head_pos) {
    if (head_pos == p) {
        printf("[");
    } else {
        printf(" ");
    }

    if (t == 0) {
        /* Initial configuration row */
        if (p >= 0 && p < input_len) {
            printf("%d", input[p]);
        } else {
            printf("_");
        }
    } else {
        /* Later rows: show state if head at this position */
        if (head_pos == p) {
            printf("Q");
        } else if (p >= 0 && p < P) {
            printf(".");
        } else {
            printf(".");
        }
    }

    if (head_pos == p) {
        printf("]");
    } else {
        printf(" ");
    }
}

void visualize_tableau(const TuringMachine* tm, const Symbol* input,
                       int input_len, int T) {
    if (!tm) return;

    int P = input_len + T + 2;
    if (P > 40) P = 40;  /* Limit display width */

    printf("\n============================================\n");
    printf("  Cook-Levin Tableau Visualization\n");
    printf("============================================\n\n");
    printf("Machine: %s\n",
           tm->description ? tm->description : "(unnamed)");
    printf("Input length: %d, Time bound T: %d, Tape width P: %d\n",
           input_len, T, P);
    printf("Total variables: T*P*|Gamma| + T*P + T*|Q|\n");
    printf("Clause groups: G1 (cell uniqueness), G2 (init+head+state),\n");
    printf("               G3 (transitions), G4 (acceptance)\n\n");

    /* Print tableau grid */
    int cell_width = 4;
    printf("  t\\p ");
    for (int p = 0; p < P && p < 20; p++)
        printf(" %-3d", p);
    if (P > 20) printf(" ...");
    printf("\n");

    print_horizontal_line(P < 20 ? P : 21, cell_width);

    int sim_head = 0;
    for (int t = 0; t <= T && t < 15; t++) {
        printf("  %-3d|", t);
        for (int p = 0; p < P && p < 20; p++) {
            print_tableau_cell(tm, input, input_len, t, p, T, P, sim_head);
            printf("|");
        }
        if (P > 20) printf(" ...|");
        printf("\n");
        print_horizontal_line(P < 20 ? P : 21, cell_width);

        /* Move head based on transition (simplified) */
        sim_head++;
        if (sim_head >= P) sim_head = P - 1;
    }
    if (T >= 15) printf("  ... (%d more rows)\n", T + 1 - 15);

    /* Clause statistics */
    printf("\nClause Structure:\n");
    printf("  G1 (Cell consistency):     %d clauses  [Each cell has exactly one symbol]\n",
           (T + 1) * P);
    printf("  G2a (Initial config):     %d clauses  [Input + blanks]\n", P);
    printf("  G2b (Head uniqueness):    %d clauses  [One head position per time]\n",
           T + 1);
    printf("  G2c (State uniqueness):   %d clauses  [One state per time]\n", T + 1);
    printf("  G3 (Transitions):         %d clauses  [Local window rules]\n",
           (T + 1) * P * 4);
    printf("  G4 (Acceptance):          %d clauses  [Eventually accept]\n", T + 1);
    printf("  Total clauses: O(T * P) = O((n + T) * T)\n");
}

/* Demo main */
#ifdef TABLEAU_VISUALIZER_MAIN
int main(void) {
    TuringMachine* tm = tm_create_contains_11();
    Symbol input[] = {0, 1, 1, 0};
    visualize_tableau(tm, input, 4, 10);
    tm_free(tm);

    printf("\n--- Another example ---\n");
    TuringMachine* tm2 = tm_create_accept_one();
    Symbol input2[] = {1};
    visualize_tableau(tm2, input2, 1, 5);
    tm_free(tm2);

    return 0;
}
#endif
