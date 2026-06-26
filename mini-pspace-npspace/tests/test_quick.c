#include "pspace.h"
#include <stdio.h>
#include <stdlib.h>

#define ASSERT_TRUE(cond, msg) do { \
    if (!(cond)) { printf("FAIL: %s\n", msg); exit(1); } \
    else { printf("  PASS: %s\n", msg); } \
} while(0)

#define ASSERT_EQ_INT(expected, actual, msg) do { \
    if ((expected) != (actual)) { \
        printf("FAIL: %s — expected %d, got %d\n", msg, expected, actual); \
        exit(1); \
    } else { printf("  PASS: %s (got %d)\n", msg, actual); } \
} while(0)

int main(void) {
    setbuf(stdout, NULL);
    printf("===== Quick Assertion Tests =====\n\n");

    /* L1: struct sizes */
    ASSERT_TRUE(sizeof(ComplexityClass) > 0, "ComplexityClass enum defined");
    ASSERT_TRUE(sizeof(SpaceTM) > 0, "SpaceTM struct defined");

    /* L4: class relationships */
    ASSERT_EQ_INT(SPACE_CLASS_RELATION_KNOWN_SEPARATION,
                  complexity_compare(CLASS_L, CLASS_PSPACE),
                  "L != PSPACE (proven)");
    ASSERT_EQ_INT(SPACE_CLASS_RELATION_KNOWN_SUBSET,
                  complexity_compare(CLASS_PSPACE, CLASS_NPSPACE),
                  "PSPACE ⊆ NPSPACE");
    ASSERT_EQ_INT(SPACE_CLASS_RELATION_OPEN,
                  complexity_compare(CLASS_P, CLASS_PSPACE),
                  "P ⊆? PSPACE (open)");

    /* L5: QBF evaluation — (x0|x1)&(!x0|!x1) ≡ x0 XOR x1
     * E x0 A x1: need x0 s.t. holds for both x1=0 AND x1=1 → FALSE
     * A x0 E x1: for each x0, pick x1 = !x0 → TRUE */
    {
        int cls[2][3] = {{QBF_LIT(0,0), QBF_LIT(1,0), QBF_PAD},
                         {QBF_LIT(0,1), QBF_LIT(1,1), QBF_PAD}};
        int r = qbf_evaluate(2, "EA", cls, 2);
        ASSERT_EQ_INT(0, r, "QBF E x0 A x1: x0⊕x1 = FALSE");
        r = qbf_evaluate(2, "AE", cls, 2);
        ASSERT_EQ_INT(1, r, "QBF A x0 E x1: x0⊕x1 = TRUE");
    }

    /* L5: QBF — exists x0,x1,x2: (x0|x1|x2) */
    {
        int cls2[1][3] = {{QBF_LIT(0,0), QBF_LIT(1,0), QBF_LIT(2,0)}};
        int r = qbf_evaluate(3, "EEE", cls2, 1);
        ASSERT_EQ_INT(1, r, "QBF EEE: (x0|x1|x2) = TRUE");
    }

    /* L5: QBF — forall x0,x1,x2: (x0|x1|x2) */
    {
        int cls3[1][3] = {{QBF_LIT(0,0), QBF_LIT(1,0), QBF_LIT(2,0)}};
        int r = qbf_evaluate(3, "AAA", cls3, 1);
        ASSERT_EQ_INT(0, r, "QBF AAA: (x0|x1|x2) = FALSE (x0=x1=x2=0 fails)");
    }

    /* L5: Savitch reachability */
    {
        int* edges[2];
        int e0[] = {0, 1}, e1[] = {1, 2};
        edges[0] = e0; edges[1] = e1;
        ASSERT_EQ_INT(1, savitch_reachability(0, 1, 1, 3, edges, 2),
                      "PATH(0,1) direct edge");
        ASSERT_EQ_INT(0, savitch_reachability(1, 0, 1, 3, edges, 2),
                      "PATH(1,0) no edge");
    }

    /* L5: config counting */
    {
        double c = count_tm_configs(5, 10, 3);
        ASSERT_TRUE(c > 1000, "count_tm_configs(5,10,3) > 1000");
    }

    /* L4: space hierarchy */
    ASSERT_EQ_INT(1, space_hierarchy_separates(1.0, 100.0),
                  "space hierarchy separates f=o(g)");

    printf("\n===== ALL ASSERTIONS PASSED =====\n");
    return 0;
}
