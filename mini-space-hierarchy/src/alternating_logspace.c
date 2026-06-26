/* alternating_logspace.c — 交互ログ領域: AL = P, AP = PSPACE
 * 参考: Chandra-Kozen-Stockmeyer 1981, "Alternation"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/spaceh.h"

/* Alternating Turing Machine configurations with logspace bound */
typedef enum { EXISTENTIAL, UNIVERSAL } node_type;

typedef struct alt_node {
    node_type type;
    char config[256];
    struct alt_node** children;
    int num_children;
    int evaluated;
    int value; /* 0=reject, 1=accept */
} alt_node;

alt_node* alt_node_create(node_type t, const char* cfg) {
    alt_node* n = malloc(sizeof(alt_node));
    n->type = t;
    strncpy(n->config, cfg, 255);
    n->children = NULL;
    n->num_children = 0;
    n->evaluated = 0;
    n->value = 0;
    return n;
}

/* AL = Alternating Logspace = P (Chandra-Kozen-Stockmeyer 1981)
 * Simulate AL acceptance via existential/universal alternation */
int al_accept(alt_node* root) {
    if (root->num_children == 0) {
        /* Leaf: direct evaluation */
        root->evaluated = 1;
        root->value = (strstr(root->config, "ACCEPT") != NULL) ? 1 : 0;
        return root->value;
    }
    
    if (root->type == EXISTENTIAL) {
        for (int i = 0; i < root->num_children; i++) {
            if (al_accept(root->children[i])) {
                root->evaluated = 1;
                root->value = 1;
                return 1;
            }
        }
        root->value = 0;
        return 0;
    } else { /* UNIVERSAL */
        for (int i = 0; i < root->num_children; i++) {
            if (!al_accept(root->children[i])) {
                root->value = 0;
                return 0;
            }
        }
        root->value = 1;
        return 1;
    }
}

/* CKS Theorem demonstration: AL = P */
void cks_theorem_demo() {
    printf("=== Chandra-Kozen-Stockmeyer Theorem (1981) ===\n\n");
    printf("Alternation hierarchy for logarithmic space:\n\n");
    printf("  Σ₀^L = Π₀^L = L (deterministic logspace)\n");
    printf("  Σ₁^L = NL (existential logspace)\n");
    printf("  Π₁^L = co-NL = NL (Immerman-Szelepcsényi!)\n");
    printf("  Σ₂^L = NP^NL\n");
    printf("  ...\n");
    printf("  AL = ∪ₖ Σₖ^L = P (theorem!)\n");
    printf("  AP = ∪ₖ Σₖ^P = PSPACE (theorem!)\n\n");
    
    printf("Key insight:\n");
    printf("  Alternating time = deterministic space\n");
    printf("  Alternating space = deterministic time (exponential)\n");
    printf("  AL = P because poly(n) alternations × O(log n) space\n");
    printf("       can be simulated in poly(n) time deterministically\n\n");
    
    printf("Example AL computation (QBF with O(log n) alternations):\n");
    alt_node* root = alt_node_create(EXISTENTIAL, "x1");
    alt_node* c1 = alt_node_create(UNIVERSAL, "x2");
    alt_node* c2 = alt_node_create(EXISTENTIAL, "ACCEPT");
    root->children = malloc(2 * sizeof(alt_node*));
    root->children[0] = c1; root->children[1] = c2;
    root->num_children = 2;
    c1->children = malloc(sizeof(alt_node*));
    c1->children[0] = c2; c1->num_children = 1;
    
    printf("ALT tree: ∃x1 → ∀x2 → ACCEPT → result=%s\n",
           al_accept(root) ? "ACCEPT" : "REJECT");
    
    printf("\nThis is essentially a SAT instance with 2 alternations\n");
    printf("(which is in Σ₂^P, but with logspace worktape → in P)\n");
    
    free(root->children); free(c1->children); free(root); free(c1); free(c2);
}

void alternating_logspace_demo(void) {
    cks_theorem_demo();
}
