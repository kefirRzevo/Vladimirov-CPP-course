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
bool checkProperty1(RBTree<int>& t) {
	assert(t.root());
	return true; //Due to my solution, as it enum is inherited from bool
}

//2. All NIL nodes (figure 1) are considered black.
bool checkProperty2(RBTree<int>& t) {
	assert(t.root());
	return true; //Due to my solution, as it is nullptr
}

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
	for (size_t i = 1; i < leaves.size(); ++i) {
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
	EXPECT_TRUE(checkProperty1(t));
	EXPECT_TRUE(checkProperty2(t));
	EXPECT_TRUE(checkProperty3(t));
	EXPECT_TRUE(checkProperty4(t));
	EXPECT_TRUE(checkProperty5(t));
}

TEST(RBTreeTest, end_to_end_tests) {
	std::ifstream in1{"../test/test1.txt", std::ios::in};
	auto out1 = process(in1);
	std::vector<int> outCorrect1{2, 0, 3};
	EXPECT_TRUE(out1 == outCorrect1);
}

int main()
{
	::testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}