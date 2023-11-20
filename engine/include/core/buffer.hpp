#ifndef ENGINEY_BUFFER_HPP
#define ENGINEY_BUFFER_HPP

#include <cstdint>

#include "core/defines.hpp"
#include "core/log.hpp"

namespace EngineY {

    struct Buffer {
        void* data = nullptr;
        uint64_t size = 0;

        Buffer() {}
        Buffer(void* data , uint64_t size)
            : data(data) , size(size) {}

        static Buffer Copy(const Buffer& other) {
            Buffer b;
            b.Allocate(other.size);
            memcpy(b.data , other.data , other.size);
            return b;
        }

        static Buffer Copy(const void* data , uint64_t size) {
            Buffer b;
            b.Allocate(size);
            memcpy(b.data , data , size);
            return b;
        }

        void Allocate(uint64_t size) {
            free_tracker(data);
            data = nullptr;

            if (size == 0)
                return;

            data = tracked_new(uint8_t , size);
            this->size = size;
        }

        void Release() {
            free_tracker(data);
            data = nullptr;
            size = 0;
        }

        void ZeroMem() {
            if (data != nullptr)
                memset(data , 0 , size);
        }

        template <typename T>
        T& Read(uint64_t offset) {
            return *(T*)((uint8_t*)data + offset);
        }

        template <typename T>
        const T& Read(uint64_t offset) const {
            return *(T*)((uint8_t*)data + offset);
        }

        uint8_t* ReadBytes(uint64_t size , uint64_t offset) {
            ENGINE_ASSERT(offset + size <= this->size , "Buffer::ReadBytes |> Out of bounds");
            uint8_t* ptr = tracked_new(uint8_t , size);
            memcpy(ptr , (uint8_t*)data + offset , size);
            return (uint8_t*)data + offset;
        }

        void Write(const void* data , uint64_t size , uint64_t offset = 0) {
            ENGINE_ASSERT(offset + size <= this->size , "Buffer::Write |> Out of bounds");
            memcpy((uint8_t*)this->data + offset , data , size);
        }
        

        operator bool() const {
            return data != nullptr;
        }

        uint8_t& operator[](uint64_t offset) {
            return *(uint8_t*)((uint8_t*)data + offset);
        }

        uint8_t operator[](uint64_t offset) const {
            return *(uint8_t*)((uint8_t*)data + offset);
        }

        template <typename T>
        T* As() {
            return (T*)data;
        }

        inline uint64_t Size() const {
            return size;
        }
    };

    struct SafeBuffer : public Buffer {
        ~SafeBuffer() {
            Release();
        }

        static SafeBuffer Copy(const SafeBuffer& other) {
            SafeBuffer b;
            b.Allocate(other.Size());
            memcpy(b.data , other.data , other.Size());
            return b;
        }
    };

} // namespace EngineY


#endif // !ENGINEY_BUFFER_HPP
