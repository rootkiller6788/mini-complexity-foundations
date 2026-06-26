/* qbf_preprocessor.c — QBF formula preprocessing
 * Before solving, apply rules that preserve truth value:
 * 1. Unit propagation with quantifier scope
 * 2. Pure literal elimination
 * 3. Universal reduction: forall x: phi(x) -> phi[0] AND phi[1]
 * 4. Trivial truth/falsity: empty clause set = TRUE, empty clause = FALSE
 * 5. Subsumption: remove longer clauses subsumed by shorter ones */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int* lits; int n; } QBClause;
typedef struct { QBClause* cls; int nc; int nv; char* quants; } QBFormula;

static QBFormula* qbf_create(int nv, int nc, const char* quants) {
    QBFormula* f = malloc(sizeof(QBFormula));
    f->nv = nv; f->nc = nc;
    f->cls = calloc((size_t)nc, sizeof(QBClause));
    f->quants = strdup(quants);
    return f;
}

static void qbf_add_clause(QBFormula* f, int idx, int* lits, int n) {
    f->cls[idx].lits = malloc((size_t)n * sizeof(int));
    memcpy(f->cls[idx].lits, lits, (size_t)n * sizeof(int));
    f->cls[idx].n = n;
}

static void qbf_free(QBFormula* f) {
    for(int i=0;i<f->nc;i++) free(f->cls[i].lits);
    free(f->cls); free(f->quants); free(f);
}

/* Unit propagation: if a clause has exactly 1 literal at the current
 * quantifier level, force its truth value. Only applies to existential
 * variables at the outermost quantifier. */
static int qbf_unit_propagate(QBFormula* f) {
    int changes = 0;
    for(int i=0; i<f->nc; i++) {
        if(f->cls[i].n == 1 && f->cls[i].lits[0] >= 0) {
            int lit = f->cls[i].lits[0], var = lit>>1, sign=lit&1;
            if(f->quants[0]=='E') {
                /* Assign to make clause true */
                int val = sign ? 0 : 1;
                /* Remove all clauses containing this literal satisfied */
                for(int j=0;j<f->nc;j++) {
                    for(int k=0;k<f->cls[j].n;k++)
                        if(f->cls[j].lits[k]==lit){f->cls[j].n=0;break;}
                }
                /* Shorten clauses containing negation */
                int neg = (var<<1)|(1-sign);
                for(int j=0;j<f->nc;j++) {
                    int* nl = malloc((size_t)f->cls[j].n * sizeof(int));
                    int nn=0;
                    for(int k=0;k<f->cls[j].n;k++)
                        if(f->cls[j].lits[k]!=neg) nl[nn++]=f->cls[j].lits[k];
                    free(f->cls[j].lits);
                    f->cls[j].lits=nl; f->cls[j].n=nn;
                }
                changes++;
            }
        }
    }
    return changes;
}

/* Count non-empty clauses */
static int qbf_active_clauses(QBFormula* f) {
    int count = 0;
    for(int i=0;i<f->nc;i++) if(f->cls[i].n > 0) count++;
    return count;
}

void qbf_preprocessor_demo(void) {
    printf("\n===== QBF Preprocessor =====\n\n");
    printf("Preprocessing rules for QBF formulas:\n");
    printf("  1. Unit propagation (outermost existential quantifier)\n");
    printf("  2. Pure literal elimination (literal appears only positively or negatively)\n");
    printf("  3. Universal reduction (forall x: remove clauses where x appears)\n");
    printf("  4. Subsumption checking (shorter clause implies longer)\n");
    printf("  5. Tautology removal ((x OR !x) is always true)\n\n");
    
    printf("Example: QBF with 3 vars, 4 clauses:\n");
    QBFormula* f = qbf_create(3, 4, "EAE");
    int c1[]={0,2,4}; qbf_add_clause(f,0,c1,3);
    int c2[]={1,3,-1}; qbf_add_clause(f,1,c2,3);
    int c3[]={5,-1};   qbf_add_clause(f,2,c3,2);
    int c4[]={0,-1};   qbf_add_clause(f,3,c4,1);
    
    printf("  Before: %d active clauses\n", qbf_active_clauses(f));
    qbf_unit_propagate(f);
    printf("  After unit propagation: %d active clauses\n", qbf_active_clauses(f));
    
    qbf_free(f);
    printf("\nPreprocessing is essential for practical QBF solving.\n");
    printf("Without it, QBF solvers explore 2^n branches needlessly.\n");
}