#include "SSKey.h"

SSKey::SSKey() {
    const std::string base64_security_code = "pGf+lh2QOloKXU/mVh5k4ErP76mkcG6Dt1TZMZTj4Ks=";
    base64_decode(base64_security_code, security_code_);

    if ( security_code_.size() != AES_KEY_LENGTH ) {
        throw std::runtime_error("Security code must be 32 bytes (256 bits) long.");
    }
}

SSKey::~SSKey() {}

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <vector>
#include <string>
#include <iostream>



// Base64 decode
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

// Calculate decode length
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