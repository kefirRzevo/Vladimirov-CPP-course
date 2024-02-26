#include <fstream>
#include <exception>
#include <algorithm>
#include <filesystem>
#include <gtest/gtest.h>

#include "Utils.hpp"
#include "CpuBitonicSort.hpp"
#include "GpuBitonicSort.hpp"

using namespace opencl;

static const char* execPath = nullptr;

TEST(CpuSortTest, TestReadFunction) {
    std::stringstream is;
    is << "12 23 123 13 9 43";
    std::vector<int> out{12, 23, 123, 13, 9, 43};
    std::vector<int> ans;
    readData(ans, is);
    auto equals = std::equal(ans.cbegin(), ans.cend(), out.cbegin(), out.cend());
    EXPECT_TRUE(equals);
}

TEST(CpuSortTest, CpuEnd2endTest) {
    namespace fs = std::filesystem;
    std::string inputPath = "../tests/";
    for (auto&& p: fs::directory_iterator(inputPath)) {
        std::string filePath = p.path().generic_string();
        if (filePath.find(".dat") == std::string::npos ||
            filePath.find(".ans") != std::string::npos) {
            continue;
        }
        std::ifstream in{filePath, std::ios::in};
        std::ifstream ans{filePath + ".ans", std::ios::in};
        std::vector<int> inData;
        std::vector<int> ansData;
        readData(ansData, ans);
        
        readDataWithSize(inData, in);
        auto oldSize = inData.size();
        expand(inData);
        auto newSize = inData.size();
        cpuBitonicSort(inData);
        for (auto i = newSize; i > oldSize; --i) {
            inData.pop_back();
        }

        auto equal = std::equal(inData.cbegin(), inData.cend(), ansData.cbegin(), ansData.cend());
        if (!equal) {
            std::ofstream err{filePath + ".err", std::ios::out};
            err << "My sort" << std::endl;
            write(inData, err);
            err << std::endl << "Correct sort" << std::endl;
            write(ansData, err);
        }
        EXPECT_TRUE(equal);
    }
}

TEST(GpuSortTest, GpuEnd2endTest) {
    namespace fs = std::filesystem;
    std::string inputPath = "../tests/";
    OpenClApp app(execPath);
    app.buildProgram<int>("../src/BitonicInt.cl");
    for (auto&& p: fs::directory_iterator(inputPath)) {
        std::string filePath = p.path().generic_string();
        if (filePath.find(".dat") == std::string::npos ||
            filePath.find(".ans") != std::string::npos) {
            continue;
        }
        std::ifstream in{filePath, std::ios::in};
        std::ifstream ans{filePath + ".ans", std::ios::in};
        std::vector<int> inData;
        std::vector<int> ansData;
        readData(ansData, ans);

        readDataWithSize(inData, in);
        auto oldSize = inData.size();
        expand(inData);
        auto newSize = inData.size();
        app.gpuBitonicSort(inData);
        for (auto i = newSize; i > oldSize; --i) {
            inData.pop_back();
        }

        auto equal = std::equal(inData.cbegin(), inData.cend(), ansData.cbegin(), ansData.cend());
        if (!equal) {
            std::ofstream err{filePath + ".err", std::ios::out};
            err << "My sort" << std::endl;
            write(inData, err);
            err << std::endl << "Correct sort" << std::endl;
            write(ansData, err);
        }
        EXPECT_TRUE(equal);
    }
}

TEST(CompareSortTest, CpuVSGpu) {
    namespace fs = std::filesystem;
    std::string inputPath = "../tests/";
    auto buildDir = fs::absolute(fs::path{execPath}.parent_path());
    //fs::create_directory(buildDir / std::string("../res"));
    std::ofstream comp{buildDir.c_str() + std::string("/../res/compare.txt")};    

    OpenClApp app(execPath);
    app.buildProgram<int>("../src/BitonicInt.cl");
    for (auto&& p: fs::directory_iterator(buildDir / inputPath)) {
        std::string filePath = p.path().generic_string();
        if (filePath.find(".dat") == std::string::npos ||
            filePath.find(".ans") != std::string::npos) {
            continue;
        }
        std::ifstream in{filePath, std::ios::in};
        std::vector<int> cpuData;
        readDataWithSize(cpuData, in);
        expand(cpuData);
        std::vector<int> gpuData = cpuData;

        auto [cpuSortCpu] = cpuBitonicSort(cpuData);
        auto [gpuSortCpu, gpuSortGpu] = app.gpuBitonicSort(gpuData);
        comp << cpuData.size() << " " << cpuSortCpu << " ";
        comp << gpuSortCpu << " " << gpuSortGpu << "\n";
    }
    comp.flush();
    EXPECT_TRUE(true);
}

int main(int, char* argv[])
{
    try {
        execPath = argv[0];
        ::testing::InitGoogleTest();
        return RUN_ALL_TESTS();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
