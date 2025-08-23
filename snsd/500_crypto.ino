// Function to calculate modular square root using mini-gmp
bool modular_sqrt(mpz_t result, const mpz_t n, const mpz_t p) {
    mpz_t legendre, exponent, temp;
    mpz_init(legendre);
    mpz_init(exponent);
    mpz_init(temp);

    // Legendre symbol: (n/p) = n^((p-1)/2) mod p
    mpz_sub_ui(temp, p, 1); // temp = p - 1
    mpz_fdiv_q_ui(exponent, temp, 2); // exponent = (p - 1) / 2
    mpz_powm(legendre, n, exponent, p); // legendre = n^((p-1)/2) mod p

    if (mpz_cmp_ui(legendre, 1) != 0) {
        // No modular square root exists
        mpz_clear(legendre);
        mpz_clear(exponent);
        mpz_clear(temp);
        return false;
    }

    // Direct calculation for p ≡ 3 (mod 4)
    mpz_add_ui(temp, p, 1); // temp = p + 1
    mpz_fdiv_q_ui(temp, temp, 4); // temp = (p + 1) / 4
    mpz_powm(result, n, temp, p); // result = n^((p+1)/4) mod p

    // Cleanup
    mpz_clear(legendre);
    mpz_clear(exponent);
    mpz_clear(temp);
    return true;
}

// Function to reconstruct full public key
String reconstructPublicKey(const String &xHex) {
    mpz_t x, y, rhs, p, a, b;
    mpz_init(x);
    mpz_init(y);
    mpz_init(rhs);
    mpz_init(p);
    mpz_init(a);
    mpz_init(b);

    // Set curve parameters for secp256k1
    mpz_set_str(p, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F", 16);
    mpz_set_ui(a, 0); // a = 0
    mpz_set_ui(b, 7); // b = 7

    // Parse X-coordinate
    mpz_set_str(x, xHex.c_str(), 16);

    // Calculate rhs = x^3 + ax + b mod p
    mpz_powm_ui(rhs, x, 3, p); // rhs = x^3 mod p
    mpz_add(rhs, rhs, b); // rhs = x^3 + b mod p
    mpz_mod(rhs, rhs, p); // Normalize rhs mod p

    // Calculate Y-coordinate
    if (!modular_sqrt(y, rhs, p)) {
        // Handle error: invalid public key
        logInfo("Error: No modular square root exists for the given X-coordinate.");
        mpz_clear(x);
        mpz_clear(y);
        mpz_clear(rhs);
        mpz_clear(p);
        mpz_clear(a);
        mpz_clear(b);
        return "";
    }

    // Ensure even Y (mimic '02' prefix behavior)
    if (mpz_odd_p(y)) {
        mpz_sub(y, p, y);
    }

    // Convert X and Y back to hex strings
    char xHexStr[65], yHexStr[65];
    mpz_get_str(xHexStr, 16, x);
    mpz_get_str(yHexStr, 16, y);

    // Cleanup
    mpz_clear(x);
    mpz_clear(y);
    mpz_clear(rhs);
    mpz_clear(p);
    mpz_clear(a);
    mpz_clear(b);

    // Return full 128-character public key
    return String(xHexStr) + String(yHexStr);
}

// Encrypt a direct message
String encryptDirectMessageContent(const String &sharedSecretHex, const String &text) {
    // Convert shared secret from hex to binary
    uint8_t sharedSecret[32];
    fromHex(sharedSecretHex, sharedSecret, 32);

    // Generate a random IV
    uint8_t iv[16];
    generateRandomIV(iv, sizeof(iv));

    // Prepare plaintext with zero padding
    String data = text;
    while (data.length() % 16 != 0) data += char(0x00);

    // Convert plaintext to byte array
    int byteSize = data.length();
    uint8_t messageBin[byteSize];
    data.getBytes(messageBin, byteSize);

    byte outputBuffer[byteSize];

    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, sharedSecret, 256); // AES-256 requires a 32-byte key
    mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, byteSize, iv, messageBin, outputBuffer);
    mbedtls_aes_free(&aes);

    // Base64 encode the ciphertext
    size_t base64CiphertextLen = 0;
    mbedtls_base64_encode(nullptr, 0, &base64CiphertextLen, outputBuffer, byteSize);
    char base64Ciphertext[base64CiphertextLen + 1]; // +1 for null terminator
    mbedtls_base64_encode((unsigned char *)base64Ciphertext, base64CiphertextLen + 1, &base64CiphertextLen, outputBuffer, byteSize);

    // Base64 encode the IV
    size_t base64IvLen = 0;
    mbedtls_base64_encode(nullptr, 0, &base64IvLen, iv, sizeof(iv));
    char base64Iv[base64IvLen + 1]; // +1 for null terminator
    mbedtls_base64_encode((unsigned char *)base64Iv, base64IvLen + 1, &base64IvLen, iv, sizeof(iv));

    // Combine Base64-encoded ciphertext and IV for transmission
    String encryptedContent = String(base64Ciphertext) + "?iv=" + String(base64Iv);

    // Trim any accidental whitespace or newlines (unlikely but a safeguard)
    encryptedContent.trim();
    return encryptedContent;
}

String decryptDirectMessageContent(const String &sharedSecretHex, const String &encryptedContent) {
    // Find the separator between ciphertext and IV
    int separatorIndex = encryptedContent.indexOf("?iv=");
    if (separatorIndex == -1) {
        logInfo("Decryption failed: Invalid format (missing ?iv=).");
        return "";
    }

    // Split ciphertext and IV
    String encryptedMessageBase64 = encryptedContent.substring(0, separatorIndex);
    String ivBase64 = encryptedContent.substring(separatorIndex + 4);

    // Debug: Log split content
    logInfo("Encrypted Message Base64: " + encryptedMessageBase64);
    logInfo("IV Base64: " + ivBase64);

    // Decode Base64 for ciphertext
    size_t ciphertextLen = 0;
    size_t ciphertextMaxLen = encryptedMessageBase64.length(); // Estimate max length
    uint8_t ciphertext[ciphertextMaxLen];
    int ret = mbedtls_base64_decode(ciphertext, ciphertextMaxLen, &ciphertextLen,
                                    (const unsigned char *)encryptedMessageBase64.c_str(),
                                    encryptedMessageBase64.length());
    if (ret != 0) {
        logInfo("Decryption failed: Error decoding Base64 ciphertext.");
        return "";
    }

    // Decode Base64 for IV
    size_t ivLen = 0;
    size_t ivMaxLen = ivBase64.length(); // Estimate max length
    uint8_t iv[ivMaxLen];
    ret = mbedtls_base64_decode(iv, ivMaxLen, &ivLen,
                                (const unsigned char *)ivBase64.c_str(), ivBase64.length());
    if (ret != 0 || ivLen != 16) {
        logInfo("Decryption failed: Error decoding Base64 IV or invalid IV length.");
        return "";
    }

    // Convert shared secret from hex to binary
    uint8_t sharedSecret[32];
    fromHex(sharedSecretHex, sharedSecret, 32);

    byte outputBuffer[ciphertextLen];

    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_dec(&aes, sharedSecret, 256);
    mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, ciphertextLen, iv, ciphertext, outputBuffer);
    mbedtls_aes_free(&aes);

    // Remove padding
    uint8_t paddingLen = outputBuffer[ciphertextLen - 1];
    if (paddingLen > 16) {
        logInfo("Decryption failed: Invalid padding.");
        return "";
    }

    size_t plaintextLen = ciphertextLen - paddingLen;

    // Convert to string and return
    String decryptedData = String((char *)outputBuffer).substring(0, plaintextLen);
    logInfo("Decrypted Data: " + decryptedData);

    // If no prefix is expected, simply return the data
    return decryptedData;
}
