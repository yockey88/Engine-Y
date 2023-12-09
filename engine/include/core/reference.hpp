#ifndef ENGINEY_REFERENCE_HPP
#define ENGINEY_REFERENCE_HPP

#include <atomic>
#include <unordered_set>


///> Reference class is taken from Hazel Engine (sorry Cherno im learning :D I took a lot of inspo from Hazel) 

namespace EngineY {

    class RefCounted {
        mutable std::atomic<uint32_t> ref_count = 0;

        public:
            virtual ~RefCounted() {}

            void IncRefCount() { ref_count++; }
            void DecRefCount() { ref_count--; }

            uint32_t GetRefCount() const { return ref_count; }
    };

namespace RefUtils {

    void RegisterReference(void* ref); 
    void RemoveReference(void* ref); 
    bool IsReferenceValid(void* ref);

}

    template <typename T>
    class Ref {
        mutable T* instance = nullptr;

        template <typename U>
        friend class Ref;

        void IncRef() const {
            if (instance != nullptr) {
                instance->IncRefCount();
                RefUtils::RegisterReference(instance); 
            }
        }

        void DecRef() const {
            if (instance != nullptr) {
                instance->DecRefCount();

                if (instance->GetRefCount() == 0) {
                    RefUtils::RemoveReference(instance);
                    delete instance;
                    instance = nullptr;
                }
            }
        }

        public:
            Ref() : instance(nullptr) {}
            Ref(std::nullptr_t) : instance(nullptr) {}

            Ref(T* instance) : instance(instance) {
                static_assert(
                    std::is_base_of<RefCounted , T>::value ,
                    "T must be derived from RefCounted"
                );

                IncRef();
            }

            template<typename U>
            Ref(Ref<U>&& other) {
                instance = (T*)other.instance;
                other.instance = nullptr;
            }

            template<typename U>
            Ref(Ref<U>& other) {
                instance = (T*)other.instance;
                IncRef();
            }

            template <typename U>
            Ref(const Ref<U>& other) {
                instance = (T*)other.instance;
                IncRef();
            }

            // writes other reference here without incrementing
            static Ref<T> WriteTo(const Ref<T>& other) {
                Ref<T> ref;
                ref.instance = other.instance;
                return ref;
            }

            static Ref<T> Clone(const Ref<T>& other) {
                Ref<T> ref;
                ref.instance = other.instance;
                ref.IncRef();
                return ref;
            }

            ~Ref() {
                DecRef();
            }

            Ref(const Ref<T>& other) {
                instance = other.instance;
                IncRef();
            }

            Ref& operator=(std::nullptr_t) {
                DecRef();
                instance = nullptr;
                return *this;
            }

            Ref& operator=(const Ref<T>& other) {
                if (this == &other)
                    return *this;

                other.IncRef();
                DecRef();
            }

            template<typename U>
            Ref& operator=(Ref<U>&& other) {
                DecRef();

                instance = (T*)other.instance;
                other.instance = nullptr;
                return *this;
            }

            template <typename U>
            Ref& operator=(const Ref<U>& other) {
                other.IncRef();
                instance = (T*)other.instance;
                return *this;
            }

            operator bool() { return instance != nullptr; }
            operator bool() const { return instance != nullptr; }
            
            T* operator->() { return instance; }
            const T* operator->() const { return instance; }

            T* Raw() { return instance; }
            const T* Raw() const { return instance; }

            void Reset(T* instance = nullptr) {
                DecRef();
                this->instance = instance;
            }

            template <typename U>
            Ref<U> As() const {
                return Ref<U>(*this);
            }

            template <typename... Args>
            static Ref<T> Create(Args&&... args) {
                return Ref<T>(new T(std::forward<Args>(args)...));
            }

            bool operator==(const Ref<T>& other) const {
                return instance == other.instance;
            }

            bool operator !=(const Ref<T>& other) const {
                return instance != other.instance;
            }

            bool EqualsObj(const Ref<T>& other) const {
                return instance == other.instance;
            }
    };

    // template <typename T>
    // class WeakRef {};

} // namespace EngineY

#endif // !ENGINEY_REFERENCE_HPP
