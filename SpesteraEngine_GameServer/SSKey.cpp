#include "SSKey.h"

SSKey::SSKey() {
    const std::string base64_security_code = "pGf+lh2QOloKXU/mVh5k4ErP76mkcG6Dt1TZMZTj4Ks=";
    base64_decode(base64_security_code, security_code_);

    if ( security_code_.size() != AES_KEY_LENGTH ) {
        throw std::runtime_error("Security code must be 32 bytes (256 bits) long.");
    }
}

SSKey::~SSKey() {}

#include <openssl/rand.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <vector>
#include <string>
#include <iostream>


void SSKey::base64_decode(const std::string& in, std::vector<unsigned char>& out) {
    BIO* bio;
    BIO* b64;
    int decodeLen = calcDecodeLength(in.c_str());
    out.resize(decodeLen);

    bio = BIO_new_mem_buf(in.c_str(), -1);
    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_push(b64, bio);
    decodeLen = BIO_read(bio, out.data(), in.size());
    out.resize(decodeLen);
    BIO_free_all(bio);
}

int SSKey::calcDecodeLength(const char* b64input) {
    int len = strlen(b64input);
    int padding = 0;
    if ( b64input[len - 1] == '=' ) {
        padding++;
        if ( b64input[len - 2] == '=' )
            padding++;
    }
    return ( len * 3 ) / 4 - padding;
}

void SSKey::handle_errors() {
    ERR_print_errors_fp(stderr);
    throw std::runtime_error("An OpenSSL error occurred");
}

std::string SSKey::decrypt_ssk(const std::string& base64_encoded_token) {
    std::vector<unsigned char> encrypted_data;
    base64_decode(base64_encoded_token, encrypted_data);

    unsigned char iv[AES_IV_LENGTH];
    std::memcpy(iv, encrypted_data.data(), AES_IV_LENGTH);

    std::vector<unsigned char> ciphertext(encrypted_data.size() - AES_IV_LENGTH);
    std::memcpy(ciphertext.data(), encrypted_data.data() + AES_IV_LENGTH, ciphertext.size());

    std::vector<unsigned char> plaintext(ciphertext.size() + AES_BLOCK_SIZE);
    int len = 0;
    int plaintext_len = 0;

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if ( !ctx ) {
        handle_errors();
    }

    if ( 1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, security_code_.data(), iv) ) {
        EVP_CIPHER_CTX_free(ctx);
        handle_errors();
    }

    if ( 1 != EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), ciphertext.size()) ) {
        EVP_CIPHER_CTX_free(ctx);
        handle_errors();
    }
    plaintext_len = len;

    if ( 1 != EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) ) {
        EVP_CIPHER_CTX_free(ctx);
        handle_errors();
    }
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    std::string result(reinterpret_cast< char* >( plaintext.data() ), plaintext_len);

    return result;
}

std::string SSKey::encrypt_ssk(const std::string& plaintext) {
    unsigned char iv[AES_IV_LENGTH];

    if ( !RAND_bytes(iv, AES_IV_LENGTH) ) {
        handle_errors();
    }

    std::vector<unsigned char> ciphertext(plaintext.size() + AES_BLOCK_SIZE);
    int len = 0;
    int ciphertext_len = 0;

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if ( !ctx ) {
        handle_errors();
    }

    if ( 1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, security_code_.data(), iv) ) {
        EVP_CIPHER_CTX_free(ctx);
        handle_errors();
    }

    if ( 1 != EVP_EncryptUpdate(ctx, ciphertext.data(), &len,
        reinterpret_cast< const unsigned char* >( plaintext.data() ),
        plaintext.size()) ) {
        EVP_CIPHER_CTX_free(ctx);
        handle_errors();
    }
    ciphertext_len = len;

    if ( 1 != EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) ) {
        EVP_CIPHER_CTX_free(ctx);
        handle_errors();
    }
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    ciphertext.resize(ciphertext_len);

    std::vector<unsigned char> encrypted_data(iv, iv + AES_IV_LENGTH);
    encrypted_data.insert(encrypted_data.end(), ciphertext.begin(), ciphertext.end());

    std::string base64_encrypted_data = base64_encode(encrypted_data.data(), encrypted_data.size());

    return base64_encrypted_data;
}

std::string SSKey::base64_encode(const unsigned char* data, size_t length) {
    BIO* bio;
    BIO* b64;
    BUF_MEM* bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, data, length);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);

    std::string encoded_data(bufferPtr->data, bufferPtr->length);

    BIO_free_all(bio);

    return encoded_data;
}