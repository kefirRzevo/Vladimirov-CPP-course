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

#include <memory>
#include <cassert>
#include <fstream>
#include <cstddef>
#include <iterator>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <unordered_set>

namespace tree {

template<typename K>
struct Node;
template<typename K>
struct Iterator;
template<typename K, class Compare = std::less<K>>
class RBTree;

enum class Color: bool
{
    Red,
    Black,
};

enum class Tag: bool
{
    Child,
    Thread,
};

template<typename K>
struct Node final
{
    using NodePtr = Node*;
    using ConstNodePtr = const Node*;

    NodePtr parent_ = nullptr;
    NodePtr left_ = nullptr;
    NodePtr right_ = nullptr;

    Color color_ = Color::Red;
    Tag lTag_ = Tag::Thread;
    Tag rTag_ = Tag::Thread;

    size_t size_ = 1U;
    K key_ = K{0};

    Node() {}

    Node(const K& key):
        key_(key) {}

    NodePtr left() const {
        return lTag_ == Tag::Child ? left_ : nullptr;
    }

    NodePtr right() const {
        return rTag_ == Tag::Child ? right_ : nullptr;
    }

    bool isLeftChild() const {
        return parent_ ? this == parent_->left() : false;
    }

    bool isRightChild() const {
        return parent_ ? this == parent_->right() : false;
    }

    size_t size() const {
        return leftSize() + rightSize() + 1U;
    }

    size_t leftSize() const {
        return left() ? left_->size_ : 0U;
    }

    size_t rightSize() const {
        return right() ? right_->size_ : 0U;
    }

    NodePtr leftMost() {
        NodePtr node = this;
        while (node->left()) {
            node = node->left_;
        }
        return node;
    }

    NodePtr rightMost() {
        NodePtr node = this;
        while (node->right()) {
            node = node->right_;
        }
        return node;
    }

    NodePtr next() const {
        if (right()) {
            return right_->leftMost();
        }
        return right_;
    }

    NodePtr prev() const {
        if (left()) {
            return left_->rightMost();
        }
        return left_;
    }

    void textDump() const {
        std::cout << "Node   " << this    << "\n";
        std::cout << "Parent " << parent_ << "\n";
        std::cout << "Left   " << left_   << "\n";
        std::cout << "Right  " << right_  << "\n";
        std::cout << "Color     " << (color_ == Color::Black ? "black" : "red"   ) << "\n";
        std::cout << "Left  Tag " << (lTag_  == Tag::Child   ? "child" : "thread") << "\n";
        std::cout << "Right Tag " << (rTag_  == Tag::Child   ? "child" : "thread") << "\n";
        std::cout << "Size  " << size_ << "\n";
        std::cout << "Value " << key_  << "\n\n";
    }

    void recursiveDump() const {
        textDump();
        if (left()) {
            left_->recursiveDump();
        }
        if (right()) {
            right_->recursiveDump();
        }
    }
};

template<typename K>
struct Iterator final
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
        return std::addressof(node_->key_);
    }

    pointer operator->() {
        return std::addressof(node_->key_);
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
        return node_ == rhs.node_;
    }

    bool operator!=(const Iterator& rhs) const {
        return node_ != rhs.node_;
    }

    NodePtr node_ = nullptr;
};

template<typename K, class Compare>
class RBTree final
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
    struct keyHash
    {
        std::size_t operator() (const Node<K>& node) const {
            return std::hash<K>() (node.key_);
        }
    };

    struct keyEqual
    {
        bool operator() (const Node<K>& lhs, const Node<K>& rhs) const {
            return lhs.key_ == rhs.key_;
        }
    };

    Node<K> nilNode_ = Node<K>{};
    NodePtr root_ = nullptr;
    NodePtr nil_ = std::addressof(nilNode_);

    std::unordered_set<Node<K>, keyHash, keyEqual> nodes_;
    Compare compare_;

    void nilInit() {
        nil_->parent_ = nullptr;
        nil_->left_ = nil_;
        nil_->right_ = nil_;
        nil_->color_ = Color::Black;
        nil_->size_ = 0U;
    }

    NodePtr createNode(const K& key) {
        auto it = nodes_.emplace(key).first;
        return const_cast<NodePtr>(std::addressof(*it));
    }

    void deleteNode(NodePtr node) {
        nodes_.erase(*node);
    }

    void rotateLeft(NodePtr x) {
        assert(x);
		NodePtr y = x->right_;
		x->right_ = y->left_;
        if (y->left()) {
            y->left_->parent_ = x;
        }
		y->parent_ = x->parent_;
		if (x == root_) {
			root_ = y;
		} else if (x->isLeftChild()) {
			x->parent_->left_ = y;
		} else if (x->isRightChild()) {
			x->parent_->right_ = y;
		} else {
            throw std::out_of_range("X is not left nor right child");
        }
		y->left_ = x;
		x->parent_ = y;

        if (y->lTag_ == Tag::Thread) {
            y->lTag_ = Tag::Child;
            x->rTag_ = Tag::Thread;
            x->right_ = y;
        }
        y->size_ = x->size_;
        x->size_ = x->size();
    }

    void rotateRight(NodePtr x) {
        assert(x);
		NodePtr y = x->left_;
		x->left_ = y->right_;
		if (y->right()) {
			y->right_->parent_ = x;
		}
		y->parent_ = x->parent_;
		if (x == root_) {
			root_ = y;
		} else if (x->isRightChild()) {
			x->parent_->right_ = y;
		} else if (x->isLeftChild()) {
			x->parent_->left_ = y;
		} else {
            throw std::out_of_range("Node is not left nor right child");
        }
		y->right_ = x;
		x->parent_ = y;

        if (y->rTag_ == Tag::Thread) {
            y->rTag_ = Tag::Child;
            x->lTag_ = Tag::Thread;
            x->left_ = y;
        }
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
				curr = curr->right();
			} else if (compare_(key, curr->key_)) {
				curr = curr->left();
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

    NodePtr insertFirstNode(NodePtr node) {
        assert(root_ == nullptr);
        node->left_ = nil_;
        node->right_ = nil_;
        node->color_ = Color::Black;
        nil_->left_ = node;
        nil_->right_ = node;
        root_ = node;
        return node;
    }

    void fixMinMaxInsert(NodePtr node) {
        if (compare_(nil_->right_->key_, node->key_)) {
            nil_->right_ = node;
        }
        if (compare_(node->key_, nil_->left_->key_)) {
            nil_->left_ = node;
        }
    }

	NodePtr insertImpl(const K& key) {
        auto [found, y] = visit(key, [] (NodePtr&) {});
        if (found) {
            return y;
        }

		NodePtr node = createNode(key);
        node->parent_ = y;
        if (y == nil_) {
            return insertFirstNode(node);
        }

        visit(key, [] (NodePtr& node_) { node_->size_++; });
        if (compare_(node->key_, y->key_)) {
            y->lTag_ = Tag::Child;
            node->left_ = y->left_;
            node->right_ = y;
			y->left_ = node;
		} else if (compare_(y->key_, node->key_)) {
            y->rTag_ = Tag::Child;
            node->right_ = y->right_;
            node->left_ = y;
			y->right_ = node;
		} else {
            assert(0 && "Unreachable");
        }
        fixMinMaxInsert(node);
		fixInsert(node);
        return node;
	}

	void fixInsert(NodePtr node) {
        assert(node);
        assert(node->parent_);
        NodePtr nodeParent = node->parent_;
		while (node != root_ && nodeParent->color_ == Color::Red) {
            assert(nodeParent->parent_);
            NodePtr nodeGrandparent = nodeParent->parent_;
            if (nodeParent->isLeftChild()) {
                NodePtr y = nodeGrandparent->right_;
                if (nodeGrandparent->rTag_ == Tag::Child && y->color_ == Color::Red) {
                    nodeParent->color_ = Color::Black;
                    y->color_ = Color::Black;
                    nodeGrandparent->color_ = Color::Red;
                    node = nodeGrandparent;
                    nodeParent = node->parent_;
                } else {
                    if (node->isRightChild()) {
                        node = nodeParent;
                        nodeParent = node->parent_;
                        rotateLeft(node);
                    }
                    nodeParent->color_ = Color::Black;
                    nodeGrandparent->color_ = Color::Red;
                    rotateRight(nodeGrandparent);
                }
            } else if (nodeParent->isRightChild()) {
                NodePtr y = nodeGrandparent->left_;
                if (nodeGrandparent->lTag_ == Tag::Child && y->color_ == Color::Red) {
                    nodeParent->color_ = Color::Black;
                    y->color_ = Color::Black;
                    nodeGrandparent->color_ = Color::Red;
                    node = nodeGrandparent;
                    nodeParent = node->parent_;
                } else {
                    if (node->isLeftChild()) {
                        node = nodeParent;
                        nodeParent = node->parent_;
                        rotateRight(node);
                    }
                    nodeParent->color_ = Color::Black;
                    nodeGrandparent->color_ = Color::Red;
                    rotateLeft(nodeGrandparent);
                }
            } else {
                throw std::out_of_range("Node is not left nor right child");
            }  
            if (node == root_) {
                break;
            }
		}
		root_->color_ = Color::Black;
	}

    void transplant(NodePtr old_, NodePtr new_, bool changeThreads) {
        assert(old_);
        assert(new_);
        NodePtr* place;
        NodePtr newThread;
        Tag* placeTag;
        if (old_ == root_) {
            place = &root_;
        } else if (old_->isLeftChild()) {
            place = &old_->parent_->left_;
            placeTag = &old_->parent_->lTag_;
            newThread = old_->left_;
        } else if (old_->isRightChild()) {
            place = &old_->parent_->right_;
            placeTag = &old_->parent_->rTag_;
            newThread = old_->right_;
        } else {
            throw std::out_of_range("Node is not left nor right child");
        }

        if (new_ != nil_) {
            *place = new_;
            new_->parent_ = old_->parent_;
            if (changeThreads) {
                new_->rightMost()->right_ = old_->right_;
            }
        } else if (old_ == root_) {
            root_ = nullptr;
            nil_->parent_ = nullptr;
        } else {
            *placeTag = Tag::Thread;
            if (changeThreads) {
                *place = newThread;
            }
        }
    }

    NodePtr preFixErase(NodePtr node, NodePtr x, NodePtr y) {
        NodePtr xParent = nullptr;
        if (node->lTag_ == Tag::Child && node->rTag_ == Tag::Child) {
            if (y->parent_ == node) {
                xParent = y;
            } else {
                xParent = y->parent_;
                transplant(y, x, false);
                y->rTag_ = Tag::Child;
                y->right_ = node->right_;
                y->right_->parent_ = y;
            }
            transplant(node, y, false);
            y->lTag_ = Tag::Child;
            y->left_ = node->left_;
            y->left_->parent_ = y;
            y->left_->rightMost()->right_ = y;
            std::swap(y->color_, node->color_);
            std::swap(y->size_, node->size_);
        } else {
            xParent = y->parent_;
            transplant(y, x, true);
            fixMinMaxErase(node);
        }
        return xParent;
    }

    void fixMinMaxErase(NodePtr node) {
        if (nil_->left_ == node) {
            nil_->left_ = root_->leftMost();
        }
        if (nil_->right_ == node) {
            nil_->right_ = root_->rightMost();
        }
    }

    NodePtr eraseImpl(const K& key) {
        auto [found, node] = visit(key, [] (NodePtr&) {});
        if (!found) {
            return nil_;
        }

        NodePtr next = node->next();
        NodePtr y = node;
        NodePtr x = nil_;
        Color yColor = y->color_;

        if (node->lTag_ == Tag::Thread) {
            x = node->rTag_ == Tag::Child ? node->right_ : nil_;
        } else if (node->rTag_ == Tag::Thread) {
            x = node->lTag_ == Tag::Child ? node->left_ : nil_;
        } else {
            y = node->right_->leftMost();
            x = y->rTag_ == Tag::Child ? y->right_ : nil_;
            yColor = y->color_;
        }
        visit(y->key_, [] (NodePtr& node_) { node_->size_--; });
        NodePtr xParent = preFixErase(node, x, y);
        deleteNode(node);
        if (yColor == Color::Black) {
            fixErase(x, xParent);
        }
        return next;
    }

    void fixErase(NodePtr node, NodePtr nodeParent) {
        assert(nodeParent);
        while (node != root_ && node->color_ == Color::Black) {
            if (node == nodeParent->left_) {
                NodePtr w = nodeParent->right_; assert(w);
                if (w->color_ == Color::Red) {
                    w->color_ = Color::Black;
                    nodeParent->color_ = Color::Red;
                    rotateLeft(nodeParent);
                    w = nodeParent->right_; assert(w);
                }
                if ((w->lTag_ == Tag::Thread || w->left_->color_ == Color::Black) &&
                    (w->rTag_ == Tag::Thread || w->right_->color_ == Color::Black)) {
                    w->color_ = Color::Red;
                    node = nodeParent;
                    nodeParent = node->parent_;
                } else {
                    if (w->rTag_ == Tag::Thread || w->right_->color_ == Color::Black) {
                        w->left_->color_ = Color::Black;
                        w->color_ = Color::Red;
                        rotateRight(w);
                        w = nodeParent->right_; assert(w);
                    }
                    w->color_ = nodeParent->color_;
                    nodeParent->color_ = Color::Black;
                    if (w->right()) {
                        w->right_->color_ = Color::Black;
                    }
                    rotateLeft(nodeParent);
                    break;
                }
            } else if (node == nodeParent->right_) {
                NodePtr w = nodeParent->left_;
                if (w->color_ == Color::Red) {
                    w->color_ = Color::Black;
                    nodeParent->color_ = Color::Red;
                    rotateRight(nodeParent);
                    w = nodeParent->left_; assert(w);
                }
                if ((w->lTag_ == Tag::Thread || w->left_->color_ == Color::Black) &&
                    (w->rTag_ == Tag::Thread || w->right_->color_ == Color::Black)) {
                    w->color_ = Color::Red;
                    node = nodeParent;
                    nodeParent = nodeParent->parent_;
                } else {
                    if (w->lTag_ == Tag::Thread || w->left_->color_ == Color::Black) {
                        w->right_->color_ = Color::Black;
                        w->color_ = Color::Red;
                        rotateLeft(w);
                        w = nodeParent->left_; assert(w);
                    }
                    w->color_ = nodeParent->color_;
                    nodeParent->color_ = Color::Black;
                    if (w->left()) {
                        w->left_->color_ = Color::Black;
                    }
                    rotateRight(nodeParent);
                    break;
                }
            } else {
                throw std::out_of_range("Node is not left nor right child");
            }
        }
        if (node) {
            node->color_ = Color::Black;
        }
    }

    void dumpNodeImpl(NodePtr node, std::fstream& file) const {
        file << "\tnode_" << node << "[label = \"{" << node->key_ << "} | " << node->size_ << "\",";
        file << "shape=record, style=filled, fontcolor=white, fillcolor=";
        file << (node->color_ == Color::Black ? "black" : "red") << "];\n";

        if (node->left()) {
            file << "\tnode_" << node << " -> node_" << node->left_ << ";\n";
        } else if (node->left_ != nil_) {
            file << "\tnode_" << node << " -> node_" << node->left_ << " [style=dotted, tailport=w];\n";
            file << "\tnode_null_l_" << node->key_ << "[label = \"\", shape=triangle, style=filled, fillcolor=black];\n";
            file << "\tnode_" << node << " -> node_null_l_" << node->key_ << ";\n";
        } else {
            file << "\tnode_" << node << " -> node_nil_l_" << node->key_ << " [style=dotted, tailport=w];\n";
            file << "\tnode_nil_l_" << node->key_ << "[label = \"NIL\", shape=triangle, style=filled, fillcolor=white];\n";
        }

        if (node->right()) {
            file << "\tnode_" << node << " -> node_" << node->right_ << ";\n";
        } else if (node->right_ != nil_) {
            file << "\tnode_" << node << " -> node_" << node->right_ << " [style=dotted, tailport=e];\n";
            file << "\tnode_null_r_" << node->key_ << "[label = \"\", shape=triangle, style=filled, fillcolor=black];\n";
            file << "\tnode_" << node << " -> node_null_r_" << node->key_ << ";\n";
        } else {
            file << "\tnode_" << node << " -> node_nil_r_" << node->key_ << " [style=dotted, tailport=e];\n";
            file << "\tnode_nil_r_" << node->key_ << "[label = \"NIL\", shape=triangle, style=filled, fillcolor=white];\n";
        }
    }

    void dumpImpl(std::fstream& file) const {
        file << "digraph {\n";
        file << "\tnode_" << root_ << ";\n";
        for (auto it = begin(); it != end(); it++) {
            dumpNodeImpl(it.node_, file);
        }
        file << "}\n";
        file.flush();
    }

    size_t countNodesLess(NodePtr node) const {
        assert(node);
        if (node == nil_) {
            return root_->size_;
        }
        size_t sum = node->leftSize();
        while (node != root_) {
            if (node->isRightChild()) {
                sum += node->parent_->leftSize() + 1U;
            }
            node = node->parent_;
        }
        return sum;
    }

public:
    RBTree() {
        nilInit();
    }

    iterator begin() const {
        return iterator(nil_->left_);
    }

    iterator end() const {
        return iterator(nil_);
    }

    reverse_iterator rbegin() const {
        return reverse_iterator(end());
    }

    reverse_iterator rend() const {
        return reverse_iterator(begin());
    }

    bool empty() const {
        return !root_;
    }

    size_t size() const {
        return root_ ? root_->size_ : 0U;
    }

    ConstNodePtr root() const {
        return root_;
    }

    void clear() {
        nodes_.clear();
        nilInit();
        root_ = nullptr;
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
            bool keyBigger = compare_(curr->key_, key);
            if (keyBigger) {
                if (curr->rTag_ == Tag::Thread) {
                    break;
                }
                curr = curr->right_;
            } else {
                prev = curr;
                if (curr->lTag_ == Tag::Thread) {
                    break;
                }
                curr = curr->left_;
            }
        }
        return iterator(prev);
    }

    iterator upper_bound(const K& key) {
        NodePtr prev = nil_;
        NodePtr curr = root_;
        while (curr) {
            bool keyLess = compare_(key, curr->key_);
            if (keyLess) {
                prev = curr;
                if (curr->lTag_ == Tag::Thread) {
                    break;
                }
                curr = curr->left_;
            } else {
                if (curr->rTag_ == Tag::Thread) {
                    break;
                }
                curr = curr->right_;
            }
        }
        return iterator(prev);
    }

    size_t distance(iterator first, iterator last) const {
        if (empty() || first.node_ == nil_) {
            return 0U;
        }
        if (last.node_ != nil_ && compare_(*last, *first)) {
            return 0U;
        }
        return countNodesLess(last.node_) - countNodesLess(first.node_);
    }

    void dump(std::fstream& file) const {
        dumpImpl(file);
    }

    void textDump() const {
        std::cout << "Root       " << root_ << "\n";
        std::cout << "RootParent " << root_->parent_ << "\n";
        std::cout << "Leftest    " << nil_->left_ << "\n";
        std::cout << "Rightest   " << nil_->right_ << "\n";
        std::cout << "Nil        " << nil_ << "\n";
        std::cout << "NilParent  " << nil_->parent_ << "\n\n";

        std::cout << "Begin " << begin().node_ << "\n";
        std::cout << "End   " << end().node_ << "\n";
        std::cout << "Size  " << size() << "\n\n";

        root_->recursiveDump();
        std::cout.flush();
    }
};

} //namespace tree
