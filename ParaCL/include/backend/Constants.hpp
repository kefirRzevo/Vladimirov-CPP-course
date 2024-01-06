#pragma once

#include <string>
#include <memory>
#include <iostream>

#include "Utils.hpp"

namespace paracl
{

enum class Constant : char
{
    INT,
    FLOAT,
    STR,
};

//inline std::istream& operator>>(std::istream& is, Constant& val) {
//    return is >> (char&)val;
//}

//inline std::ostream& operator<<(std::ostream& os, const Constant& val) {
//    return os << (char&)val;
//}

class Const
{
protected:
    size_t size_;
    Constant type_;

public:
    Const(size_t size, Constant type) :
        size_(size), type_(type) {}

    //static std::unique_ptr<Const> create(Constant type);

    size_t getSize() const {
        return size_;
    }

    Constant getType() const {
        return type_;
    }

    virtual void read(std::istream& ) = 0;

    virtual void write(std::ostream& os) const = 0;

    virtual ~Const() = default;
};

class ConstInt final : public Const
{
private:
    using Const::size_;
    using Const::type_;

    int val_ = 0;

public:
    ConstInt() :
        Const(sizeof(int), Constant::INT) {}

    ConstInt(int val)
    : ConstInt{} {
        val_ = val;
    }

    void read(std::istream& is) override {
        utils::read(is, val_);
    }

    void write(std::ostream& os) const override {
        utils::write(os, val_);
    }
};

class ConstStr final : public Const
{
private:
    using Const::size_;
    using Const::type_;

    std::string val_ = "";

public:
    ConstStr() :
        Const(sizeof(size_t), Constant::STR) {}

    ConstStr(const std::string& val) :
        Const(sizeof(size_t) + val.size(), Constant::STR), val_(val) {}

    void read(std::istream& is) override {
        size_t size = 0U;
        utils::read(is, size);
        val_.reserve(size);
        is.read(const_cast<char*>(val_.c_str()), size);
        size_ = sizeof(size_t) + val_.size();
    }

    void write(std::ostream& os) const override {
        utils::write(os, size_);
        os.write(val_.c_str(), val_.size());
    }
};

class ConstFloat final : public Const
{
private:
    using Const::size_;
    using Const::type_;

    float val_ = 0.f;

public:
    ConstFloat() :
        Const(sizeof(float), Constant::FLOAT) {}

    ConstFloat(float val)
    : ConstFloat{} {
        val_ = val;
    }

    void read(std::istream& is) override {
        is >> val_;
        utils::read(is, val_);
    }

    void write(std::ostream& os) const override {
        utils::write(os, val_);
    }
};
/*
inline std::unique_ptr<Const> Const::create(Constant type) {
    switch(type) {
    case Constant::INT:
        return std::make_unique<ConstInt>();
    case Constant::STR:
        return std::make_unique<ConstStr>();
    case Constant::FLOAT:
        return std::make_unique<ConstFloat>();
    default:
        throw std::logic_error("Unknown type");
    }
}
*/
} // namespace paracl
