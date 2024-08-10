#pragma once

#include <string>
#include <openssl/evp.h>

class SSKey
{
public:
    SSKey(const std::string& security_code);
    ~SSKey();

    std::string encrypt_ssk(const std::string& key);
    std::string decrypt_ssk(const std::string& encrypted_key);
    bool validate_ssk(const std::string& decrypted_ssk, const std::string& encrypted_ssk);

private:
    std::string security_code_;
    static const int AES_KEY_LENGTH = 32;
    static const int AES_IV_LENGTH = 16;
};