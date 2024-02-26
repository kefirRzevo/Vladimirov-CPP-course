#pragma once
//*
#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#endif

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_ENABLE_EXCEPTIONS

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "Utils.hpp"

#ifdef __APPLE__
#include "OpenCL/opencl.hpp"
#else
#include "CL/opencl.hpp"
#endif

namespace opencl
{

class OpenClApp final
{
private:
    cl::Platform platform_;
    cl::Device device_;
    cl::Context context_;
    cl::CommandQueue queue_;

    cl::Program program_;

    const char* execFilepath_;

    static cl::Platform selectPlatform();

    static cl::Device selectDevice(const cl::Platform& platform);

    std::string fileToString(const std::string& filepath) const;

    std::string kernelPath(const std::string& kernelFilepath) const;

    //using vadd_t = cl::KernelFunctor<cl::Buffer, cl::, cl::Buffer>;
public:
    OpenClApp(const char* execFilepath)
    : platform_(selectPlatform()),
      device_(selectDevice(platform_)),
      context_(device_),
      queue_(context_, cl::QueueProperties::Profiling),
      execFilepath_(execFilepath) {
        //auto name = platform_.getInfo<CL_PLATFORM_NAME>();
        //auto profile = platform_.getInfo<CL_PLATFORM_PROFILE>();
        //std::cout << "Selected: " << name << ": " << profile << std::endl;
    }

    template<typename T>
    void buildProgram(const std::string& kernelFilepath) {
        auto kernel = kernelPath(kernelFilepath);
        program_ = cl::Program(context_, fileToString(kernel), true);
    }

    template<typename T>
    GpuSortPassedTime gpuBitonicSort(std::vector<T>& data) {
        auto size = data.size();
        if (std::popcount(size) != 1) {
            throw std::logic_error("Data size isn't power of 2");
        } else if (size == 1U) {
            return {};
        }

        size_t bufSize = size * sizeof(T);
        cl::Buffer buf(context_, CL_MEM_WRITE_ONLY, bufSize);

        using namespace std::chrono;
        auto cpuStart = high_resolution_clock::now();
        cl::copy(queue_, data.begin(), data.end(), buf);
        auto cpuEnd = high_resolution_clock::now();
        auto cpuElapsed = duration_cast<microseconds>(cpuEnd - cpuStart).count();

        cl::KernelFunctor<cl::Buffer, uint, uint> parallel(program_, "bitonic");
        cl::NDRange globalRange(size);
        cl::EnqueueArgs args(queue_, globalRange);

        cl_ulong gpuStart = 0;
        cl_ulong gpuEnd = 0;
        long long gpuElapsed = 0;

        cpuStart = high_resolution_clock::now();
        for (size_t stage = 2U; stage <= size; stage = stage << 1) {
            for (size_t step = stage >> 1; step > 0; step = step >> 1) {
                cl::Event event = parallel(args, buf, stage, step);
                event.wait();
                gpuStart = event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
                gpuEnd = event.getProfilingInfo<CL_PROFILING_COMMAND_END>();
                gpuElapsed += (gpuEnd - gpuStart) / 1000;
            }
        }
        cl::copy(queue_, buf, data.begin(), data.end());
        cpuEnd = high_resolution_clock::now();

        cpuElapsed += duration_cast<microseconds>(cpuEnd - cpuStart).count();
        cpuElapsed -= gpuElapsed;
        return {cpuElapsed, gpuElapsed};
    }
};

} // namespace opencl
//*/