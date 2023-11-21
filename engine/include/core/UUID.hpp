#ifndef ENGINEY_UUID_HPP
#define ENGINEY_UUID_HPP

#include "hash.hpp"
#include "RNG.hpp"

namespace EngineY {

    static RNG::RngEngineU32 k32Rng;
    static RNG::RngEngineU64 k64Rng;

    struct UUID32 {
        uint32_t uuid = 0;
        uint32_t operator()() { return uuid; }
        inline void operator=(const UUID32& other) { uuid = other.uuid; }
        inline bool operator==(const UUID32& other) const { return uuid == other.uuid; }
        inline bool operator!=(const UUID32& other) const { return uuid != other.uuid; }
        inline bool operator<(const UUID32& other) const { return uuid < other.uuid; }
        inline bool operator>(const UUID32& other) const { return uuid > other.uuid; }
        inline bool operator<=(const UUID32& other) const { return uuid <= other.uuid; }
        inline bool operator>=(const UUID32& other) const { return uuid >= other.uuid; }
        inline void operator=(const uint32_t& other) { uuid = other; }
        inline bool operator==(const uint32_t& other) const { return uuid == other; }
        inline bool operator!=(const uint32_t& other) const { return uuid != other; }
        inline bool operator<(const uint32_t& other) const { return uuid < other; }
        inline bool operator>(const uint32_t& other) const { return uuid > other; }
        inline bool operator<=(const uint32_t& other) const { return uuid <= other; }
        inline bool operator>=(const uint32_t& other) const { return uuid >= other; }
        constexpr UUID32() : uuid(0) {}
        constexpr UUID32(uint32_t uuid) : uuid(uuid) {}
    };

    /// \todo Register UUID with engine
    inline UUID32 GetNewUUID32() {
        return { k32Rng() };
    }
    
    struct UUID32Hash {
        std::size_t operator()(const UUID32& uuid) const {
            return Hash::CRC32(&uuid, sizeof(UUID32));
        }
    };

    struct UUID {
        uint64_t uuid = 0;
        uint64_t operator()() { return uuid; }
        inline void operator=(const UUID& other) { uuid = other.uuid; }
        inline bool operator==(const UUID& other) const { return uuid == other.uuid; }
        inline bool operator!=(const UUID& other) const { return uuid != other.uuid; }
        inline bool operator<(const UUID& other) const { return uuid < other.uuid; }
        inline bool operator>(const UUID& other) const { return uuid > other.uuid; }
        inline bool operator<=(const UUID& other) const { return uuid <= other.uuid; }
        inline bool operator>=(const UUID& other) const { return uuid >= other.uuid; }
        inline void operator=(const uint64_t& other) { uuid = other; }
        inline bool operator==(const uint64_t& other) const { return uuid == other; }
        inline bool operator!=(const uint64_t& other) const { return uuid != other; }
        inline bool operator<(const uint64_t& other) const { return uuid < other; }
        inline bool operator>(const uint64_t& other) const { return uuid > other; }
        inline bool operator<=(const uint64_t& other) const { return uuid <= other; }
        inline bool operator>=(const uint64_t& other) const { return uuid >= other; }
        constexpr UUID() : uuid(0) {}
        constexpr UUID(uint64_t uuid) : uuid(uuid) {}
        constexpr UUID(UUID32 uuid) : uuid(uuid.uuid) {}
    };

    /// \todo Register UUID with engine
    inline UUID GetNewUUID() { 
        RNG::RngEngineU64 rng;
        return { k64Rng() };
    }

    struct UUIDHash {
        std::size_t operator()(const UUID& uuid) const {
            return Hash::CRC32(&uuid, sizeof(UUID));
        }
    };

}

template<typename stream>
inline stream& operator<<(stream& strm ,  EngineY::UUID32& uuid) {
    return strm << uuid.uuid;
}

template<typename stream>
inline stream& operator<<(stream& strm ,  EngineY::UUID& uuid) {
    return strm << uuid.uuid;
}

template<>
struct std::hash< EngineY::UUID32> {
    std::size_t operator()(const  EngineY::UUID32& uuid) const {
        return  EngineY::Hash::CRC32(&uuid, sizeof( EngineY::UUID32));
    }
};

template<>
struct std::less< EngineY::UUID32> {
    bool operator()(const  EngineY::UUID32& lhs , const  EngineY::UUID32& rhs) const {
        return lhs.uuid < rhs.uuid;
    }
};

template<>
struct std::hash< EngineY::UUID> {
    std::size_t operator()(const  EngineY::UUID& uuid) const {
        return  EngineY::Hash::CRC32(&uuid, sizeof( EngineY::UUID));
    }
};

template<>
struct std::less< EngineY::UUID> {
    bool operator()(const  EngineY::UUID& lhs , const  EngineY::UUID& rhs) const {
        return lhs.uuid < rhs.uuid;
    }
};

#endif
