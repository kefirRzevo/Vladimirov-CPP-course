#pragma once

#include <bit>
#include <chrono>
#include <vector>
#include <exception>
#include <algorithm>

#include "Utils.hpp"

namespace opencl
{

template<typename T>
inline CpuSortPassedTime cpuBitonicSort(std::vector<T>& data) {
    auto size = data.size();
    if (std::popcount(size) != 1) {
        throw std::logic_error("Data size isn't power of 2");
    } else if (size == 1U) {
        return {};
    }

    using namespace std::chrono;
    auto cpuStart = high_resolution_clock::now();
    for (size_t k = 2U; k <= size; k = k << 1) {
        for (size_t j = k >> 1; j > 0; j = j >> 1) {
            for (size_t i = 0U; i < size; i++) {
                size_t ij = i ^ j;
                if (ij > i) {
                    if ((i & k) == 0 && data[i] > data[ij]) {
                        std::swap(data[i], data[ij]);
                    }
                    if ((i & k) != 0 && data[i] < data[ij]) {
                        std::swap(data[i], data[ij]);
                    }
                }
            }
        }
    }
    auto cpuEnd = high_resolution_clock::now();
    auto cpuElapsed = duration_cast<microseconds>(cpuEnd - cpuStart).count();
    return {cpuElapsed};
}

} // namespace opencl
