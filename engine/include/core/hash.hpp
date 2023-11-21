#ifndef ENGINEY_HASH_HPP
#define ENGINEY_HASH_HPP

#include <array>
#include <string_view>

namespace EngineY {

namespace Hash {

    static constexpr uint32_t kFnvOffsetBasisU32 = 0x811C9DC5;
    static constexpr uint32_t kFnvPrimeU32 = 0x01000193;

    static constexpr uint64_t kFnvOffsetBasis = 0xBCF29CE484222325;
    static constexpr uint64_t kFnvPrime = 0x100000001B3;

    static constexpr uint64_t FNV(std::string_view str) {
        uint64_t hash = kFnvOffsetBasis;
        for (auto& c : str) {
            hash ^= c;
            hash *= kFnvPrime;
        }
        hash ^= str.length();
        hash *= kFnvPrime;

        return hash;
    }

    static constexpr uint32_t FNV32(std::string_view str) {
        uint32_t hash = kFnvOffsetBasisU32;
        for (auto& c : str) {
            hash ^= c;
            hash *= kFnvPrimeU32;
        }
        hash ^= '\0';
        hash *= kFnvPrimeU32;
        return hash;
    }

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
    static_assert(
        kCRC32Table.size() == 256 && kCRC32Table[1] == 0x77073096 &&
        kCRC32Table[255] == 0x2D02EF8D , 
        "CRC32 table is invalid"
    );

    static constexpr uint32_t CRC32(const void* data , size_t length) {
        uint32_t crc = 0xFFFFFFFF;

        const char* buffer = static_cast<const char*>(data);

        for (size_t i = 0; i < length; ++i) {
            char c = buffer[i];
            crc = (crc >> 8) ^ kCRC32Table[(crc ^ c) & 0xFF];
        }

        return ~crc;
    };

    inline static constexpr uint32_t CRC32(std::string_view str) {
        return CRC32(str.data() , str.length());
    }

}

}

#endif
