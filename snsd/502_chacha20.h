#ifndef CHACHA20_H
#define CHACHA20_H

#include <stdint.h>
#include <stddef.h>

struct chacha20_ctx {
    uint32_t state[16];
    uint8_t buffer[64];  // Add buffer to context
    size_t buffer_used;  // Track how much of buffer is used
};

void chacha20_init_ctx(struct chacha20_ctx *ctx, const uint8_t key[32], const uint8_t nonce[12]);
void chacha20_block(struct chacha20_ctx *ctx);  // Remove output parameter
void chacha20_encrypt(struct chacha20_ctx *ctx, uint8_t *output, const uint8_t *input, size_t length);

#endif 