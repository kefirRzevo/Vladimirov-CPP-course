#pragma once

#include <string>
#include <memory>
#include <iostream>

namespace paracl
{

enum class Constant : char
{
    INT,
    FLOAT,
    STR,
};

std::istream& operator<<(std::istream& is, Constant& val) {
    return is >> (char&)val;
}

class Const
{
protected:
    size_t address_ = 0U;
    size_t size_ = 0U;
    Constant type_;

public:
    Const(size_t size, Constant type) :
        size_(size), type_(type) {}

    static std::unique_ptr<Const> create(Constant type);

    size_t getAddress() const {
        return address_;
    }

    size_t getSize() const {
        return size_;
    }

    Constant getType() const {
        return type_;
    }

    void setAddress(size_t address) {
        address_ = address;
    }

    virtual void read(std::istream& ) = 0;

    virtual void write(std::ostream& ) const = 0;

    virtual ~Const() = default;
};

class ConstInt final : public Const
{
private:
    using Const::address_;
    using Const::size_;
    using Const::type_;

    int val_ = 0;

public:
    ConstInt() :
        Const(sizeof(Constant) + sizeof(int), Constant::INT) {}

    ConstInt(int val)
    : ConstInt{} {
        val_ = val;
    }

    int getVal() const {
        return val_;
    }

    void setVal(int val) {
        val_ = val;
    }

    void read(std::istream& is) override {
        is >> val_;
    }

    void write(std::ostream& os) const override {
        os << toChar(Constant::INT) << val_;
    }
};

class ConstStr final : public Const
{
private:
    using Const::address_;
    using Const::size_;
    using Const::type_;

    std::string val_ = "";

public:
    ConstStr() :
        Const(sizeof(Constant) + sizeof(size_t), Constant::STR) {}

    ConstStr(const std::string& val)
    : ConstStr{} {
        val_ = val;
    }

    std::string getVal() const {
        return val_;
    }

    void setVal(const std::string& val) {
        val_ = val;
    }

    void read(std::istream& is) override {
        is >> val_;
        size_ = sizeof(Constant) + sizeof(size_t) + val_.size();
    }

    void write(std::ostream& os) const override {
        os << toChar(Constant::STR) << size_ << val_;
    }
};

class ConstFloat final : public Const
{
private:
    using Const::address_;
    using Const::size_;
    using Const::type_;

    float val_ = 0.f;

public:
    ConstFloat() :
        Const(sizeof(Constant) + sizeof(float), Constant::FLOAT) {}

    ConstFloat(float val)
    : ConstFloat{} {
        val_ = val;
    }

    float getVal() const {
        return val_;
    }

    void setVal(float val) {
        val_ = val;
    }

    void read(std::istream& is) override {
        is >> val_;
    }

    void write(std::ostream& os) const override {
        os << toChar(Constant::FLOAT) << val_;
    }
};

std::unique_ptr<Const> Const::create(Constant type) {
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

} // namespace paracl
