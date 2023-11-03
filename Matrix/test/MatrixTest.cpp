#include <filesystem>
#include <gtest/gtest.h>
#include "../include/Matrix.hpp"

using namespace matrix;

TEST(TestStorage, test0) {
	Storage<int> s1{10, 2};

	Storage<int> s2{s1};
	EXPECT_TRUE(s1 == s2);

	Storage<int> s3{};
	s3 = s1;
	EXPECT_TRUE(s1 == s3);

	Storage<int> s4{};
	s4 = s2;
	EXPECT_TRUE(s4 == s2);

	Storage<int> s5{4, {1, 2, 3}};
	Storage<int> s6{4};
	s6[0] = 1;
	s6[1] = 2;
	s6[2] = 3;
	EXPECT_TRUE(s5 == s6);

	s6.clear();
	EXPECT_TRUE(s6.size() == 0 && s6.begin().ptr_ == nullptr);

	s6.resize(6);
	EXPECT_TRUE(s6.size() == 6);
}

TEST(TestMatrix, test1) {
	Matrix<int> m1{4, 6};
	m1[1][2] = 3;
	m1[3][4] = 7;
	Matrix<int> m2 = m1;
	m1.transpose();
	m1.transpose();
	EXPECT_TRUE(m1.equals(m2));

	Matrix<int> m3{{1, 2, 3}, {4, 5, 6}};
	Matrix<int> m4{{7}, {8}, {9}};
	m3 *= m4;
	Matrix<int> m5{{50}, {122}};
	EXPECT_TRUE(m5.equals(m3));

	Matrix<float> m6{1, 1, {1.000001}};
	Matrix<float> m7{1, 1, {1.000002}};
	EXPECT_TRUE(m6.equals(m7));

	Matrix<int> m8{{1}, {4, 5, 6}, {7, 8}};
	EXPECT_TRUE(m8.rows() == 3U && m8.cols() == 3U);
}

TEST(MatrixDetTest, end2endTest) {

	namespace fs = std::filesystem;
	std::string inputPath = "../tests/";
	for (auto& p: fs::directory_iterator(inputPath)) {
		std::string filePath = p.path().generic_string();
		if (filePath.find(".dat") == std::string::npos ||
			filePath.find(".ans") != std::string::npos) {
			continue;
		}
		std::ifstream in{filePath, std::ios::in};
		std::ifstream out{filePath + ".ans", std::ios::in};
		size_t mtxSize = 0;
		in >> mtxSize;
		Matrix<double> mtx{mtxSize};
		in >> mtx;
		double ans = 0;
		out >> ans;
		double det = mtx.det();
		if (!::matrix::equals(ans, det)) {
			std::cout << mtx;
			std::cout << ans << std::endl;
			std::cout << det << std::endl;
		}
		EXPECT_TRUE(::matrix::equals(ans, det));
    }

}

int main()
{
	::testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}
