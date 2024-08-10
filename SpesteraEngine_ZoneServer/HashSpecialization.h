#pragma once
#include <functional>
#include <utility>
#include <boost/asio.hpp>

using CellKey = std::pair<int, int>;

namespace std {

    template <>
    struct hash<CellKey> {
        std::size_t operator()(const CellKey& k) const {
            return std::hash<int>()(k.first) ^ (std::hash<int>()(k.second) << 1);
        }
    };

    template <>
    struct hash<boost::asio::ip::udp::endpoint> {
        std::size_t operator()(const boost::asio::ip::udp::endpoint& endpoint) const {
            std::size_t h1 = std::hash<std::string>{}(endpoint.address().to_string());
            std::size_t h2 = std::hash<unsigned short>{}(endpoint.port());
            return h1 ^ (h2 << 1);
        }
    };
}