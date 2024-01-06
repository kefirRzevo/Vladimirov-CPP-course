#pragma once

#include <fstream>
#include <iostream>

namespace utils
{

template<typename T>
inline std::istream& read(std::istream& is, T& val) {
    is.read(reinterpret_cast<char*>(std::addressof(val)), sizeof(val));
    return is;
}

template<typename T>
inline std::ostream& write(std::ostream& os, const T& val) {
    os.write(reinterpret_cast<const char*>(std::addressof(val)), sizeof(val));
    return os;
}

template<typename T>
inline void read(const char* buf, T& val) {
    val = *reinterpret_cast<const T*>(buf);
}

template<typename T>
void write(char* buf, const T& val) {
    *reinterpret_cast<T*>(buf) = val;
}

} // namespace utils
