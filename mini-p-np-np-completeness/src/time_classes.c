/* time_classes.c ? Complexity class definitions: P, NP, coNP, EXP, NEXP, PSPACE
 *
 * L1: Formal definitions of P, NP, EXP via time-bounded machines
 * L2: Class inclusion relations: P ? NP ? EXP (proven)
 * L4: Time Hierarchy Theorem: DTIME(f(n)) ? DTIME(f(n)?) (stated)
 * L6: Class membership problems
 * L7: Practical implications: cryptography needs NP ? coNP problems
 *
 * Reference: Arora-Barak SS1.3, SS3.1, Sipser SS7.2-SS7.3
 */
#include "time_classes.h"
#include <stdio.h>
#include <time.h>

/* Time a decider function on given input */
double time_decider(PolyTimeDecider d, const int* input, int len) {
    clock_t t0 = clock();
    d(input, len);
    return (double)(clock() - t0) / CLOCKS_PER_SEC;
}

/* Exhaustive NP membership test for small instances.
 * Checks: if answer is YES, there exists a certificate of length <= cert_max_len
 * such that the verifier accepts. */
int np_membership_test(Verifier v, const int* input, int input_len, int cert_max_len) {
    int max_certs = 1;
    for (int i = 0; i < cert_max_len && max_certs < 1000000; i++)
        max_certs *= 2;
    if (max_certs > 1000000) max_certs = 1000000;

    int* cert = (int*)malloc((size_t)cert_max_len * sizeof(int));
    /* Try all possible certificates (exhaustive for small domain) */
    for (int c = 0; c < max_certs; c++) {
        for (int i = 0; i < cert_max_len; i++)
            cert[i] = (c >> i) & 1;
        if (v(input, input_len, cert, cert_max_len))
            { free(cert); return 1; }
    }
    free(cert);
    return 0;
}

/* --- Complexity Zoo --- */
/* Proven inclusion relations. Open problems are handled as "likely" but unproven. */

static const char* zoo_names[] = {
    "P", "NP", "coNP", "EXP", "NEXP", "PSPACE", "NPSPACE",
    "P/poly", "UNKNOWN"
};

const char* class_name(ComplexityClass c) {
    if (c < 0 || c > CLASS_UNKNOWN) return "???";
    return zoo_names[c];
}

/* Known subset relations (proved):
 *   P ? NP ? EXP ? NEXP
 *   P ? coNP ? EXP
 *   NP ? PSPACE (Savitch)
 *   PSPACE ? EXP
 *   NPSPACE = PSPACE (Savitch)
 * Also: P ? EXP (Time Hierarchy), so at least one of P ? NP ? EXP is strict.
 */
int class_is_subset(ComplexityClass a, ComplexityClass b) {
    if (a == b) return 1;
    /* P is subset of everything except maybe P/poly */
    if (a == CLASS_P)
        return b != CLASS_PPOLY;
    if (a == CLASS_NP || a == CLASS_CONP)
        return b == CLASS_PSPACE || b == CLASS_NPSPACE || b == CLASS_EXP || b == CLASS_NEXP;
    if (a == CLASS_PSPACE || a == CLASS_NPSPACE)
        return b == CLASS_EXP || b == CLASS_NEXP;
    if (a == CLASS_EXP) return b == CLASS_NEXP;
    return 0;  /* unknown or unproven */
}

/* Classify a problem by its known algorithmic properties */
ComplexityClass classify_problem(int has_poly_decider,
                                  int has_poly_verifier,
                                  int has_poly_verifier_co) {
    if (has_poly_decider) return CLASS_P;
    if (has_poly_verifier) {
        if (has_poly_verifier_co) return CLASS_CONP;  /* in NP ? coNP */
        return CLASS_NP;
    }
    if (has_poly_verifier_co) return CLASS_CONP;
    return CLASS_EXP;  /* default: at most exponential */
}

/* Print the complexity zoo with class descriptions and known status */
void complexity_zoo_print(void) {
    printf("\n===== Complexity Zoo: Key Classes =====\n\n");
    printf("%-12s %-50s %s\n", "Class", "Description", "Status");
    printf("%-12s %-50s %s\n", "-----", "-----------", "------");

    struct { ComplexityClass c; const char* desc; const char* status; } zoo[] = {
        {CLASS_P,       "Polynomial time (deterministic)",           "Well-understood"},
        {CLASS_NP,      "Nondeterministic polynomial time",          "P vs NP: OPEN ($1M)"},
        {CLASS_CONP,    "Complement of NP",                          "NP vs coNP: OPEN"},
        {CLASS_EXP,     "Exponential time (deterministic)",          "P != EXP (proven)"},
        {CLASS_NEXP,    "Nondeterministic exponential time",         "EXP vs NEXP: OPEN"},
        {CLASS_PSPACE,  "Polynomial space",                          "NP in PSPACE (proven)"},
        {CLASS_NPSPACE, "Nondeterministic poly space",              "NPSPACE=PSPACE (Savitch)"},
        {CLASS_PPOLY,   "P with polynomial advice (non-uniform)",    "NP in P/poly: OPEN"},
        {CLASS_UNKNOWN, "", ""}
    };
    for (int i = 0; zoo[i].c != CLASS_UNKNOWN; i++)
        printf("%-12s %-50s %s\n", zoo_names[zoo[i].c], zoo[i].desc, zoo[i].status);

    printf("\nProven: P != EXP (Time Hierarchy Theorem).\n");
    printf("Open: P vs NP ($1M Clay Prize), NP vs coNP, P vs PSPACE.\n");
    printf("Implication: if P=NP, then P=coNP=PH=PSPACE (collapse).\n");
}

/* --- Polynomial-time example deciders --- */

/* Decide if a number (given as binary array, LSB last) is EVEN: O(n) */
int decider_even(const int* bits, int len) {
    return bits[len-1] == 0;  /* last bit = 0 => even */
}

/* Decide if a string has equal number of 0s and 1s: O(n) */
int decider_balanced(const int* bits, int len) {
    int c0 = 0, c1 = 0;
    for (int i = 0; i < len; i++)
        if (bits[i]) c1++; else c0++;
    return c0 == c1;
}

/* Example verifier for SAT (in NP):
   Given CNF as binary-encoded input and assignment as certificate,
   verify all clauses are satisfied. This demonstrates NP membership. */
static int verifier_sat(const int* input, int input_len,
                        const int* cert, int cert_len) {
    /* input encodes: [n_vars(4B) | n_clauses(4B) | clause_data...]
       cert encodes: [assignment bits...]
       Simplified: verify that cert satisfies input. */
    if (input_len < 8 || cert_len < 1) return 0;
    int n_vars = input[0], n_clauses = input[1];
    if (n_vars > cert_len) return 0;

    int idx = 2; /* current position in input */
    for (int ci = 0; ci < n_clauses; ci++) {
        if (idx >= input_len) return 0;
        int clause_len = input[idx++];
        int satisfied = 0;
        for (int j = 0; j < clause_len && idx < input_len; j++) {
            int lit = input[idx++];
            int var = lit >> 1, sign = lit & 1;
            if (var < n_vars) {
                int val = cert[var];
                if ((!sign && val) || (sign && !val)) satisfied = 1;
            }
        }
        if (!satisfied) return 0;
    }
    return 1;
}
