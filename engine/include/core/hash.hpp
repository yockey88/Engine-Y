#ifndef ENGINEY_HASH_HPP
#define ENGINEY_HASH_HPP

#include <array>
#include <string_view>

namespace EngineY {

namespace Hash {

    static const uint32_t kFnvOffsetBasisU32 = 0x811C9DC5;
    static const uint32_t kFnvPrimeU32 = 0x01000193;

    static const uint64_t kFnvOffsetBasis = 0xBCF29CE484222325;
    static const uint64_t kFnvPrime = 0x100000001B3;

    uint64_t FNV(std::string_view str);
    uint32_t FNV32(std::string_view str);

    constexpr auto GenCRC32Table() {
        constexpr int num_bytes = 256;
        constexpr int num_iterations = 8;
        constexpr uint32_t polynomial = 0xEDB88320;

        std::array<uint32_t , num_bytes> table{};

        for (uint32_t b = 0; b < num_bytes; ++b) {
            uint32_t crc = b;
            for (uint32_t i = 0; i < num_iterations; ++i) {
                int32_t mask = -(static_cast<int32_t>(crc) & 1);
                crc = (crc >> 1) ^ (polynomial & mask);
            }
            table[b] = crc;
        }

        return table;
    }

    static constexpr auto kCRC32Table = GenCRC32Table();
    static_assert(kCRC32Table.size() == 256 && kCRC32Table[1] == 0x77073096 &&
                  kCRC32Table[255] == 0x2D02EF8D , "CRC32 table is invalid");

    // void* allows is to generalize the function to any type
    uint32_t CRC32(const void* data , size_t length);

    inline uint32_t CRC32(std::string_view str) {
        return CRC32(str.data() , str.length());
    }

}

}

#endif