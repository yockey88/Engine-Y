#ifndef YE_TYPES_HPP
#define YE_TYPES_HPP

#include <type_traits>
#include <string>

#include "hash.hpp"

namespace YE {

    template<typename T , bool NS>
    class TypeDataBase {
        protected:
            std::string_view DemangleName(std::string_view type_name) const {
                size_t space = type_name.find(' ');
                if (space != std::string_view::npos) {
                    type_name.remove_prefix(space + 1);
                }

                if constexpr (NS) {
                    size_t namespace_name = type_name.find("::");
                    if (namespace_name != std::string_view::npos) {
                        type_name.remove_prefix(namespace_name + 2);
                    }
                }

                return type_name;
            }
    };

    template<typename T , bool NS = false>
    class TypeData : public TypeDataBase<T , NS> {

        std::string_view name; 

        public:
            TypeData()
                : name(TypeDataBase<T , NS>::DemangleName(typeid(T).name())) {}

            std::string_view Name() { return name; }
            const std::string_view& Name() const { return name; }
            uint32_t Hash() const { return Hash::FNV32(name); }
    };

}

#endif // !YE_TYPES_HPP