#ifndef GADGET_REDUCTIONS_H
#define GADGET_REDUCTIONS_H

#include "redcomp.h"

/* ===== Gadget Construction Primitives (L3, L5) =====
 *
 * A "gadget" is a local sub-structure in a reduction graph that
 * enforces a constraint. Gadgets are the BUILDING BLOCKS of
 * NP-completeness reductions.
 *
 * Classic gadget types (Garey & Johnson, 1979):
 *   Variable gadget: forces a binary choice (true/false)
 *   Clause gadget: ensures at least one literal is satisfied
 *   Consistency gadget: connects variable and clause gadgets
 *
 * Each gadget encodes a CNF constraint into a graph-theoretic property. */

/* Gadget types used in standard NP-completeness reductions */
typedef enum {
    GADGET_VARIABLE,          /* Diamond/choice pattern: encode x_i or !x_i */
    GADGET_CLAUSE_OR,         /* OR of up to 3 literals */
    GADGET_CLAUSE_TRIANGLE,   /* Triangle: must cover 2 of 3 vertices */
    GADGET_XOR,               /* XOR constraint: parity */
    GADGET_IMPLIES,           /* Implication: a => b */
    GADGET_NAND,              /* NAND: NOT (a AND b) */
    GADGET_MAJORITY,          /* Majority of 3 */
    GADGET_THRESHOLD,         /* k-out-of-n threshold */
    GADGET_EQUALITY,          /* Force two vertices same color / state */
    GADGET_DIAMOND_CHAIN      /* HC reduction: chain of diamond gadgets */
} GadgetType;

/* A gadget is a small graph with designated input/output vertices.
   These are composed to build the full reduction graph. */
typedef struct {
    GadgetType type;
    int n_vertices;            /* Number of vertices in this gadget */
    int n_edges;               /* Number of internal edges */
    int **adj;                 /* Adjacency matrix [n_vertices][n_vertices] */
    int *input_vertices;       /* Array of input vertex indices */
    int n_inputs;              /* Number of input vertices */
    int *output_vertices;      /* Array of output vertex indices */
    int n_outputs;             /* Number of output vertices */
    const char *constraint;    /* What constraint this gadget enforces */
} Gadget;

/* Create a variable gadget: a pair of vertices connected by an edge.
   Choosing one = setting variable true, the other = false.
   This is the fundamental binary choice pattern. */
Gadget *gadget_variable_create(int *va, int *vb);

/* Create a clause triangle gadget: 3 vertices forming a triangle.
   In VC reduction: must cover at least 2 vertices.
   In 3COL reduction: vertices force color constraints. */
Gadget *gadget_clause_triangle_create(int *v0, int *v1, int *v2);

/* Create an XOR gadget: encodes parity constraint.
   Used in reductions to parity-based problems. */
Gadget *gadget_xor_create(int a, int b, int *out);

/* Create a diamond chain gadget (for Hamiltonian Cycle):
   2*(m+1) vertices arranged as a chain of m diamonds.
   Path through left side = variable true; right side = false. */
Gadget *gadget_diamond_chain_create(int chain_length, int *vertex_base);

/* Connect two gadgets: add edges between output of g1 and input of g2.
   Returns number of cross-edges added. */
int gadget_connect(Gadget *g1, Gadget *g2, int **graph, int graph_n);

/* Merge multiple gadgets into a single graph.
   Allocates and returns the merged adjacency matrix.
   n_total returns the total vertex count. */
int **gadget_merge(Gadget **gadgets, int n_gadgets, int *n_total);

/* Free a gadget and all its allocated memory */
void gadget_free(Gadget *g);

/* Print a gadget's structure for debugging */
void gadget_print(const Gadget *g, FILE *fp);

/* ===== Gadget Catalog (L6) =====
 * The standard gadget library used in Karp's 21 reductions. */

/* Enumerate all known gadget types with descriptions */
void gadget_catalog_print(void);

/* Verify a gadget's internal consistency:
   edges are symmetric, vertices within bounds, etc. */
int gadget_verify(const Gadget *g);

#endif
