#pragma once

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#undef YY_DECL
#define YY_DECL paracl::Parser::symbol_type paracl::Lexer::getNextToken()

#include <fstream>

#include "Driver.hpp"
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
    std::ifstream in_;
    position curPos_;

public:
    Lexer(Driver& driver, std::string* filepath)
    : driver_(driver), in_(*filepath), curPos_(filepath) {
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
