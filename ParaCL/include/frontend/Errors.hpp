#pragma once

#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <iostream>
#include <string_view>

#include "location.hpp"

namespace paracl
{

struct Error
{
    location loc_;
    std::string msg_;

    Error(const location& loc, std::string_view msg) :
        loc_(loc), msg_(msg) {}

    virtual void print(std::ostream& os) const {
        os << loc_ << " error '" << msg_ << "'" << std::endl;
    }

    virtual ~Error() = default;
};

struct Lexical : public Error
{
    using Error::loc_;
    using Error::msg_;

    Lexical(const location& loc, std::string_view msg) :
        Error(loc, msg) {}

    void print(std::ostream& os) const override {
        os << loc_ << " error: lexical '" << msg_ << "'" << std::endl;
    }
};

struct UnknownToken : public Lexical
{
    using Lexical::loc_;
    using Lexical::msg_;

    std::string token_;

    UnknownToken(const location& loc, std::string_view token) :
        Lexical(loc, "unknown type name "), token_(token) {}

    void print(std::ostream& os) const override {
        os << loc_ << " error: " << msg_ << "'" << token_ << "'" << std::endl;
    }
};

struct UnterminatedComment : public Lexical
{
    using Lexical::loc_;
    using Lexical::msg_;

    UnterminatedComment(const location& loc) :
        Lexical(loc, "unterminated /* comment") {}

    void print(std::ostream& os) const override {
        os << loc_ << " error: " << msg_ << std::endl;
    }
};

struct Syntax : public Error
{
    using Error::loc_;
    using Error::msg_;

    Syntax(const location& loc, std::string_view msg) :
        Error(loc, msg) {}

    void print(std::ostream& os) const override {
        os << loc_ << " " << msg_ << std::endl;
    }
};

struct Semantic : public Error
{
    using Error::loc_;
    using Error::msg_;

    Semantic(const location& loc, std::string_view msg)
    : Error(loc, msg) {}

    void print(std::ostream& os) const override {
        os << loc_ << " error: semantic '" << msg_ << "'" << std::endl;
    }
};

struct UnassignableExpression : public Semantic
{
    using Error::loc_;
    using Error::msg_;

    UnassignableExpression(const location& loc)
    : Semantic(loc, "expression is not assignable") {}

    void print(std::ostream& os) const override {
        os << loc_ << " error: " << msg_ << std::endl;
    }
};

struct UndeclaredIdentifier : public Semantic
{
    using Error::loc_;
    using Error::msg_;

    std::string ident_;

    UndeclaredIdentifier(const location& loc, std::string_view ident)
    : Semantic(loc, "use of undeclared identifier"), ident_(ident) {}

    void print(std::ostream& os) const override {
        os << loc_ << " error: " << msg_ << " '" << ident_ << "'" << std::endl;
    }
};

struct OutOfLoopStatement : public Semantic
{
    using Error::loc_;
    using Error::msg_;

    std::string stat_;

    OutOfLoopStatement(const location& loc, std::string_view stat) :
        Semantic(loc, "statement not in loop or switch statement"),
        stat_(stat) {}

    void print(std::ostream& os) const override {
        os << loc_ << " error: '" << stat_ << "' " << msg_ << std::endl;
    }
};

class ErrorReporter final
{
private:
    std::vector<std::unique_ptr<Error>> errors_;

public:
    ErrorReporter() = default;

    bool hasErrors() const {
        return errors_.size();
    }

    template<typename ErrorType, typename... ErrorArgs>
    void reportError(ErrorArgs&&... args) {
        auto uptr = std::make_unique<ErrorType>(
            std::forward<ErrorArgs>(args)...
        );
        errors_.push_back(std::move(uptr));
    }

    void reportAllErrors(std::ostream& os) const {
        for (auto&& error : errors_) {
            error->print(os);
        }
    }

    void clear() {
        errors_.clear();
    }
};

} // namespace paracl
