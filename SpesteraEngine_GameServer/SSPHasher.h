#pragma once

#include <string>
#include <vector>

class SSPHasher {
public:
    SSPHasher(int salt_size = 16, int key_size = 32, int iterations = 10000);

    bool VerifyPassword(const std::string& stored_hash, const std::string& input_password);

private:
    int salt_size_;
    int key_size_;
    int iterations_;

    static const char Delimiter = ';';

    std::vector<unsigned char> base64_decode(const std::string& input);
    std::string base64_encode(const unsigned char* data, size_t length);
};