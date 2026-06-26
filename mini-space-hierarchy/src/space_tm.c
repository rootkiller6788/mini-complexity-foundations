/* space_tm.c — Space-Bounded Turing Machine Simulator
 * A TM using s(n) space has at most |Q|·s(n)·|Γ|^{s(n)} configurations.
 * We implement: TM simulation, config graph construction,
 * PATH in config graph = acceptance.
 *
 * This is THE core tool for space complexity: space bounds convert
 * an infinite computation into a finite graph reachability problem. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CONFIGS 4096
#define MAX_TAPE     64

typedef struct {
    int state; int head; int tape[MAX_TAPE]; int tape_len;
} Config;

typedef struct {
    int n_states, n_syms, q0, q_accept, q_reject;
    int delta[8][4][3]; /* [state][sym] -> {new_sym, new_state, move} */
    int n_configs;
    Config configs[MAX_CONFIGS];
    char edges[MAX_CONFIGS][MAX_CONFIGS]; /* adjacency: 1 if edge exists */
} SpaceTM;

/* Initialize a simple space-bounded TM */
static void sptm_init(SpaceTM* m) {
    memset(m, 0, sizeof(SpaceTM));
    m->n_states = 4; m->n_syms = 3; m->q0 = 0; m->q_accept = 2; m->q_reject = 3;
    /* delta[0][0] = {write, next, move} */
    m->delta[0][0][0]=0; m->delta[0][0][1]=1; m->delta[0][0][2]=+1;
    m->delta[0][1][0]=1; m->delta[0][1][1]=0; m->delta[0][1][2]=+1;
    m->delta[0][2][0]=2; m->delta[0][2][1]=2; m->delta[0][2][2]=0;
    m->delta[1][0][0]=0; m->delta[1][0][1]=0; m->delta[1][0][2]=-1;
    m->delta[1][1][0]=1; m->delta[1][1][1]=2; m->delta[1][1][2]=0;
    m->delta[1][2][0]=2; m->delta[1][2][1]=2; m->delta[1][2][2]=0;
}

/* Check if two configs are equal */
static int config_eq(Config* a, Config* b) {
    if(a->state != b->state || a->head != b->head || a->tape_len != b->tape_len) return 0;
    for(int i=0;i<a->tape_len;i++) if(a->tape[i]!=b->tape[i]) return 0;
    return 1;
}

/* Apply one step of TM: config -> next_config. Return 1 if valid move. */
static int sptm_step(SpaceTM* m, Config* src, Config* dst, int space_bound) {
    int sym = src->tape[src->head];
    if(sym >= m->n_syms || sym < 0) sym = 0;
    int* d = m->delta[src->state][sym];
    memcpy(dst, src, sizeof(Config));
    dst->tape[src->head] = d[0];
    dst->state = d[1];
    dst->head = src->head + d[2];
    if(dst->head < 0 || dst->head >= space_bound) return 0;
    if(dst->head >= dst->tape_len) dst->tape_len = dst->head + 1;
    return 1;
}

/* Build configuration graph for space-bound s */
static void sptm_build_graph(SpaceTM* m, const int* input, int input_len, int space_bound) {
    Config start; memset(&start, 0, sizeof(start));
    start.state = m->q0; start.head = 0; start.tape_len = input_len;
    for(int i=0;i<input_len;i++) start.tape[i]=input[i];
    for(int i=input_len;i<MAX_TAPE;i++) start.tape[i]=0;
    
    m->n_configs = 1;
    memcpy(&m->configs[0], &start, sizeof(Config));
    memset(m->edges, 0, sizeof(m->edges));
    
    /* BFS to enumerate all reachable configs within space bound */
    int frontier = 0;
    while(frontier < m->n_configs && m->n_configs < MAX_CONFIGS) {
        Config next;
        if(sptm_step(m, &m->configs[frontier], &next, space_bound)) {
            /* Check if next is a new config */
            int found = -1;
            for(int i=0;i<m->n_configs;i++) {
                if(config_eq(&m->configs[i], &next)) { found=i; break; }
            }
            if(found < 0 && m->n_configs < MAX_CONFIGS) {
                found = m->n_configs++;
                memcpy(&m->configs[found], &next, sizeof(Config));
            }
            if(found >= 0) m->edges[frontier][found] = 1;
        }
        frontier++;
    }
}

/* PATH in config graph via BFS: is there a path from start to accept? */
static int sptm_accepts(SpaceTM* m) {
    int* visited = calloc((size_t)m->n_configs, sizeof(int));
    int* queue = malloc((size_t)m->n_configs * sizeof(int));
    int qh=0, qt=0;
    queue[qt++]=0; visited[0]=1;
    
    while(qh < qt) {
        int u = queue[qh++];
        if(m->configs[u].state == m->q_accept) { free(visited); free(queue); return 1; }
        for(int v=0; v<m->n_configs; v++) {
            if(m->edges[u][v] && !visited[v]) {
                visited[v]=1; queue[qt++]=v;
            }
        }
    }
    free(visited); free(queue);
    return 0;
}

void space_tm_demo(void) {
    printf("\n===== Space-Bounded TM Simulator =====\n\n");
    SpaceTM m; sptm_init(&m);
    
    int input[] = {1,0,1};
    int space_bounds[] = {1,2,3,5,10,20};
    
    printf("Input [1,0,1]. Testing space bounds:\n");
    printf("  space  configs  accepts?\n");
    printf("  -----  -------  --------\n");
    for(int si=0;si<6;si++) {
        int s = space_bounds[si];
        sptm_build_graph(&m, input, 3, s);
        int accepts = sptm_accepts(&m);
        printf("  %-6d %-7d %s\n", s, m.n_configs, accepts?"YES":"NO");
    }
    
    printf("\nKey insight: with polynomial space, the config graph has\n");
    printf("2^poly configs. PATH takes O(|V|+|E|) time = 2^poly = EXPTIME.\n");
    printf("But Savitch solves PATH in O(log^2|V|) = O(s^2) SPACE.\n");
    printf("Therefore: NPSPACE = PSPACE (Savitch 1970).\n");
}