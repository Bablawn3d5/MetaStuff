/* -----------------------------------------------------------------------------------------------

meta::registerMembers<T> is used for class registration and it has the following form when specialized:

namespace meta
{

template <>
auto registerMembers<YourClass>()
{
    return members(
        member(...),
        ...
    );
}

}

! Some important details:
1) This specialization should be defined in header, because compiler needs to deduce the return type.
2) This function is called by MetaHolder during it's static member initialization, so the tuple is created
   only once and then registerMembers function is never called again.
3) registerMembers could easily be a free function, but befriending such function is hard if you want to
   be able to get pointers to private members... Writing "friend class Meta" in your preferred class
   is just much easier. Though this might be somehow fixed in the future.
4) If the class is not registered then doForAllMembers(<your function>) will do nothing,
   because the function will return empty tuple.
5) MemberPtr.h is included in this file just so that user can #include "Meta.h" and get MemberPtr.h
   included too, which is always needed for registration.

-------------------------------------------------------------------------------------------------*/

#pragma once

#ifdef _MSC_VER
#pragma warning (disable : 4396) // silly VS warning about inline friend stuff...
#endif

#include <type_traits>
#include <tuple>
#include <utility>
#include <string>

// type_list is array of types
template <typename... Args>
struct type_list
{
    template <std::size_t N>
    using type = std::tuple_element_t<N, std::tuple<Args...>>;
    using indices = std::index_sequence_for<Args...>;
    static const size_t size = sizeof...(Args);
};

namespace meta
{

template <typename... Args>
auto members(Args&&... args);

// function used for registration of classes by user
template <typename Class>
inline auto registerMembers();

// function used for registration of class name by user
template <typename Class>
constexpr auto registerName();

// returns set name for class
template <typename Class>
constexpr auto getName();

// returns std::tuple of Members
template <typename Class>
const auto& getMembers();

// Check if class has registerMembers<T> specialization (has been registered)
template <typename Class>
constexpr bool isRegistered();

// Check if Class has non-default ctor registered
template <typename Class>
constexpr bool ctorRegistered();

template <typename T>
struct constructor_args {
    using types = type_list<>;
};

template <typename T>
using constructor_arguments = typename constructor_args<T>::types;

// Check if user registered non default constructor
template <typename Class>
constexpr bool ctorRegistered();

// Check if class T has member
template <typename Class>
bool hasMember(const std::string& name);

template <typename Class, typename F>
void doForAllMembers(F&& f);

// Do F for member named 'name' with type T. It's important to pass correct type of the member
template <typename Class, typename T, typename F>
void doForMember(const std::string& name, F&& f);

// Get value of the member named 'name'
template <typename T, typename Class>
T getMemberValue(Class& obj, const std::string& name);

// Set value of the member named 'name'
template <typename T, typename Class, typename V,
    typename = std::enable_if_t<std::is_constructible<T, V>::value>>
void setMemberValue(Class& obj, const std::string& name, V&& value);

// Get value of the enum member named 'name' as string
template <typename T, typename Class,
    typename = std::enable_if_t<std::is_enum<T>::value>>
std::string getEnumMemberValueString(Class& obj, const std::string& name);

// Sets value of the enum member named 'name' using a string
template <typename T, typename Class,
    typename = std::enable_if_t<std::is_enum<T>::value>>
void setEnumMemberValueString(Class& obj, const std::string& name, const std::string& value);


}

#include "Meta.inl"
