#pragma once

#include <list>
#include <array>
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
struct NodeValue
{
    Triangle<T> triag;
    BoundingBox<T> box;
    size_t num;

    NodeValue():
        triag(), box(), num(0U)
        {}

    NodeValue(const Triangle<T>& triag_, const BoundingBox<T>& box_, size_t triagNum):
        triag(triag_), box(box_), num(triagNum)
        {}
};

template<typename T>
class Node
{
    public:

        BoundingBox<T> box_;
        std::list<NodeValue<T>> data_;

        Node(const BoundingBox<T>& box):
            box_(box)
            {}

        virtual ~Node() = default;

        virtual bool insert(const NodeValue<T>& value) = 0;

        virtual bool needsSplit() const = 0;

        virtual std::pair<bool, NodeValue<T>> search(const NodeValue<T>& value) const = 0;

        virtual void getIntersections(std::unordered_set<size_t>& storage) const = 0;

        virtual void dump(std::fstream& file) const = 0;
};

template<typename T>
class LeafNode: public Node<T>
{
    public:

        LeafNode(const BoundingBox<T>& box):
            Node<T>(box)
            {}

        bool insert(const NodeValue<T>& value) override
            {
                if(this->box_.contains(value.box))
                {
                    this->data_.push_back(value);
                    return true;
                }
                return false;
            }

        bool needsSplit() const override
            {
                return this->data_.size() > 3U;
            }

        std::pair<bool, NodeValue<T>> search(const NodeValue<T>& value) const override
            {
                if(this->box_.contains(value.box))
                {
                    for(const auto& val: this->data_)
                    {
                        if(val.box == value.box)
                        {
                            if(val.triag == value.triag)
                            {
                                return std::make_pair(true, val);
                            }
                        }
                    }
                }
                return std::make_pair(false, NodeValue<T>{});
            }

        void getIntersections(std::unordered_set<size_t>& storage) const override
            {
                for(auto dataIt1 = this->data_.begin(); dataIt1 != this->data_.end(); ++dataIt1)
                {
                    for(auto dataIt2 = std::next(dataIt1); dataIt2 != this->data_.end(); ++dataIt2)
                    {
                        if(dataIt1->box.intersects(dataIt2->box))
                        {
                            if(dataIt1->triag.intersects(dataIt2->triag))
                            {
                                storage.insert(dataIt1->num);
                                storage.insert(dataIt2->num);
                            }
                        }
                    }
                }
            }

        void dump(std::fstream& file) const override
            {
                file << "Node" << this << "[";
                file << "label = \"Box: ";
                file << "min {" << this->box_.min_.x_ << ", " << this->box_.min_.y_ << ", " << this->box_.min_.z_ << "}, ";
                file << "max {" << this->box_.max_.x_ << ", " << this->box_.max_.y_ << ", " << this->box_.max_.z_ << "}\n";
                for(const auto& val: this->data_)
                {
                    file << "triag" << val.num << " ";
                    file << "min {" << val.box.min_.x_ << ", " << val.box.min_.y_ << ", " << val.box.min_.z_ << "} ";
                    file << "max {" << val.box.max_.x_ << ", " << val.box.max_.y_ << ", " << val.box.max_.z_ << "}\n";
                }
                file << "\", shape=\"square\"]";
            }
};

template<typename T>
class BranchNode: public Node<T>
{
    private:

        void splitBox(const Vector<T>& sepPoint)
            {
                assert(this->box_.contains(sepPoint));

                children_[static_cast<size_t>(Quadrant::xyz)].reset(new LeafNode{BoundingBox{sepPoint, this->box_.max_}});

                Vector xy_zMin = {sepPoint.x_, sepPoint.y_, this->box_.min_.z_};
                Vector xy_zMax = {this->box_.max_.x_, this->box_.max_.y_, sepPoint.z_};
                children_[static_cast<size_t>(Quadrant::xy_z)].reset(new LeafNode{BoundingBox{xy_zMin, xy_zMax}});

                Vector x_yzMin = {sepPoint.x_, this->box_.min_.y_, sepPoint.z_};
                Vector x_yzMax = {this->box_.max_.x_, sepPoint.y_, this->box_.max_.z_};
                children_[static_cast<size_t>(Quadrant::x_yz)].reset(new LeafNode{BoundingBox{x_yzMin, x_yzMax}});

                Vector x_y_zMin = {sepPoint.x_, this->box_.min_.y_, this->box_.min_.z_};
                Vector x_y_zMax = {this->box_.max_.x_, sepPoint.y_, sepPoint.z_};
                children_[static_cast<size_t>(Quadrant::x_y_z)].reset(new LeafNode{BoundingBox{x_y_zMin, x_y_zMax}});

                Vector _xyzMin = {this->box_.min_.x_, sepPoint.y_, sepPoint.z_};
                Vector _xyzMax = {sepPoint.x_, this->box_.max_.y_, this->box_.max_.z_};
                children_[static_cast<size_t>(Quadrant::_xyz)].reset(new LeafNode{BoundingBox{_xyzMin, _xyzMax}});

                Vector _xy_zMin = {this->box_.min_.x_, sepPoint.y_, this->box_.min_.z_};
                Vector _xy_zMax = {sepPoint.x_, this->box_.max_.y_, sepPoint.z_};
                children_[static_cast<size_t>(Quadrant::_xy_z)].reset(new LeafNode{BoundingBox{_xy_zMin, _xy_zMax}});
        
                Vector _x_yzMin = {this->box_.min_.x_, this->box_.min_.y_, sepPoint.z_};
                Vector _x_yzMax = {sepPoint.x_, sepPoint.y_, this->box_.max_.z_};
                children_[static_cast<size_t>(Quadrant::_x_yz)].reset(new LeafNode{BoundingBox{_x_yzMin, _x_yzMax}});

                children_[static_cast<size_t>(Quadrant::_x_y_z)].reset(new LeafNode{BoundingBox{this->box_.min_, sepPoint}});
            }

    public:

        Vector<T> sepPoint_;
        std::array<std::unique_ptr<Node<T>>, Quadrant::count> children_;

        BranchNode(const BoundingBox<T>& box, const Vector<T>& sepPoint):
            Node<T>(box), sepPoint_(sepPoint)
            {
                splitBox(sepPoint);
            }

        bool insert(const NodeValue<T>& value) override
            {
                if(this->box_.contains(value.box))
                {
                    auto [intersects, quadrant] = this->box_.intersects(sepPoint_);
                    if(intersects)
                    {
                        this->data_.push_back(value);
                        return true;
                    }
                    else
                    {
                        if(this->children_[static_cast<size_t>(quadrant)]->needsSplit())
                        {
                            split(this->children_[static_cast<size_t>(quadrant)]);
                        }
                        return this->children_[static_cast<size_t>(quadrant)]->insert(value);
                    }
                }
                return false;
            }

        bool needsSplit() const override
            {
                return false;
            }

        std::pair<bool, NodeValue<T>> search(const NodeValue<T>& value) const override
            {
                if(this->box_.contains(value.box))
                {
                    auto [intersects, quadrant] = this->box_.intersects(sepPoint_);
                    if(intersects)
                    {
                        for(const auto& val: this->data_)
                        {
                            if(val.box == value.box)
                            {
                                if(val.triag == value.triag)
                                {
                                    return std::make_pair(true, val);
                                }
                            }
                        }
                    }
                    else
                    {
                        assert(this->children_[static_cast<size_t>(quadrant)]);
                        return this->children_[static_cast<size_t>(quadrant)]->search(value);
                    }
                }
                return std::make_pair(false, NodeValue<T>{});
            }

        void getIntersections(std::unordered_set<size_t>& storage) const override
            {
                for(auto dataIt1 = this->data_.begin(); dataIt1 != this->data_.end(); ++dataIt1)
                {
                    for(auto dataIt2 = std::next(dataIt1); dataIt2 != this->data_.end(); ++dataIt2)
                    {
                        if(dataIt1->box.intersects(dataIt2->box))
                        {
                            if(dataIt1->triag.intersects(dataIt2->triag))
                            {
                                storage.insert(dataIt1->num);
                                storage.insert(dataIt2->num);
                            }
                        }
                    }
                }

                for(size_t i = 0; i < this->children_.size(); ++i)
                {
                    if(this->children_[i])
                    {
                        auto childData = this->children_[i]->data_;
                        for(auto childDataIt = childData.begin(); childDataIt != childData.end(); ++childDataIt)
                        {
                            for(auto dataIt = this->data_.begin(); dataIt != this->data_.end(); ++dataIt)
                            {
                                if(childDataIt->box.intersects(dataIt->box))
                                {
                                    if(childDataIt->triag.intersects(dataIt->triag))
                                    {
                                        storage.insert(childDataIt->num);
                                        storage.insert(dataIt->num);
                                    }
                                }
                            }
                        }
                        this->children_[i]->getIntersections(storage);
                    }
                }
            }

        void dump(std::fstream& file) const override
            {
                file << "Node" << this << "[";
                file << "label = \"Box: ";
                file << "min {" << this->box_.min_.x_ << ", " << this->box_.min_.y_ << ", " << this->box_.min_.z_ << "}, ";
                file << "max {" << this->box_.max_.x_ << ", " << this->box_.max_.y_ << ", " << this->box_.max_.z_ << "}\n";
                file << "SepPoint {" << sepPoint_.x_ << ", " << sepPoint_.y_ << ", " << sepPoint_.z_ << "}\n";
                for(const auto& val: this->data_)
                {
                    file << "triag" << val.num << " ";
                    file << "min {" << val.box.min_.x_ << ", " << val.box.min_.y_ << ", " << val.box.min_.z_ << "} ";
                    file << "max {" << val.box.max_.x_ << ", " << val.box.max_.y_ << ", " << val.box.max_.z_ << "}\n";
                }
                file << "\", shape=\"square\"]";

                for(size_t i = 0; i < children_.size(); ++i)
                {
                    file << "Node" << this << "->Node" << children_[i].get() << "\n";
                    children_[i]->dump(file);
                }
            }
};

template<typename T>
void split(std::unique_ptr<Node<T>>& node)
    {
        auto leaf = dynamic_cast<LeafNode<T>*>(node.get());
        if(!leaf)
        {
            return;
        }

        auto box = leaf->box_;
        auto data = leaf->data_;

        Vector<T> sepPoint{};
        if(data.size() != 0)
        {
            for(const auto& elem: data)
            {
                sepPoint += (elem.box.min_ + elem.box.max_) / T{2};
            }
            sepPoint /= static_cast<T>(data.size());
        }
        if(!box.contains(sepPoint))
        {
            sepPoint = box.getCenter();
        }
        assert(sepPoint.valid());

        node.reset(new BranchNode<T>{box, sepPoint});
        auto branch = dynamic_cast<BranchNode<T>*>(node.get());

        for(auto dataIt = data.begin(); dataIt != data.end(); ++dataIt)
        {
            auto [intersects, quadrant] = dataIt->box.intersects(sepPoint);
            if(!intersects)
            {
                branch->children_[static_cast<size_t>(quadrant)]->data_.push_back(*dataIt);
            }
            else
            {
                branch->data_.push_back(*dataIt);
            }
        }
    }

template<typename T>
void collect(std::unique_ptr<Node<T>>& node)
    {
        auto branch = dynamic_cast<BranchNode<T>*>(node.get());
        if(!branch)
        {
            return;
        }

        for(auto i = 0; i < branch->children_.size(); i++)
        {
            if(branch->children_[i])
            {
                if(auto asBranchNode = dynamic_cast<BranchNode<T>>(branch->children_[i]))
                {
                    asBranchNode->collect();
                }
                branch->data_.insert(branch->children_[i]->data_);
            }
        }

        auto box = branch->box_;
        auto data = branch->data_;

        node.reset(new LeafNode<T>{box});
        node->data_ = data;
    }

template<typename T>
class OctoTree
{
    public:

        std::unique_ptr<Node<T>> root_;

        OctoTree()
            {
                T inf = std::numeric_limits<T>::infinity();
                BoundingBox<float> rootBox{{-inf, -inf, -inf}, {inf, inf, inf}};
                root_.reset(new LeafNode<T>{rootBox});
            }

        bool insert(const NodeValue<T>& value)
            {
                if(root_->needsSplit())
                {
                    split(root_);
                }
                return root_->insert(value);
            }

        std::pair<bool, NodeValue<T>> search(const NodeValue<T>& value) const
            {
                return root_->search(value);
            }

        void getIntersections(std::unordered_set<size_t>& storage) const
            {
                root_->getIntersections(storage);
            }

        void dump(std::fstream& file) const
            {
                file << "digraph D {\n";
                root_->dump(file);
                file << "}\n";
            }
};

} //namespace triangles
