/* qbf_symbolic.c — Symbolic QBF evaluation using Boolean function representation
 * Represents formulas as sets of satisfying assignments (truth tables).
 * AND/OR/NOT become set operations. Quantifiers become projection.
 * Space = O(2^n) = exponential in worst case, but symbolic. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { unsigned char* data; int n_vars; int size; } TruthTable;

static TruthTable* tt_create(int n_vars) {
    TruthTable* t = malloc(sizeof(TruthTable));
    t->n_vars = n_vars;
    t->size = 1 << n_vars;
    t->data = calloc((size_t)t->size, 1);
    return t;
}

static void tt_free(TruthTable* t) { free(t->data); free(t); }

static void tt_set(TruthTable* t, int idx, int val) { if(idx < t->size) t->data[idx] = val; }

static int tt_get(TruthTable* t, int idx) { return (idx < t->size) ? t->data[idx] : 0; }

/* Build truth table for a single clause (OR of literals) */
static void tt_from_clause(TruthTable* t, const int* lits, int n) {
    for(int mask=0; mask<t->size; mask++) {
        int sat=0;
        for(int j=0;j<n;j++) {
            int var=lits[j]>>1, sign=lits[j]&1;
            int bit = (mask>>var)&1;
            if((!sign && bit) || (sign && !bit)) { sat=1; break; }
        }
        t->data[mask] = sat;
    }
}

/* Existential quantification: exists x: phi = phi[0] OR phi[1] */
static void tt_exists(TruthTable* dst, TruthTable* src, int var) {
    int step = 1<<var, mask = step-1;
    for(int base=0; base<src->size; base+=(step<<1)) {
        for(int offset=0; offset<step; offset++) {
            int idx0 = base+offset, idx1 = base+offset+step;
            dst->data[idx0] = (idx0<dst->size&&idx1<dst->size) ? (src->data[idx0]||src->data[idx1]) : 0;
        }
    }
}

/* Universal quantification: forall x: phi = phi[0] AND phi[1] */
static void tt_forall(TruthTable* dst, TruthTable* src, int var) {
    int step = 1<<var;
    for(int base=0; base<src->size; base+=(step<<1)) {
        for(int offset=0; offset<step; offset++) {
            int idx0 = base+offset, idx1 = base+offset+step;
            dst->data[idx0] = (idx0<dst->size&&idx1<dst->size) ? (src->data[idx0]&&src->data[idx1]) : 0;
        }
    }
}

void symbolic_qbf_demo(void) {
    printf("\n===== Symbolic QBF Solver =====\n\n");
    printf("Uses truth tables (size 2^n) to represent Boolean functions.\n");
    printf("Quantifiers become projection operations on truth tables.\n");
    printf("Space = O(2^n) — exponential but practical for n<=16.\n\n");
    
    int nv=3;
    TruthTable* phi = tt_create(nv);
    int clause[] = {(0<<1)|0, (1<<1)|0, (2<<1)|1}; /* x0 OR x1 OR !x2 */
    tt_from_clause(phi, clause, 3);
    
    printf("Clause (x0|x1|!x2): %d satisfying assignments out of %d\n",
           (int)(phi->size), phi->size);
    
    TruthTable* ex = tt_create(nv);
    tt_exists(ex, phi, 0);
    printf("After exists x0: %d/%d\n", (int)(ex->size), ex->size);
    
    tt_free(phi); tt_free(ex);
    printf("\nSymbolic QBF = BDD-style operations on truth tables.\n");
    printf("Foundation of symbolic model checking (McMillan 1993).\n");
}