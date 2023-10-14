#include "core/hash.hpp"

namespace YE {

namespace Hash {
    
    uint64_t FNV(std::string_view str) {
        uint64_t hash = kFnvOffsetBasis;
        for (auto& c : str) {
            hash ^= c;
            hash *= kFnvPrime;
        }
        hash ^= str.length();
        hash *= kFnvPrime;

        return hash;
    }

    uint32_t FNV32(std::string_view str) {
        uint32_t hash = kFnvOffsetBasisU32;
        for (auto& c : str) {
            hash ^= c;
            hash *= kFnvPrimeU32;
        }
        hash ^= '\0';
        hash *= kFnvPrimeU32;
        return hash;
    }

    uint32_t CRC32(const void* data , size_t length) {
        uint32_t crc = 0xFFFFFFFF;

        const char* buffer = static_cast<const char*>(data);

        for (size_t i = 0; i < length; ++i) {
            char c = buffer[i];
            crc = (crc >> 8) ^ kCRC32Table[(crc ^ c) & 0xFF];
        }

        return ~crc;
    };

}

}