#pragma once
#include <zlib.h>
#include <string>
#include "NetworkProtocol.pb.h"

class BinaryCompressor
{
public:
    void compress_string(const std::string& input, std::string& output);
    void decompress_string(const char* input, std::size_t input_length, std::string& output);
    Wrapper decompress_message_to_wrapper(const std::string& compressedMessage);
};