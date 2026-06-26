/* reduction_catalog.c — Karp's 21 NP-Complete Problems (1972) (L6)
 *
 * Richard Karp's landmark 1972 paper "Reducibility Among Combinatorial
 * Problems" established 21 problems as NP-complete by constructing a
 * chain of polynomial-time many-one reductions from SAT.
 *
 * This catalog provides:
 *   - Complete list of the 21 problems
 *   - Reduction source for each
 *   - Category classification (Graph, Number, Logic, Set)
 *   - Formal problem descriptions
 *   - Cross-references to Garey & Johnson (1979)
 *
 * Historical significance: Karp's paper demonstrated that NP-completeness
 * is NOT an isolated property of SAT but PERVASIVE across all areas of
 * combinatorial optimization. It established NP-completeness as the
 * central organizing principle of computational complexity.
 *
 * Reference: Karp, R.M. (1972). "Reducibility Among Combinatorial Problems."
 *   In: Complexity of Computer Computations, Plenum Press, pp. 85-103.
 * Reference: Garey & Johnson (1979), "Computers and Intractability" */

#include "redcomp.h"
#include <string.h>
#include <stdio.h>

/* ===== Full Catalog Entry (L3, L6) ===== */

typedef struct {
    int id;
    const char *name;
    const char *abbrev;
    const char *reduction_from;
    const char *reduction_from_name;
    const char *category;
    const char *subcategory;
    const char *instance_description;
    const char *question;
    int gj_ref_number;        /* Garey & Johnson reference number */
    int year_first_proved;    /* Year first proved NP-complete */
    const char *first_proved_by;
    int is_strongly_npc;      /* 1 if strongly NP-complete */
    int has_ptas;              /* 1 if PTAS exists */
    int best_approx_ratio;    /* Best known approximation ratio (×100) */
} NPCProblem;

static const NPCProblem catalog[] = {
    {1,  "SATISFIABILITY",       "SAT",    "--",    "--",
     "Logic", "Propositional", "Set C of clauses (disjunctions of literals)",
     "Is there a truth assignment satisfying all clauses?",
     1, 1971, "Cook / Levin", 0, 0, -1},

    {2,  "3-SATISFIABILITY",     "3SAT",   "SAT",   "SATISFIABILITY",
     "Logic", "Propositional", "3-CNF formula",
     "Is there a truth assignment satisfying the 3-CNF?",
     2, 1972, "Karp (via Cook)", 0, 0, -1},

    {3,  "3-DIMENSIONAL MATCHING","3DM",   "3SAT",  "3SAT",
     "Graph", "Matching", "Set M ⊆ W×X×Y, where |W|=|X|=|Y|=q",
     "Does M contain a matching of size q?",
     19, 1972, "Karp", 0, 0, -1},

    {4,  "CLIQUE",               "CLIQUE", "3SAT",  "3SAT",
     "Graph", "Subgraph", "Graph G, integer k",
     "Does G contain a complete subgraph of size k?",
     8, 1972, "Karp", 0, 0, -1},

    {5,  "VERTEX COVER",         "VC",     "3SAT",  "3SAT",
     "Graph", "Covering", "Graph G, integer k",
     "Is there a set of ≤k vertices covering all edges?",
     7, 1972, "Karp", 0, 0, 200},

    {6,  "SET COVER",            "SETCOV", "VC",    "VERTEX COVER",
     "Set",   "Covering", "Collection C of subsets of S, integer k",
     "Does C contain a cover of S of size ≤k?",
     4, 1972, "Karp", 0, 0, -1},

    {7,  "HITTING SET",          "HS",     "VC",    "VERTEX COVER",
     "Set",   "Covering", "Collection C of subsets of S, integer k",
     "Is there H⊆S, |H|≤k, hitting all sets in C?",
     5, 1972, "Karp", 0, 0, -1},

    {8,  "FEEDBACK VERTEX SET",  "FVS",    "VC",    "VERTEX COVER",
     "Graph", "Acyclic", "Digraph G, integer k",
     "Is there a set of ≤k vertices covering all cycles?",
     12, 1972, "Karp", 0, 0, 200},

    {9,  "FEEDBACK ARC SET",     "FAS",    "VC",    "VERTEX COVER",
     "Graph", "Acyclic", "Digraph G, integer k",
     "Is there a set of ≤k arcs covering all cycles?",
     13, 1972, "Karp", 0, 0, -1},

    {10, "DIRECTED HAMILTONIAN CYCLE","DHC","3SAT","3SAT",
     "Graph", "Path/Cycle", "Directed graph G",
     "Does G contain a directed Hamiltonian cycle?",
     14, 1972, "Karp", 0, 0, -1},

    {11, "UNDIRECTED HAMILTONIAN CYCLE","UHC","DHC","DIRECTED HC",
     "Graph", "Path/Cycle", "Undirected graph G",
     "Does G contain a Hamiltonian cycle?",
     15, 1972, "Karp", 0, 0, -1},

    {12, "TRAVELING SALESMAN",   "TSP",    "UHC",   "UNDIRECTED HC",
     "Graph", "Path/Cycle", "Complete graph with edge weights, bound B",
     "Is there a Hamiltonian cycle of total weight ≤B?",
     16, 1972, "Karp", 1, 0, -1},

    {13, "3-COLORING",           "3COL",   "3SAT",  "3SAT",
     "Graph", "Coloring", "Graph G",
     "Can G be colored with 3 colors?",
     17, 1972, "Karp / Stockmeyer", 0, 0, -1},

    {14, "CLIQUE COVER",         "CC",     "3COL",  "3-COLORING",
     "Graph", "Covering", "Graph G, integer k",
     "Can vertices be partitioned into ≤k cliques?",
     9, 1972, "Karp", 0, 0, -1},

    {15, "EXACT COVER BY 3-SETS","X3C",    "3DM",   "3D MATCHING",
     "Set",   "Covering", "Set X with |X|=3q, collection C of 3-subsets",
     "Does C contain an exact cover of X?",
     24, 1972, "Karp", 0, 0, -1},

    {16, "SET PACKING",          "SP",     "CLIQUE","CLIQUE",
     "Set",   "Packing", "Collection C of sets, integer k",
     "Are there k mutually disjoint sets in C?",
     3, 1972, "Karp", 0, 0, -1},

    {17, "PARTITION",            "PART",   "3DM",   "3D MATCHING",
     "Number","Partition", "Set A of numbers (or multiset)",
     "Can A be partitioned into two subsets with equal sum?",
     28, 1972, "Karp", 0, 0, -1},

    {18, "KNAPSACK",             "KNAP",   "PART",  "PARTITION",
     "Number","Packing", "Weights w_i, values v_i, capacity W, target V",
     "Is there a subset of total weight ≤W and value ≥V?",
     27, 1972, "Karp", 0, 0, -1},

    {19, "SUBSET SUM",           "SSUM",   "3DM",   "3D MATCHING",
     "Number","Sum", "Set S of integers, target t",
     "Is there a subset of S summing to t?",
     26, 1972, "Karp", 0, 0, -1},

    {20, "0-1 INTEGER PROGRAMMING","01IP", "SSUM", "SUBSET SUM",
     "Number","Programming","Integer matrix A, vector b, 0-1 variables x",
     "Is there a 0-1 vector x such that Ax ≤ b?",
     25, 1972, "Karp", 0, 0, -1},

    {21, "MAX 2-SAT",            "MAX2SAT","3SAT", "3SAT",
     "Logic", "Optimization", "2-CNF formula, integer k",
     "Is there an assignment satisfying ≥k clauses?",
     18, 1972, "Karp", 0, 0, -1},
};

/* Problem sizes for demonstration (instance complexity) */
typedef struct {
    int typical_n;        /* Typical instance size */
    int best_known_algo;  /* Best known algorithm time exponent (k for O(n^k)) */
    int trivial_bound;    /* Trivial algorithm exponent */
} NPCProblemComplexity;

static const NPCProblemComplexity complexity[] = {
    {1000, 2, 2},   /* SAT: typical 1000 vars, best = O(2^n) */
    {1000, 2, 2},   /* 3SAT */
    {100, 2, 2},
    {100, 2, 3},    /* CLIQUE: O(n^3) naive, best exp */
    {200, 2, 3},    /* VC: 2-approx, O(1.2^n) exact */
    {500, 2, 3},
    {500, 2, 3},
    {200, 2, 2},
    {200, 2, 2},
    {100, 2, 2},
    {100, 2, 2},
    {100, 2, 2},
    {100, 2, 3},
    {100, 2, 2},
    {300, 2, 2},
    {200, 2, 2},
    {100, 2, 2},
    {100, 2, 2},
    {100, 2, 2},
    {50, 2, 2},
    {100, 2, 2},
};

/* ===== Category Analysis ===== */

typedef struct {
    const char *category;
    int count;
    int ids[8];      /* IDs of problems in this category */
} CategoryStats;

static void analyze_categories(void) {
    CategoryStats stats[5];
    const char *cats[] = {"Graph", "Number", "Logic", "Set", NULL};
    for (int c = 0; cats[c]; c++) {
        stats[c].category = cats[c];
        stats[c].count = 0;
    }

    for (int i = 0; i < 21; i++) {
        for (int c = 0; cats[c]; c++) {
            if (strcmp(catalog[i].category, cats[c]) == 0) {
                int idx = stats[c].count;
                if (idx < 8) stats[c].ids[idx] = catalog[i].id;
                stats[c].count++;
                break;
            }
        }
    }

    printf("--- Category Distribution ---\n\n");
    for (int c = 0; cats[c]; c++) {
        printf("  %-8s: %2d problems  (",
               stats[c].category, stats[c].count);
        for (int j = 0; j < stats[c].count && j < 8; j++) {
            printf("%s%d", j > 0 ? ", " : "", stats[c].ids[j]);
        }
        printf(")\n");
    }
}

/* ===== Reduction Chain Analysis ===== */

/* Build and display the reduction DAG */
static void print_reduction_tree(void) {
    printf("\n--- Reduction Chain (DAG rooted at SAT) ---\n\n");

    /* SAT roots the chain */
    int from_sat = 0, from_3sat = 0, from_vc = 0, from_other = 0;
    for (int i = 0; i < 21; i++) {
        if (strcmp(catalog[i].reduction_from, "--") == 0) from_sat++;
        else if (strcmp(catalog[i].reduction_from, "3SAT") == 0) from_3sat++;
        else if (strcmp(catalog[i].reduction_from, "VC") == 0 ||
                 strcmp(catalog[i].reduction_from, "D-HC") == 0 ||
                 strcmp(catalog[i].reduction_from, "U-HC") == 0) from_vc++;
        else from_other++;
    }

    printf("  SAT (root) ──> %d direct reductions\n", from_3sat + 1);
    printf("   └─ 3SAT ──> %d reductions\n", from_3sat);
    printf("        └─> CLIQUE, VC, DHC, 3COL, 3DM\n");
    printf("             ├─ VC ──> SETCOV, HS, FVS, FAS  (4)\n");
    printf("             ├─ DHC ──> UHC ──> TSP\n");
    printf("             ├─ 3COL ──> CC\n");
    printf("             ├─ 3DM ──> X3C, PART, SSUM\n");
    printf("             └─ CLIQUE ──> SP\n");
    printf("  Chain depth: 4 (SAT → 3SAT → ... → TSP)\n\n");
}

/* ===== Garey & Johnson Cross-Reference ===== */

/* Map Karp IDs to Garey & Johnson reference numbers.
   GJ (1979) standardized the notation and added hundreds more NPC problems. */
static void print_gj_references(void) {
    printf("--- Garey & Johnson (1979) Cross-Reference ---\n\n");
    printf("  Karp ID  Problem                   GJ Ref  Notes\n");
    printf("  -------  ------------------------  ------  -----\n");
    for (int i = 0; i < 21; i++) {
        printf("    %2d     %-24s  [SP%02d]  %s\n",
               catalog[i].id, catalog[i].name,
               catalog[i].gj_ref_number,
               catalog[i].is_strongly_npc ? "strongly NPC" : "NPC");
    }
}

/* ===== Demonstration ===== */

void catalog_demo(void) {
    printf("\n===== Karp's 21 NP-Complete Problems (1972) =====\n\n");

    printf("R.M. Karp: 'Reducibility Among Combinatorial Problems'\\n");
    printf("Published in: Complexity of Computer Computations (1972)\\n");
    printf("21 problems proven NP-complete via reduction from SAT.\\n\\n");

    /* List all problems */
    printf("--- Complete Catalog ---\n\n");
    printf("  ID  Problem                Reduced From     Category\\n");
    printf("  --- ---------------------- ---------------  --------\\n");
    for (int i = 0; i < 21; i++) {
        printf("  %2d  %-22s  %-15s  %s\\n",
               catalog[i].id, catalog[i].name,
               catalog[i].reduction_from, catalog[i].category);
    }

    /* Descriptive list */
    printf("\n--- Full Problem Descriptions ---\n\n");
    for (int i = 0; i < 21; i++) {
        printf("  [%2d] %s (%s)\\n", catalog[i].id,
               catalog[i].name, catalog[i].abbrev);
        printf("       Instance: %s\\n", catalog[i].instance_description);
        printf("       Question: %s\\n", catalog[i].question);
        printf("       Reduces from: %s\\n", catalog[i].reduction_from_name);
        printf("       First proved by: %s (%d)\\n\\n",
               catalog[i].first_proved_by, catalog[i].year_first_proved);
    }

    /* Category analysis */
    analyze_categories();

    /* Reduction tree */
    print_reduction_tree();

    /* Garey & Johnson references */
    print_gj_references();

    /* Historical impact */
    printf("\n--- Historical Significance ---\n\n");
    printf("Karp's paper is one of the most influential in CS:\\n");
    printf("  - Established NP-completeness as a ROBUST phenomenon.\\n");
    printf("  - Showed problems from graphs, numbers, logic, and sets\\n");
    printf("    are ALL equivalent under polynomial reductions.\\n");
    printf("  - Gave a TEMPLATE for proving NP-completeness: reduce\\n");
    printf("    from a known NPC problem using a gadget construction.\\n");
    printf("  - Led to Garey & Johnson's 'Computers and Intractability'\\n");
    printf("    (1979), which catalogs 300+ NPC problems.\\n");
    printf("  - Today, THOUSANDS of problems are known to be NPC.\\n");
    printf("  - Cited >20,000 times.\\n\\n");

    printf("Key philosophical message:\\n");
    printf("  'If P != NP, then NONE of these 21 problems has\\n");
    printf("   a polynomial-time solution. If P = NP, then ALL do.'\\n");
    printf("  This dichotomous nature makes P vs NP the most\\n");
    printf("  important open question in computer science.\\n");
}
