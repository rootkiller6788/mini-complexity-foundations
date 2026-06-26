/* degree_structure.c -- NP Degree Structure Under Polynomial-Time Reductions
 *
 * If P != NP, the NP-m-degrees form a dense partial order:
 * between any two comparable degrees, there exists another degree.
 *
 * This is Ladner's structural corollary: not only does ONE intermediate
 * degree exist, but INFINITELY MANY exist, densely ordered.
 *
 * The Low/High hierarchy (Schoning 1983) further stratifies NP sets:
 *   Low_k  = {A in NP | NP^A subset Sigma_k^P}  (weak oracle)
 *   High_k = {A in NP | Sigma_{k+1}^P subset NP^A} (strong oracle)
 *
 * Key property: If Low_k = NP for any k, then PH collapses to Sigma_k.
 * This connects NP degree structure to the Polynomial Hierarchy.
 *
 * References:
 *   - Ladner, JACM 22(1):155-171, 1975
 *   - Schoning, TCS 21, 1982
 *   - Arora & Barak, Section 3.4
 */
#include "ladner.h"

/* ---- Degree Lattice Data ---- */

static NP_Degree deg_lattice[] = {
    {"P",      0,  1, 0,  "All P problems",            "Null oracle"},
    {"Low_1",  1,  1, 0,  "Sparse NP sets",            "Very weak"},
    {"Low_2",  5,  1, 0,  "Graph Isomorphism (maybe)",  "Weak"},
    {"Low_3",  10, 1, 0,  "Factoring (maybe)",          "Weak-moderate"},
    {"Int_1",  25, 0, 0,  "Ladner L_1",                "Intermediate"},
    {"Int_2",  40, 0, 0,  "Ladner L_2",                "Intermediate"},
    {"Int_3",  55, 0, 0,  "Ladner L_3",                "Intermediate"},
    {"Int_4",  70, 0, 0,  "Ladner L_4",                "Intermediate"},
    {"High_1", 85, 0, 1,  "SAT-self-reducible problems","Strong"},
    {"High_2", 95, 0, 1,  "TQBF-like",                  "Very strong"},
    {"NPC",    100,0, 1,  "SAT, CLIQUE, 3SAT",          "Complete"},
    {NULL, 0, 0, 0, NULL, NULL}
};

/* ---- Degree Comparison ---- */

/* Compare two NP degrees by level.
 * Returns: -1 if a < b, 0 if a = b, +1 if a > b */
int deg_cmp(NP_Degree a, NP_Degree b) {
    if (a.level < b.level) return -1;
    if (a.level > b.level) return 1;
    return 0;
}

/* Find the midpoint degree between lo and hi.
 * This demonstrates the DENSITY property: between any two
 * comparable degrees, there exists another degree. */
int deg_interp(int lo, int hi) {
    return lo + (hi - lo) / 2;
}

/* Check if a degree is a Low degree */
int deg_is_low(NP_Degree d) {
    return d.is_low;
}

/* Check if a degree is a High degree */
int deg_is_high(NP_Degree d) {
    return d.is_high;
}

/* Check if a degree is intermediate (neither Low nor High nor NPC) */
int deg_is_intermediate(NP_Degree d) {
    return !d.is_low && !d.is_high && d.level < 100;
}

/* ---- Degree Density Verification ---- */

/* Demonstrate that between any two degrees there exists a third.
 * This is the "density" property proven by Ladner. */
void deg_density_demo(void) {
    printf("\n--- NP Degree Density (Ladner's Structural Corollary) ---\n");
    printf("Between any two comparable NP-m-degrees, there\n");
    printf("exists a third distinct degree.\n\n");

    printf("Examples of intermediate degrees:\n");
    for (int i = 0; deg_lattice[i].name; i++) {
        if (deg_is_intermediate(deg_lattice[i])) {
            int next_level = 100;
            const char* next_name = "NPC";
            for (int j = 0; deg_lattice[j].name; j++) {
                if (deg_lattice[j].level > deg_lattice[i].level &&
                    deg_lattice[j].level < next_level) {
                    next_level = deg_lattice[j].level;
                    next_name = deg_lattice[j].name;
                }
            }
            int mid = deg_interp(deg_lattice[i].level, next_level);
            printf("  %-12s (level %3d) < DEGREE %3d < %-12s (level %3d)\n",
                   deg_lattice[i].name, deg_lattice[i].level,
                   mid, next_name, next_level);
        }
    }
}

/* ---- Low/High Hierarchy ---- */

void deg_low_high_demo(void) {
    printf("\n--- Low/High Hierarchy (Schoning 1983) ---\n");
    printf("Low_k  = {A in NP : NP^A \subseteq \Sigma_k^P}\n");
    printf("High_k = {A in NP : \Sigma_{k+1}^P \subseteq NP^A}\n\n");

    printf("Classification of NP degrees:\n");
    printf("%-12s  %-5s %-5s %s\n", "Degree", "Low?", "High?", "Oracle strength");
    printf("%-12s  %-5s %-5s %s\n", "------", "----", "-----", "----------------");
    for (int i = 0; deg_lattice[i].name; i++) {
        NP_Degree d = deg_lattice[i];
        printf("%-12s  %-5s %-5s %s\n",
               d.name,
               deg_is_low(d) ? "YES" : "NO",
               deg_is_high(d) ? "YES" : "NO",
               d.oracle_strength);
    }
    printf("\nTHEOREM: If Low_k = NP for any k, then PH = \Sigma_k^P.\n");
    printf("This connects NP degree theory to the Polynomial Hierarchy.\n");
}

/* ---- Demo ---- */

void degree_structure_demo(void) {
    printf("\n===== NP Degree Structure =====\n\n");

    printf("Theorem (Ladner 1975): If P != NP, there exist INFINITELY MANY\n");
    printf("distinct NP-m-degrees between P and NPC, densely ordered.\n\n");

    /* Print the degree lattice */
    printf("--- NP Degree Lattice ---\n");
    printf("%-12s %6s %5s %5s %-25s %s\n",
           "Degree", "Level", "Low?", "High?", "Example", "Oracle");
    printf("%-12s %6s %5s %5s %-25s %s\n",
           "------", "-----", "----", "-----", "-------", "------");
    for (int i = 0; deg_lattice[i].name; i++) {
        NP_Degree d = deg_lattice[i];
        printf("%-12s %6d %5s %5s %-25s %s\n",
               d.name, d.level,
               deg_is_low(d) ? "YES" : "NO",
               deg_is_high(d) ? "YES" : "NO",
               d.example, d.oracle_strength);
    }

    /* Comparisons */
    printf("\n--- Degree Comparisons ---\n");
    int p_idx = 0, npc_idx = 10;
    int cmp = deg_cmp(deg_lattice[p_idx], deg_lattice[npc_idx]);
    printf("P < NPC: deg_cmp = %d (expect -1)\n", cmp);

    int int1_idx = 4, int2_idx = 5;
    cmp = deg_cmp(deg_lattice[int1_idx], deg_lattice[int2_idx]);
    printf("Int_1 < Int_2: deg_cmp = %d (expect -1)\n", cmp);

    /* Density */
    deg_density_demo();

    /* Low/High */
    deg_low_high_demo();

    printf("\n--- Structural Summary ---\n");
    printf("1. P is the minimum NP-m-degree (level 0).\n");
    printf("2. NPC is the maximum NP-m-degree (level 100).\n");
    printf("3. Between them: INFINITELY MANY distinct degrees.\n");
    printf("4. The Low/High hierarchy stratifies degrees by oracle power.\n");
    printf("5. If all NP sets were Low (PH collapses) or all High (PH collapses).\n");
    printf("6. Therefore: intermediate degrees are NECESSARY for PH intact.\n\n");

    printf("Ladner's theorem is not just an existence result --\n");
    printf("it reveals the RICH STRUCTURE of the NP degree poset.\n");
}
