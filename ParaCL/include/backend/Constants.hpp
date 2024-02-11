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

inline std::istream& operator>>(std::istream& is, Constant& val) {
    return is >> (std::underlying_type_t<Constant>&)val;
}

inline std::ostream& operator<<(std::ostream& os, const Constant& val) {
    return os << (std::underlying_type_t<Constant>&)val;
}

class Const
{
protected:
    size_t size_;
    Constant type_;

public:
    Const(size_t size, Constant type) :
        size_(size), type_(type) {}

    size_t getSize() const {
        return size_;
    }

    Constant getType() const {
        return type_;
    }

    virtual void read(const char* buf) = 0;

    virtual void write(char* buf) const = 0;

    virtual void disassemble(std::ostream& os) const = 0;

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

    void read(const char* buf) override {
        utils::read(buf, val_);
    }

    void write(char* buf) const override {
        utils::write(buf, val_);
    }

    void disassemble(std::ostream& os) const override {
        os << "int " << val_;
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

    void read(const char* buf) override {
        size_t size = 0U;
        utils::read(buf, size);
        val_.clear();
        val_.append(buf + sizeof(size), size);
        size_ = sizeof(size_t) + val_.size();
    }

    void write(char* buf) const override {
        utils::write(buf, size_);
        for (size_t i = 0; i < val_.size(); ++i) {
            utils::write(buf + sizeof(size_t) + i, val_.at(i));
        }
    }

    void disassemble(std::ostream& os) const override {
        os << "string \"" << val_ << "\"";
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

    void read(const char* buf) override {
        utils::read(buf, val_);
    }

    void write(char* buf) const override {
        utils::write(buf, val_);
    }

    void disassemble(std::ostream& os) const override {
        os << "float " << val_;
    }
};

} // namespace paracl
