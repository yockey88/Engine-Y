#ifndef YE_REFERENCES_HPP
#define YE_REFERENCES_HPP

#include <atomic>

#include "defines.hpp"

namespace YE {

    class RefCounted {
        mutable std::atomic<uint32_t> ref_count = 0;

        public:
            virtual ~RefCounted() {}

            void IncRefCount() { ++ref_count; }
            void DecRefCount() { --ref_count; }

            inline uint32_t RefCount() const { return ref_count.load(); }
    };

    /// \todo Finish this class later
    template<typename T>
    class Ref {
        template<typename T2>
        friend class Ref;

        mutable T* instance;

        void IncRef() const {
            if (instance != nullptr) {
                instance->IncRefCount();
                // add to memory manager
            }
        }

        void DecRef() const {
            if (instance != nullptr) {
                instance->DecRefCount();
                if (instance->RefCount() == 0) {
                    ydelete instance;
                    // remove from memory manager
                    instance = nullptr;
                }
            }
        }

        public:
            Ref() : instance(nullptr) {}
            Ref(std::nullptr_t null) : instance(nullptr) {}
            Ref(T* instance) : instance(instance) {
                static_assert(
                    std::is_base_of<RefCounted , T>::value ,
                    "T must be derived from RefCounted"
                );

                IncRef();
    };

} // namespace YE

#endif // !YE_REFERENCES_HPP
