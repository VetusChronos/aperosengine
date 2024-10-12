/* 
* sha256.c modified by Vetus (Chronos)
* based on libcrypto/sha/sha256.c (see LICENSE) 
*/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "my_sha256.h"

// Ensure compatibility with different endian systems
#if defined(_MSC_VER) && !defined(__clang__) && !defined(__attribute__)
#define __attribute__(a)
#endif

#include "cmake_config.h" // For HAVE_ENDIAN_H

#if defined(_WIN32)
#include <winsock2.h>

#define be16toh(x) ntohs((x))
#define htobe16(x) htons((x))
#define le32toh(x) (x)
#define be32toh(x) ntohl((x))
#define htole32(x) (x)
#define htobe32(x) ntohl((x))

#elif defined(HAVE_ENDIAN_H)
#include <endian.h>

#elif defined(__APPLE__)
#include <machine/endian.h>

#elif defined(__linux__)
#include <endian.h>

#elif defined(__FreeBSD__)
#include <sys/endian.h>

#elif defined(__sun)
#include <sys/byteorder.h>
#define be16toh(x) BE_16(x)
#define htobe16(x) BE_16(x)
#define le32toh(x) LE_32(x)
#define be32toh(x) BE_32(x)
#define htole32(x) LE_32(x)
#define htobe32(x) BE_32(x)

#else
#error "Unsupported platform"
#endif

// Ensure alignment for various architectures
#ifndef __STRICT_ALIGNMENT
#define __STRICT_ALIGNMENT
#if defined(__i386) || defined(__x86_64) || defined(__aarch64__) || \
    ((defined(__arm__) || defined(__arm)) && __ARM_ARCH >= 6)
#undef __STRICT_ALIGNMENT
#endif
#endif

/** libcrypto/crypto_internal.h **/
#define CTASSERT(x) \
    extern char _ctassert[(x) ? 1 : -1] __attribute__((__unused__))

static inline uint32_t crypto_load_be32toh(const uint8_t *src) {
    uint32_t v;
    memcpy(&v, src, sizeof(v));
    return be32toh(v);
}

static inline void crypto_store_htobe32(uint8_t *dst, uint32_t v) {
    v = htobe32(v);
    memcpy(dst, &v, sizeof(v));
}

static inline uint32_t crypto_ror_u32(uint32_t v, size_t shift) {
	return (v << (32 - shift)) | (v >> shift);
}

/** libcrypto/hidden/crypto_namespace.h **/
# define LCRYPTO_UNUSED(x)
# define LCRYPTO_USED(x)
# define LCRYPTO_ALIAS1(pre,x)
# define LCRYPTO_ALIAS(x)

/* Ensure that SHA_LONG and uint32_t are equivalent. */
CTASSERT(sizeof(SHA_LONG) == sizeof(uint32_t));

static void sha256_block_data_order(SHA256_CTX *ctx, const void *_in, size_t num);

static const SHA_LONG K256[64] = {
	0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL,
	0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL,
	0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL,
	0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL,
	0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
	0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL,
	0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL,
	0xc6e00bf3UL, 0xd5a79147UL, 0x06ca6351UL, 0x14292967UL,
	0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL,
	0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
	0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL,
	0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL,
	0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL,
	0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL,
	0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
	0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL,
};

static inline SHA_LONG Sigma0(SHA_LONG x) {
    return crypto_ror_u32(x, 2) ^ crypto_ror_u32(x, 13) ^ crypto_ror_u32(x, 22);
}

static inline SHA_LONG Sigma1(SHA_LONG x) {
    return crypto_ror_u32(x, 6) ^ crypto_ror_u32(x, 11) ^ crypto_ror_u32(x, 25);
}

static inline SHA_LONG sigma0(SHA_LONG x) {
    return crypto_ror_u32(x, 7) ^ crypto_ror_u32(x, 18) ^ (x >> 3);
}

static inline SHA_LONG sigma1(SHA_LONG x) {
    return crypto_ror_u32(x, 17) ^ crypto_ror_u32(x, 19) ^ (x >> 10);
}

static inline SHA_LONG Ch(SHA_LONG x, SHA_LONG y, SHA_LONG z) {
    return (x & y) ^ (~x & z);
}

static inline SHA_LONG Maj(SHA_LONG x, SHA_LONG y, SHA_LONG z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

static inline void sha256_msg_schedule_update(SHA_LONG *W0, SHA_LONG W1, SHA_LONG W9, SHA_LONG W14) {
    *W0 += sigma1(W14) + W9 + sigma0(W1);
}

static inline void sha256_round(SHA_LONG *a, SHA_LONG *b, SHA_LONG *c, SHA_LONG *d,
                                 SHA_LONG *e, SHA_LONG *f, SHA_LONG *g, SHA_LONG *h,
                                 SHA_LONG Kt, SHA_LONG Wt) {
    SHA_LONG T1 = *h + Sigma1(*e) + Ch(*e, *f, *g) + Kt + Wt;
    SHA_LONG T2 = Sigma0(*a) + Maj(*a, *b, *c);

    *h = *g;
    *g = *f;
    *f = *e;
    *e = *d + T1;
    *d = *c;
    *c = *b;
    *b = *a;
    *a = T1 + T2;
}

static void sha256_block_data_order(SHA256_CTX *ctx, const void *_in, size_t num) {
    const uint8_t *in = _in;
    SHA_LONG a, b, c, d, e, f, g, h;
    SHA_LONG X[16];
    int i;

    while (num--) {
        a = ctx->h[0];
        b = ctx->h[1];
        c = ctx->h[2];
        d = ctx->h[3];
        e = ctx->h[4];
        f = ctx->h[5];
        g = ctx->h[6];
        h = ctx->h[7];

        // Load message schedule
        if ((size_t)in % 4 == 0) {
            const SHA_LONG *in32 = (const SHA_LONG *)in;
            for (i = 0; i < 16; i++) {
                X[i] = be32toh(in32[i]);
            }
        } else {
            for (i = 0; i < 16; i++) {
                X[i] = crypto_load_be32toh(&in[i * 4]);
            }
        }
        in += SHA256_CBLOCK;

        // Process the first 16 words
        for (i = 0; i < 16; i++) {
            sha256_round(&a, &b, &c, &d, &e, &f, &g, &h, K256[i], X[i]);
        }

        // Process the remaining words
        for (i = 16; i < 64; i++) {
            sha256_msg_schedule_update(&X[i % 16], X[(i + 1) % 16], X[(i + 9) % 16], X[(i + 14) % 16]);
            sha256_round(&a, &b, &c, &d, &e, &f, &g, &h, K256[i], X[i % 16]);
        }

        ctx->h[0] += a;
        ctx->h[1] += b;
        ctx->h[2] += c;
        ctx->h[3] += d;
        ctx->h[4] += e;
        ctx->h[5] += f;
        ctx->h[6] += g;
        ctx->h[7] += h;
    }
}

int SHA256_Init(SHA256_CTX *c) {
    memset(c, 0, sizeof(*c));

    c->h[0] = 0x6a09e667UL;
    c->h[1] = 0xbb67ae85UL;
    c->h[2] = 0x3c6ef372UL;
    c->h[3] = 0xa54ff53aUL;
    c->h[4] = 0x510e527fUL;
    c->h[5] = 0x9b05688cUL;
    c->h[6] = 0x1f83d9abUL;
    c->h[7] = 0x5be0cd19UL;

    c->md_len = SHA256_DIGEST_LENGTH;
    c->num = 0;

    return 1;
}
LCRYPTO_ALIAS(SHA256_Init);

int SHA256_Update(SHA256_CTX *c, const void *data, size_t len) {
    const uint8_t *data_ptr = (const uint8_t *)data;
    size_t fill;
    uint32_t left;

    if (len == 0) {
        return 1;
    }

    left = c->num & 0x3F; // Number of bytes already processed in the buffer
    fill = 64 - left;    // Number of bytes remaining in the current buffer

    if (left && len >= fill) {
        // Fill the buffer and process the entire block
        memcpy((uint8_t *)c->data + left, data_ptr, fill);
        c->num += fill;
        sha256_block_data_order(c, (uint8_t *)c->data, 1);
        data_ptr += fill;
        len -= fill;
        left = 0;
    }

    // Process 64 byte blocks
    if (len >= 64) {
        size_t blocks = len / 64;
        sha256_block_data_order(c, data_ptr, blocks);
        data_ptr += blocks * 64;
        len -= blocks * 64;
    }

    // Copy any remaining data to the buffer
    if (len) {
        memcpy(c->data, data_ptr, len);
        c->num += len;
    }

    return 1;
}

LCRYPTO_ALIAS(SHA256_Update);

void SHA256_Transform(SHA256_CTX *c, const unsigned char *data) {
	sha256_block_data_order(c, data, 1);
}
LCRYPTO_ALIAS(SHA256_Transform);

int SHA256_Final(unsigned char *md, SHA256_CTX *c) {
    uint8_t *p = (uint8_t *)c->data;
    size_t n = c->num & 0x3F;

    p[n++] = 0x80;

    if (n > 56) {
        memset(p + n, 0, 64 - n);
        sha256_block_data_order(c, p, 1);
        n = 0;
    }

    memset(p + n, 0, 56 - n);

    SHA_LONG bits[2];
    bits[0] = htobe32((c->num >> 29) & 0xFFFFFFFF);
    bits[1] = htobe32(c->num << 3);

    memcpy(p + 56, bits, sizeof(bits));

    sha256_block_data_order(c, p, 1);

    for (size_t i = 0; i < SHA256_DIGEST_LENGTH / 4; i++) {
        ((SHA_LONG *)md)[i] = htobe32(c->h[i]);
    }

    return 1;
}
LCRYPTO_ALIAS(SHA256_Final);

unsigned char * SHA256(const unsigned char *d, size_t n, unsigned char *md) {
	SHA256_CTX c;
	static unsigned char m[SHA256_DIGEST_LENGTH];

	if (md == NULL) { md = m; }

	SHA256_Init(&c);
	SHA256_Update(&c, d, n);
	SHA256_Final(md, &c);

	memset(&c, 0, sizeof(c));

	return (md);
}
LCRYPTO_ALIAS(SHA256);
