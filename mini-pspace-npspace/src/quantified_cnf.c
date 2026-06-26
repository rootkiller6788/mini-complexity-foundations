/* quantified_cnf.c — QDIMACS format parser for QBF
 * Standard format for QBF competitions (QBFLIB, QBFEVAL).
 * Reads quantifier prefix + CNF clauses from QDIMACS files. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { int n_vars; int n_clauses; char* prefix; int** clauses; int* clause_lens; } QDIMACS;

static QDIMACS* qdimacs_parse(const char* str) {
    QDIMACS* q = calloc(1, sizeof(QDIMACS));
    const char* p = str;
    while(*p) {
        while(*p && isspace(*p)) p++;
        if(!*p) break;
        if(*p=='c') { while(*p && *p!='\n') p++; continue; }
        if(*p=='p') {
            sscanf(p, "p cnf %d %d", &q->n_vars, &q->n_clauses);
            q->prefix = calloc((size_t)(q->n_vars+1), 1);
            q->clauses = malloc((size_t)q->n_clauses * sizeof(int*));
            q->clause_lens = malloc((size_t)q->n_clauses * sizeof(int));
            while(*p && *p!='\n') p++;
        } else if(*p=='e' || *p=='a') {
            char type = *p; p++;
            while(*p && *p!='\n') {
                int var; if(sscanf(p,"%d",&var)==1) {
                    if(var>0 && var<=q->n_vars) q->prefix[var-1]=toupper(type);
                    while(*p && !isspace(*p)) p++;
                }
                while(*p && isspace(*p)) p++;
            }
        } else if(isdigit(*p) || *p=='-') {
            static int clause_idx=0;
            int lits[256], n=0;
            while(*p && *p!='\n') {
                int val; if(sscanf(p,"%d",&val)==1 && val!=0) {
                    int var=abs(val)-1, sign=(val<0)?1:0;
                    if(var>=0 && var<q->n_vars) lits[n++]=(var<<1)|sign;
                    while(*p && !isspace(*p)) p++;
                } else if(val==0) break;
                while(*p && isspace(*p)) p++;
            }
            if(n>0 && clause_idx<q->n_clauses) {
                q->clauses[clause_idx]=malloc((size_t)n*sizeof(int));
                memcpy(q->clauses[clause_idx],lits,(size_t)n*sizeof(int));
                q->clause_lens[clause_idx]=n;
                clause_idx++;
            }
        }
        while(*p && *p!='\n') p++;
        if(*p=='\n') p++;
    }
    return q;
}

void qdimacs_demo(void) {
    printf("\n===== QDIMACS Parser =====\n\n");
    printf("QDIMACS format: standard for QBF problems.\n");
    printf("  p cnf <vars> <clauses>\n");
    printf("  e 1 2 3 0   (existential vars)\n");
    printf("  a 4 5 0     (universal vars)\n");
    printf("  clauses in DIMACS format...\n\n");
    
    const char* example = "c QDIMACS example\np cnf 2 2\ne 1 0\na 2 0\n1 2 0\n-1 -2 0\n";
    QDIMACS* q = qdimacs_parse(example);
    printf("Parsed: %d vars, %d clauses\n", q?q->n_vars:0, q?q->n_clauses:0);
    if(q) printf("Quantifier prefix: %s\n", q->prefix);
    if(q) { free(q->prefix); for(int i=0;i<q->n_clauses;i++)free(q->clauses[i]); free(q->clauses); free(q->clause_lens); free(q); }
}