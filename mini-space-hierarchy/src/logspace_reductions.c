/* logspace_reductions.c -- Logspace Reductions
 * A <=_L B: logspace reduction, f in O(log n) space.
 * x in A iff f(x) in B. NL is closed under <=_L.
 * Reference: Jones (1975); Arora & Barak 4.2
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/spaceh.h"

typedef struct {
    int input_len;
    int work_tape[64];
    int work_head;
    int work_used;
    char* output;
    int output_len;
    int output_capacity;
} LogspaceTransducer;

static LogspaceTransducer* lt_create(int input_len, int max_output) {
    LogspaceTransducer* lt = malloc(sizeof(LogspaceTransducer));
    lt->input_len = input_len;
    memset(lt->work_tape, 0, sizeof(lt->work_tape));
    lt->work_head = 0; lt->work_used = 1;
    lt->output = malloc((size_t)(max_output + 1));
    lt->output_len = 0; lt->output_capacity = max_output;
    lt->output[0] = 0;
    return lt;
}

static void lt_free(LogspaceTransducer* lt) {
    if (lt) { free(lt->output); free(lt); }
}

/* 3SAT <=_L SAT: copy input, O(log n) counter */
static void reduce_3sat_to_sat(const char* input, char* output, int max_out) {
    int len = (int)strlen(input), out_pos = 0;
    for (int i = 0; i < len && out_pos < max_out - 1; i++)
        output[out_pos++] = input[i];
    output[out_pos] = 0;
}

/* SAT <=_L 3SAT: clause size k>3 -> k-2 clauses of size 3.
 * (x1|x2|x3|x4) -> (x1|x2|z1)&(!z1|x3|z2)&(!z2|x4) */
static int sat_to_3sat_clause(const int* clause, int clause_size,
                               int* aux_start, char* buf, int buf_size) {
    int pos = 0;
    if (clause_size <= 3) {
        pos += snprintf(buf + pos, (size_t)(buf_size - pos), "(");
        for (int i = 0; i < clause_size; i++) {
            if (i > 0) pos += snprintf(buf + pos, (size_t)(buf_size - pos), "|");
            pos += snprintf(buf + pos, (size_t)(buf_size - pos), "%sx%d",
                           clause[i] < 0 ? "!" : "", abs(clause[i]));
        }
        pos += snprintf(buf + pos, (size_t)(buf_size - pos), ")");
    } else {
        int aux = *aux_start;
        pos += snprintf(buf + pos, (size_t)(buf_size - pos),
                       "(%sx%d|%sx%d|z%d)",
                       clause[0] < 0 ? "!" : "", abs(clause[0]),
                       clause[1] < 0 ? "!" : "", abs(clause[1]), aux);
        for (int i = 1; i < clause_size - 3; i++) {
            pos += snprintf(buf + pos, (size_t)(buf_size - pos),
                           "&(!z%d|%sx%d|z%d)",
                           aux + i - 1,
                           clause[i + 1] < 0 ? "!" : "", abs(clause[i + 1]),
                           aux + i);
        }
        pos += snprintf(buf + pos, (size_t)(buf_size - pos),
                       "&(!z%d|%sx%d|%sx%d)",
                       aux + clause_size - 4,
                       clause[clause_size - 2] < 0 ? "!" : "",
                       abs(clause[clause_size - 2]),
                       clause[clause_size - 1] < 0 ? "!" : "",
                       abs(clause[clause_size - 1]));
        *aux_start = aux + clause_size - 3;
    }
    return pos;
}

/* VERTEX-COVER <=_L CLIQUE: VC(G,k) iff CLIQUE(~G,|V|-k) */
static void reduce_vc_to_clique(int n, char** adj, int k,
                                 char* output, int max_out) {
    int pos = 0;
    pos += snprintf(output + pos, (size_t)(max_out - pos),
                    "CLIQUE n=%d k=%d edges=", n, n - k);
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (!adj[i][j] && pos < max_out - 20) {
                pos += snprintf(output + pos, (size_t)(max_out - pos),
                                "%d-%d ", i, j);
            }
        }
    }
}

/* PATH <=_L 2-SAT: both NL-complete */
static void reduce_path_to_2sat(int n, char** adj, int s, int t,
                                 char* output, int max_out) {
    int pos = 0;
    pos += snprintf(output + pos, (size_t)(max_out - pos),
                    "2SAT nvars=%d: ", n);
    pos += snprintf(output + pos, (size_t)(max_out - pos), "(%d) ", s + 1);
    pos += snprintf(output + pos, (size_t)(max_out - pos), "(%d) ", t + 1);
    for (int u = 0; u < n && pos < max_out - 30; u++) {
        for (int v = 0; v < n && pos < max_out - 30; v++) {
            if (adj[u][v]) {
                pos += snprintf(output + pos, (size_t)(max_out - pos),
                                "(!%d|%d) ", u + 1, v + 1);
            }
        }
    }
}

void logspace_reduction_demo(void) {
    printf("\n===== Logspace Reductions =====\n\n");
    printf("A <=_L B: f in O(log n) space, x in A iff f(x) in B.\n\n");

    printf("--- 3SAT <=_L SAT ---\n");
    char sat_out[256];
    reduce_3sat_to_sat("(x1|x2|x3)&(!x1|x2|x4)", sat_out, 255);
    printf("  Input:  (x1|x2|x3)&(!x1|x2|x4)\n");
    printf("  Output: %s\n", sat_out);
    printf("  Space: O(log n) for loop counter.\n\n");

    printf("--- SAT <=_L 3SAT ---\n");
    int clause[] = {1, 2, 3, 4, -5};
    char ts_out[512];
    int aux_start = 100;
    sat_to_3sat_clause(clause, 5, &aux_start, ts_out, 511);
    printf("  Input: (x1|x2|x3|x4|!x5) size 5\n");
    printf("  Output: %s\n", ts_out);
    printf("  Space: O(log n) for clause + aux counters.\n\n");

    printf("--- VERTEX-COVER <=_L CLIQUE ---\n");
    int n = 4;
    char** adj = malloc((size_t)n * sizeof(char*));
    for (int i = 0; i < n; i++)
        adj[i] = calloc((size_t)n, sizeof(char));
    adj[0][1] = adj[1][0] = 1;
    adj[1][2] = adj[2][1] = 1;
    adj[2][3] = adj[3][2] = 1;
    char vc_out[512];
    reduce_vc_to_clique(n, adj, 2, vc_out, 511);
    printf("  Line P4, VC=2 -> CLIQUE: %s\n", vc_out);
    for (int i = 0; i < n; i++) free(adj[i]);
    free(adj);

    printf("\n--- PATH <=_L 2-SAT ---\n");
    char ps_out[512];
    char** adj2 = malloc(4 * sizeof(char*));
    for (int i = 0; i < 4; i++) adj2[i] = calloc(4, sizeof(char));
    adj2[0][1] = adj2[1][2] = adj2[2][3] = 1;
    reduce_path_to_2sat(4, adj2, 0, 3, ps_out, 511);
    printf("  PATH(0->3) -> 2SAT: %s\n", ps_out);
    for (int i = 0; i < 4; i++) free(adj2[i]);
    free(adj2);

    printf("\n--- Key Properties ---\n");
    printf("  1. Logspace reductions are transitive\n");
    printf("  2. NL, L, PSPACE closed under <=_L reductions\n");
    printf("  3. PATH is NL-complete under <=_L\n");
    printf("  4. TQBF is PSPACE-complete under <=_L\n");
    printf("  5. Polynomial-time reductions too powerful for NL/L\n");
}
