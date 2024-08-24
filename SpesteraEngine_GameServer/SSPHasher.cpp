#include "SSPHasher.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <stdexcept>
#include <cstring>

SSPHasher::SSPHasher(int salt_size, int key_size, int iterations)
    : salt_size_(salt_size), key_size_(key_size), iterations_(iterations) {}

bool SSPHasher::VerifyPassword(const std::string& stored_hash, const std::string& input_password) {
    size_t delimiter_pos = stored_hash.find(Delimiter);
    if ( delimiter_pos == std::string::npos ) {
        throw std::runtime_error("Invalid stored hash format");
    }

    std::string base64_salt = stored_hash.substr(0, delimiter_pos);
    std::string base64_hash = stored_hash.substr(delimiter_pos + 1);

    std::vector<unsigned char> salt = base64_decode(base64_salt);
    std::vector<unsigned char> original_hash = base64_decode(base64_hash);

    std::vector<unsigned char> derived_key(key_size_);

    if ( !PKCS5_PBKDF2_HMAC(input_password.c_str(), input_password.length(),
        salt.data(), salt.size(), iterations_, EVP_sha256(),
        key_size_, derived_key.data()) ) {
        throw std::runtime_error("Error in PKCS5_PBKDF2_HMAC");
    }

    return CRYPTO_memcmp(original_hash.data(), derived_key.data(), key_size_) == 0;
}

std::vector<unsigned char> SSPHasher::base64_decode(const std::string& input) {
    BIO* bio = BIO_new_mem_buf(input.data(), input.size());
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_push(b64, bio);

    std::vector<unsigned char> output(input.size() * 3 / 4);
    int out_len = BIO_read(bio, output.data(), output.size());
    output.resize(out_len);

    BIO_free_all(bio);
    return output;
}

std::string SSPHasher::base64_encode(const unsigned char* data, size_t length) {
    BIO* bio, * b64;
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