/* hierarchy_table.c — Compute and display full time complexity hierarchy
 * For each class from TIME(log n) to TIME(2^{2^n}), show:
 *   - Representative problem, decision procedure, containment relations */
#include <stdio.h>
#include <string.h>

typedef struct { const char* name; const char* bound; const char* example; int level; } Class;

static Class classes[] = {
    {"TIME(log n)",  "log n",    "Binary search", 0},
    {"TIME(n)",      "n",        "Linear scan", 1},
    {"TIME(n log n)","n log n",  "Sorting", 2},
    {"TIME(n^2)",    "n^2",      "Matrix multiply (naive)", 3},
    {"TIME(n^3)",    "n^3",      "Floyd-Warshall", 4},
    {"P",            "poly(n)",  "All above", 5},
    {"NP",           "poly(n) NT","SAT", 6},
    {"PH",           "poly(n) AT","QSAT_k", 7},
    {"PSPACE",       "poly(n) S", "TQBF", 8},
    {"EXP",          "2^{poly}", "Go endgame", 9},
    {"NEXP",         "2^{poly} NT","Succinct SAT", 10},
    {"2-EXP",        "2^{2^{poly}}","Presburger arith", 11},
    {NULL, NULL, NULL, -1}
};

void hierarchy_table(void) {
    printf("\n===== Full Time Complexity Hierarchy =====\n\n");
    printf("%-20s %-12s %-25s %s\n", "Class", "Bound", "Example", "Known?");
    printf("%-20s %-12s %-25s %s\n", "─────", "─────", "───────", "──────");
    for(int i=0; classes[i].name; i++) {
        const char* known;
        if(i<=3) known = "⊊ next (hierarchy)";
        else if(i==4) known = "⊊ EXP (proved)";
        else if(i<=7) known = "⊆ next (open)";
        else if(i==8) known = "⊊ 2-EXP (proved)";
        else known = "⊊ next (hierarchy)";
        printf("%-20s %-12s %-25s %s\n", classes[i].name, classes[i].bound, classes[i].example, known);
    }
    printf("\nKey insight: every other separation is PROVED via hierarchy.\n");
    printf("Only the P⊆NP and NP⊆PSPACE gaps are OPEN.\n");
}