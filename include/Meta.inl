#include <cassert>
#include <tuple>

#include "Member.h"
#include "detail/template_helpers.h"
#include "detail/MetaHolder.h"

namespace meta
{

template <typename... Args>
auto members(Args&&... args)
{
    // just this... but may become more complex later, who knows!
    //  Still, better no to expose too much to end-user.
    return std::make_tuple(std::forward<Args>(args)...);
}

template <typename Class>
inline auto registerMembers()
{
    return std::make_tuple();
}

template <typename Class>
constexpr auto registerName()
{
    return "";
}

template <typename Class>
constexpr auto getName()
{
    return detail::MetaHolder<Class, decltype(registerMembers<Class>())>::name();
}

template <typename Class>
const auto& getMembers()
{
    return detail::MetaHolder<Class, decltype(registerMembers<Class>())>::members;
}

template <typename Class>
constexpr bool isRegistered()
{
    return !std::is_same<std::tuple<>, decltype(registerMembers<Class>())>::value;
}

// Check if Class has non-default ctor registered
template <typename Class>
constexpr bool ctorRegistered()
{
    return !std::is_same<type_list<>, constructor_arguments<Class>>::value;
}

template <typename Class>
bool hasMember(const std::string& name)
{
    bool found = false;
    doForAllMembers<Class>(
        [&found, &name](const auto& member)
        {
            if (member.getName() == name) {
                found = true;
            }
        }
    );
    return found;
}

template <typename Class, typename F>
void doForAllMembers(F&& f)
{
    //static_assert(isRegistered<Class>(), "Class is not registered");
    detail::for_tuple(std::forward<F>(f), getMembers<Class>());
}

template <typename Class, typename T, typename F>
void doForMember(const std::string& name, F&& f)
{
    doForAllMembers<Class>(
        [&](const auto& member)
        {
            if (member.getName() == name) {
                using MemberT = meta::get_member_type<decltype(member)>;
                assert((std::is_same<MemberT, T>::value) && "Member doesn't have type T");
                detail::call_if<std::is_same<MemberT, T>::value>(std::forward<F>(f), member);
            }
        }
    );
}

template <typename T, typename Class>
T getMemberValue(Class& obj, const std::string& name)
{
    T value;
    doForMember<Class, T>(name,
        [&value, &obj](const auto& member)
        {
            value = member.get(obj);
        }
    );
    return value;
}

template <typename T, typename Class, typename V,
    typename>
void setMemberValue(Class& obj, const std::string& name, V&& value)
{
    doForMember<Class, T>(name,
        [&obj, value = std::forward<V>(value)](const auto& member)
        {
            member.set(obj, value);
        }
    );
}

// Get value of the enum member named 'name' as string
template <typename T, typename Class,
    typename>
std::string getEnumMemberValueString(Class& obj, const std::string& name) {
    std::string value;
    doForMember<Class, T>(name,
        [&value, &obj](const auto& member)
        {
            value = member.toString(member.get(obj));
        }
    );
    return value;
}

// Sets value of the enum member named 'name' using a string
template <typename T, typename Class,
    typename>
void setEnumMemberValueString(Class& obj, const std::string& name, const std::string& value){
    doForMember<Class, T>(name,
        [&obj, &value](const auto& member)
        {
            member.set(obj, member.fromString(value));
        }
    );
}

} // end of namespace meta
