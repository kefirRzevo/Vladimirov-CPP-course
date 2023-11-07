#pragma once

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include <string>
#include "../../src/location.hpp"

#define YY_DECL paracl::frontend::symbol_type paracl::frontend::Lexer::getNextToken()

namespace paracl::frontend
{

class Driver;

class Lexer final : public yyFlexLexer
{
private:
    Driver* driver_ = nullptr;
    position curPos_;

public:
    Lexer(Driver* driver, const std::string& file) :
        driver_(driver), curPos_(file) {}

    paracl::frontend::symbol_type getNextToken();

    location updateLocation() {
        auto old_ = curPos_;
        curPos_ += yyleng;
        auto new_ = curPos_;
        return location{old_, new_};
    }
};

} // namespace paracl::frontend
