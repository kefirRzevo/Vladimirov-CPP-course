#include <set>
#include <chrono>
#include <fstream>
#include <cstdlib>
#include <filesystem>
#include <gtest/gtest.h>
#include "../include/Utils.hpp"
#include "../include/Tree.hpp"

using namespace tree;

using NodePtr = Node<int>*;
using ConstNodePtr = const Node<int>*;

std::filesystem::path execPath = ".";

size_t countBlackNodes(ConstNodePtr node, ConstNodePtr root) {
	size_t res = 0U;
	while (node && node != root) {
		if (node->color_ == Color::Black) {
			res++;
		}
		node = node->parent_;
	}
	return res;
}

//https://en.wikipedia.org/wiki/Red–black_tree
//1. Every node is either red or black.
bool checkProperty1(RBTree<int>&) {
	return true; //Due to my solution, as it enum is inherited from bool
}

//2. All NIL nodes (figure 1) are considered black.
bool checkProperty2(RBTree<int>&) {
	return true; //Due to my solution, as it is nullptr
}

//3. A red node does not have a red child.
bool checkProperty3(RBTree<int>& t) {
	for(auto it = t.begin(); it != t.end(); ++it) {
		if (it.node_->color_ == Color::Red) {
			if (it.node_->left() && it.node_->left_->color_ == Color::Red) {
				return false;
			}
			if (it.node_->right() && it.node_->right_->color_ == Color::Red) {
				return false;
			}
		}
	}
	return true;
}

//4. Every path from a given node to any of its descendant NIL nodes goes through the same number of black nodes.
bool checkProperty4(RBTree<int>& t) {
	std::vector<NodePtr> leaves;
	for(auto it = t.begin(); it != t.end(); ++it) {
		if (it.node_->lTag_ == Tag::Thread || it.node_->rTag_ == Tag::Thread) {
			leaves.push_back(it.node_);
		}
	}
	if (leaves.empty()) {
		return true;
	}
	size_t depth = countBlackNodes(leaves[0], t.root());
	for (size_t i = 1U; i < leaves.size(); ++i) {
		if (countBlackNodes(leaves[i], t.root()) != depth) {
			return false;
		}
	}
	return true;
}

//5. Root is black
bool checkProperty5(RBTree<int>& t) {
	return t.root()->color_ == Color::Black;
}

TEST(RBTreeTest, treeTestClear) {
    RBTree<int> t1;
    for (int i = 1; i <= 10; ++i) {
        t1.insert(i);
	}
    t1.erase(1);
    t1.erase(7);
    t1.erase(4);
    t1.clear();

    EXPECT_TRUE(t1.empty());
    EXPECT_EQ(t1.begin().node_, t1.end().node_);
}

TEST(RBTreeTest, treeTestNotEqual) {
    RBTree<int> t1;
    for (int i = 1; i <= 10; ++i) {
        t1.insert(i);
	}
    t1.erase(1);
    t1.erase(7);
    t1.erase(4);

    RBTree<int> t2;
    for (int i = 1; i <= 10; ++i) {
        t2.insert(i);
	}
    t2.erase(1);
    t2.erase(4);

    RBTree<int> t3;
    for (int i = 1; i <= 10; ++i) {
        t3.insert(i);
	}
    t3.erase(1);
    t3.erase(7);
    t3.erase(5);

    EXPECT_FALSE (t1 == t2);
    EXPECT_FALSE (t1 == t3);
    EXPECT_FALSE (t2 == t3);

    EXPECT_TRUE (t1 != t2);
    EXPECT_TRUE (t1 != t3);
    EXPECT_TRUE (t2 != t3);
}

TEST(RBTreeTest, treeTestEqual) {
    RBTree<int> t1;
    for (int i = 1; i <= 10; ++i) {
        t1.insert(i);
	}
    t1.erase(1);
    t1.erase(7);
    t1.erase(4);

    RBTree<int> t2;
    for (int i = 1; i <= 10; ++i) {
        t2.insert(i);
	}
    t2.erase(7);
    t2.erase(4);
	t2.erase(1);

    EXPECT_TRUE(t1 == t2);
}

TEST(RBTreeTest, treeTestBig5) {
	RBTree<int> t1;
    for (int i = 1; i <= 10; ++i) {
        t1.insert(i);
	}
	RBTree<int> t2{t1};
	EXPECT_TRUE(t1 == t2);

	RBTree<int> t3;
    for (int i = 10; i <= 20; ++i) {
        t3.insert(i);
	}
	t3 = t1;
	EXPECT_TRUE(t1 == t3);

	RBTree<int> t4;
    for (int i = 1; i <= 10; ++i) {
        t4.insert(i);
	}
	RBTree<int> t5{std::move(t4)};
	EXPECT_TRUE(t1 == t5);

	RBTree<int> t6;
    for (int i = 10; i <= 20; ++i) {
        t6.insert(i);
	}
	RBTree<int> t7{t1};
	t6 = std::move(t7);
	EXPECT_TRUE(t1 == t6);
}

TEST(RBTreeTest, treeTestUpperBound)
{
    RBTree<int> t1;
    for (int i = 1; i <= 10; ++i) {
        t1.insert(i);
	}
    t1.erase(1);
    t1.erase(7);
    t1.erase(4);
    EXPECT_EQ(t1.upper_bound(11), t1.end());
    EXPECT_EQ(*t1.upper_bound(2), 3);
    EXPECT_EQ(*t1.upper_bound(1), 2);
    EXPECT_EQ(*t1.upper_bound(-1), 2);
}

TEST(RBTreeTest, treeTestLowerBound)
{
    RBTree<int> t1;
    for (int i = 1; i <= 10; ++i) {
        t1.insert(i);
	}
    t1.erase(1);
    t1.erase(7);
    t1.erase(4);
    EXPECT_EQ(t1.lower_bound(11), t1.end());
    EXPECT_EQ(*t1.lower_bound(2), 2);
    EXPECT_EQ(*t1.lower_bound(1), 2);
    EXPECT_EQ(*t1.lower_bound(-1), 2);
}

TEST(RBTreeTest, treeTestInsert)
{
    RBTree<int> t1;
    t1.insert(1);
    EXPECT_EQ(*(t1.begin()), 1);
}

TEST(RBTreeTest, treeTestEraseLastOne)
{
    RBTree<int> t1;
    t1.insert(1);
    t1.erase(1);

    EXPECT_TRUE(t1.empty());
    EXPECT_EQ(t1.begin(), t1.end());
}

TEST(RBTreeTest, treeTestEraseLeftmost)
{
    RBTree<int> t1;
    for (int i = 6; i <= 10; ++i) {
        t1.insert(i);
    }

    EXPECT_EQ(*t1.begin (), 6);
    t1.erase(6);
    EXPECT_EQ(*t1.begin(), 7);
}

TEST(RBTreeTest, treeTestEraseRightmost)
{
    RBTree<int> t1;
    for (int i = 6; i <= 10; ++i) {
        t1.insert(i);
    }
    auto last = std::prev(t1.end());
    EXPECT_EQ(*last, 10);

    t1.erase(*last);
    last = std::prev(t1.end());
    EXPECT_EQ (*last, 9);
}

TEST(RBTreeTest, treeTestEraseOneKey)
{
    RBTree<int> t1;
    t1.insert(1);
    t1.erase(1);

    EXPECT_TRUE(t1.empty());
}

TEST(RBTreeTest, treeTestEraseTwoKey)
{
    RBTree<int> t1;
    t1.insert(1);
    t1.insert(2);

    EXPECT_EQ(*t1.begin(), 1);
    EXPECT_EQ(*t1.rbegin(), 2);
    t1.erase (1);

    EXPECT_EQ(*t1.begin(), 2);
    EXPECT_EQ(*t1.rbegin(), 2);
}

TEST(RBTreeTest, treeTestEraseLastKey)
{
    RBTree<int> t1;
    t1.insert(1);
    t1.insert(2);

    EXPECT_EQ(*t1.begin(), 1);
    EXPECT_EQ(*t1.rbegin(), 2);
    t1.erase(2);

    EXPECT_EQ(*t1.begin(), 1);
    EXPECT_EQ(*t1.rbegin(), 1);
}

TEST(RBTreeTest, treeTestDoubleInsert)
{
    RBTree<int> t1;
    for (int i = 0; i < 10; ++i) {
        t1.insert(i);
    }

    auto oldSize = t1.size();
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(t1.size(), oldSize);
    }
}

TEST(RBTreeTest, treeTestDeleteWrong)
{
    RBTree<int> t1;
    for (int i = 0; i < 10; ++i) {
        t1.insert(i);
    }

    auto oldSize = t1.size();
    for (int i = -1; i < -10; --i) {
        EXPECT_EQ(t1.size(), oldSize);
    }
}

TEST(RBTreeTest, treeTestDeleteAll)
{
    RBTree<int> t1;
    for (int i = 0; i < 10; ++i) {
        t1.insert(i);
    }

    for (int i = 0; i < 10; ++i) {
		EXPECT_NO_THROW(t1.erase(i));
    }

    EXPECT_EQ(t1.size(), 0);
    EXPECT_TRUE(t1.empty());
}

TEST(RBTreeTest, treeTest) {
	RBTree<int> t;
	std::fstream file{"../mylog.dot", std::ios::out};
	t.insert(10);
	t.insert(20);
	t.insert(13);
	t.insert(17);
	t.insert(49);
	t.insert(42);
	t.insert(3);
	t.insert(93);
	t.insert(27);
	t.insert(16);
	t.insert(12);
	t.erase(12);
	t.dump(file);
	EXPECT_TRUE(checkProperty1(t));
	EXPECT_TRUE(checkProperty2(t));
	EXPECT_TRUE(checkProperty3(t));
	EXPECT_TRUE(checkProperty4(t));
	EXPECT_TRUE(checkProperty5(t));
}

TEST(RBTreeTest, end2endTest) {
	namespace fs = std::filesystem;
	auto inputPath = fs::path{execPath}.parent_path() / "../tests";
	if (!fs::exists(inputPath)) {
		throw std::logic_error("There is no directory tests/");
	}
	for (auto& p: fs::directory_iterator(inputPath)) {
		std::string filePath = p.path().generic_string();
		if (filePath.find(".dat") == std::string::npos ||
			filePath.find(".ans") != std::string::npos) {
			continue;
		}
		std::ifstream in{filePath, std::ios::in};
		std::ifstream out{filePath + ".ans", std::ios::in};
		auto out1 = myProcess(in);
		in.clear();
		in.seekg(0);
		auto out2 = stdProcess(in);
		auto out3 = result(out);
		EXPECT_TRUE(out1 == out3);
		EXPECT_TRUE(out2 == out3);
    }
}

TEST(RBTreeTest, speedTest) {
	std::fstream file{"../tests/compare.txt", std::ios::out};

	for (int size = 10; size < 1000; size+=10) {
		file << size << "\t";
		std::set<int> set;
		RBTree<int> rb;
		srand(size);
		for (int i = 0; i < size; ++i) {
			int key = std::rand() % size;
			set.insert(key);
			rb.insert(key);
		}
		assert(set.size() == rb.size());

		int sumDist = 0;
		auto start = std::chrono::high_resolution_clock::now();
		for (auto it1 = set.begin(); it1 != set.end(); ++it1) {
			for (auto it2 = it1; it2 != set.end(); ++it2) {
				sumDist += std::distance(it1, it2);
			}
		}
		auto end = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		file << sumDist << "\t" << static_cast<double>(elapsed.count()) * 0.001 << "\t";

		sumDist = 0;
		start = std::chrono::high_resolution_clock::now();
		for (auto it1 = rb.begin(); it1 != rb.end(); ++it1) {
			for (auto it2 = it1; it2 != rb.end(); ++it2) {
				sumDist += rb.distance(it1, it2);
			}
		}
		end = std::chrono::high_resolution_clock::now();
		elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		file << sumDist << "\t" << static_cast<double>(elapsed.count()) * 0.001 << std::endl;
	}
}

int main(int , char* argv[])
{
	try {
		execPath = argv[0];
		::testing::InitGoogleTest();
	} catch (const std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
	}
	return RUN_ALL_TESTS();
}
