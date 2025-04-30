#include "502_chacha20.h"

#define ROTL32(v, n) (((v) << (n)) | ((v) >> (32 - (n))))

#define QUARTERROUND(a, b, c, d) \
    a += b; d ^= a; d = ROTL32(d, 16); \
    c += d; b ^= c; b = ROTL32(b, 12); \
    a += b; d ^= a; d = ROTL32(d, 8); \
    c += d; b ^= c; b = ROTL32(b, 7);

void chacha20_init_ctx(struct chacha20_ctx *ctx, const uint8_t key[32], const uint8_t nonce[12]) {
    // Initialize context
    memset(ctx, 0, sizeof(*ctx));
    
    // Constants - "expand 32-byte k"
    ctx->state[0] = 0x61707865;
    ctx->state[1] = 0x3320646e;
    ctx->state[2] = 0x79622d32;
    ctx->state[3] = 0x6b206574;
    
    // Key
    for (int i = 0; i < 8; i++) {
        ctx->state[4 + i] = ((uint32_t)key[4*i + 0] << 0) |
                           ((uint32_t)key[4*i + 1] << 8) |
                           ((uint32_t)key[4*i + 2] << 16) |
                           ((uint32_t)key[4*i + 3] << 24);
    }
    
    // Counter (starts at 0)
    ctx->state[12] = 0;
    
    // Nonce
    ctx->state[13] = ((uint32_t)nonce[0] << 0) |
                     ((uint32_t)nonce[1] << 8) |
                     ((uint32_t)nonce[2] << 16) |
                     ((uint32_t)nonce[3] << 24);
    ctx->state[14] = ((uint32_t)nonce[4] << 0) |
                     ((uint32_t)nonce[5] << 8) |
                     ((uint32_t)nonce[6] << 16) |
                     ((uint32_t)nonce[7] << 24);
    ctx->state[15] = ((uint32_t)nonce[8] << 0) |
                     ((uint32_t)nonce[9] << 8) |
                     ((uint32_t)nonce[10] << 16) |
                     ((uint32_t)nonce[11] << 24);
                     
    ctx->buffer_used = 64; // Force new block generation on first use
}

void chacha20_block(struct chacha20_ctx *ctx) {
    uint32_t x[16];
    memcpy(x, ctx->state, sizeof(x));
    
    // Perform ChaCha20 rounds
    for (int i = 0; i < 10; i++) {
        // Column rounds
        QUARTERROUND(x[0], x[4], x[8], x[12])
        QUARTERROUND(x[1], x[5], x[9], x[13])
        QUARTERROUND(x[2], x[6], x[10], x[14])
        QUARTERROUND(x[3], x[7], x[11], x[15])
        // Diagonal rounds
        QUARTERROUND(x[0], x[5], x[10], x[15])
        QUARTERROUND(x[1], x[6], x[11], x[12])
        QUARTERROUND(x[2], x[7], x[8], x[13])
        QUARTERROUND(x[3], x[4], x[9], x[14])
    }
    
    // Add input state to the result
    for (int i = 0; i < 16; i++) {
        uint32_t v = x[i] + ctx->state[i];
        ctx->buffer[4*i + 0] = v & 0xff;
        ctx->buffer[4*i + 1] = (v >> 8) & 0xff;
        ctx->buffer[4*i + 2] = (v >> 16) & 0xff;
        ctx->buffer[4*i + 3] = (v >> 24) & 0xff;
    }
    
    // Reset buffer usage
    ctx->buffer_used = 0;
    
    // Increment counter with overflow check
    ctx->state[12]++;
    if (ctx->state[12] == 0) {
        // Counter wrapped, increment next word
        ctx->state[13]++;
    }
}

void chacha20_encrypt(struct chacha20_ctx *ctx, uint8_t *output, const uint8_t *input, size_t length) {
    if (!output || !input || length == 0) {
        logInfo("ChaCha20: Invalid parameters");
        return;
    }
    
    logInfo("ChaCha20 processing length: " + String(length));
    size_t total_processed = 0;
    
    // Log first few bytes of input
    String inputHex;
    for(size_t i = 0; i < std::min(length, (size_t)16); i++) {
        char hex[3];
        snprintf(hex, sizeof(hex), "%02x", input[i]);
        inputHex += hex;
    }
    logInfo("First bytes of input: " + inputHex);
    
    while (length > 0) {
        // Generate new block if needed
        if (ctx->buffer_used >= 64) {
            logInfo("Generating new block at offset: " + String(total_processed));
            chacha20_block(ctx);
        }
        
        // Calculate how many bytes we can process from current block
        size_t available = 64 - ctx->buffer_used;
        size_t use = length > available ? available : length;
        
        logInfo("Processing block of size: " + String(use) + 
                " at offset: " + String(total_processed) + 
                " buffer_used: " + String(ctx->buffer_used) +
                " remaining: " + String(length));
        
        // XOR the input with the keystream
        for (size_t i = 0; i < use; i++) {
            output[total_processed + i] = input[total_processed + i] ^ 
                                        ctx->buffer[ctx->buffer_used + i];
        }
        
        // Log a few bytes of output after each block
        if (total_processed < 16) {
            String outputHex;
            for(size_t i = total_processed; i < std::min(total_processed + use, (size_t)16); i++) {
                char hex[3];
                snprintf(hex, sizeof(hex), "%02x", output[i]);
                outputHex += hex;
            }
            logInfo("Output bytes at " + String(total_processed) + ": " + outputHex);
        }
        
        ctx->buffer_used += use;
        length -= use;
        total_processed += use;
    }
    
    logInfo("ChaCha20 completed, total processed: " + String(total_processed));
} 