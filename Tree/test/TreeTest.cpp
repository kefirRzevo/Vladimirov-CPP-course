#include <set>
#include <chrono>
#include <cstdlib>
#include <gtest/gtest.h>
#include "../include/Utils.hpp"
#include "../include/Tree.hpp"

using namespace tree;

using NodePtr = Node<int>*;
using ConstNodePtr = const Node<int>*;

size_t countBlackNodes(NodePtr node, ConstNodePtr root) {
	if (!node) { return 0U; }
	size_t res = 0U;
	while (node != root) {
		if (node->color_ == Color::Black) { res++; }
		node = node->parent_;
	}
	return res;
}

//https://en.wikipedia.org/wiki/Red–black_tree
//1. Every node is either red or black.
//bool checkProperty1(RBTree<int>& t) {
//	return true; //Due to my solution, as it enum is inherited from bool
//}

//2. All NIL nodes (figure 1) are considered black.
//bool checkProperty2(RBTree<int>& t) {
//	return true; //Due to my solution, as it is nullptr
//}

//3. A red node does not have a red child.
bool checkProperty3(RBTree<int>& t) {
	for(auto it = t.begin(); it != t.end(); ++it) {
		if (it.node_->color_ == Color::Red) {
			if (it.node_->left_ && it.node_->left_->color_ == Color::Red) { return false; }
			if (it.node_->right_ && it.node_->right_->color_ == Color::Red) { return false; }
		}
	}
	return true;
}

//4. Every path from a given node to any of its descendant NIL nodes goes through the same number of black nodes.
bool checkProperty4(RBTree<int>& t) {
	std::vector<NodePtr> leaves;
	for(auto it = t.begin(); it != t.end(); ++it) {
		if (!it.node_->left_ && !it.node_->right_) { leaves.push_back(it.node_); }
	}

	if (leaves.empty()) { return true; }
	size_t depth = countBlackNodes(leaves[0], t.root());
	for (size_t i = 1U; i < leaves.size(); ++i) {
		if (countBlackNodes(leaves[i], t.root()) != depth) { return false; }
	}
	return true;
}

//5. Root is black
bool checkProperty5(RBTree<int>& t) {
	return t.root()->color_ == Color::Black;
}

TEST(RBTreeTest, treeInsert) {
	RBTree<int> t;
	t.insert(1);
	t.insert(2);
	t.insert(5);
	t.insert(6);
	t.insert(4);
	t.insert(4);
	t.insert(12);
	t.insert(8);
	t.insert(9);
	//EXPECT_TRUE(checkProperty1(t));
	//EXPECT_TRUE(checkProperty2(t));
	EXPECT_TRUE(checkProperty3(t));
	EXPECT_TRUE(checkProperty4(t));
	EXPECT_TRUE(checkProperty5(t));
}

TEST(RBTreeTest, end_to_end_tests) {
	std::ifstream in1{"../test/001.dat", std::ios::in};
	auto out1 = myProcess(in1);
	std::vector<int> outCorrect1{2, 0, 3};
	EXPECT_TRUE(out1 == outCorrect1);

	std::ifstream in2{"../test/002.dat", std::ios::in};
	auto out2 = myProcess(in2);
	std::vector<int> outCorrect2{0, 0, 2, 2, 0, 1, 0, 2, 4, 3};
	EXPECT_TRUE(out2 == outCorrect2);	
}

TEST(RBvsSTDSET, speed_test) {
	std::fstream file{"../test/compare.txt", std::ios::out};

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

int main()
{
	::testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}
