#include "502_chacha20.h"

// NIP-44 encryption/decryption implementation

// NIP-44 constant salt
const uint8_t NIP44_SALT[8] = { 0x6e, 0x69, 0x70, 0x34, 0x34, 0x2d, 0x76, 0x32 }; // "nip44-v2"

// Helper function to calculate integer log2
uint32_t _math_int_log2(uint32_t x) {
    uint32_t r = 0;
    while (x >>= 1) {
        r++;
    }
    return r;
}

// Calculate padded length according to NIP-44 spec
size_t calcPaddedLen(size_t unpadded_len) {
    logInfo("calcPaddedLen input: " + String(unpadded_len));
    
    if (unpadded_len < 1 || unpadded_len > 65535) {
        logInfo("calcPaddedLen failed: Invalid length " + String(unpadded_len));
        return 0;
    }
    
    // For messages <= 32 bytes, pad to 32
    if (unpadded_len <= 32) {
        logInfo("calcPaddedLen: using 32 byte padding");
        return 32;
    }
    
    // Calculate next power of 2 based on (len-1)
    size_t len_minus_one = unpadded_len - 1;
    size_t next_power = 1;
    
    while (next_power <= len_minus_one) {
        next_power <<= 1;
        logInfo("next_power now: " + String(next_power));
    }
    
    // Calculate chunk size
    size_t chunk = next_power <= 256 ? 32 : next_power / 8;
    logInfo("chunk size: " + String(chunk));
    
    // Calculate final padded length
    return chunk * (((unpadded_len - 1) / chunk) + 1);
}

// Pad plaintext according to NIP-44 spec
std::vector<uint8_t> padMessage(const String &plaintext) {
    size_t unpadded_len = plaintext.length();
    logInfo("padMessage input length: " + String(unpadded_len));
    
    // Validate length
    if (unpadded_len < 1 || unpadded_len > 65535) {
        logInfo("padMessage: Invalid length");
        return std::vector<uint8_t>();
    }
    
    // Calculate padded length
    size_t padded_len = calcPaddedLen(unpadded_len);
    logInfo("padMessage: calculated padded length: " + String(padded_len));
    
    if (padded_len == 0) {
        logInfo("padMessage: padding calculation failed");
        return std::vector<uint8_t>();
    }
    
    try {
        // Create result vector with size for prefix + padded content
        std::vector<uint8_t> result(2 + padded_len);
        logInfo("padMessage: allocated vector size: " + String(result.size()));
        
        // Write length prefix (big-endian uint16)
        result[0] = (unpadded_len >> 8) & 0xFF;
        result[1] = unpadded_len & 0xFF;
        
        // Copy plaintext
        memcpy(result.data() + 2, plaintext.c_str(), unpadded_len);
        
        // Zero-pad the remainder
        if (padded_len > unpadded_len) {
            memset(result.data() + 2 + unpadded_len, 0, padded_len - unpadded_len);
        }
        
        logInfo("padMessage: final padded size: " + String(result.size()));
        return result;
    } catch (...) {
        logInfo("padMessage: exception occurred");
        return std::vector<uint8_t>();
    }
}

// Unpad according to NIP-44 spec
String unpadMessage(const std::vector<uint8_t> &padded) {
    if (padded.size() < 34) {
        logInfo("unpadMessage failed: Padded size too small " + String(padded.size()));
        return "";
    }
    
    // Read length prefix
    size_t unpadded_len = (padded[0] << 8) | padded[1];
    size_t expected_padded_len = calcPaddedLen(unpadded_len);
    
    logInfo("unpadMessage: unpaddedLen=" + String(unpadded_len) + 
            " expectedPaddedLen=" + String(expected_padded_len) + 
            " actualPaddedSize=" + String(padded.size() - 2));
    
    // Validate lengths
    if (unpadded_len == 0 || 
        unpadded_len > 65535 || 
        unpadded_len > padded.size() - 2 ||
        padded.size() - 2 != expected_padded_len) {
        logInfo("unpadMessage failed: Invalid lengths");
        return "";
    }
    
    // Extract unpadded content
    return String((char*)(padded.data() + 2), unpadded_len);
}

// Update HKDF implementation to match reference
void hkdf_sha256(const uint8_t *salt, size_t salt_len,
                 const uint8_t *ikm, size_t ikm_len,
                 const uint8_t *info, size_t info_len,
                 uint8_t *okm, size_t okm_len) {
    // HKDF-Extract phase
    uint8_t prk[32];
    mbedtls_md_context_t ctx;
    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1);
    
    // Extract phase
    mbedtls_md_hmac_starts(&ctx, salt, salt_len);
    mbedtls_md_hmac_update(&ctx, ikm, ikm_len);
    mbedtls_md_hmac_finish(&ctx, prk);
    
    // Expand phase
    uint8_t T[32] = {0}; // T(0) is empty string
    uint8_t counter = 1;
    size_t offset = 0;
    
    while (okm_len > 0) {
        mbedtls_md_hmac_starts(&ctx, prk, 32);
        
        if (offset > 0) {
            mbedtls_md_hmac_update(&ctx, T, 32);
        }
        
        if (info_len > 0) {
            mbedtls_md_hmac_update(&ctx, info, info_len);
        }
        
        mbedtls_md_hmac_update(&ctx, &counter, 1);
        mbedtls_md_hmac_finish(&ctx, T);
        
        size_t todo = (okm_len < 32) ? okm_len : 32;
        memcpy(okm + offset, T, todo);
        
        counter++;
        offset += todo;
        okm_len -= todo;
    }
    
    mbedtls_md_free(&ctx);
}

// Update getMessageKeys to use HKDF
bool getMessageKeys(const uint8_t *conversation_key, const uint8_t *nonce,
                   uint8_t *chacha_key, uint8_t *chacha_nonce, uint8_t *hmac_key) {
    
    // Use constant salt for key derivation
    uint8_t derived_key[76]; // For chacha_key(32) + chacha_nonce(12) + hmac_key(32)
    
    hkdf_sha256(NIP44_SALT, sizeof(NIP44_SALT),
                conversation_key, 32,
                nonce, 32,
                derived_key, sizeof(derived_key));
    
    // Split derived key into components
    memcpy(chacha_key, derived_key, 32);
    memcpy(chacha_nonce, derived_key + 32, 12);
    memcpy(hmac_key, derived_key + 44, 32);
    
    return true;
}

// Update HMAC calculation to match reference
void hmac_aad(const uint8_t *key, size_t key_len,
              const uint8_t *message, size_t msg_len,
              const uint8_t *aad, size_t aad_len,
              uint8_t *output) {
    mbedtls_md_context_t ctx;
    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1);
    mbedtls_md_hmac_starts(&ctx, key, key_len);

    // First update with the nonce (aad)
    if (aad_len > 0) {
        mbedtls_md_hmac_update(&ctx, aad, aad_len);
    }
    
    // Then update with the ciphertext
    if (msg_len > 0) {
        mbedtls_md_hmac_update(&ctx, message, msg_len);
    }

    mbedtls_md_hmac_finish(&ctx, output);
    mbedtls_md_free(&ctx);
}

// Helper function to verify base64 encoding/decoding
bool verifyBase64(const std::vector<uint8_t>& input, String& output) {
    // First calculate required length
    size_t base64_len = 0;
    int ret = mbedtls_base64_encode(nullptr, 0, &base64_len, input.data(), input.size());
    if (ret != MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) {
        logInfo("Base64 length calculation failed");
        return false;
    }
    
    // Allocate buffer with space for null terminator
    std::vector<uint8_t> base64_out(base64_len + 1);
    ret = mbedtls_base64_encode(base64_out.data(), base64_len + 1, &base64_len,
                               input.data(), input.size());
    if (ret != 0) {
        logInfo("Base64 encode failed");
        return false;
    }
    
    // Verify we can decode back to original
    size_t decoded_len = 0;
    ret = mbedtls_base64_decode(nullptr, 0, &decoded_len,
                               base64_out.data(), base64_len);
    if (ret != MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL || decoded_len != input.size()) {
        logInfo("Base64 decode length check failed");
        return false;
    }
    
    std::vector<uint8_t> decoded(decoded_len);
    ret = mbedtls_base64_decode(decoded.data(), decoded_len, &decoded_len,
                               base64_out.data(), base64_len);
    if (ret != 0 || decoded_len != input.size()) {
        logInfo("Base64 decode failed");
        return false;
    }
    
    // Verify content matches
    if (memcmp(decoded.data(), input.data(), input.size()) != 0) {
        logInfo("Base64 decoded content mismatch");
        return false;
    }
    
    output = String((char*)base64_out.data());
    return true;
}

// Base64 encoding helper
String base64_encode(const uint8_t* input, size_t length) {
    size_t base64_len = 0;
    
    // Calculate required length
    int ret = mbedtls_base64_encode(nullptr, 0, &base64_len, input, length);
    if (ret != MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) {
        return "";
    }
    
    // Allocate buffer with space for null terminator
    std::vector<uint8_t> base64_out(base64_len + 1);
    ret = mbedtls_base64_encode(base64_out.data(), base64_len + 1, &base64_len,
                               input, length);
    if (ret != 0) {
        return "";
    }
    
    return String((char*)base64_out.data());
}

// Update the encryption function with ChaCha20 and proper MAC
String encryptMessageNip44(const String &plaintext, const String &sharedSecretHex) {
    try {
        // Convert shared secret from hex
        uint8_t conversation_key[32];
        if (!fromHex(sharedSecretHex, conversation_key, 32)) {
            return "";
        }
        
        // Generate random nonce
        uint8_t nonce[32];
        generateRandomIV(nonce, 32);
        
        // Pad message according to NIP-44 spec
        std::vector<uint8_t> padded = padMessage(plaintext);
        if (padded.empty()) {
            return "";
        }
        
        // Derive encryption keys
        uint8_t chacha_key[32];
        uint8_t chacha_nonce[12];
        uint8_t hmac_key[32];
        if (!getMessageKeys(conversation_key, nonce, chacha_key, chacha_nonce, hmac_key)) {
            return "";
        }
        
        // Encrypt with ChaCha20
        std::vector<uint8_t> ciphertext(padded.size());
        struct chacha20_ctx chacha;
        chacha20_init_ctx(&chacha, chacha_key, chacha_nonce);
        chacha20_encrypt(&chacha, ciphertext.data(), padded.data(), padded.size());
        
        // Calculate MAC over nonce+ciphertext
        uint8_t mac[32];
        hmac_aad(hmac_key, 32,
                 ciphertext.data(), ciphertext.size(),
                 nonce, 32,
                 mac);
        
        // Construct final payload: version + nonce + ciphertext + mac
        std::vector<uint8_t> payload;
        payload.push_back(0x02);  // Version 2
        payload.insert(payload.end(), nonce, nonce + 32);
        payload.insert(payload.end(), ciphertext.begin(), ciphertext.end());
        payload.insert(payload.end(), mac, mac + 32);
        
        // Base64 encode
        return base64_encode(payload.data(), payload.size());
    }
    catch (...) {
        return "";
    }
}

// Update decryption function with ChaCha20 and proper MAC verification
String decryptMessageNip44(const String &payload, const String &sharedSecretHex) {
    try {
        logInfo("Decrypting payload of length: " + String(payload.length()));
        
        // First get the required buffer size
        size_t binary_len = 0;
        int ret = mbedtls_base64_decode(nullptr, 0, &binary_len,
                                      (const uint8_t*)payload.c_str(),
                                      payload.length());
                                      
        if (ret != MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) {
            logInfo("Decrypt failed: Base64 length check failed");
            return "";
        }
        
        // Allocate buffer
        std::vector<uint8_t> binary(binary_len);
        
        // Decode base64
        ret = mbedtls_base64_decode(binary.data(), binary_len, &binary_len,
                                   (const uint8_t*)payload.c_str(),
                                   payload.length());
        
        if (ret != 0) {
            logInfo("Decrypt failed: Base64 decode failed with code: " + String(ret));
            return "";
        }
        
        logInfo("Decoded binary length: " + String(binary_len));
        
        // Basic length validation
        if (binary_len < 65) { // version(1) + nonce(32) + mac(32)
            logInfo("Decrypt failed: Binary too short");
            return "";
        }
        
        // Extract components
        const uint8_t *nonce = binary.data() + 1;
        const uint8_t *ciphertext = binary.data() + 33;
        const uint8_t *mac = binary.data() + binary_len - 32;
        size_t ciphertext_len = binary_len - (1 + 32 + 32);
        
        logInfo("Decryption component sizes:");
        logInfo("Total binary length: " + String(binary_len));
        logInfo("Ciphertext length: " + String(ciphertext_len));
        logInfo("Ciphertext starts at: " + String(33));
        logInfo("MAC starts at: " + String(binary_len - 32));
        
        // Validate version
        if (binary[0] != 0x02) {
            logInfo("Decrypt failed: Invalid version " + String(binary[0]));
            return "";
        }
        
        // Convert shared secret from hex
        uint8_t conversation_key[32];
        if (!fromHex(sharedSecretHex, conversation_key, 32)) {
            logInfo("Decrypt failed: Invalid shared secret hex");
            return "";
        }
        
        // Get message keys
        uint8_t chacha_key[32];
        uint8_t chacha_nonce[12];
        uint8_t hmac_key[32];
        if (!getMessageKeys(conversation_key, nonce, chacha_key, chacha_nonce, hmac_key)) {
            logInfo("Decrypt failed: getMessageKeys failed");
            return "";
        }
        
        // Verify MAC
        uint8_t calculated_mac[32];
        hmac_aad(hmac_key, 32,
                 ciphertext, ciphertext_len,
                 nonce, 32,
                 calculated_mac);
        
        // Log MAC comparison
        String calcMacHex;
        for(size_t i = 0; i < 16; i++) {
            char hex[3];
            snprintf(hex, sizeof(hex), "%02x", calculated_mac[i]);
            calcMacHex += hex;
        }
        logInfo("First bytes of calculated MAC: " + calcMacHex);
        
        if (memcmp(calculated_mac, mac, 32) != 0) {
            logInfo("Decrypt failed: MAC verification failed");
            return "";
        }
        
        // Decrypt with ChaCha20
        std::vector<uint8_t> plaintext(ciphertext_len);
        struct chacha20_ctx chacha;
        chacha20_init_ctx(&chacha, chacha_key, chacha_nonce);
        chacha20_encrypt(&chacha, plaintext.data(), ciphertext, ciphertext_len);
        
        // Add logging for decrypted size
        logInfo("Plaintext buffer size: " + String(plaintext.size()));
        
        // Unpad
        String result = unpadMessage(plaintext);
        logInfo("Unpadded result length: " + String(result.length()));
        
        return result;
    } catch (...) {
        logInfo("Decrypt failed: Exception caught");
        return "";
    }
}

// HMAC-SHA256 implementation
void hmac_sha256(const uint8_t* key, size_t key_len,
                 const uint8_t* msg, size_t msg_len,
                 uint8_t* hmac) {
    mbedtls_md_context_t ctx;
    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1);
    mbedtls_md_hmac_starts(&ctx, key, key_len);
    mbedtls_md_hmac_update(&ctx, msg, msg_len);
    mbedtls_md_hmac_finish(&ctx, hmac);
    mbedtls_md_free(&ctx);
} 