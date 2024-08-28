#include "BinaryCompressor.h"
#include <stdexcept>
#include <iostream>
#include <vector>
#include "GSProtocol.pb.h"

void BinaryCompressor::compress_string(const std::string& input, std::string& output)
{
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    if (deflateInit(&zs, Z_BEST_COMPRESSION) != Z_OK) {
        std::cerr << "deflateInit failed" << std::endl;
        throw std::runtime_error("deflateInit failed while compressing.");
    }

    zs.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(input.data()));
    zs.avail_in = static_cast<uInt>(input.size());

    int ret;
    std::vector<char> outbuffer(512 * 1024);
    std::string outstring;

    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer.data());
        zs.avail_out = static_cast<uInt>(outbuffer.size());

        ret = deflate(&zs, Z_FINISH);

        if (ret != Z_STREAM_END && ret != Z_OK) {
            deflateEnd(&zs);
            throw std::runtime_error("deflate failed while compressing.");
        }

        size_t compressed_size = outbuffer.size() - zs.avail_out;
        outstring.append(outbuffer.data(), compressed_size);
    } while (ret == Z_OK);

    deflateEnd(&zs);

    output.swap(outstring);
}

void BinaryCompressor::decompress_string(const char* input, std::size_t input_length, std::string& output)
{
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    if (inflateInit(&zs) != Z_OK) {
        throw std::runtime_error("inflateInit failed while decompressing.");
    }

    zs.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(input));
    zs.avail_in = static_cast<uInt>(input_length);

    int ret;
    std::vector<char> outbuffer(512 * 1024);
    std::string outstring;

    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer.data());
        zs.avail_out = static_cast<uInt>(outbuffer.size());

        ret = inflate(&zs, Z_NO_FLUSH);

        if (ret != Z_STREAM_END && ret != Z_OK) {
            inflateEnd(&zs);
            std::cerr << "inflate failed with error code " << ret << " while decompressing. avail_in: " 
                      << zs.avail_in << ", avail_out: " << zs.avail_out << std::endl;
            throw std::runtime_error("inflate failed while decompressing.");
        }

        size_t decompressed_size = outbuffer.size() - zs.avail_out;
        outstring.append(outbuffer.data(), decompressed_size);

    } while (ret == Z_OK);

    inflateEnd(&zs);

    if (ret != Z_STREAM_END) {
        throw std::runtime_error("inflate failed to reach the end of stream while decompressing.");
    }

    output.swap(outstring);
}

GSWrapperRequest BinaryCompressor::decompress_message_to_wrapper(const std::string& compressedMessage)
{
    std::string decompressed_msg;
    decompress_string(compressedMessage.data(), compressedMessage.size(), decompressed_msg);
    GSWrapperRequest wrapper_msg;
    wrapper_msg.ParseFromString(decompressed_msg);

    return wrapper_msg;
}


