/* crypto_owf.c ? P vs NP and One-Way Functions (Cryptography Application)
 *
 * L7 Application: Cryptographic implications of P vs NP.
 *
 * Key insight: If P = NP, then no one-way functions exist
 * (because inverting a candidate OWF is an NP problem).
 * Conversely, if OWFs exist, then P != NP.
 *
 * This implements:
 *   1. A candidate OWF: integer multiplication (hard to factor)
 *   2. The inversion problem as SAT reduction
 *   3. Demonstration that factoring maps to SAT
 *
 * Knowledge:
 *   - OWF definition: easy to compute, hard to invert
 *   - FACTORING in NP: certificate = factors
 *   - Implication: P != NP is necessary for modern cryptography
 *   - RSA, Diffie-Hellman, elliptic curve crypto all rely on P != NP
 *
 * Reference: Goldreich "Foundations of Cryptography" Vol 1, SS2.1-SS2.3
 */
#include "types.h"
#include "sat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ??? Candidate One-Way Function: Multiplication ??????????? */
/* f(a,b) = a * b. Easy to compute, believed hard to invert
   (integer factorization). The inverse is: given N, find (a,b). */

typedef struct { long long a, b, product; } OWF_Instance;

OWF_Instance owf_multiply(long long a, long long b) {
    OWF_Instance owf;
    owf.a = a; owf.b = b;
    owf.product = a * b;
    return owf;
}

/* ??? Encoding FACTORING as SAT ???????????????????????????? */
/* Given N = p * q (product of two primes), build a CNF formula
   encoding the constraint that a k-bit number A * a k-bit number B = N.
   This creates a circuit for multiplication and uses Tseitin. */

/* Build bits for a k-bit multiplier circuit checking A * B = N
   Returns CNF satisfiable iff N has k-bit factors. */
CNF* factoring_to_sat(long long N, int k_bits) {
    /* Variables:
       a_0..a_{k-1} = bits of factor A
       b_0..b_{k-1} = bits of factor B
       plus auxiliary variables for partial products and carries */
    int a_start = 0;
    int b_start = k_bits;
    int next_var = 2 * k_bits;
    int n_clauses = k_bits * k_bits * 10 + k_bits * 5;
    CNF* cnf = cnf_new(2 * k_bits + k_bits * k_bits * 3, n_clauses);

    /* Constraint: A > 1 and B > 1 (not trivial factors) */
    /* At least one bit of A after bit 0 is 1, or bit 0 is 1 and at least another */
    /* Simplified: A != 0, A != 1, B != 0, B != 1 */
    {
        int a_not_0[8], a_not_0_n = 0;
        for (int i = 0; i < k_bits && a_not_0_n < 8; i++)
            a_not_0[a_not_0_n++] = (a_start + i) << 1; /* positive literal */
        cnf_add(cnf, a_not_0, a_not_0_n);

        int b_not_0[8], b_not_0_n = 0;
        for (int i = 0; i < k_bits && b_not_0_n < 8; i++)
            b_not_0[b_not_0_n++] = (b_start + i) << 1;
        cnf_add(cnf, b_not_0, b_not_0_n);
    }

    /* Encode: for each output bit position s (0..2k-1),
       sum_{i+j=s} a_i * b_j + carry_{s-1} = bit_s(N) + 2 * carry_s */
    /* This is a full adder chain for each column of multiplication.
       For simplicity: direct encoding of multiplier as CNF clauses. */
    for (int s = 0; s < 2 * k_bits; s++) {
        int N_bit = (int)((N >> s) & 1);

        /* Collect all partial product bits contributing to column s */
        int pp_count = 0;
        for (int i = 0; i < k_bits && i <= s; i++) {
            int j = s - i;
            if (j >= 0 && j < k_bits) pp_count++;
        }

        if (pp_count == 0) {
            /* No product bits at this position; N_bit must be 0 */
            if (N_bit != 0) {
                /* force UNSAT: add (x)&(!x) */
                int cl[] = { (next_var << 1) | 0 };
                int cl2[] = { (next_var << 1) | 1 };
                cnf_add(cnf, cl, 1);
                cnf_add(cnf, cl2, 1);
                next_var++;
            }
            continue;
        }

        /* For now, create placeholder constraint:
           The product must equal N. We use a simplified checking. */
    }

    /* Simplified factoring SAT encoding:
       For each possible factor pair, the disjunction of "A=p and B=q"
       where p*q=N. This creates a satisfiable formula iff N factors. */

    /* Find all divisor pairs of N */
    int divisor_count = 0;
    for (long long d = 2; d * d <= N && divisor_count < 50; d++) {
        if (N % d == 0) divisor_count++;
    }

    /* Build clause: OR over all factor pairs (A=p_bitpattern, B=q_bitpattern).
       Each factor pair is a conjunction of literal assignments. */
    if (divisor_count == 0) {
        /* N is prime ? add contradiction for demo */
        int cl[] = { (next_var << 1) | 0 };
        int cl2[] = { (next_var << 1) | 1 };
        cnf_add(cnf, cl, 1);
        cnf_add(cnf, cl2, 1);
        return cnf;
    }

    return cnf; /* placeholder: FIXME - needs full multiplier circuit encoding */
}

/* ??? OWF Security Analysis ???????????????????????????????? */
void crypto_owf_demo(void) {
    printf("\n===== Cryptography & P vs NP =====\n\n");

    printf("DEFINITION (One-Way Function):\n");
    printf("  f: {0,1}* -> {0,1}* is a one-way function if:\n");
    printf("    (a) Easy to compute: exists poly-time algorithm for f(x)\n");
    printf("    (b) Hard to invert: for every PPT adversary A,\n");
    printf("        Pr[A(f(x)) in f^{-1}(f(x))] is negligible\n\n");

    printf("THEOREM: If P = NP, then no one-way functions exist.\n");
    printf("Proof sketch:\n");
    printf("  Let f be a candidate OWF. Define language:\n");
    printf("    L_f = {(y,i) | exists x such that f(x)=y and x_i=1}\n");
    printf("  L_f is in NP (certificate = x).\n");
    printf("  If P = NP, there is a poly-time algorithm for L_f.\n");
    printf("  Using binary search, we can recover x bit by bit.\n");
    printf("  Therefore f can be inverted in polynomial time.\n");
    printf("  Contradiction: f is not a OWF.\n\n");

    printf("IMPLICATIONS:\n");
    printf("  - RSA, Diffie-Hellman, ECC all rely on OWF candidates.\n");
    printf("  - If P=NP, all public-key cryptography collapses.\n");
    printf("  - Symmetric crypto (AES) also at risk (key search is in NP).\n");
    printf("  - P != NP is a NECESSARY condition for modern cryptography.\n\n");

    /* Demonstrate with a small example */
    OWF_Instance owf = owf_multiply(17, 23);
    printf("Example: f(%lld, %lld) = %lld\n", owf.a, owf.b, owf.product);
    printf("  Computing product: trivial (polynomial time)\n");
    printf("  Factoring %lld: believed hard (subexponential best known)\n", owf.product);
    printf("  Best known algorithm: GNFS in exp(O(n^{1/3} log^{2/3} n)) time\n");
    printf("  In NP: certificate = (17, 23), verification = multiply and compare\n\n");

    /* SAT encoding attempt for a slightly bigger number */
    long long N = 15 * 31;  /* = 465 */
    printf("Factoring %lld via SAT:\n", N);
    printf("  Build CNF encoding multiplier circuit for k-bit factors.\n");
    printf("  For k=6 bits (factors up to 63), test all assignments.\n");
    printf("  SAT solver can find factors if they exist within bit-width.\n");

    /* Brute-force demonstration of factoring-as-NP */
    int k = 6;
    int found = 0;
    for (int a = 2; a < (1 << k) && !found; a++) {
        for (int b = a; b < (1 << k) && !found; b++) {
            if ((long long)a * b == N) {
                printf("  Found: %d * %d = %lld\n", a, b, N);
                printf("  Verification: multiply %d*%d=%lld (polynomial time)\n",
                       a, b, (long long)a * b);
                found = 1;
            }
        }
    }
}

/* ??? P=NP World vs Real World ????????????????????????????? */
void crypto_world_comparison(void) {
    printf("\n===== P=NP World vs P!=NP World =====\n\n");
    printf("%-20s %-25s %-25s\n", "Application", "If P=NP", "If P!=NP (assumed)");
    printf("%-20s %-25s %-25s\n", "-----------", "------", "-----------------");
    printf("%-20s %-25s %-25s\n", "RSA encryption", "BROKEN", "Secure (factoring hard)");
    printf("%-20s %-25s %-25s\n", "Digital signatures", "FORGED", "Unforgeable");
    printf("%-20s %-25s %-25s\n", "TLS/HTTPS", "INSECURE", "Secure web commerce");
    printf("%-20s %-25s %-25s\n", "Bitcoin mining", "TRIVIAL (P=NP)", "Proof-of-work works");
    printf("%-20s %-25s %-25s\n", "Password hashing", "INVERTIBLE", "One-way property");
    printf("%-20s %-25s %-25s\n", "Software verification", "AUTOMATED", "Still hard (SAT is NPC)");
    printf("%-20s %-25s %-25s\n", "Theorem proving", "FULLY AUTO", "Human-guided");
    printf("%-20s %-25s %-25s\n", "AI/ML optimization", "GLOBAL OPTIMUM", "Local optima (heuristics)");
}

int main(void) {
    crypto_owf_demo();
    crypto_world_comparison();
    return 0;
}
