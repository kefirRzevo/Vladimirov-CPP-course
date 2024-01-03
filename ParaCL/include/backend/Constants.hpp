#pragma once

#include <string>
#include <cstring>

namespace paracl
{

enum class Constant : char
{
    INT,
    FLOAT,
    STR,
};

class Const
{
protected:
    size_t address_;
    size_t size_;
    Constant type_;

public:
    Const(size_t size, Constant type) :
        size_(size), type_(type) {}

    virtual ~Const() = default;

    void setAddress(size_t address) {
        address_ = address;
    }

    size_t getAddress() const {
        return address_;
    }

    size_t getSize() const {
        return size_;
    }

    Constant getType() const {
        return type_;
    }

    virtual size_t read(void* addr) = 0;

    virtual size_t write(void* addr) const = 0;
};

class IntConst final : public Const
{
private:
    using Const::address_;
    using Const::size_;
    using Const::type_;

    int val_;

public:
    IntConst(int val) :
        Const(sizeof(Constant) + sizeof(int), Constant::INT), val_(val) {}

    int getVal() const {
        return val_;
    }

    size_t read(void* addr) override {
        val_ = *reinterpret_cast<int*>(addr);
        return sizeof(int);
    }

    size_t write(void* addr) const override {
        *reinterpret_cast<Constant*>(addr) = Constant::INT;
        *reinterpret_cast<int*>(addr + sizeof(Constant)) = val_;
        return size_;
    }
};

struct StrConst : public Const
{
private:
    using Const::address_;
    using Const::size_;
    using Const::type_;

    std::string val_;

public:
    StrConst(const std::string& val) :
        Const(sizeof(Constant) + sizeof(size_t) + val.size(), Constant::STR), val_(val) {}

    std::string getVal() const {
        return val_;
    }

    size_t read(void* addr) override {
        size_t size = *reinterpret_cast<size_t*>(addr);
        char* data = reinterpret_cast<char*>(addr + sizeof(size_t));
        val_.assign(data, size);
        return sizeof(size_t) + val_.size();
    }

    size_t write(void* addr) const override {
        *reinterpret_cast<Constant*>(addr) = Constant::STR;
        *reinterpret_cast<size_t*>(addr + sizeof(Constant)) = val_.size();
        char* to = reinterpret_cast<char*>(addr + sizeof(Constant) + sizeof(size_t));
        std::memcpy(to, val_.c_str(), val_.size());
        return size_;
    }
};

class FloatConst final : public Const
{
private:
    using Const::address_;
    using Const::size_;
    using Const::type_;

    float val_;

public:
    FloatConst(int val) :
        Const(sizeof(Constant) + sizeof(float), Constant::FLOAT), val_(val) {}

    int getVal() const {
        return val_;
    }

    size_t read(void* addr) override {
        val_ = *reinterpret_cast<float*>(addr);
        return sizeof(float);
    }

    size_t write(void* addr) const override {
        *reinterpret_cast<Constant*>(addr) = Constant::FLOAT;
        *reinterpret_cast<float*>(addr + sizeof(Constant)) = val_;
        return size_;
    }
};

Constant readConstant(void* addr) {
    return *reinterpret_cast<Constant*>(addr);
}

} // namespace paracl
