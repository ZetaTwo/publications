#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

// gcc -o sha2-test sha2-no-constants.c

#define SHFR(x, n)    (x >> n)
#define ROTR(x, n)   ((x >> n) | (x << ((sizeof (x) << 3) - n)))
#define CH(x, y, z)  ((x & y) ^ (~x & z))
#define MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))

#define N 11 // Experimentally min value for high enough precision
#define P 312  // We need primes up to including 311
#define H 8
#define K 64
#define SHA256_BLOCK_SIZE (512 / 8)
#define SHA256_DIGEST_SIZE ( 256 / 8)

#define SHA256_F1(x) (ROTR(x,  2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define SHA256_F2(x) (ROTR(x,  6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SHA256_F3(x) (ROTR(x,  7) ^ ROTR(x, 18) ^ SHFR(x,  3))
#define SHA256_F4(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ SHFR(x, 10))


#define UNPACK32(x, str)                      \
{                                             \
    *((str) + 3) = (uint8_t) ((x)      );       \
    *((str) + 2) = (uint8_t) ((x) >>  8);       \
    *((str) + 1) = (uint8_t) ((x) >> 16);       \
    *((str) + 0) = (uint8_t) ((x) >> 24);       \
}

#define PACK32(str, x)                        \
{                                             \
    *(x) =   ((uint32_t) *((str) + 3)      )    \
           | ((uint32_t) *((str) + 2) <<  8)    \
           | ((uint32_t) *((str) + 1) << 16)    \
           | ((uint32_t) *((str) + 0) << 24);   \
}

#define UNPACK64(x, str)                      \
{                                             \
    *((str) + 7) = (uint8_t) ((x)      );       \
    *((str) + 6) = (uint8_t) ((x) >>  8);       \
    *((str) + 5) = (uint8_t) ((x) >> 16);       \
    *((str) + 4) = (uint8_t) ((x) >> 24);       \
    *((str) + 3) = (uint8_t) ((x) >> 32);       \
    *((str) + 2) = (uint8_t) ((x) >> 40);       \
    *((str) + 1) = (uint8_t) ((x) >> 48);       \
    *((str) + 0) = (uint8_t) ((x) >> 56);       \
}

#define PACK64(str, x)                        \
{                                             \
    *(x) =   ((uint64_t) *((str) + 7)      )    \
           | ((uint64_t) *((str) + 6) <<  8)    \
           | ((uint64_t) *((str) + 5) << 16)    \
           | ((uint64_t) *((str) + 4) << 24)    \
           | ((uint64_t) *((str) + 3) << 32)    \
           | ((uint64_t) *((str) + 2) << 40)    \
           | ((uint64_t) *((str) + 1) << 48)    \
           | ((uint64_t) *((str) + 0) << 56);   \
}

#define SHA256_SCR(i)                         \
{                                             \
    w[i] =  SHA256_F4(w[i -  2]) + w[i -  7]  \
          + SHA256_F3(w[i - 15]) + w[i - 16]; \
}


typedef struct
{
    uint64_t tot_len;
    uint64_t len;
    uint8_t block[2 * SHA256_BLOCK_SIZE];
    uint32_t h[8];
} sha256_ctx;

static uint32_t sha256_h0[8];
static uint32_t sha256_k[64];

void init(uint32_t *h, uint32_t *k) {
    size_t pi = 0;
    uint8_t ip[P - 2];
    for (size_t p = 0; p < (P - 2); p++) {
        ip[p] = 1;
    }
    for (size_t p = 0; p < (P - 2); p++) {
        if (ip[p] == 0) {
            continue;
        }
        if (pi < H) {
            double x = p + 2;
            double xn = x / 2;
            for (size_t i = 0; i < N; i++) {
                xn = 1.0 / 2 * xn + x / (2 * xn);
            }
            h[pi] = (uint32_t)(uint64_t)(xn * (1LL << 32));
        }
        if (pi < K) {
            double x = p + 2;
            double xn = x / 3;
            for (size_t i = 0; i < N; i++) {
                xn = 2.0 / 3 * xn + x / (3 * xn * xn);
            }
            k[pi] = (uint32_t)(uint64_t)(xn * (1LL << 32));
        }
        pi++;
        for (size_t n = p+2; n < (P - 2); n++) {
            size_t val = (p + 2) * n - 2;
            if (val + 1 > (P - 2)) {
                break;
            }
            ip[val] = 0;
        }
    }
}


static void sha256_transf(uint32_t *sha256_k, sha256_ctx *ctx, const uint8_t *message,
                          uint64_t block_nb)
{
    uint32_t w[64];
    uint32_t wv[8];
    uint32_t t1, t2;
    const uint8_t *sub_block;
    uint64_t i;

    int j;
    for (i = 0; i < block_nb; i++)
    {
        sub_block = message + (i << 6);

        for (j = 0; j < 16; j++)
        {
            PACK32(&sub_block[j << 2], &w[j]);
        }

        for (j = 16; j < 64; j++)
        {
            SHA256_SCR(j);
        }

        for (j = 0; j < 8; j++)
        {
            wv[j] = ctx->h[j];
        }

        for (j = 0; j < 64; j++)
        {
            t1 = wv[7] + SHA256_F2(wv[4]) + CH(wv[4], wv[5], wv[6]) + sha256_k[j] + w[j];
            t2 = SHA256_F1(wv[0]) + MAJ(wv[0], wv[1], wv[2]);
            wv[7] = wv[6];
            wv[6] = wv[5];
            wv[5] = wv[4];
            wv[4] = wv[3] + t1;
            wv[3] = wv[2];
            wv[2] = wv[1];
            wv[1] = wv[0];
            wv[0] = t1 + t2;
        }

        for (j = 0; j < 8; j++)
        {
            ctx->h[j] += wv[j];
        }
    }
}

void sha256_init(uint32_t *sha256_h0, sha256_ctx *ctx)
{
    int i;
    for (i = 0; i < 8; i++)
    {
        ctx->h[i] = sha256_h0[i];
    }

    ctx->len = 0;
    ctx->tot_len = 0;
}

void sha256_update(uint32_t *sha256_k, sha256_ctx *ctx, const uint8_t *message, uint64_t len)
{
    uint64_t block_nb;
    uint64_t new_len, rem_len, tmp_len;
    const uint8_t *shifted_message;

    tmp_len = SHA256_BLOCK_SIZE - ctx->len;
    rem_len = len < tmp_len ? len : tmp_len;

    memcpy(&ctx->block[ctx->len], message, rem_len);

    if (ctx->len + len < SHA256_BLOCK_SIZE)
    {
        ctx->len += len;
        return;
    }

    new_len = len - rem_len;
    block_nb = new_len / SHA256_BLOCK_SIZE;

    shifted_message = message + rem_len;

    sha256_transf(sha256_k, ctx, ctx->block, 1);
    sha256_transf(sha256_k, ctx, shifted_message, block_nb);

    rem_len = new_len % SHA256_BLOCK_SIZE;

    memcpy(ctx->block, &shifted_message[block_nb << 6], rem_len);

    ctx->len = rem_len;
    ctx->tot_len += (block_nb + 1) << 6;
}

void sha256_final(uint32_t *sha256_k, sha256_ctx *ctx, uint8_t *digest)
{
    uint64_t block_nb;
    uint64_t pm_len;
    uint64_t len_b;
    uint64_t tot_len;

    int i;

    block_nb = (1 + ((SHA256_BLOCK_SIZE - 9) < (ctx->len % SHA256_BLOCK_SIZE)));

    tot_len = ctx->tot_len + ctx->len;
    ctx->tot_len = tot_len;

    len_b = tot_len << 3;
    pm_len = block_nb << 6;

    memset(ctx->block + ctx->len, 0, pm_len - ctx->len);
    ctx->block[ctx->len] = 0x80;
    UNPACK64(len_b, ctx->block + pm_len - 8);

    sha256_transf(sha256_k, ctx, ctx->block, block_nb);

    for (i = 0; i < 8; i++)
    {
        UNPACK32(ctx->h[i], &digest[i << 2]);
    }
}

void sha256(uint32_t *sha256_k, uint32_t *sha256_h0, const uint8_t *message, uint64_t len, uint8_t *digest)
{
    sha256_ctx ctx;

    sha256_init(sha256_h0, &ctx);
    sha256_update(sha256_k, &ctx, message, len);
    sha256_final(sha256_k, &ctx, digest);
}

int main()
{
    init(sha256_h0, sha256_k);

    /*for (size_t i = 0; i < H; i++)
    {
        printf("H[%ld] = %x\n", i, sha256_h0[i]);
    }
    for (size_t i = 0; i < K; i++)
    {
        printf("K[%ld] = %x\n", i, sha256_k[i]);
    }*/

    uint8_t digest[SHA256_DIGEST_SIZE];
    uint8_t message[] = "abc";
    sha256(sha256_k, sha256_h0, message, 3, digest);
    printf("SHA256(\"abc\") = ");
    for (size_t i = 0; i < SHA256_DIGEST_SIZE; i++)
    {
        printf("%02x", digest[i]);
    }
    printf("\n");
}
