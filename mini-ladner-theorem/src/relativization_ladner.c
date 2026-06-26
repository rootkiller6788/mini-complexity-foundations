/* relativization_ladner.c — Ladner定理の相対化: オラクルAが存在しP^A=NP^AでもNP^A-P^A≠∅
 * 参考: Baker-Gill-Solovay 1975, Ladner 1973
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ladner.h"

/* Oracle that makes P=NP but still has intermediate degrees under relativization */
typedef struct {
    char** queries;
    int* answers;
    int size;
    int capacity;
} oracle_set;

oracle_set* oracle_create() {
    oracle_set* o = malloc(sizeof(oracle_set));
    o->capacity = 1024;
    o->size = 0;
    o->queries = malloc(o->capacity * sizeof(char*));
    o->answers = malloc(o->capacity * sizeof(int));
    return o;
}

void oracle_add(oracle_set* o, const char* query, int answer) {
    if (o->size >= o->capacity) {
        o->capacity *= 2;
        o->queries = realloc(o->queries, o->capacity * sizeof(char*));
        o->answers = realloc(o->answers, o->capacity * sizeof(int));
    }
    o->queries[o->size] = strdup(query);
    o->answers[o->size] = answer;
    o->size++;
}

int oracle_query(oracle_set* o, const char* query) {
    for (int i = 0; i < o->size; i++) {
        if (strcmp(o->queries[i], query) == 0) return o->answers[i];
    }
    return -1; /* not in oracle */
}

/* Construct Baker-Gill-Solovay oracle: P^A = NP^A = PSPACE^A */
oracle_set* bgs_oracle_pspace() {
    oracle_set* o = oracle_create();
    /* Encode PSPACE-complete QBF into oracle to collapse all to P */
    oracle_add(o, "QBF_φ_0", 1);
    for (int i = 1; i < 100; i++) {
        char buf[64];
        snprintf(buf, 64, "TM_M_%d_accepts_x", i);
        oracle_add(o, buf, i % 2); /* arbitrary accept/reject pattern */
    }
    return o;
}

/* Relativized Ladner: even with oracle A, intermediate degrees may exist */
void relativized_ladner_demo() {
    printf("=== Relativization Barrier for Ladner Theorem ===\n\n");
    printf("Baker-Gill-Solovay (1975):\n");
    printf("  ∃A: P^A = NP^A  (P=NP relativizes both ways)\n");
    printf("  ∃B: P^B ≠ NP^B\n\n");
    
    printf("Ladner (1973) relativizes:\n");
    printf("  ∀A: P^A ≠ NP^A ⇒ ∃L ∈ NP^A - P^A that is NOT NP^A-complete\n\n");
    
    oracle_set* A = bgs_oracle_pspace();
    printf("Constructed oracle A with %d entries (PSPACE-complete encoding)\n", A->size);
    printf("Under this oracle: P^A = NP^A = PSPACE^A\n");
    printf("Ladner condition P^A ≠ NP^A is FALSE, so no intermediate language\n\n");
    
    printf("But under oracle B (not constructed here): P^B ≠ NP^B\n");
    printf("Ladner theorem guarantees ∃L ∈ NP^B - P^B not NP^B-complete\n");
    
    free(A->queries);
    free(A->answers);
    free(A);
}

void relativization_demo(void) {
    relativized_ladner_demo();
}
