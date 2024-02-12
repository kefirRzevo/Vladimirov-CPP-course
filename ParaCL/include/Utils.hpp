#pragma once

#include <fstream>
#include <iostream>
#include <string_view>
#include <functional>

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
    std::size_t operator()(const std::string_view& key) const {
        return std::hash<std::string_view>()(key);
    }
};

struct StringEqual
{
    bool operator()(const std::string_view& lhs, const std::string_view& rhs) const {
        return lhs == rhs;
    }
};

} // namespace paracl
