#pragma once

#include "SymTable.hpp"
#include "AST.hpp"

class Driver
{
private:
    std::unique_ptr<Tree> tree_;
    std::unique_ptr<ScopeChecker> scopes_;

};
