/* padding.c — Padding Argument
 * Translational technique: if two complexity classes are equal,
 * their padded versions are also equal.
 * Used to translate separations between complexity classes.
 * Key: If P=NP then EXP=NEXP (padding both sides).
 * Contrapostive: If EXP≠NEXP then P≠NP.
 * But EXP vs NEXP is also OPEN! Shows limits of relativization. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tht.h"

/* Pad input with extra zeros to reach target length.
   Pad(L, t) = {x0^{2^{t(|x|)}} | x in L} */
static char* pad_input(const char* x, int target_len) {
    int x_len = (int)strlen(x);
    int pad_len = target_len - x_len;
    if (pad_len < 0) pad_len = 0;
    char* padded = malloc((size_t)(target_len + 1));
    strcpy(padded, x);
    for (int i = 0; i < pad_len; i++)
        padded[x_len + i] = '0';
    padded[target_len] = '\0';
    return padded;
}

static void unpad(char* s) {
    int len = (int)strlen(s);
    while (len > 0 && s[len-1] == '0') {
        s[len-1] = '\0';
        len--;
    }
}

void padding_demo(void) {
    printf("\n===== Padding Argument =====\n\n");
    printf("Padding is a translational technique in complexity:\\n\n");
    printf("Definition: Pad(L, t) = {x#0^{2^{t(|x|)}} | x in L}\\n");
    printf("  where t is a time-constructible function.\\n\\n");

    printf("Key Properties:\\n");
    printf("  1. If L in TIME(n^k), then Pad(L,n) in TIME(2^n).\\n");
    printf("  2. If L in NTIME(n^k), then Pad(L,n) in NTIME(2^n).\\n");
    printf("  3. If P = NP, then EXP = NEXP (by padding both sides).\\n");
    printf("  4. If P != NP via diagonalization, then EXP != NEXP.\\n");
    printf("     But EXP vs NEXP is also OPEN! → diagonalization fails.\\n\\n");

    printf("--- Padding Demo ---\n");
    const char* test = "101";
    char* padded = pad_input(test, 15);
    printf("  Input:    '%s' (len=%d)\\n", test, (int)strlen(test));
    printf("  Padded:   '%s' (len=%d)\\n", padded, (int)strlen(padded));
    unpad(padded);
    printf("  Unpadded: '%s' (len=%d)\\n\\n", padded, (int)strlen(padded));
    free(padded);

    printf("--- Why Padding Matters ---\n");
    printf("Padding shows the limits of relativizing proofs.\\n");
    printf("A relativizing proof of P != NP would, via padding,\\n");
    printf("also prove EXP != NEXP. But no such proof is known!\\n");
    printf("This suggests we need non-relativizing techniques.\\n");
}
