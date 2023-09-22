#pragma once

/*

Class NodeValue. Class that contains information that should be storaged in the class Node.

Abstract class Node. Node interface. Contains typical functions like insert, search
and getIntersections.

Class BranchNode: public Node. A class that contains 8 children and separation point.
Boxes that intersect with coordinate planes, passing through the separation point
within BranchNode.box_ are stored in the BranchNode.data_. So they can intersect
with each other or with children's data_.

Class LeafNode: public Node. A class that contains Nodes. Can be split to BranchNode.

Class OctoTree. Main object, that one can create to get objects that intersect with others.

*/

#include <memory>
#include <fstream>
#include <cassert>
#include <unordered_set>
#include "BoundingBox.hpp"
#include "Triangle.hpp"

namespace triangles
{

template<typename T>
struct NodeValue;
template<typename T>
class Node;
template<typename T>
class BranchNode;
template<typename T>
class LeafNode;
template<typename T>
class OctoTree;

template<typename T>
void split(std::unique_ptr<Node<T>>& node);
template<typename T>
void collect(std::unique_ptr<Node<T>>& node);

template<typename T>
struct NodeValue {
    Triangle<T> triag;
    BoundingBox<T> box;
    size_t num;

    NodeValue():
        triag(), box(), num() {}

    NodeValue(const Triangle<T>& triag_, const BoundingBox<T>& box_, size_t triagNum):
        triag(triag_), box(box_), num(triagNum) {}

    bool intersects(const NodeValue<T>& nodeVal) const {
        return box.intersects(nodeVal.box) && triag.intersects(nodeVal.triag);
    }

    bool operator==(const NodeValue<T>& nodeVal) const {
        return box == nodeVal.box && triag == nodeVal.triag;
    }

    bool operator!=(const NodeValue<T>& nodeVal) const {
        return !operator==(nodeVal);
    }
};

template<typename T>
class Node {

    protected:
        static void intersect(std::unordered_set<size_t>& storage,
        const std::vector<NodeValue<T>>& vec1, const std::vector<NodeValue<T>>& vec2) {
            for (size_t i = 0; i < vec1.size(); ++i) {
                for (size_t j = 0; j < vec2.size(); ++j) {
                    if (vec1[i].intersects(vec2[j])) {
                        storage.insert(vec1[i].num);
                        storage.insert(vec2[j].num);
                    }
                }
            }
        }

        static void intersect(std::unordered_set<size_t>& storage,
        const std::vector<NodeValue<T>>& vec) {
            if (vec.size() < 2U) {
                return;
            }
            for (size_t i = 0; i < vec.size(); ++i) {
                for (size_t j = i + 1; j < vec.size(); ++j) {
                    if (vec[i].intersects(vec[j])) {
                        storage.insert(vec[i].num);
                        storage.insert(vec[j].num);
                    }
                }
            }
        }

        static void intersect(std::unordered_set<size_t>& storage,
        const std::vector<NodeValue<T>>& vec, const NodeValue<T>& val) {
            for (size_t i = 0; i < vec.size(); ++i) {
                if (vec[i].intersects(val)) {
                    storage.insert(vec[i].num);
                    storage.insert(val.num);
                }
            }
        }

    public:
        size_t level_;
        BoundingBox<T> box_;
        std::vector<NodeValue<T>> data_;

        Node(const BoundingBox<T>& box, size_t level):
            level_(level), box_(box) {}

        virtual ~Node() = default;

        virtual std::pair<bool, NodeValue<T>> search(const NodeValue<T>& value) const = 0;

        virtual bool insert(const NodeValue<T>& value) = 0;

        virtual void getAllIntersections(std::unordered_set<size_t>& storage, const NodeValue<T>& val) const = 0;

        virtual void getIntersections(std::unordered_set<size_t>& storage) const = 0;

        virtual void dump(std::fstream& file) const = 0;
};

template<typename T>
class LeafNode: public Node<T> {

    public:
        using Node<T>::level_;
        using Node<T>::box_;
        using Node<T>::data_;

        LeafNode(const BoundingBox<T>& box, size_t level):
            Node<T>(box, level) {}

        bool needsSplit() const {
            return data_.size() > OctoTree<T>::maxThreshold || level_ > OctoTree<T>::maxDepth;
        }

        std::pair<bool, NodeValue<T>> search(const NodeValue<T>& value) const override {
            if (box_.contains(value.box)) {
                for (const auto& val: data_) {
                    if (val == value) {
                        return std::make_pair(true, val);
                    }
                }
            }
            return std::make_pair(false, NodeValue<T>{});
        }

        bool insert(const NodeValue<T>& value) override {
            if (box_.contains(value.box)) {
                data_.push_back(value);
                return true;
            }
            return false;
        }

        void getAllIntersections(std::unordered_set<size_t>& storage, const NodeValue<T>& val) const override {
            Node<T>::intersect(storage, data_, val);
        }

        void getIntersections(std::unordered_set<size_t>& storage) const override {
            Node<T>::intersect(storage, data_);
        }

        void dump(std::fstream& file) const override {
            file << "Node" << this << "[";
            file << "label = \"Box: ";
            file << "min {" << box_.min_.x_ << ", " << box_.min_.y_ << ", " << box_.min_.z_ << "}, ";
            file << "max {" << box_.max_.x_ << ", " << box_.max_.y_ << ", " << box_.max_.z_ << "}\n";
            for (const auto& val: data_) {
                file << "triag" << val.num << " ";
            }
            file  << "\n\", shape=\"square\"]";
        }
};

template<typename T>
class BranchNode: public Node<T> {

    public:
        using Node<T>::level_;
        using Node<T>::box_;
        using Node<T>::data_;

        Vector<T> sepPoint_;
        std::array<std::unique_ptr<Node<T>>, Quadrant::count> children_;

        BranchNode(const BoundingBox<T>& box, const Vector<T>& sepPoint, size_t level):
            Node<T>(box, level), sepPoint_(sepPoint) {
            for (auto i = 0; i < Quadrant::count; ++i) {
                children_[i].reset(new LeafNode{box.split(sepPoint, static_cast<Quadrant>(i)), level + 1});
            }
        }

        std::pair<bool, NodeValue<T>> search(const NodeValue<T>& value) const override {
            if (box_.contains(value.box)) {
                auto [intersects, quadrant] = box_.intersects(sepPoint_);
                if (intersects) {
                    for (const auto& val: data_) {
                        if (val == value) {
                            return std::make_pair(true, val);
                        }
                    }
                } else {
                    assert(children_[static_cast<size_t>(quadrant)]);
                    return children_[static_cast<size_t>(quadrant)]->search(value);
                }
            }
            return std::make_pair(false, NodeValue<T>{});
        }

        bool insert(const NodeValue<T>& value) override {
            if (box_.contains(value.box)) {
                auto [intersects, quadrant] = box_.intersects(sepPoint_);
                if (intersects) {
                    data_.push_back(value);
                    return true;
                } else {
                    assert(children_[static_cast<size_t>(quadrant)]);
                    auto asLeaf = dynamic_cast<LeafNode<T>*>(children_[static_cast<size_t>(quadrant)].get());
                    if (asLeaf) {
                        if (asLeaf->needsSplit()) {
                            split(children_[static_cast<size_t>(quadrant)]);
                        }
                    }
                    return children_[static_cast<size_t>(quadrant)]->insert(value);
                }
            }
            return false;
        }

        void getAllIntersections(std::unordered_set<size_t>& storage, const NodeValue<T>& val) const override {
            Node<T>::intersect(storage, data_, val);
            for (size_t i = 0; i < children_.size(); ++i) {
                assert(children_[i]);
                children_[i]->getAllIntersections(storage, val);
            }
        }

        void getIntersections(std::unordered_set<size_t>& storage) const override {
            Node<T>::intersect(storage, data_);
            for (size_t i = 0; i < children_.size(); ++i) {
                assert(children_[i]);
                children_[i]->getIntersections(storage);
                Node<T>::intersect(storage, data_, children_[i]->data_);
                for (size_t j = 0; j < data_.size(); ++j) {
                    children_[i]->getAllIntersections(storage, data_[j]);
                }
            }
        }

        void dump(std::fstream& file) const override {
            file << "Node" << this << "[";
            file << "label = \"Box: ";
            file << "min {" << box_.min_.x_ << ", " << box_.min_.y_ << ", " << box_.min_.z_ << "}, ";
            file << "max {" << box_.max_.x_ << ", " << box_.max_.y_ << ", " << box_.max_.z_ << "}\n";
            file << "SepPoint {" << sepPoint_.x_ << ", " << sepPoint_.y_ << ", " << sepPoint_.z_ << "}\n";
            for (const auto& val: data_) {
                file << "triag" << val.num << " ";
            }
            file << "\", shape=\"square\"]";

            for (size_t i = 0; i < children_.size(); ++i) {
                file << "\nNode" << this << "->Node" << children_[i].get() << "\n";
                children_[i]->dump(file);
            }
        }
};

template<typename T>
void split(std::unique_ptr<Node<T>>& node) {
    auto asLeaf = dynamic_cast<LeafNode<T>*>(node.get());
    if (!asLeaf) {
        return;
    }

    auto box = asLeaf->box_;
    auto data = std::move(asLeaf->data_);
    auto lvl = asLeaf->level_;

    Vector<T> sepPoint{};
    if (data.size() != 0) {
        for (const auto& elem: data) {
            sepPoint += (elem.box.min_ + elem.box.max_) / T{2};
        }
        sepPoint /= static_cast<T>(data.size());
    }
    if (!box.contains(sepPoint)) {
        sepPoint = box.getCenter();
    }
    assert(sepPoint.valid());

    node.reset(new BranchNode<T>{box, sepPoint, lvl});
    auto asBranch = dynamic_cast<BranchNode<T>*>(node.get());

    for (auto dataIt = data.begin(); dataIt != data.end(); ++dataIt) {
        auto [intersects, quadrant] = dataIt->box.intersects(sepPoint);
        if (!intersects) {
            asBranch->children_[static_cast<size_t>(quadrant)]->data_.push_back(*dataIt);
        } else {
            asBranch->data_.push_back(*dataIt);
        }
    }
}

template<typename T>
void collect(std::unique_ptr<Node<T>>& node) {
    auto asBranch = dynamic_cast<BranchNode<T>*>(node.get());
    if (!asBranch) {
        return;
    }

    for (auto i = 0; i < asBranch->children_.size(); i++) {
        if (asBranch->children_[i]) {
            if (auto asBranchNode = dynamic_cast<BranchNode<T>>(asBranch->children_[i])) {
                asBranchNode->collect();
            }
            asBranch->data_.insert(asBranch->children_[i]->data_);
        }
    }

    auto box = asBranch->box_;
    auto data = std::move(asBranch->data_);
    auto lvl = asBranch->level_;

    node.reset(new LeafNode<T>{box, lvl});
    node->data_ = data;
}

template<typename T>
class OctoTree {

    public:
        std::unique_ptr<Node<T>> root_;
        std::unordered_set<size_t> collisions_;

        static constexpr size_t maxDepth = 8U;
        static constexpr size_t maxThreshold = 4U;

        OctoTree() {
            T inf = std::numeric_limits<T>::infinity();
            BoundingBox<float> rootBox{{-inf, -inf, -inf}, {inf, inf, inf}};
            root_.reset(new LeafNode<T>{rootBox, 0U});
        }

        bool insert(const NodeValue<T>& value) {
            auto [foundFlag, foundValue] = search(value);
            if (foundFlag) {
                collisions_.insert(value.num);
                collisions_.insert(foundValue.num);
                return true;
            }

            auto asLeaf = dynamic_cast<LeafNode<T>*>(root_.get());
            if (asLeaf) {
                if (asLeaf->needsSplit()) {
                    split(root_);
                }
            }

            return root_->insert(value);
        }

        std::pair<bool, NodeValue<T>> search(const NodeValue<T>& value) const {
            return root_->search(value);
        }

        void getIntersections(std::unordered_set<size_t>& storage) const {
            root_->getIntersections(storage);
            storage.insert(collisions_.cbegin(), collisions_.cend());
        }

        void dump(std::fstream& file) const {
            file << "digraph D {\n";
            root_->dump(file);
            file << "\n}";
            file << std::endl;
        }
};

} //namespace triangles
