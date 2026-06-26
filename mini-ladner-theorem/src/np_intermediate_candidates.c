/* np_intermediate_candidates.c -- Natural NP-Intermediate Candidates
 *
 * While Ladner's theorem proves NP-intermediate languages EXIST
 * (if P != NP), identifying NATURAL problems in this category
 * remains one of the great open problems in complexity theory.
 *
 * The strongest contemporary candidates:
 *
 *   1. Graph Isomorphism (GI)
 *      - Babai (2015): quasipolynomial time exp(O(log^c n))
 *      - NOT known NPC: would collapse PH to Sigma_2
 *      - Status: strongest candidate for natural NPI
 *
 *   2. Integer Factoring
 *      - Best classical: GNFS ~ exp(O(n^{1/3}))
 *      - Shor (1994): quantum polynomial time
 *      - NOT known NPC: factoring NPC => PH collapses
 *      - Crypto relevance: RSA security depends on it
 *
 *   3. Discrete Logarithm
 *      - Similar to factoring in complexity status
 *      - Crypto relevance: Diffie-Hellman, DSA
 *
 *   4. Shortest Vector Problem (SVP) approximation
 *      - Best known: exponential (or subexp for certain params)
 *      - Crypto relevance: NIST PQC lattice-based schemes
 *
 *   5. Ring Isomorphism
 *      - Recently shown to be in quasi-poly time (Kayal-NeZh-2019)
 *      - Status: may be intermediate or even in P
 *
 *   6. Group Isomorphism
 *      - Long-standing candidate
 *      - Best known: n^{O(log n)}
 *
 * The Ladner connection: ALL of these are in NP but neither
 * known to be in P nor known to be NPC. They populate the
 * NP-intermediate space that Ladner proved MUST exist.
 */

#include "ladner.h"

/* ---- Candidate Database ---- */

static NPICandidate candidates[] = {
    {"Graph Isomorphism",
     "Are two graphs isomorphic?",
     "exp(O((log n)^c)) -- Babai 2015",
     2015, 1,
     "None directly, but related to hidden subgroup problem"},

    {"Integer Factoring",
     "Given n, find nontrivial factor",
     "exp(O(n^{1/3} log^{2/3} n)) -- GNFS",
     1993, 1,
     "RSA cryptosystem"},

    {"Discrete Logarithm",
     "Given g, h in group G, find x: g^x = h",
     "Subexponential (index calculus)",
     1976, 1,
     "Diffie-Hellman, DSA, ElGamal"},

    {"SVP (approx)",
     "Shortest vector in lattice (approx factor)",
     "exp(O(n)) for exact; poly for exp(n) approx",
     2004, 1,
     "NIST PQC lattice cryptography"},

    {"Ring Isomorphism",
     "Are two rings isomorphic?",
     "Quasi-poly (Kayal-NeZh-2019)",
     2019, 1,
     "Algebraic complexity"},

    {"Group Isomorphism",
     "Are two groups (given by multiplication table) isomorphic?",
     "n^{O(log n)}",
     1978, 1,
     "Group theory, hidden subgroup"},

    {"Unique-SAT",
     "Does formula have EXACTLY ONE solution?",
     "NP-complete (randomized reduction from SAT)",
     1986, 0,
     "Valiant-Vazirani: US in P => NP = RP"},

    {NULL, NULL, NULL, 0, 0, NULL}
};

/* ---- Database Queries ---- */

int npi_candidate_count(void) {
    int count = 0;
    for (int i = 0; candidates[i].name; i++)
        if (candidates[i].likely_npi) count++;
    return count;
}

/* Get candidate by index */
static NPICandidate* npi_get(int idx) {
    int cnt = 0;
    for (int i = 0; candidates[i].name; i++) {
        if (candidates[i].likely_npi) {
            if (cnt == idx) return &candidates[i];
            cnt++;
        }
    }
    return NULL;
}

/* Find candidate by name */
static NPICandidate* npi_find(const char* name) {
    for (int i = 0; candidates[i].name; i++)
        if (strcmp(candidates[i].name, name) == 0)
            return &candidates[i];
    return NULL;
}

/* ---- Analysis Functions ---- */

/* Check if a candidate is in NP (all are) */
static int npi_is_in_np(NPICandidate* c) {
    (void)c;
    return 1;  /* all candidates are in NP */
}

/* Check if crypto-relevant */
static int npi_has_crypto_relevance(NPICandidate* c) {
    return (c->crypto_relevance && strlen(c->crypto_relevance) > 0) ? 1 : 0;
}

/* Estimate "Ladner level": how confident are we this is NPI?
 * Returns 0-100: higher = more likely intermediate */
static int npi_ladner_confidence(NPICandidate* c) {
    int score = 50;  /* baseline */
    if (c->likely_npi) score += 20;
    if (c->year >= 2010) score += 10;  /* recent results support NPI */
    if (strstr(c->best_known, "quasi") || strstr(c->best_known, "Quasi"))
        score += 15;
    if (npi_has_crypto_relevance(c)) score += 5;
    return (score > 100) ? 100 : score;
}

/* ---- Demo ---- */

void np_intermediate_candidates_demo(void) {
    printf("\n===== NP-Intermediate Candidates =====\n\n");

    printf("Ladner's theorem (1975): NP-intermediate languages EXIST\n");
    printf("(if P != NP). But which NATURAL problems are intermediate?\n\n");

    printf("--- Current Candidates ---\n\n");
    printf("%-22s %-30s %6s %3s\n",
           "Problem", "Best Known Algorithm", "Year", "NPI?");
    printf("%-22s %-30s %6s %3s\n",
           "----------------------", "------------------------------",
           "------", "----");

    for (int i = 0; candidates[i].name; i++) {
        NPICandidate* c = &candidates[i];
        printf("%-22s %-30s %6d %3s\n",
               c->name, c->best_known, c->year,
               c->likely_npi ? "YES" : "???");
    }

    /* Crypto analysis */
    printf("\n--- Cryptographic Relevance ---\n");
    for (int i = 0; candidates[i].name; i++) {
        if (npi_has_crypto_relevance(&candidates[i])) {
            printf("  %-22s => %s\n",
                   candidates[i].name, candidates[i].crypto_relevance);
        }
    }

    /* Ladner confidence */
    printf("\n--- Ladner NPI Confidence Scores ---\n");
    printf("  %-22s %6s %s\n", "Problem", "Score", "Status");
    printf("  %-22s %6s %s\n", "----------------------", "------", "------");
    for (int i = 0; candidates[i].name; i++) {
        if (!candidates[i].likely_npi) continue;
        int score = npi_ladner_confidence(&candidates[i]);
        printf("  %-22s %6d %s\n", candidates[i].name, score,
               score >= 80 ? "Strong NPI candidate" :
               score >= 60 ? "Good NPI candidate" : "Weak NPI candidate");
    }

    /* Historical perspective */
    printf("\n--- Historical Perspective ---\n");
    printf("1975: Ladner proves NP-intermediate languages EXIST\n");
    printf("1977: Berman-Hartmanis conjecture (all NPC p-isomorphic)\n");
    printf("1982: Mahaney proves sparse NPC => P=NP\n");
    printf("1986: Valiant-Vazirani: Unique-SAT randomized reduction\n");
    printf("1994: Shor's algorithm: factoring in BQP\n");
    printf("2015: Babai: Graph Isomorphism in quasipolynomial time!\n");
    printf("2019: Kayal-NeZh: Ring Isomorphism in quasipolynomial time\n");
    printf("2026: Still no PROVEN natural NP-intermediate problem.\n\n");

    printf("--- The Big Question ---\n");
    printf("Ladner guarantees NP-intermediate languages EXIST,\n");
    printf("but all known ones are ARTIFICIAL. Finding a NATURAL\n");
    printf("NP-intermediate problem (and proving it) remains one\n");
    printf("of the greatest challenges in complexity theory.\n");
    printf("If GI is proven NP-intermediate, it would be the first\n");
    printf("NATURAL confirmation of Ladner's structural prediction.\n");
}
