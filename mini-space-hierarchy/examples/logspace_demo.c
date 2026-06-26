/* logspace_demo.c — End-to-end Logspace (L) Demo
 * Demonstrates multiple L algorithms using O(log n) space.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/spaceh.h"

static int logspace_gcd(int a, int b) {
    while (b) { int t = b; b = a % b; a = t; }
    return a;
}

static long long logspace_modpow(long long a, long long b, long long m) {
    long long r = 1; a %= m;
    while (b > 0) { if (b & 1) r = (r * a) % m; a = (a * a) % m; b >>= 1; }
    return r;
}

static int logspace_palindrome(const char* s, int len) {
    for (int i = 0; i < len / 2; i++)
        if (s[i] != s[len - 1 - i]) return 0;
    return 1;
}

static void logspace_parity_demo(const int* arr, int n) {
    int parity = 0;
    for (int i = 0; i < n; i++) parity ^= arr[i];
    printf("  Parity of %d-element array: %d\n", n, parity);
}

int main(void) {
    setbuf(stdout, NULL);
    printf("========================================\n");
    printf("  Logspace (L) — End-to-End Demo\n");
    printf("========================================\n\n");

    printf("L = SPACE(log n). Algorithms using O(log n) workspace.\n\n");

    printf("--- Algorithm 1: Binary Addition (O(log n) space) ---\n");
    char* a = "1101", *b = "1011", result[8] = {0};
    logspace_add_binary(a, b, result, 4);
    printf("  %s + %s = %s (13 + 11 = 24)\n", a, b, result);
    printf("  Space: carry bit + index counter = O(log n)\n\n");

    printf("--- Algorithm 2: Palindrome Check (O(log n) space) ---\n");
    const char* tests[] = {"racecar", "hello", "abba", "complexity"};
    for (int i = 0; i < 4; i++) {
        printf("  palindrome("%s") = %s\n",
               tests[i],
               logspace_palindrome(tests[i], (int)strlen(tests[i])) ? "YES" : "NO");
    }
    printf("  Space: two pointers = O(log n)\n\n");

    printf("--- Algorithm 3: GCD (O(log n) space) ---\n");
    printf("  GCD(1071, 462) = %d\n", logspace_gcd(1071, 462));
    printf("  GCD(233, 144) = %d (consecutive Fibonacci)\n", logspace_gcd(233, 144));
    printf("  GCD(101, 103) = %d (coprime)\n", logspace_gcd(101, 103));
    printf("  Space: Euclidean algorithm = O(log n)\n\n");

    printf("--- Algorithm 4: Modular Exponentiation (O(log n) space) ---\n");
    printf("  7^256 mod 13 = %lld\n", logspace_modpow(7, 256, 13));
    printf("  5^100 mod 97 = %lld\n", logspace_modpow(5, 100, 97));
    printf("  3^1000 mod 1000 = %lld\n", logspace_modpow(3, 1000, 1000));
    printf("  Space: O(log n) for exponent bit counter\n\n");

    printf("--- Algorithm 5: Parity (O(log n) space) ---\n");
    int arr1[] = {1,0,1,1,0,1,0,0,1,1};
    logspace_parity_demo(arr1, 10);
    int arr2[] = {1,1,1,1,1,1,1,1,1,1};
    logspace_parity_demo(arr2, 10);
    printf("  Space: accumulator = O(log n) bits\n\n");

    printf("--- Space Usage Comparison ---\n");
    printf("  %8s %12s %12s %12s\n", "n", "O(log n)", "O(n)", "O(n^2)");
    for (int n = 10; n <= 10000; n *= 10)
        printf("  %8d %10.0f B %10d B %10d KB\n",
               n, log2((double)n), n * (int)sizeof(int),
               (n * n * (int)sizeof(int)) / 1024);

    printf("\nKey takeaway: Many basic operations are in L (logspace).\n");
    printf("The big open: Is directed PATH in L? (L vs NL problem)\n");
    return 0;
}
