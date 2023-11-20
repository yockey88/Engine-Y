#ifndef ENGINEY_RNG_HPP
#define ENGINEY_RNG_HPP

#include <random>
#include <limits>

namespace EngineY {

namespace RNG {

    class RngEngineU32 {
        uint32_t lower_bound , upper_bound;
        std::random_device dev;
        std::mt19937 rng{ dev() };
        std::uniform_int_distribution<uint32_t> dist{ lower_bound , upper_bound };
        
        public:
            RngEngineU32(
                uint32_t lower_bound = (std::numeric_limits<uint32_t>::min)() , 
                uint32_t upper_bound = (std::numeric_limits<uint32_t>::max)()
            ) : lower_bound(lower_bound) , upper_bound(upper_bound) {}
            ~RngEngineU32() {}

            uint32_t operator()() { return dist(rng); }
    };

    class RngEngineU64 {
        uint64_t lower_bound , upper_bound;
        std::random_device dev;
        std::mt19937 rng{ dev() };
        std::uniform_int_distribution<uint64_t> dist{ lower_bound , upper_bound };

        public:
            RngEngineU64(
                uint64_t lower_bound = (std::numeric_limits<uint64_t>::min)() , 
                uint64_t upper_bound = (std::numeric_limits<uint64_t>::max)()
            ) : lower_bound(lower_bound) , upper_bound(upper_bound) {}
            ~RngEngineU64() {}

            uint64_t operator()() { return dist(rng); }
    };

    class RngEngineF32 {
        float lower_bound , upper_bound;
        std::random_device dev;
        std::mt19937 rng{ dev() };
        std::uniform_real_distribution<float> dist{ lower_bound , upper_bound };

        public:
            RngEngineF32(
                float lower_bound = (std::numeric_limits<float>::min)() , 
                float upper_bound = (std::numeric_limits<float>::max)()
            ) : lower_bound(lower_bound) , upper_bound(upper_bound) {}
            ~RngEngineF32() {}

            float operator()() { return dist(rng); }
    };

    class RngEngineF64 {
        double lower_bound , upper_bound;
        std::random_device dev;
        std::mt19937 rng{ dev() };
        std::uniform_real_distribution<double> dist{ lower_bound , upper_bound };

        public:
            RngEngineF64(
                double lower_bound = (std::numeric_limits<double>::min)() , 
                double upper_bound = (std::numeric_limits<double>::max)()
            ) : lower_bound(lower_bound) , upper_bound(upper_bound) {}
            ~RngEngineF64() {}

            double operator()() { return dist(rng); }
    };

}

}

#endif