#ifndef ENGINEY_TYPE_STRUCTS_HPP
#define ENGINEY_TYPE_STRUCTS_HPP

#include <string>
#include <vector>

#include "reflection/type_description.hpp" 

namespace EngineY {

namespace reflection {

    struct Member {
        std::string name;
        size_t size;

        std::string type_name;

        enum MemberType {
            FUNCTION , 
            DATA 
        } member_type;

        bool operator==(const Member& other) const {
            return (
                name == other.name &&
                size == other.size &&
                type_name == other.type_name &&
                member_type == other.member_type
            );
        }
    };

    struct ClassInfo {
        std::string name;
        size_t size;

        std::vector<Member> members;
 
        template <class T>
        static ClassInfo Of() {
            static_assert(reflection::Described<T>::value , "Type has not been reflected!!");

            ClassInfo info;
            using Desc = reflection::Description<T>;

            info.name = Desc::class_name;
            info.size = sizeof(T);

            for (const auto& memb_name : Desc::MemberNames) {
                const bool is_func = *Desc::IsFunctionByName(memb_name);
                const auto type_name = *Desc::GetTypeNameByName(memb_name);

                Member memb;
                memb.name = memb_name;
                memb.size = (*Desc::GetMemberSizeByName(memb_name) * !is_func);
                memb.type_name = std::string_view(type_name.data() , type_name.size());
                memb.member_type = is_func ? Member::MemberType::FUNCTION : Member::MemberType::DATA;

                info.members.push_back(memb);
            }

            return info;
        }

        bool operator==(const ClassInfo& other) const {
            return (
                name == other.name &&
                size == other.size &&
                members == other.members
            );
        }
    };

} // namespace reflection


} // namespace EngineY

#endif // !ENGINEY_TYPE_STRUCTS_HPP
