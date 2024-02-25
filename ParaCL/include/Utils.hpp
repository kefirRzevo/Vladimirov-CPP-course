#pragma once

#include <functional>
#include <string_view>

namespace utils
{

template<typename T>
inline void read(const char* buf, T& val) {
    val = *reinterpret_cast<const T*>(buf);
}

template<typename T>
inline void write(char* buf, const T& val) {
    *reinterpret_cast<T*>(buf) = val;
}

} // namespace utils

namespace paracl
{

struct StringHash
{
    std::size_t operator()(std::string_view key) const {
        return std::hash<std::string_view>()(key);
    }
};

struct StringEqual
{
    bool operator()(std::string_view lhs, std::string_view rhs) const {
        return lhs == rhs;
    }
};

} // namespace paracl
