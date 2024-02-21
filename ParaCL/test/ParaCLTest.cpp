#include <vector>
#include <fstream>
#include <cstdlib>
#include <exception>
#include <algorithm>
#include <filesystem>
#include <gtest/gtest.h>

inline void readData(std::vector<int>& data, const std::string& filepath) {
    std::ifstream is{filepath, std::ios::in};
    is.seekg(0, std::ios::beg);
    int temp = 0;
    while (is >> temp) {
        data.emplace_back(temp);
    }
}

TEST(ParaclTest, RunEnd2EndTests) {
    namespace fs = std::filesystem;
    std::string inputPath = "../tests/";
    for (auto&& p: fs::directory_iterator(inputPath)) {
        auto testPath = fs::path(p.path());
        std::string testStem = testPath.stem();
        std::string testFilepath = std::string(testPath);
        if (testPath.extension().c_str() != std::string(".cl")) {
            continue;
        }
        auto projDir = testPath.parent_path().parent_path();
        std::string testIn = projDir / std::string("tests") / (testStem + ".in");
        std::string testAns = projDir / std::string("tests") / (testStem + ".ans");
        std::string testOut = projDir / std::string("res") / (testStem + ".out");
        std::string testCpp =  projDir / std::string("res") / (testStem + ".cpp");
        std::string testCppOut =  testCpp + ".out";
        std::string proj = projDir / std::string("build/paracl");
        std::system((proj + " --cpp " + testCpp + " " + testFilepath + " < " + testIn + " > " + testOut).c_str());
        std::vector<int> outData;
        readData(outData, testOut);
        std::vector<int> ansData;
        readData(ansData, testAns);
        auto equal = std::equal(outData.cbegin(), outData.cend(), ansData.cbegin(), ansData.cend());
        EXPECT_TRUE(equal);

        std::string testBin =  projDir / std::string("res") / (testStem);
        std::system(("g++ -std=c++17 -o " + testBin + " " + testCpp).c_str());
        std::system(("./" + testBin + " < " + testIn + " > " + testCppOut).c_str());
        std::vector<int> cppData;
        readData(cppData, testCppOut);
        auto equalCpp = std::equal(cppData.cbegin(), cppData.cend(), ansData.cbegin(), ansData.cend());
        EXPECT_TRUE(equalCpp);
    }
}

int main()
{
    try {
        ::testing::InitGoogleTest();
        return RUN_ALL_TESTS();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}