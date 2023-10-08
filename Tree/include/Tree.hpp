#pragma once

/*

Class Node. Tree consists from nodes.

Class Iterator. To give possibility to iterate over the tree.

Class Tree. Functionality:
    begin() - iterator to smallest node in the tree
    end()   - iterator to nil
    rbegin() - reversed iterator to begin
    rend() - reversed iterator to end
    empty() - check whether tree is filles with something
    size() - returns number of the nodes in the tree
    root() - returns constant pointer to the root of the tree
    clear() - delete every node from the tree
    insert(const key_type& key) - insert value in the tree
    erase(const key_type& key) - erase value from the tree
    find(const key_type& key) - returns iterator to the found node or to the nil(end)
    lower_bound(const key_type& key) - returns iterator to the lowest or equal node
    upper_bound(const key_type& key) - returns iterator to the greatest or equal node
    distance(iterator first, iterator last) - returns number of nodes in the range
    dump(std::fstream& file) - graphviz dump to the particular file
*/

#include <set>
#include <memory>
#include <vector>
#include <fstream>
#include <cstddef>
#include <iterator>
#include <iostream>
#include <algorithm>
#include <stdexcept>

namespace tree {

#define rep std::cout << __LINE__ << std::endl;

template<typename K>
struct Node;
template<typename K>
struct Iterator;
template<typename K, class Compare = std::less<K>>
class RBTree;

enum class Color: bool
{
    Black,
    Red,
};

template<typename K>
struct Node
{
    using NodePtr = Node*;
    using ConstNodePtr = const Node*;

    NodePtr parent_ = nullptr;
    NodePtr left_ = nullptr;
    NodePtr right_ = nullptr;
    Color color_ = Color::Red;
    size_t size_ = 1U;
    K key_ = K{0};

    Node() {}

    Node(const K& key):
        key_(key) {}

    bool isLeftChild() const {
        return parent_ ? this == parent_->left_ : false;
    }

    bool isRightChild() const {
        return parent_ ? this == parent_->right_ : false;
    }

    size_t size() const {
        return leftSize() + rightSize() + 1U;
    }

    size_t leftSize() const {
        return left_ ? left_->size_ : 0U;
    }

    size_t rightSize() const {
        return left_ ? left_->size_ : 0U;
    }

    NodePtr leftMost() {
        NodePtr node = this;
        while (node->left_) {
            node = node->left_;
        }
        return node;
    }

    NodePtr rightMost() {
        NodePtr node = this;
        while (node->right_) {
            node = node->right_;
        }
        return node;
    }

    NodePtr next() {
        if (right_) {
            return right_->leftMost();
        }
        NodePtr curr = this;
        NodePtr parent = curr->parent_;
        while (curr->isRightChild()) {
            curr = parent;
            parent = parent->parent_;
        }
        return parent;
    }

    NodePtr prev() {
        if (left_) {
            return left_->rightMost();
        }
        NodePtr curr = this;
        NodePtr parent = curr->parent_;
        while (curr->isLeftChild()) {
            curr = parent;
            parent = parent->parent_;
        }
        return parent;
    }

    void dumpImpl(std::fstream& file) {
        file << "\tnode_" << this << "[label = \"{" << key_ << "} | " << size_ << "\",";
        file << "shape=record, style=filled, fontcolor=white, fillcolor=";
        file << (color_ == Color::Black ? "black" : "red") << "];\n";

        if (left_) {
            file << "\tnode_" << this << " -> node_" << left_ << ";\n";
        } else {
            file << "\tnode_" << this << " -> node_null_l_" << key_ << ";\n";
            file << "\tnode_null_l_" << key_ << " [label = \"\", shape=triangle, style=filled, fillcolor=black];\n";
        }

        if (right_) {
            file << "\tnode_" << this << " -> node_" << right_ << ";\n";
        } else {
            file << "\tnode_" << this << " -> node_null_r_" << key_ << ";\n";
            file << "\tnode_null_r_" << key_ << " [label = \"\", shape=triangle, style=filled, fillcolor=black];\n";
        }
    }

    void dump() const {
        std::cout << "Node   " << this << "\n";
        std::cout << "Color  " << (color_ == Color::Black ? "black" : "red") << "\n";
        std::cout << "Value  " << key_ << "\n";
        std::cout << "Size   " << size_ << "\n";
        std::cout << "Parent " << parent_ << "\n";
        std::cout << "Left   " << left_ << "\n";
        std::cout << "Right  " << right_ << "\n\n";
    }

    void recursiveDump() const {
        dump();
        if (left_) {
            left_->recursiveDump();
        }
        if (right_) {
            right_->recursiveDump();
        }
    }
};

template<typename K>
struct Iterator
{
    using NodePtr = Node<K>*;
    using ConstNodePtr = const Node<K>*;
    
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = K;
    using pointer = K*;
    using reference = K&;

    Iterator() {}

    Iterator(NodePtr node):
        node_(node) {}

    reference operator*() const {
        return node_->key_;
    }

    pointer get() {
        return &node_->key_;
    }

    pointer operator->() {
        return &node_->key_;
    }

    Iterator& operator++() {
        node_ = node_->next();
        return *this;
    }

    Iterator operator++(int) {
        Iterator me = *this;
        node_ = node_->next();
        return me;
    }

    Iterator& operator--() {
        node_ = node_->prev();
        return *this;
    }

    Iterator operator--(int) {
        Iterator me = *this;
        node_ = node_->prev();
        return me;
    }

    bool operator==(const Iterator& rhs) const {
        return node_->key_ == rhs.node_->key_;
    }

    bool operator!=(const Iterator& rhs) const {
        return node_->key_ != rhs.node_->key_;
    }

    NodePtr node_ = nullptr;
};

template<typename K, class Compare>
class RBTree
{
public:
    using NodePtr = Node<K>*;
    using ConstNodePtr = const Node<K>*;

    using value_type = K;
    using pointer = K*;
    using reference = K&;

    using iterator = Iterator<K>;
    using reverse_iterator = std::reverse_iterator<Iterator<K>>;

private:
    NodePtr nil_ = nullptr;
    NodePtr root_ = nullptr;

    Compare compare_;

    NodePtr createNode(const K& key) const {
		return new Node<K>{key};
    }

    void deleteNode(NodePtr node) const {
        delete node;
    }

    void rotateLeft(NodePtr x) {
        /*
        // parent    --->   parent
        //   |                |
        //   x                y
        //  / \              / \
        // a   y            x   c
        //    / \          / \
        //   b   c        a   b
        */
        assert(x);
		NodePtr y = x->right_;
        assert(x->right_);
		x->right_ = y->left_;
        if (y->left_) {
            assert(y->left_->parent_);
            y->left_->parent_ = x;
        }
        assert(y->parent_);
		y->parent_ = x->parent_;
		if (x == root_) {
            assert(root_);
			root_ = y;
		} else if (x->isLeftChild()) {
            assert(x->parent_->left_);
			x->parent_->left_ = y;
		} else if (x->isRightChild()) {
            assert(x->parent_->right_);
			x->parent_->right_ = y;
		} else {
            throw std::out_of_range("X is not left nor right child");
        }
		y->left_ = x;
		x->parent_ = y;

        y->size_ = x->size_;
        x->size_ = x->size();
    }

    void rotateRight(NodePtr x) {
        /*
        //    parent    --->   parent
        //      |                |
        //      x                y
        //     / \              / \
        //    y   c            a   x
        //   / \                  / \
        //  a   b                b   c
        */
        assert(x);
		NodePtr y = x->left_;
        assert(x->left_);
		x->left_ = y->right_;
		if (y->right_) {
            assert(y->right_->parent_);
			y->right_->parent_ = x;
		}
        assert(y->parent_);
		y->parent_ = x->parent_;
		if (x == root_) {
            assert(root_);
			root_ = y;
		} else if (x->isRightChild()) {
            assert(x->parent_->right_);
			x->parent_->right_ = y;
		} else if (x->isLeftChild()) {
            assert(x->parent_->left_);
			x->parent_->left_ = y;
		} else {
            throw std::out_of_range("X is not left nor right child");
        }
		y->right_ = x;
		x->parent_ = y;

        y->size_ = x->size_;
        x->size_ = x->size();
    }

    template<typename A>
    std::pair<bool, NodePtr> visit(const K& key, A action) {
        NodePtr prev = nil_;
        NodePtr curr = root_;
		while (curr) {
            prev = curr;
            action(curr);
			if (compare_(curr->key_, key)) {
				curr = curr->right_;
			} else if (compare_(key, curr->key_)) {
				curr = curr->left_;
			} else {
                return std::make_pair(true, curr);
            }
		}
        return std::make_pair(false, prev);
    }

    NodePtr findImpl(const K& key) {
        auto [found, node] = visit(key, [] (NodePtr&) {});
        return found ? node : nil_;
    }

	NodePtr insertImpl(const K& key) {
        auto [found, y] = visit(key, [] (NodePtr&) {});
        if (found) { return y; }

		NodePtr node = createNode(key);
        if (!root_) {
            node->parent_ = nil_;
            node->color_ = Color::Black;
            nil_->left_ = node;
            nil_->right_ = node;
            root_ = node;
            return node;
        }

        visit(key, [] (NodePtr& node_) { node_->size_++; });
		node->parent_ = y;
		if (y == nil_) {
			root_ = node;
		} else if (compare_(node->key_, y->key_)) {
			y->left_ = node;
		} else {
			y->right_ = node;
		}

		fixInsert(node);
        return node;
	}

	void fixInsert(NodePtr node) {
        assert(node);

        if (compare_(nil_->right_->key_, node->key_)) {
            nil_->right_ = node;
        }
        if (compare_(node->key_, nil_->left_->key_)) {
            nil_->left_ = node;
        }

        assert(node->parent_);
		while (node != root_ && node->parent_->color_ == Color::Red) {

            assert(node->parent_->parent_);
            if (node->parent_->isRightChild()) {

                NodePtr y = node->parent_->parent_->left_;
                if (y && y->color_ == Color::Red) {
                    node->parent_->color_ = Color::Black;
                    y->color_ = Color::Black;
                    node->parent_->parent_->color_ = Color::Red;
                    node = node->parent_->parent_;
                } else {

                    if (node->isLeftChild()) {
                        node = node->parent_;
                        rotateRight(node);
                    }
                    node->parent_->color_ = Color::Black;
                    node->parent_->parent_->color_ = Color::Red;
                    rotateLeft(node->parent_->parent_);
                }
            } else {

                NodePtr y = node->parent_->parent_->right_;
                if (y && y->color_ == Color::Red) {
                    node->parent_->color_ = Color::Black;
                    y->color_ = Color::Black;
                    node->parent_->parent_->color_ = Color::Red;
                    node = node->parent_->parent_;
                } else {

                    if (node->isRightChild()) {
                        node = node->parent_;
                        rotateLeft(node);
                    }
                    node->parent_->color_ = Color::Black;
                    node->parent_->parent_->color_ = Color::Red;
                    rotateRight(node->parent_->parent_);
                }
            }
            if (node == root_) {
                break;
            }
		}
		root_->color_ = Color::Black;
	}

    void transplant(NodePtr x, NodePtr y) {
        assert(x);
        assert(y);
        if (root_ == x) {
            root_ = y;
        } else if (x->isLeftChild()) {
            x->parent_->left_ = y;
        } else {
            x->parent_->right_ = y;
        }
    }

    NodePtr eraseImpl(const K& key) {
        auto [found, node] = visit(key, [] (NodePtr&) {});
        if (!found) { return nil_; }

        visit(key, [] (NodePtr& node_) { node_->size_--; });
        NodePtr next = node->next();
        NodePtr y = node;
        NodePtr x = nullptr;
        NodePtr xParent = nullptr;
rep
        if (!y->left_) { rep
            x = y->right_;
        } else if (!y->right_) { rep
            x = y->left_;
        } else { rep
            y = y->right_->leftMost();
            x = y->right_;
        }

        if (y != node) { rep
            node->left_->parent_ = y;
            y->left_ = node->left_;
            if (y != node->right_) { rep
                xParent = y->parent_;
                if (x) { rep x->parent_ = y->parent_; }
                y->parent_->left_ = x;
                y->right_ = node->right_;
                node->right_->parent_ = y;
            } else { rep
                xParent = y;
            }

            transplant(node, y);
            y->parent_ = node->parent_;
            std::swap(y->color_, node->color_);
            y = node;
        } else { rep
            xParent = y->parent_;
            if (x) { rep x->parent_ = y->parent_; }
            transplant(node, x);

            if (nil_->left_ == node) {
                if (!node->right_) {
                    nil_->left_ = node->parent_;
                } else {
                    nil_->left_ = x->leftMost();
                }
            }

            if (nil_->right_ == node) {
                if (!node->left_) {
                    nil_->right_ = node->parent_;
                } else {
                    nil_->right_ = x->rightMost();
                }
            }
        }
        xParent->dump();

        deleteNode(node);
        if (y->color_ == Color::Black) { fixErase(x, xParent); }
        return next;
    }

    void fixErase(NodePtr node, NodePtr nodeParent) {
        assert(nodeParent);
        while (node != root_ && (!node || node->color_ == Color::Black)) {

            if (node == nodeParent->left_) {

                NodePtr w = nodeParent->right_; assert(w);
                if(w->color_ == Color::Red) {
                    w->color_ = Color::Black;
                    nodeParent->color_ = Color::Red;
                    rotateLeft(nodeParent);
                    w = nodeParent->right_; assert(w);
                }

                if ((!w->left_ || w->left_->color_ == Color::Black) &&
                    (!w->right_ || w->right_->color_ == Color::Black)) {
                    w->color_ = Color::Red;
                    node = nodeParent;
                    nodeParent = node->parent_;
                } else {

                    if (!w->right_ || w->right_->color_ == Color::Black) {
                        w->left_->color_ = Color::Black;
                        w->color_ = Color::Red;
                        rotateRight(w);
                        w = nodeParent->right_; assert(w);
                    }

                    w->color_ = nodeParent->color_;
                    nodeParent->color_ = Color::Black;
                    if (w->right_) { w->right_->color_ = Color::Black; }
                    rotateLeft(nodeParent);
                    break;
                }
            } else {

                NodePtr w = nodeParent->left_;
                if(w->color_ == Color::Red) {
                    w->color_ = Color::Black;
                    nodeParent->color_ = Color::Red;
                    rotateRight(nodeParent);
                    w = nodeParent->left_; assert(w);
                }

                if ((!w->left_ || w->left_->color_ == Color::Black) &&
                    (!w->right_ || w->right_->color_ == Color::Black)) {
                    w->color_ = Color::Red;
                    node = nodeParent;
                    nodeParent = nodeParent->parent_;
                } else {

                    if (!w->left_ || w->left_->color_ == Color::Black) {
                        w->right_->color_ = Color::Black;
                        w->color_ = Color::Red;
                        rotateLeft(w);
                        w = nodeParent->left_; assert(w);
                    }

                    w->color_ = nodeParent->color_;
                    nodeParent->color_ = Color::Black;
                    if (w->left_) { w->left_->color_ = Color::Black; }
                    rotateRight(nodeParent);
                    break;
                }
            }
        }
        if (node) { node->color_ = Color::Black; }
    }

    void dumpImpl(std::fstream& file) {
        for (auto it = begin(); it != end(); it++) {
            it.node_->dumpImpl(file);
        }
    }

public:
    RBTree() {
        nil_ = createNode(K{0});
        nil_->parent_ = nullptr;
        nil_->left_ = nil_;
        nil_->right_ = nil_;
        nil_->size_ = 0U;
    }

    RBTree(const RBTree<K>& rhs) = delete;
    RBTree& operator=(const RBTree<K>& rhs) = delete;
    RBTree(RBTree<K>&& rhs) = delete;
    RBTree& operator=(RBTree<K>&& rhs) = delete;

    ~RBTree() {
        clear();
        deleteNode(nil_);
        nil_ = nullptr;
    }

    iterator begin() const {
        return iterator(nil_->left_);
    }

    iterator end() const {
        return iterator(nil_);
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    bool empty() const {
        return !root_;
    }

    size_t size() const {
        return root_ ? root_->size_ : 0U;
    }

    const NodePtr root() const {
        return root_;
    }

    void clear() {
        std::vector<NodePtr> nodes;
        for (auto it = begin(); it != end(); it++) {
            nodes.push_back(it.node_);
        }
        for (auto node: nodes) {
            deleteNode(node);
        }
        root_ = nullptr;
        nil_->parent_ = nullptr;
        nil_->left_ = nil_;
        nil_->right_ = nil_;
    }

    iterator insert(const K& key) {
        return iterator(insertImpl(key));
    }

    iterator erase(const K& key) {
        return iterator(eraseImpl(key));
    }

    iterator find(const K& key) {
        return iterator(findImpl(key));
    }

    iterator lower_bound(const K& key) {
        NodePtr prev = nil_;
        NodePtr curr = root_;
        while (curr) {
            bool keyLess = compare_(key, curr->key_);
            if (keyLess) {
                curr = curr->right_;
            } else {
                prev = curr;
                curr = curr->right_;
            }
        }
        return iterator(prev);
    }

    iterator upper_bound(const K& key) {
        NodePtr prev = nil_;
        NodePtr curr = root_;
        while (curr) {
            bool keyBigger = compare_(curr->key_, key);
            if (keyBigger) {
                curr = curr->right_;
            } else {
                prev = curr;
                curr = curr->left_;
            }
        }
        return iterator(prev);
    }

    size_t distance(iterator first, iterator last) const {
        if (compare_(*last, *first)) { return 0U; }
        size_t dist = 1U;
        for(auto it = first; it != last; ++it) { dist++; }
        return dist;
    }

    void dump(std::fstream& file) {
        file << "digraph {\n";
        dumpImpl(file);
        file << "}\n";
    }

    void dumpInfo() {
        std::cout << "Root       " << root_ << "\n";
        std::cout << "RootParent " << root_->parent_ << "\n";
        std::cout << "Leftest    " << nil_->left_ << "\n";
        std::cout << "Rightest   " << nil_->right_ << "\n";
        std::cout << "Nil        " << nil_ << "\n";
        std::cout << "NilParent  " << nil_->parent_ << "\n\n";

        std::cout << "Begin      " << begin().node_ << "\n";
        std::cout << "End        " << end().node_ << "\n";
        nil_->parent_ = nullptr;
    }
};

} //namespace tree
