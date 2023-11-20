#ifndef ENGINEY_ALLOCATOR_HPP
#define ENGINEY_ALLOCATOR_HPP

#include <cstddef>

namespace EngineY {

    class Allocator {
        public:
            virtual ~Allocator() {}

            virtual void* New(std::size_t size) = 0;
            virtual void Delete(void* ptr) = 0;
    };

} // namespace EngineY

#endif // !ENGINEY_ALLOCATOR_HPP
