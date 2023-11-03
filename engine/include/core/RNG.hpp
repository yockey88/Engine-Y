#ifndef YE_RNG_HPP
#define YE_RNG_HPP

#include <random>
#include <limits>

namespace YE {

namespace RNG {

    class RngEngineU32 {
        uint32_t min , max;
        std::random_device dev;
        std::mt19937 rng{ dev() };
        std::uniform_int_distribution<uint32_t> dist{ min , max };
        
        public:
            RngEngineU32(
                uint32_t min = std::numeric_limits<uint32_t>::min() , 
                uint32_t max = std::numeric_limits<uint32_t>::max()
            ) : min(min) , max(max) {}
            ~RngEngineU32() {}

            uint32_t operator()() { return dist(rng); }
    };

    class RngEngineU64 {
        uint64_t min , max;
        std::random_device dev;
        std::mt19937 rng{ dev() };
        std::uniform_int_distribution<uint64_t> dist{ min , max };

        public:
            RngEngineU64(
                uint64_t min = std::numeric_limits<uint64_t>::min() , 
                uint64_t max = std::numeric_limits<uint64_t>::max()
            ) : min(min) , max(max) {}
            ~RngEngineU64() {}

            uint64_t operator()() { return dist(rng); }
    };

    class RngEngineF32 {
        float min , max;
        std::random_device dev;
        std::mt19937 rng{ dev() };
        std::uniform_real_distribution<float> dist{ min , max };

        public:
            RngEngineF32(
                float min = std::numeric_limits<float>::min() , 
                float max = std::numeric_limits<float>::max()
            ) : min(min) , max(max) {}
            ~RngEngineF32() {}

            float operator()() { return dist(rng); }
    };

    class RngEngineF64 {
        double min , max;
        std::random_device dev;
        std::mt19937 rng{ dev() };
        std::uniform_real_distribution<double> dist{ min , max };

        public:
            RngEngineF64(
                double min = std::numeric_limits<double>::min() , 
                double max = std::numeric_limits<double>::max()
            ) : min(min) , max(max) {}
            ~RngEngineF64() {}

            double operator()() { return dist(rng); }
    };

}

}

#endif