#include <filesystem>
//*
#include "GpuBitonicSort.hpp"

namespace opencl
{

cl::Platform OpenClApp::selectPlatform() {
    cl::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    for (auto&& platform : platforms) {
        cl::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
        for (auto&& device : devices) {
            if (device.getInfo<CL_DEVICE_TYPE>() == CL_DEVICE_TYPE_GPU) {
                return platform;
            }
        }
    }
    throw std::runtime_error("Couldn't find platform");
}

cl::Device OpenClApp::selectDevice(const cl::Platform& platform) {
    cl::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    return devices.front();
}

std::string OpenClApp::fileToString(const std::string& filepath) const {
    std::ifstream is{filepath};
    std::ostringstream oss;
    oss << is.rdbuf();
    std::string string = oss.str();
    return string;
}

std::string OpenClApp::kernelPath(const std::string& kernelFilepath) const {
    namespace fs = std::filesystem;
    auto buildDir = fs::absolute(fs::path{execFilepath_}.parent_path());
    return buildDir / kernelFilepath;
}

} // namespace opencl
//*/