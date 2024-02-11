#pragma once

#include <fstream>
#include <iostream>

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
