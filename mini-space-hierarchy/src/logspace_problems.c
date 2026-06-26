/* logspace_problems.c -- Problems in L */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spaceh.h"

static int logspace_multiply_binary(const char* a, const char* b, int len) {
    int result = 0;
    for(int i=0;i<len;i++) {
        int ba=a[len-1-i]-'0';
        for(int j=0;j<len;j++) {
            int bb=b[len-1-j]-'0';
            result+=ba*bb*(1<<(i+j));
        }
    }
    return result;
}

static int logspace_gcd(int a, int b) { while(b){int t=b;b=a%b;a=t;} return a; }

static long long logspace_modpow(long long a, long long b, long long m) {
    long long r=1; a%=m; while(b>0){if(b&1)r=(r*a)%m;a=(a*a)%m;b>>=1;} return r;
}

void logspace_problems_demo(void) {
    printf("\\n===== L: Logspace Computation =====\\n\\n");
    printf("Problems PROVEN in L (O(log n) space):\\n");
    printf("  1. Integer arithmetic (+, -, *, /)\\n");
    printf("  2. Modular exponentiation (a^b mod m)\\n");
    printf("  3. GCD (Euclidean algorithm)\\n");
    printf("  4. Undirected connectivity (Reingold 2008!)\\n");
    printf("  5. String matching, palindrome\\n\\n");

    printf("--- Binary Multiplication ---\\n");
    printf("  101(5) * 110(6) = %d\\n",
           logspace_multiply_binary("101","110",3));
    printf("  Space: O(log n) for loop counters.\\n\\n");

    printf("--- GCD ---\\n");
    printf("  GCD(1071,462) = %d\\n", logspace_gcd(1071,462));
    printf("  GCD(101,103) = %d (coprime)\\n\\n", logspace_gcd(101,103));

    printf("--- Modular Exponentiation ---\\n");
    printf("  7^256 mod 13 = %lld\\n", logspace_modpow(7,256,13));
    printf("  5^100 mod 97 = %lld\\n", logspace_modpow(5,100,97));
    printf("  Space: O(log n) for exponent bits.\\n\\n");

    printf("--- The Big Open: L vs NL ---\\n");
    printf("PATH (directed s-t connectivity) is NL-complete.\\n");
    printf("Is PATH in L? This would mean L = NL.\\n");
    printf("Undirected PATH: IN L (Reingold 2008, breakthrough).\\n");
    printf("Directed PATH: STILL OPEN.\\n");
}
