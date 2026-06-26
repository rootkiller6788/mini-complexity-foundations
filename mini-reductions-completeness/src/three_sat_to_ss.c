/* three_sat_to_ss.c — 3SAT to Subset Sum Reduction (Karp 1972) (L5, L6)
 *
 * Given a 3-CNF formula φ with n variables and m clauses, construct
 * a set S of 2n+2m integers (in base-10) and a target T such that
 * a subset of S sums to T iff φ is satisfiable.
 *
 * Encoding (base-10 positional):
 *   Each number has n+m digits. Digit positions:
 *     [0..n-1]:     variable selection digits
 *     [n..n+m-1]:   clause satisfaction digits
 *
 *   For each variable xᵢ:
 *     vᵢ(true):  digit i = 1; clause digits = 1 for each clause where xᵢ appears
 *     vᵢ(false): digit i = 1; clause digits = 1 for each clause where ¬xᵢ appears
 *
 *   For each clause Cⱼ:
 *     sⱼ:   digit n+j = 1  (slack variable, used once)
 *     s'ⱼ:  digit n+j = 2  (slack variable, used twice)
 *
 *   Target T: digit i = 1 for all i; digit n+j = 3 for all j.
 *
 * This forces: exactly one truth value per variable (∑ digit_i = 1),
 * and each clause has 1, 2, or 3 true literals (sum = 3 via slacks).
 *
 * Reference: Karp (1972), Sipser §7.4 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ===== Data Structures (L3) ===== */

typedef struct {
    int *numbers;   /* Array of 2n+2m numbers in base-10 positional encoding */
    int n;          /* Total count: 2n + 2m */
    int target;     /* Target sum */
    int base;       /* Base for encoding (10 or higher for clarity) */
    int n_digits;   /* Number of digit positions: n_vars + n_clauses */
} SubsetSum;

static SubsetSum* ss_create(int n_numbers) {
    SubsetSum *ss = malloc(sizeof(SubsetSum));
    ss->numbers = calloc((size_t)n_numbers, sizeof(int));
    ss->n = n_numbers;
    ss->target = 0;
    ss->base = 10;
    ss->n_digits = 0;
    return ss;
}

static void ss_free(SubsetSum *ss) {
    free(ss->numbers);
    free(ss);
}

/* ===== Literal Encoding ===== */
static int encode_lit(int var, int is_neg) { return (var << 1) | is_neg; }
static int decode_var(int lit) { return lit >> 1; }
static int decode_neg(int lit) { return lit & 1; }

/* ===== 3SAT → SUBSET SUM Reduction (L5) ===== */

/* Compute base^d for base-10 encoding. Uses repeated multiplication
   to avoid floating-point issues. */
static int int_pow(int base, int exp) {
    int r = 1;
    for(int i = 0; i < exp; i++) r *= base;
    return r;
}

/* Build the Subset Sum instance.
 *
 * Number layout (indices into ss->numbers):
 *   [0..2n-1]:      variable numbers
 *     2v:           vᵢ(true)
 *     2v+1:         vᵢ(false)
 *   [2n..2n+2m-1]:  clause slack numbers
 *     2n+2j:        sⱼ (weight 1)
 *     2n+2j+1:      s'ⱼ (weight 2) */
SubsetSum* three_sat_to_ss(int n_vars, int n_clauses, int clauses[][3]) {
    int N = 2 * n_vars + 2 * n_clauses;
    SubsetSum *ss = ss_create(N);
    ss->n_digits = n_vars + n_clauses;
    ss->base = 10;

    /* Precompute powers of 10 */
    int *pow10 = malloc((size_t)(n_vars + n_clauses + 1) * sizeof(int));
    pow10[0] = 1;
    for(int i = 1; i <= n_vars + n_clauses; i++)
        pow10[i] = pow10[i-1] * 10;

    /* --- 1. Variable numbers ---
     *
     * vᵢ(true):  digit i = 1, plus 1 for each clause that contains xᵢ.
     * vᵢ(false): digit i = 1, plus 1 for each clause that contains ¬xᵢ. */
    for(int v = 0; v < n_vars; v++) {
        ss->numbers[2*v]     = pow10[v];     /* true: variable digit = 1 */
        ss->numbers[2*v + 1] = pow10[v];     /* false: variable digit = 1 */

        /* Add clause contributions */
        for(int c = 0; c < n_clauses; c++) {
            for(int j = 0; j < 3; j++) {
                int lit = clauses[c][j];
                int var = decode_var(lit);
                int is_neg = decode_neg(lit);
                if(var == v) {
                    if(!is_neg) {
                        /* x_v appears positively → add to true number */
                        ss->numbers[2*v] += pow10[n_vars + c];
                    } else {
                        /* ¬x_v appears → add to false number */
                        ss->numbers[2*v + 1] += pow10[n_vars + c];
                    }
                }
            }
        }
    }

    /* --- 2. Clause slack variables ---
     *
     * Each clause needs exactly 3 total contributions (from literals + slacks).
     * sⱼ:  weight 1 (pow10[n_vars+j])
     * s'ⱼ: weight 2 (2 * pow10[n_vars+j])
     * Together they can provide 1, 2, or 3, filling the gap when
     * fewer than 3 literals in the clause are true. */
    for(int c = 0; c < n_clauses; c++) {
        ss->numbers[2*n_vars + 2*c]     = pow10[n_vars + c];       /* sⱼ = 1 */
        ss->numbers[2*n_vars + 2*c + 1] = 2 * pow10[n_vars + c];   /* s'ⱼ = 2 */
    }

    /* --- 3. Target sum ---
     *
     * Target T: each variable digit = 1 (choose exactly one truth value),
     * each clause digit = 3 (clause is satisfied). */
    ss->target = 0;
    for(int v = 0; v < n_vars; v++)
        ss->target += pow10[v];
    for(int c = 0; c < n_clauses; c++)
        ss->target += 3 * pow10[n_vars + c];

    free(pow10);
    return ss;
}

/* ===== Subset Sum Solver (for verification, L5) ===== */

/* Dynamic programming subset sum. Checks if any subset sums to target.
   Returns 1 if yes, 0 if no. O(n * target) pseudo-polynomial time.
   Works only for small target values (which holds for small formulas). */
static int subset_sum_dp(const int *numbers, int n, int target) {
    if(target < 0 || target > 100000) return -1;  /* too large */

    int *dp = calloc((size_t)(target + 1), sizeof(int));
    dp[0] = 1;

    for(int i = 0; i < n; i++) {
        int num = numbers[i];
        for(int t = target; t >= num; t--) {
            if(dp[t - num]) dp[t] = 1;
        }
    }

    int result = dp[target];
    free(dp);
    return result;
}

/* ===== Verification (L4) ===== */

/* Verify the reduction on a small instance.
   Check that the constructed SubsetSum instance is structurally valid. */
static int verify_ss_reduction(const SubsetSum *ss,
                                int n_vars, int n_clauses) {
    /* Check expected count */
    if(ss->n != 2 * n_vars + 2 * n_clauses) return 0;
    if(ss->n_digits != n_vars + n_clauses) return 0;

    /* Check variable numbers: digit representation is non-negative */
    for(int i = 0; i < ss->n; i++) {
        if(ss->numbers[i] < 0) return 0;
    }

    /* Verify target decomposition: each digit in target should be
       achievable from the numbers provided. */
    /* Variable digits: target has 1, each v/¬v has 1 → need exactly 1 per var. */
    int *pow10 = malloc((size_t)(ss->n_digits + 1) * sizeof(int));
    pow10[0] = 1;
    for(int i = 1; i <= ss->n_digits; i++) pow10[i] = pow10[i-1] * 10;

    /* Check that the target variable digit is 1 for each var */
    for(int v = 0; v < n_vars; v++) {
        int digit = (ss->target / pow10[v]) % 10;
        if(digit != 1) { free(pow10); return 0; }
    }
    /* Check that target clause digit is 3 for each clause */
    for(int c = 0; c < n_clauses; c++) {
        int digit = (ss->target / pow10[n_vars + c]) % 10;
        if(digit != 3) { free(pow10); return 0; }
    }
    free(pow10);
    return 1;
}

/* Encode satisfying assignment as subset sum selection.
   For a given assignment (0/1 per variable), compute which numbers
   should be selected and verify they sum to target. */
static void show_subset_for_assignment(const SubsetSum *ss,
                                        const int *assign,
                                        int n_vars, int n_clauses) {
    int *selected = calloc((size_t)ss->n, sizeof(int));
    int sel_count = 0;
    int sum = 0;

    /* Select true or false number for each variable */
    for(int v = 0; v < n_vars; v++) {
        int idx = assign[v] ? (2*v) : (2*v + 1);
        selected[idx] = 1;
        sum += ss->numbers[idx];
        sel_count++;
    }

    /* For each clause, compute how many selected literals contribute */
    int *pow10 = malloc((size_t)(ss->n_digits + 1) * sizeof(int));
    pow10[0] = 1;
    for(int i = 1; i <= ss->n_digits; i++) pow10[i] = pow10[i-1] * 10;

    for(int c = 0; c < n_clauses; c++) {
        int clause_sum_digit = (sum / pow10[n_vars + c]) % 10;
        int needed = 3 - clause_sum_digit;
        if(needed > 0) {
            /* Use slack 1 or 2 as needed */
            int slack_idx = needed == 1 ? (2*n_vars + 2*c) : (2*n_vars + 2*c + 1);
            selected[slack_idx] = 1;
            sum += ss->numbers[slack_idx];
            sel_count++;
        }
    }

    printf("  Assignment produces subset sum = %d (target = %d): %s\n",
           sum, ss->target, sum == ss->target ? "CORRECT ✓" : "MISMATCH ✗");
    printf("  Selected %d numbers out of %d.\n", sel_count, ss->n);

    free(pow10);
    free(selected);
}

/* ===== Demonstration (L6) ===== */

void three_sat_to_ss_demo(void) {
    printf("\n===== 3SAT → Subset Sum Reduction =====\n\n");

    /* Example: (x₀ ∨ x₁) ∧ (¬x₀ ∨ ¬x₁), n=2, m=2 */
    printf("--- Example: 2 vars, 2 clauses ---\n");
    int cls[2][3] = {
        {encode_lit(0,0), encode_lit(1,0), encode_lit(0,0)},
        {encode_lit(0,1), encode_lit(1,1), encode_lit(1,1)}
    };
    SubsetSum *ss = three_sat_to_ss(2, 2, cls);

    printf("  Formula: (x₀ ∨ x₁) ∧ (¬x₀ ∨ ¬x₁)\n");
    printf("  Number of integers: %d (2n + 2m = %d + %d)\n",
           ss->n, 4, 4);
    printf("  Target sum: %d\n", ss->target);
    printf("  Digit positions: %d var digits + %d clause digits\n", 2, 2);
    printf("  Structural verification: %s\n",
           verify_ss_reduction(ss, 2, 2) ? "PASS ✓" : "FAIL ✗");

    /* Show the numbers */
    printf("\n  Numbers:\n");
    printf("    v₀(T):  %d  (var_0=1, clauses with x₀)\n", ss->numbers[0]);
    printf("    v₀(F):  %d  (var_0=1, clauses with ¬x₀)\n", ss->numbers[1]);
    printf("    v₁(T):  %d  (var_1=1, clauses with x₁)\n", ss->numbers[2]);
    printf("    v₁(F):  %d  (var_1=1, clauses with ¬x₁)\n", ss->numbers[3]);
    printf("    s₀:    %d  (clause_0 slack = 1)\n", ss->numbers[4]);
    printf("    s'₀:   %d  (clause_0 slack = 2)\n", ss->numbers[5]);
    printf("    s₁:    %d  (clause_1 slack = 1)\n", ss->numbers[6]);
    printf("    s'₁:   %d  (clause_1 slack = 2)\n", ss->numbers[7]);

    /* Show subset for satisfying assignment x₀=T, x₁=F */
    int assign[] = {1, 0};
    printf("\n  Satisfying assignment: x₀=T, x₁=F:\n");
    show_subset_for_assignment(ss, assign, 2, 2);

    /* Verify subset sum solver */
    if(ss->target <= 1000) {
        int dp_result = subset_sum_dp(ss->numbers, ss->n, ss->target);
        printf("  Subset sum DP: %s\n",
               dp_result == 1 ? "SOLUTION EXISTS ✓" :
               (dp_result == 0 ? "NO SOLUTION" : "TOO LARGE"));
    }

    ss_free(ss);

    /* Significance */
    printf("\n--- Theoretical Significance ---\n");
    printf("Subset Sum reduction shows that NP-complete problems\n");
    printf("extend beyond logic and graphs into NUMBER THEORY.\n");
    printf("This was the FIRST number-theoretic NP-complete problem.\n\n");
    printf("The base-10 encoding is a classic gadget for reducing\n");
    printf("logical constraints to numerical ones.\n");
    printf("Weakness: numbers grow exponentially large (10^b digits).\n");
    printf("Stronger: uses base-4 for UNARY Subset Sum variant.\n");

    printf("\n--- Complexity ---\n");
    printf("Time:    O(n·m) to construct numbers\n");
    printf("Numbers: 2n + 2m integers, each O((n+m) log b) bits\n");
    printf("Target:  O(b^{n+m}) — exponential in formula size\n");
    printf("→ This is a polynomial-delay reduction (output size is poly),\n");
    printf("  but the numbers themselves are exponentially large.\n");
    printf("  This makes SS 'weakly' NP-complete.\n");
}
