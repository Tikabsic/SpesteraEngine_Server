#include <iostream>
#include <iomanip>
#include <vector>
#include <stdexcept>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/rand.h>
#include <openssl/err.h>

const int AES_KEY_LENGTH = 32;
const int AES_IV_LENGTH = 16;
const int AES_BLOCK_SIZE = 16;

class SSKey {
public:
    SSKey();
    ~SSKey();
    std::string decrypt_ssk(const std::string& base64_encoded_token);
    std::string encrypt_ssk(const std::string& plaintext);

private:
    std::vector<unsigned char> security_code_;
    void base64_decode(const std::string& in, std::vector<unsigned char>& out);
    std::string base64_encode(const unsigned char* data, size_t length);
    int calcDecodeLength(const char* b64input);
    void handle_errors();
};