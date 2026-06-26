/* space_config_graph.c -- Configuration Graph Implementation
 * Config graph: converts infinite TM computation into finite
 * graph reachability. N = |Q|*s(n)*|Gamma|^{s(n)} configs.
 * PATH in config graph = TM acceptance.
 * Reference: Arora & Barak 4.1; Sipser 8.1
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/spaceh.h"
#include "../include/space_tm.h"

SpaceBoundedTM* sptm_create(const char* name, int n_states,
    int input_alpha, int work_alpha, size_t (*bound)(size_t)) {
    SpaceBoundedTM* tm = malloc(sizeof(SpaceBoundedTM));
    if (!tm) return NULL;
    strncpy(tm->name, name, 63); tm->name[63] = 0;
    tm->n_states = n_states;
    tm->input_alphabet_size = input_alpha;
    tm->work_alphabet_size = work_alpha;
    tm->q_start = 0; tm->q_accept = 1; tm->q_reject = 2;
    tm->space_bound = bound;
    tm->is_nondeterministic = 0;
    tm->steps_executed = 0; tm->space_used_max = 0;
    size_t total = (size_t)n_states * (size_t)input_alpha * (size_t)work_alpha;
    tm->delta_flat = calloc(total, sizeof(TMTransition));
    tm->delta_stride1 = input_alpha * work_alpha;
    tm->delta_stride2 = work_alpha;
    return tm;
}

void sptm_free(SpaceBoundedTM* tm) {
    if (tm) { free(tm->delta_flat); free(tm); }
}

void sptm_set_transition(SpaceBoundedTM* tm, int state, int input_sym,
    int work_sym, int next_state, int write_sym, int input_move, int work_move) {
    int idx = state * tm->delta_stride1 + input_sym * tm->delta_stride2 + work_sym;
    tm->delta_flat[idx].next_state = next_state;
    tm->delta_flat[idx].write_symbol = write_sym;
    tm->delta_flat[idx].input_move = input_move;
    tm->delta_flat[idx].work_move = work_move;
}

void sptm_set_accept_reject(SpaceBoundedTM* tm, int q_acc, int q_rej, int q_start) {
    tm->q_accept = q_acc; tm->q_reject = q_rej; tm->q_start = q_start;
}

int sptm_step(SpaceBoundedTM* tm, TMConfig* src, TMConfig* dst) {
    if (src->state == tm->q_accept || src->state == tm->q_reject) return 0;
    int input_sym = 0;
    int work_sym = (src->work_head < src->tape_len)
                    ? src->work_tape[src->work_head] : 0;
    int idx = src->state * tm->delta_stride1
            + input_sym * tm->delta_stride2 + work_sym;
    TMTransition* t = &tm->delta_flat[idx];
    memcpy(dst, src, sizeof(TMConfig));
    dst->state = t->next_state;
    dst->work_tape[src->work_head] = t->write_symbol;
    dst->input_head += t->input_move;
    dst->work_head += t->work_move;
    if (dst->work_head < 0 || dst->work_head >= dst->tape_len) return 0;
    if (dst->work_head >= dst->work_used)
        dst->work_used = dst->work_head + 1;
    tm->steps_executed++;
    if (dst->work_used > tm->space_used_max)
        tm->space_used_max = dst->work_used;
    return 1;
}

static int cfg_config_eq(TMConfig* a, TMConfig* b) {
    if (a->state != b->state || a->input_head != b->input_head
        || a->work_head != b->work_head || a->work_used != b->work_used)
        return 0;
    for (int i = 0; i < a->work_used && i < 256; i++)
        if (a->work_tape[i] != b->work_tape[i]) return 0;
    return 1;
}

ConfigGraphSpace* cfg_graph_build(SpaceBoundedTM* tm, const int* input,
                                   int input_len, int space_bound) {
    if (space_bound > 256) space_bound = 256;
    ConfigGraphSpace* cg = malloc(sizeof(ConfigGraphSpace));
    cg->n_vertices = 0;
    cg->configs = malloc(CFG_GRAPH_MAX_V * sizeof(TMConfig));
    cg->adjacency = malloc(CFG_GRAPH_MAX_V * sizeof(char*));
    for (int i = 0; i < CFG_GRAPH_MAX_V; i++)
        cg->adjacency[i] = calloc(CFG_GRAPH_MAX_V, sizeof(char));
    cg->accept_vertices = malloc(CFG_GRAPH_MAX_V * sizeof(int));
    cg->n_accept_vertices = 0;

    TMConfig start; memset(&start, 0, sizeof(start));
    start.state = tm->q_start; start.tape_len = space_bound;
    start.work_used = 1;
    for (int i = 0; i < input_len && i < 256; i++)
        start.work_tape[i] = input[i];
    memcpy(&cg->configs[0], &start, sizeof(TMConfig));
    cg->n_vertices = 1; cg->source_vertex = 0;

    int frontier = 0;
    while (frontier < cg->n_vertices && cg->n_vertices < CFG_GRAPH_MAX_V) {
        TMConfig next;
        if (sptm_step(tm, &cg->configs[frontier], &next)) {
            int found = -1;
            for (int i = 0; i < cg->n_vertices; i++) {
                if (cfg_config_eq(&cg->configs[i], &next)) { found = i; break; }
            }
            if (found < 0 && cg->n_vertices < CFG_GRAPH_MAX_V) {
                found = cg->n_vertices++;
                memcpy(&cg->configs[found], &next, sizeof(TMConfig));
                if (next.state == tm->q_accept)
                    cg->accept_vertices[cg->n_accept_vertices++] = found;
            }
            if (found >= 0) cg->adjacency[frontier][found] = 1;
        }
        frontier++;
    }
    return cg;
}

int cfg_graph_accepts(ConfigGraphSpace* cg) {
    int* visited = calloc((size_t)cg->n_vertices, sizeof(int));
    int* queue = malloc((size_t)cg->n_vertices * sizeof(int));
    int qh = 0, qt = 0, result = 0;
    queue[qt++] = cg->source_vertex; visited[cg->source_vertex] = 1;
    while (qh < qt) {
        int u = queue[qh++];
        for (int i = 0; i < cg->n_accept_vertices; i++)
            if (u == cg->accept_vertices[i]) { result = 1; goto done; }
        for (int v = 0; v < cg->n_vertices; v++)
            if (cg->adjacency[u][v] && !visited[v])
                { visited[v] = 1; queue[qt++] = v; }
    }
done:
    free(visited); free(queue);
    return result;
}

void cfg_graph_free(ConfigGraphSpace* cg) {
    if (!cg) return;
    for (int i = 0; i < CFG_GRAPH_MAX_V; i++) free(cg->adjacency[i]);
    free(cg->adjacency); free(cg->configs); free(cg->accept_vertices);
    free(cg);
}

int cfg_graph_reachable_count(ConfigGraphSpace* cg, int from_vertex) {
    int* visited = calloc((size_t)cg->n_vertices, sizeof(int));
    int* stack = malloc((size_t)cg->n_vertices * sizeof(int));
    int top = 0, count = 0;
    stack[top++] = from_vertex; visited[from_vertex] = 1;
    while (top > 0) {
        int u = stack[--top]; count++;
        for (int v = 0; v < cg->n_vertices; v++)
            if (cg->adjacency[u][v] && !visited[v])
                { visited[v] = 1; stack[top++] = v; }
    }
    free(visited); free(stack);
    return count;
}

double sc_log_n(double n) {
    if (n <= 1.0) return 1.0;
    return ceil(log2(n));
}

double sc_n_to_k(double n, int k) {
    double r = 1.0;
    for (int i = 0; i < k; i++) r *= n;
    return r;
}

double sc_two_to_nk(double n, int k) {
    return exp2(sc_n_to_k(n, k));
}

double sc_klog_n(double n, int k) {
    double ln = sc_log_n(n), r = 1.0;
    for (int i = 0; i < k; i++) r *= ln;
    return r;
}

int is_space_constructible_fn(double (*f)(double), int max_n) {
    for (int n = 1; n <= max_n; n++)
        if (n < max_n && f(n+1) > f(2*n) * 2.0) return 0;
    return 1;
}

static size_t demo_bound_fn(size_t n) { return (size_t)n; }

void space_config_graph_demo(void) {
    printf("\n===== Configuration Graph Demo =====\n\n");
    SpaceBoundedTM* tm = sptm_create("TestTM", 4, 2, 3, demo_bound_fn);
    sptm_set_accept_reject(tm, 1, 2, 0);
    sptm_set_transition(tm, 0, 0, 0, 1, 1, 1, 1);
    sptm_set_transition(tm, 0, 1, 0, 1, 0, 1, 0);
    int input[] = {0, 1, 0};
    ConfigGraphSpace* cg = cfg_graph_build(tm, input, 3, 10);
    printf("TM: 4 states, space bound=10\n");
    printf("Config graph: %d vertices\n", cg->n_vertices);
    printf("Accepting configs: %d\n", cg->n_accept_vertices);
    printf("TM accepts: %s\n", cfg_graph_accepts(cg) ? "YES" : "NO");
    printf("Reachable from start: %d configs\n",
           cfg_graph_reachable_count(cg, cg->source_vertex));
    cfg_graph_free(cg); sptm_free(tm);
    printf("\n--- Space Bound -> Config Graph Size ---\n");
    printf("  s(n)=log n   -> poly(n) configs -> NL/P\n");
    printf("  s(n)=poly(n) -> 2^poly configs -> PSPACE\n");
    printf("  s(n)=2^poly  -> 2^(2^poly) configs -> EXPSPACE\n");
}
