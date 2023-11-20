#ifndef ENGINEY_LINEAR_ALLOCATOR_HPP
#define ENGINEY_LINEAR_ALLOCATOR_HPP

#include "allocator.hpp"

namespace EngineY {

    class LinearAllocator : public Allocator {
        public:
            virtual ~LinearAllocator() override {}

            virtual void* New(std::size_t size) override;
            virtual void Delete(void* ptr) override;
    };

} // namespace EngineY

#endif // !ENGINEY_LINEAR_ALLOCATOR_HPP
