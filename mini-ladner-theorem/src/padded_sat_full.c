/* padded_sat_full.c -- Padded SAT Construction
 * SAT' = {phi # 1^{2^{|phi|}} | phi in SAT}
 * This language is in NP but not NP-complete (if P!=NP).
 * The exponential padding makes it sparse enough for Ladner's proof. */
#include "ladner.h"

static int small_sat(int assign) {
    int x1=(assign>>0)&1, x2=(assign>>1)&1, x3=(assign>>2)&1;
    return (x1||x2||x3) && ((!x1)||x2||(!x3));
}

static int extract_k(int n) {
    for(int k=1;k<=30;k++) {
        long long pl = k + (1LL<<k);
        if(pl>2000000000LL) break;
        if(pl==(long long)n) return k;
        if(pl>(long long)n) break;
    }
    return -1;
}

int padded_sat_member(int n) {
    int k = extract_k(n);
    if(k<0) return 0;
    long long pad_start=k, pad_len=1LL<<k;
    if(pad_start+pad_len!=(long long)n) return 0;
    return small_sat(k);
}

void padded_sat_full_demo(void) {
    printf("\n===== Padded SAT Construction =====\n\n");
    printf("SAT' = {phi # 1^{2^{|phi|}} | phi in SAT}\n\n");
    printf("Properties:\n");
    printf("  1. SAT' in NP: guess assignment + verify padding\n");
    printf("  2. SAT' NOT NP-complete (if P!=NP):\n");
    printf("     SAT->SAT' reduction blows up n -> 2^n\n");
    printf("     This gives sub-exp SAT algorithm -> P=NP.\n");
    printf("  3. SAT' makes Ladner's L work.\n\n");

    printf("--- Testing SAT' Membership ---\n");
    int tests[]={3,5,7,11,19,35,67,131,259,515};
    for(int i=0;i<10;i++) {
        int n=tests[i], r=padded_sat_member(n);
        printf("  n=%-5d k=%-2d SAT'=%s\n",n,extract_k(n),r?"YES":"NO");
    }
    printf("\nSAT' is sparse yet in NP -> NP-intermediate (if P!=NP).\n");
}
