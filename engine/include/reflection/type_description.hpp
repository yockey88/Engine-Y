#ifndef ENGINEY_TYPE_DESCRIPTION_HPP
#define ENGINEY_TYPE_DESCRIPTION_HPP

#include <type_traits>
#include <tuple>
#include <array>
#include <variant>
#include <vector>
#include <iostream>
#include <optional>

#include "core/utils.hpp"
#include "reflection/type_name.hpp"
#include "reflection/type_utils.hpp"

namespace EngineY {

namespace reflection {

    struct DummyName_t {};

    template <typename T , typename Tag = DummyName_t>
    struct Description;

    template <typename T , typename Tag = DummyName_t>
    using Described = is_specialized<Description<std::remove_cvref_t<T> , Tag>>;

    template <auto... MPs>
    struct MemberList {
        public:
            using Tuple = decltype(std::tuple(MPs...));

        private:
            template <size_t MPI>
            using MemberType = typename member_pointer::return_type<std::remove_cvref_t<decltype(std::get<MPI>(Tuple()))>>::type;

            template <typename MP>
            using MemberPtrType = typename member_pointer::return_type<std::remove_cvref_t<MP>>::type;

        public:
            using Variant = std::variant<filter_void_t<MemberPtrType<decltype(MPs)>>...>;

            static constexpr size_t Count() { return sizeof...(MPs); }

            template <typename TO , typename Fn>
            static constexpr auto Apply(Fn fn , TO& obj) {
                return fn(obj.*MPs...);
            }

            template <typename TO , typename Fn>
            static constexpr auto Apply(Fn fn , const TO& obj) {
                return fn(obj.*MPs...);
            }

            template <typename TO , typename Fn>
            static constexpr auto ApplyForEach(Fn fn , TO& obj) {
                return (ApplyIfMemberNotFunction(fn , MPs , obj) , ...);
            }

            template <typename Fn>
            static constexpr auto ApplyToStatic(Fn fn) {
                return fn(MPs...);
            }

        private:
            template <typename Fn , typename MP , typename TO>
            static constexpr auto ApplyIfMemberNotFunction(Fn fn , MP member , TO& obj) {
                if constexpr (!std::is_member_function_pointer_v<decltype(member)>) {
                    fn(obj.*member);
                }
            }

            template <typename TO , typename Fn>
            static constexpr auto ApplyToMember(size_t index , Fn&& f , TO&& obj) {
                int cntr = 0;

                auto unwrap_w_counter = [&cntr , &f , index]() {
                    auto unwrap = [&cntr , &f , index](auto& memb) {
                        if (cntr == index) {
                            f(memb);
                        }
                    };

                    ++cntr;

                    return unwrap;
                };

                (ApplyIfMemberNotFunction(unwrap_w_counter() , MPs , std::forward<TO>(obj)) , ...);
            }

            template <size_t MI , typename TO , typename Fn>
            constexpr static auto ApplyToMember(Fn&& fn , TO&& obj) {
                f(obj.*reflection::nth_element<MI>(MPs...));
            }

        public:
            template <typename Val , typename TO>
            static constexpr bool SetMemberValue(size_t index , const Val& val , TO&& obj) {
                bool val_set = false;

                ApplyToMember(
                    index , 
                    [&](auto& memb) {
                        using MemberNoCVR = std::remove_cvref_t<decltype(memb)>;
                        using ValueNoCVR = std::remove_cvref_t<decltype(val)>;

                        if constexpr (std::is_same_v<ValueNoCVR , MemberNoCVR>) {
                            memb = std::forward<decltype(val)>(val);
                            val_set = true;
                        }
                    } ,
                    std::forward<TO>(obj)
                );

                return val_set;
            }

            template <size_t MI , typename Val , typename TO>
            static constexpr bool SetMemberValue(const Val& value , TO&& obj) {
                bool val_set = false;

                ApplyToMember<MI>(
                    [&](auto& memb) {
                        using MemberNoCVR = std::remove_cvref_t<decltype(memb)>;
                        using ValueNoCVR = std::remove_cvref_t<decltype(value)>;

                        if constexpr (std::is_same_v<ValueNoCVR , MemberNoCVR>) {
                            memb = std::forward<decltype(value)>(value);
                            val_set = true;
                        }
                    } , std::forward<decltype(obj)>(obj)
                );

                return val_set;
            }

            template <size_t MI , typename TO>
            static constexpr auto GetMemberValue(const TO& obj) {
                static_assert(Count() > MI);

                auto filter = [&obj](auto memb) {
                    if constexpr (std::is_member_function_pointer_v<decltype(memb)>) {
                        return;
                    } else {
                        return obj.*memb;
                    }
                };

                return filter(reflection::nth_element<MI>(MPs...));
            }

            template <typename Val , typename TO>
            static constexpr std::optional<Val> GetMemberValueOfType(size_t index , const TO& obj) {
                std::optional<Val> val;

                if (Count() > index) {
                    ApplyToMember(
                        index ,
                        [&](const auto& memb) {
                            using Member = std::remove_cvref_t<decltype(memb)>;

                            if constexpr (std::is_same_v<Val , Member>) {
                                val = memb;
                            }
                        } ,
                        std::forward<decltype(obj)>(obj)
                    );
                }

                return val;
            }

            template <typename MI , typename VT , typename TO>
            static constexpr VT GetMemberValue(const TO& obj) {
                return VT(GetMemberValue<MI>(obj));
            }

            template <typename VT , typename TO>
            static constexpr VT GetMemberValue(size_t index , const TO& obj) {
                VT variant{};

                if (Count() > index) {
                    ApplyToMember(
                        index ,
                        [&](const auto& memb) {
                            variant = VT(memb);
                        } ,
                        std::forward<decltype(obj)>(obj)
                    );
                }

                return variant;
            }

            template <size_t MI>
            static constexpr auto IsFunction() {
                static_assert(Count() > MI);
                return std::is_member_function_pointer_v<decltype(reflection::nth_element<MI>(MPs...))>;
            }

            static constexpr std::optional<bool> IsFunction(size_t index) {
                std::optional<bool> is_func;

                if (Count() > index) {
                    uint32_t member_count = 0;
                    auto unwrap = [&is_func , index](auto memb , uint32_t cntr) {
                        if (cntr == index) {
                            is_func = std::is_member_function_pointer_v<decltype(memb)>;
                        }
                    };

                    (unwrap(MPs , member_count++) , ...);
                }

                return is_func;
            }

            template <size_t MI>
            static constexpr auto GetMemberSize() {
                return sizeof(MemberType<MI>);
            }

            static constexpr auto GetMemberSize(size_t index) {
                std::optional<size_t> size = 0;

                if (Count() > index) {
                    uint32_t member_count = 0;
                    auto unwrap = [&size , index](auto memb , uint32_t cntr) {
                        if (cntr == index) {
                            size = sizeof(filter_void_t<MemberPtrType<decltype(memb)>>);
                        }
                    };

                    (unwrap(MPs , member_count++) , ...);
                }

                return size;
            }

            template <size_t MI>
            static constexpr auto GetTypeName() {
                static_assert(Count() > MI);
                return reflection::type_name<MemberType<MI>>();
            }

            static constexpr auto GetTypeName(size_t index) {
                std::optional<std::string_view> type_name;

                if (Count() > index) {
                    int member_count = 0;

                    auto unwrap = [&type_name , index](auto memb , int counter) {
                        if (counter == index) {
                            type_name = reflection::type_name<MemberPtrType<decltype(memb)>>();
                        }
                    };

                    (unwrap(MPs , member_count++) , ...);
                }

                return type_name;
            }
    };

    template <class Desc , class TOType , class Tag , class List>
    struct DescriptionInterface;

#ifndef REFFLECTED

#define REFLECTION_TAG(klass , tag , ...) \
    template <> \
    struct EngineY::reflection::Description<klass , tag> \
            : public EngineY::reflection::MemberList<__VA_ARGS__> , \
              public EngineY::reflection::DescriptionInterface< \
                EngineY::reflection::Description<klass , tag> , \
                klass , tag , EngineY::reflection::MemberList<__VA_ARGS__> \
            > { \
        private: \
            static constexpr std::string_view member_str{ #__VA_ARGS__ }; \
            static constexpr std::string_view class_str{ #klass }; \
            static constexpr std::string_view delim{ ", " }; \
                                                         \
        public: \
            static constexpr std::string_view name_space { \
                class_str.data() , \
                class_str.find("::") == std::string_view::npos ? \
                    0 : class_str.find_last_of(':') - 1 \
            }; \
                                                         \
            static constexpr std::string_view class_name { class_str.data() + (name_space.size() ? name_space.size() + 2 : 0) }; \
            static constexpr std::array<std::string_view , NumMembers> MemberNames = \
                EngineY::utils::RemoveNamespace<NumMembers>( \
                    EngineY::utils::SplitString<NumMembers>(member_str , delim) \
                ); \
    }; 
#endif // !REFLECTED

#ifndef REFFLECTED
#define REFLECT(klass , ...) REFLECTION_TAG(klass , EngineY::reflection::DummyName_t , __VA_ARGS__)
#endif // !REFLECTED

    template <typename Desc , class TOType , class Tag , class List>
    struct DescriptionInterface {
        static_assert(std::is_same_v<Desc , Description<TOType , Tag>>);

        static constexpr size_t NumMembers = List::Count();

        static constexpr auto IndexOf(std::string_view name) {
            for (size_t i = 0; i < Desc::Count(); ++i) {
                if (Desc::MemberNames[i] == name) {
                    return i;
                }
            }

            return Desc::INVALID_INDEX;
        }

        static constexpr bool HasMember(std::string_view name) {
            for (const auto& name : Desc::MemberNames) {
                if (name == name) {
                    return true;
                }
            }

            return false;
        }

        static constexpr size_t INVALID_INDEX = size_t(-1);

        static constexpr std::optional<std::string_view> GetMemberName(size_t index) {
            return (NumMembers > index) ? 
                std::string_view(Desc::MemberNames[index]) : "";
        }

        template <size_t MI>
        static constexpr std::string_view GetMemberName() {
            static_assert(NumMembers > MI);
            return Desc::MemberNames[MI];
        }

        template <typename Val>
        static constexpr std::optional<Val> GetMemberValueByName(std::string_view name , const TOType& obj) {
            const auto index = IndexOf(name);
            return (NumMembers > index) ? 
                List::template GetMemberValueOfType<Val>(index , obj) :
                Val{};
        }

        template <typename VT>
        static constexpr VT GetMemberValueVariantByName(std::string_view name , const TOType& obj) {
            const auto index = IndexOf(name);
            return (NumMembers > index) ? 
                List::template GetMemberValue<VT>(index , obj) :
                VT{};
        }

        template <typename Val>
        static constexpr bool SetMemberValueByName(std::string_view name , const Val& value , TOType& obj) {
            const auto index = IndexOf(name);
            return (NumMembers > index) ? 
                List::template SetMemberValue<Val>(index , value , obj) :
                false;
        }

        static constexpr std::optional<bool> IsFunctionByName(std::string_view name) {
            const auto index = IndexOf(name);
            return (NumMembers > index) ? List::IsFunction(index) : std::nullopt;
        }

        static constexpr std::optional<size_t> GetMemberSizeByName(std::string_view name) {
            const auto index = IndexOf(name);
            return (NumMembers > index) ? 
                List::GetMemberSize(index) : std::nullopt;
        }

        static constexpr std::optional<std::string_view> GetTypeNameByName(std::string_view name) {
            const auto index = IndexOf(name);
            return (NumMembers > index) ? 
                List::GetTypeName(index) : std::nullopt;
        }

        static void Print(std::ostream& stream) {
            stream << "Namespace :: " << Desc::name_space << "\n";
            stream << "Class :: " << Desc::class_name << "\n";
            stream << "Number of Members :: " << NumMembers << "\n";
            stream << "Members :: \n";
            stream << " <---+--->\n";
            for (uint32_t i = 0; i < (uint32_t)NumMembers; ++i) {
                stream << "  " << *List::GetTypeName(i) << " " << *List::GetMemberNames(i) << " [" << *List::GetMemberSize(i) << " bytes]\n";

                if (*List::IsFunction(i)) {
                    stream << "    (function)\n";
                }
            }
            stream << " <---+--->\n";
        }

        static void Print(std::ostream& stream , const TOType& obj) {
            stream << "Namespace :: " << Desc::name_space << "\n"; 
            stream << "Class :: " << Desc::class_name << "\n";
            stream << "Number of Members :: " << NumMembers << "\n";
            stream << "Members :: \n";
            stream << " <---+--->\n";

            auto unwrap_outer = [&stream , &obj](auto... members) {
                uint32_t memb_count = 0;

                auto unwrap = [&stream , &obj](auto memb , uint32_t index) {
                    stream << "  " <<*List::GetTypeName(index) << " ";

                    if constexpr (
                        !std::is_member_function_pointer_v<decltype(memb)> &&
                        is_streamable_v<typename member_pointer::return_type<decltype(memb)>::type>
                    ) {
                        stream << " { " << (obj.*memb) << " } ";
                    }

                    stream << Desc::MemberNames[index] << " [" << *List::GetMemberSize(index) << " bytes]\n";

                    if (*List::IsFunction(index)) {
                        stream << "    (function)\n";
                    }

                    stream << '\n';
                };
                (unwrap(members , memb_count++) , ...);
            };

            List::ApplyToStaticType(unwrap_outer);

            stream << " <---+--->\n";
        }
    };
       
    template <typename T , typename... Ts>
    std::ostream& operator<<(std::ostream& os , const std::variant<T , Ts...>& v) {
        std::visit(
            [&os](auto&& arg) {
                os << arg;
            } , v
        );

        return os;
    }

} // namespace reflection

} // namespace EngineY

#endif // !ENGINEY_TYPE_DESCRIPTION_HPP
