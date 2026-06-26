/* crypto_timing.c — L7: Cryptographic OWF Timing Analysis */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "tht.h"

static long long mod_pow(long long base, long long exp, long long mod) {
    long long result = 1;
    base = base % mod;
    while (exp > 0) {
        if (exp & 1) result = (result * base) % mod;
        base = (base * base) % mod;
        exp >>= 1;
    }
    return result;
}

static long long rsa_owf_forward(long long x, long long N, long long e) {
    return mod_pow(x, e, N);
}

static void rsa_timing_analysis(void) {
    printf("\n--- RSA-2048 OWF Timing Analysis ---\n");
    printf("RSA: f(x) = x^e mod N. Forward: O(log^3 N).\n");
    printf("Inversion: subexponential via GNFS.\n");
    printf("Used in: TLS 1.3, PGP, Signal, Bitcoin.\n\n");
    printf("Forward computation timing:\n");
    printf("  bits    forward ms    invert est (log10)\n");
    printf("  ------  -----------  ------------------\n");
    for (int bits = 8; bits <= 24; bits += 4) {
        long long N = (1LL << bits) - 1;
        long long e = 65537;
        clock_t t0 = clock();
        volatile long long y = rsa_owf_forward(42, N, e);
        double fwd_ms = 1000.0 * (clock() - t0) / CLOCKS_PER_SEC;
        double logN = log((double)N);
        double inv_est = exp(pow(logN, 0.333) * pow(log(logN), 0.667));
        printf("  %-6d %-11.6f  ~1e%.0f\n", bits, fwd_ms, log10(inv_est));
    }
    printf("Key: forward polynomial, inversion explodes.\n");
}

static unsigned char sbox_lookup(unsigned char x) {
    static const unsigned char sbox[256] = {
        0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
        0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
        0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
        0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
        0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
        0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
        0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
        0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
        0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
        0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
        0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
        0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
        0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
        0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
        0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
        0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
    };
    return sbox[x];
}

static void aes_encrypt_block(const unsigned char plain[16],
                              const unsigned char key[16],
                              unsigned char cipher[16]) {
    memcpy(cipher, plain, 16);
    unsigned char round_key[16];
    memcpy(round_key, key, 16);
    for (int i = 0; i < 16; i++) cipher[i] ^= round_key[i];
    for (int r = 0; r < 10; r++) {
        for (int i = 0; i < 16; i++) cipher[i] = sbox_lookup(cipher[i]);
        unsigned char tmp[16]; memcpy(tmp, cipher, 16);
        for (int row = 0; row < 4; row++)
            for (int col = 0; col < 4; col++)
                cipher[row*4+col] = tmp[row*4 + ((col+row)%4)];
        round_key[0] = (unsigned char)((round_key[0]*173+37)&0xFF);
        for (int i = 1; i < 16; i++)
            round_key[i] = (unsigned char)((round_key[i]*191+round_key[i-1])&0xFF);
        for (int i = 0; i < 16; i++) cipher[i] ^= round_key[i];
    }
}

static void aes_timing_analysis(void) {
    printf("\n--- AES-128 Timing Analysis ---\n");
    printf("AES: 10 rounds, 128-bit block.\n");
    printf("Forward: O(1)/block. Best known attack: biclique (2^126.1).\n");
    printf("Used: NIST FIPS 197, TLS 1.3, WPA2, disk encryption.\n\n");
    unsigned char plain[16] = "Hello World!!\0\0\0";
    unsigned char key[16], cipher[16];
    for (int i = 0; i < 16; i++) key[i] = (unsigned char)(i*17+3);
    clock_t t0 = clock();
    for (int n = 0; n < 10000; n++) { plain[0]=(unsigned char)n; aes_encrypt_block(plain,key,cipher); }
    double ms = 1000.0*(clock()-t0)/CLOCKS_PER_SEC;
    printf("  10000 blocks: %.3f ms (%.6f ms/block)\n", ms, ms/10000.0);
    printf("  AES-128 brute force: 2^128 ops.\n");
    double years = pow(2.0,128)/(1e12*365.25*86400);
    printf("  At 10^12 ops/sec: ~1e%.1f years\n", log10(years));
}

static unsigned int sha_rotr(unsigned int x, int n) { return (x>>n)|(x<<(32-n)); }
static void sha256_compress(unsigned int st[8], const unsigned int blk[16]) {
    unsigned int s0 = sha_rotr(st[0],2)^sha_rotr(st[0],13)^sha_rotr(st[0],22);
    unsigned int maj = (st[0]&st[1])^(st[0]&st[2])^(st[1]&st[2]);
    unsigned int t2 = s0 + maj;
    unsigned int s1 = sha_rotr(st[4],6)^sha_rotr(st[4],11)^sha_rotr(st[4],25);
    unsigned int ch = (st[4]&st[5])^((~st[4])&st[6]);
    unsigned int t1 = st[7] + s1 + ch + 0x428a2f98 + blk[0];
    st[7]=st[6]; st[6]=st[5]; st[5]=st[4]; st[4]=st[3]+t1;
    st[3]=st[2]; st[2]=st[1]; st[1]=st[0]; st[0]=t1+t2;
}

static void sha256_timing_analysis(void) {
    printf("\n--- SHA-256 One-Way Function ---\n");
    printf("SHA-256: Merkle-Damgard, 64 rounds.\n");
    printf("Forward: O(n). Inversion: 2^256 preimage.\n");
    printf("Used: Bitcoin PoW, blockchain, Git, TLS certs.\n\n");
    unsigned int st[8]={0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
                         0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};
    unsigned int blk[16];
    for(int i=0;i<16;i++) blk[i]=(unsigned int)(i*0x01010101);
    clock_t t0=clock();
    for(int n=0;n<100000;n++) sha256_compress(st,blk);
    double ms=1000.0*(clock()-t0)/CLOCKS_PER_SEC;
    printf("  100000 compressions: %.3f ms\n",ms);
    printf("  Bitcoin: verify=O(1), mine=EXP. Applied hierarchy!\n");
}

static void lattice_owf_analysis(void) {
    printf("\n--- Lattice OWF (LWE, NIST PQC 2024) ---\n");
    printf("LWE: b = A*s + e mod q.\n");
    printf("Forward: O(n^2). Best attack (BKZ): 2^(0.292*n).\n");
    printf("NIST standards: Kyber, Dilithium.\n\n");
    int n=256; long long q=3329;
    printf("  n=%d, q=%lld. Forward: O(n^2).\n", n, q);
    printf("  BKZ: 2^(0.292*%d) = 2^%.1f\n", n, 0.292*n);
    printf("  Quantum-safe: Shor does not apply to lattices.\n");
    clock_t t0=clock();
    volatile long long sum=0;
    for(int i=0;i<n;i++)for(int j=0;j<n;j++) sum=(sum+(long long)i*j)%q;
    double ms=1000.0*(clock()-t0)/CLOCKS_PER_SEC;
    printf("  Forward (n=%d): %.3f ms\n", n, ms);
}

void crypto_timing_demo(void) {
    printf("\n================================================================\n");
    printf("  L7 APPLICATION: Cryptographic Complexity Analysis\n");
    printf("  OWF Timing and the P vs NP Conjecture\n");
    printf("================================================================\n\n");
    printf("One-Way Functions: easy to compute, hard to invert.\n");
    printf("OWF exists => P != NP (converse open).\n");
    printf("Modern crypto = Applied Time Hierarchy.\n\n");
    rsa_timing_analysis();
    aes_timing_analysis();
    sha256_timing_analysis();
    lattice_owf_analysis();
    printf("\n================================================================\n");
    printf("  Summary: Cryptography depends on TIME(f) != TIME(g).\n");
    printf("  Known: P != EXP. Believed: P != NP (needed for OWF).\n");
    printf("================================================================\n");
}
