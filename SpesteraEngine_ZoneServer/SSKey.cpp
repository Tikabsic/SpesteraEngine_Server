#include "SSKey.h"
#include <openssl/rand.h>
#include <openssl/err.h>
#include <cstring>
#include <iostream>
#include <vector>

SSKey::SSKey(const std::string& security_code) : security_code_(security_code) {
    // Ensure the security code is the correct length
    if (security_code_.length() != AES_KEY_LENGTH) {
        throw std::runtime_error("Security code must be 32 bytes (256 bits) long.");
    }
}

SSKey::~SSKey() {}

std::string SSKey::encrypt_ssk(const std::string& key) {
    unsigned char iv[AES_IV_LENGTH];
    std::vector<unsigned char> ciphertext(key.size() + AES_IV_LENGTH);

    // Generate random IV
    if (!RAND_bytes(iv, AES_IV_LENGTH)) {
        throw std::runtime_error("Failed to generate random IV");
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create cipher context");
    }

    int len;
    int ciphertext_len;

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, (unsigned char*)security_code_.data(), iv)) {
        throw std::runtime_error("Failed to initialize encryption");
    }

    if (1 != EVP_EncryptUpdate(ctx, ciphertext.data(), &len, (unsigned char*)key.data(), key.size())) {
        throw std::runtime_error("Failed to encrypt");
    }
    ciphertext_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len)) {
        throw std::runtime_error("Failed to finalize encryption");
    }
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    std::string result((char*)iv, AES_IV_LENGTH);
    result += std::string((char*)ciphertext.data(), ciphertext_len);

    return result;
}

std::string SSKey::decrypt_ssk(const std::string& encrypted_key) {
    unsigned char iv[AES_IV_LENGTH];
    std::memcpy(iv, encrypted_key.data(), AES_IV_LENGTH);

    std::vector<unsigned char> plaintext(encrypted_key.size() - AES_IV_LENGTH);
    int len;
    int plaintext_len;

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create cipher context");
    }

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, (unsigned char*)security_code_.data(), iv)) {
        throw std::runtime_error("Failed to initialize decryption");
    }

    if (1 != EVP_DecryptUpdate(ctx, plaintext.data(), &len, (unsigned char*)encrypted_key.data() + AES_IV_LENGTH, encrypted_key.size() - AES_IV_LENGTH)) {
        throw std::runtime_error("Failed to decrypt");
    }
    plaintext_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len)) {
        throw std::runtime_error("Failed to finalize decryption");
    }
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return std::string((char*)plaintext.data(), plaintext_len);
}

bool SSKey::validate_ssk(const std::string& decrypted_ssk, const std::string& encrypted_ssk) {
    return decrypt_ssk(encrypted_ssk) == decrypted_ssk;
}