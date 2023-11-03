#include <filesystem>
#include <gtest/gtest.h>
#include "../include/Matrix.hpp"

using namespace matrix;

TEST(UnitTestStorage, test0) {
	Storage<int> s1{10, 2};

	Storage<int> s2{s1};
	EXPECT_TRUE(s1 == s2);

	Storage<int> s3{10, 1};
	s3 = s1;
	EXPECT_TRUE(s1 == s3);

	Storage<int> s4{10, 2};
	Storage<int> s5{std::move(s4)};
	EXPECT_TRUE(s1 == s5);

	Storage<int> s6{10, 1};
	Storage<int> s7{s1};
	s6 = std::move(s7);
	EXPECT_TRUE(s1 == s6);

	Storage<int> s8{10, 7};
	s8.clear();
	EXPECT_TRUE(s8.empty());
	EXPECT_TRUE(s8.begin() == s8.end());

	Storage<int> s9{4, {1, 2, 3}};
	Storage<int> s10{4};
	s10[0] = 1;
	s10[1] = 2;
	s10[2] = 3;
	EXPECT_TRUE(s9 == s10);

	Storage<int> s11{4};
	s6.resize(6);
	EXPECT_TRUE(s6.size() == 6);
}

TEST(UnitTestMatrix, constructors) {
	Matrix<int> m1{{1, 2, 3}, {4, 5, 6}};

	Matrix<int> m2{m1};
	EXPECT_TRUE(m1 == m2);

	Matrix<int> m3{{0, 1, 0}, {1, 0, 1}};
	m3 = m1;
	EXPECT_TRUE(m1 == m3);

	Matrix<int> m4{{1, 2, 3}, {4, 5, 6}};
	Matrix<int> m5{std::move(m4)};
	EXPECT_TRUE(m1 == m5);

	Matrix<int> m6{{0, 1, 0}, {1, 0, 1}};
	Matrix<int> m7{m1};
	m6 = std::move(m7);
	EXPECT_TRUE(m1 == m6);

	Matrix<int> m8 = Matrix<int>::eye(3, 2);
	Matrix<int> m9{{2, 0, 0}, {0, 2, 0}, {0, 0, 2}};
	EXPECT_TRUE(m8 == m9);

	Matrix<int> m10 = Matrix<int>::diag(3, {1, 2, 3});
	Matrix<int> m11{{1, 0, 0}, {0, 2, 0}, {0, 0, 3}};
	EXPECT_TRUE(m10 == m11);
}

TEST(UnitTestMatrix, functionality) {
	Matrix<int> m1{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
	EXPECT_TRUE(m1.square());

	Matrix<int> m2{{1, 2, 3}, {4, 5, 6}};
	EXPECT_TRUE(m2.rows() == 2);
	EXPECT_TRUE(m2.cols() == 3);
	m2.clear();
	EXPECT_TRUE(m2.empty());

	Matrix<int> m3{{1, 2, 3}, {4, 5, 6}};
	Matrix<int> m4{{1, 4}, {2, 5}, {3, 6}};
	Matrix<int> m5{m3};
	m3.transpose();
	EXPECT_TRUE(m3 == m4);
	m3.transpose();
	EXPECT_TRUE(m3 == m5);

	Matrix<int> m6{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
	Matrix<int> m7{{1, 4, 7}, {2, 5, 8}, {3, 6, 9}};
	m6.transpose();
	EXPECT_TRUE(m6 == m7);

	Matrix<int> m8{{3, 2, 1}, {2, 4, 6}};
	Matrix<int> m9{{5, 7, 9}, {1, 4, 7}};
	Matrix<int> m10{{8, 9, 10}, {3, 8, 13}};
	EXPECT_TRUE(m8 + m9 == m10);
	EXPECT_TRUE(m10 - m8 == m9);
	EXPECT_TRUE(m10 - m9 == m8);

	Matrix<int> m11{{2, 4, 6}, {8, 10, 12}};
	Matrix<int> m12{{1, 2, 3}, {4, 5, 6}};
	EXPECT_TRUE(m11 / 2 == m12);
	EXPECT_TRUE(m12 * 2 == m11);
	EXPECT_TRUE(2 * m12 == m11);

	Matrix<int> m13{{4, 5, 6}, {14, 15, 16}, {24, 25, 26}, {34, 35, 36}};
	Matrix<int> m14{{1, 2}, {11, 12}, {21, 22}};
	Matrix<int> m15{{185, 200}, {515, 560}, {845, 920}, {1175, 1280}};
	EXPECT_TRUE(m13 * m14 == m15);
	EXPECT_TRUE(m15 * Matrix<int>::eye(2, 1) == m15);
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
		if (filePath.find("int") != std::string::npos) {
			Matrix<long long int> mtx{mtxSize};
			in >> mtx;
			long long int ans = 0;
			out >> ans;
			long long int det = mtx.det();
			if (!::matrix::equals(ans, det)) {
				std::cout << "My det " << det << "\n";
				std::cout << "Correct det " << ans << "\n";
				std::cout << mtx;
			}
			EXPECT_TRUE(::matrix::equals(ans, det));
		} else {
			Matrix<double> mtx{mtxSize};
			in >> mtx;
			double ans = 0;
			out >> ans;
			double det = mtx.det();
			if (!::matrix::equals(ans, det)) {
				std::cout << "My det " << det << "\n";
				std::cout << "Correct det " << ans << "\n";
				std::cout << mtx;
			}
			EXPECT_TRUE(::matrix::equals(ans, det));
		}
    }

}

int main()
{
	::testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}
