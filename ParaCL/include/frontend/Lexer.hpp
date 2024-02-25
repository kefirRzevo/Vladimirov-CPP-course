#pragma once

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#undef YY_DECL
#define YY_DECL paracl::Parser::symbol_type paracl::Lexer::getNextToken()

#include <string>
#include <memory>
#include <fstream>
#include <string_view>

#include "frontend/Driver.hpp"
#include "parser.tab.hh"
#include "location.hpp"

namespace paracl
{

class Driver;
class Parser;

class Lexer final : public yyFlexLexer
{
private:
    Driver& driver_;
    std::string filepath_;
    std::ifstream in_;
    position curPos_;

public:
    Lexer(Driver& driver) :
        driver_(driver) {}

    void setFilepath(std::string_view filepath) {
        filepath_ = filepath;
        in_.open(std::string{filepath}, std::ifstream::in);
        curPos_.initialize(std::addressof(filepath_));
        yyrestart(in_);
    }

    Parser::symbol_type getNextToken();

    location updateLocation() {
        auto old_ = curPos_;
        curPos_ += yyleng;
        return location{old_, curPos_};
    }
};

} // namespace paracl
