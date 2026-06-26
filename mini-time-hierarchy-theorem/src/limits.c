/* limits.c — Limits of Diagonalization & Barriers
 * BGS (1975): Relativization barrier
 * Razborov-Rudich (1997): Natural Proofs barrier
 * Aaronson-Wigderson (2009): Algebrization barrier
 *
 * Three fundamental barriers block standard approaches to P vs NP. */

#include <stdio.h>
#include "tht.h"

void limits_demo(void) {
    printf("\n===== Limits of Proof Techniques =====\n\n");

    printf("Three major barriers in complexity theory:\n\n");

    printf("1. RELATIVIZATION (Baker-Gill-Solovay 1975)\n");
    printf("   - Diagonalization proofs work with ANY oracle.\n");
    printf("   - But P vs NP has contradictory relativizations.\n");
    printf("   - Hence: diagonalization cannot resolve P vs NP.\n");
    printf("   - Affected: Time/space hierarchy, Cook-Levin, Ladner.\n");
    printf("   - NOT affected: IP=PSPACE, PCP theorem.\n\n");

    printf("2. NATURAL PROOFS (Razborov-Rudich 1997)\n");
    printf("   - Most known circuit lower bound proofs are 'natural'.\n");
    printf("   - A natural proof has: constructiveness + largeness.\n");
    printf("   - Natural proofs imply breaking pseudorandom generators.\n");
    printf("   - If one-way functions exist, natural proofs cannot prove P!=NP.\n");
    printf("   - Affected: AC0 lower bounds (but they are weak enough).\n\n");

    printf("3. ALGEBRIZATION (Aaronson-Wigderson 2009)\n");
    printf("   - Extends relativization to algebraic oracles.\n");
    printf("   - An oracle is now a low-degree polynomial extension.\n");
    printf("   - IP=PSPACE proof does NOT algebrize.\n");
    printf("   - But many other results DO algebrize.\n\n");

    printf("--- What Survives Each Barrier? ---\n");
    printf("%-30s %-8s %-8s %-8s\n", "Result", "Relat.", "Natural", "Algeb.");
    printf("%-30s %-8s %-8s %-8s\n", "------------------------------", "------", "------", "------");
    printf("%-30s %-8s %-8s %-8s\n", "TIME(f) != TIME(g)", "YES", "n/a", "YES");
    printf("%-30s %-8s %-8s %-8s\n", "P != EXP", "YES", "n/a", "YES");
    printf("%-30s %-8s %-8s %-8s\n", "NP != NEXP", "YES", "n/a", "YES");
    printf("%-30s %-8s %-8s %-8s\n", "L != PSPACE", "YES", "n/a", "YES");
    printf("%-30s %-8s %-8s %-8s\n", "IP = PSPACE", "YES", "n/a", "NO");
    printf("%-30s %-8s %-8s %-8s\n", "PCP Theorem", "?", "n/a", "YES");
    printf("%-30s %-8s %-8s %-8s\n", "P vs NP", "NO", "BLOCKED", "?");
    printf("%-30s %-8s %-8s %-8s\n", "NP vs coNP", "NO", "BLOCKED", "?");
    printf("%-30s %-8s %-8s %-8s\n", "AC0 != NC1 (Ajtai, FSS)", "?", "YES!", "?");

    printf("\n--- Bottom Line ---\n");
    printf("Resolving P vs NP requires techniques that bypass\n");
    printf("ALL three barriers simultaneously. Possible candidates:\\n");
    printf("  - Geometric complexity theory (Mulmuley-Sohoni)\\n");
    printf("  - Interactive proof techniques\\n");
    printf("  - Something completely new...\\n");
}
