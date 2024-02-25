#include "frontend/AST.hpp"
#include "frontend/NodeCopier.hpp"
#include "frontend/NodeGeneratorCL.hpp"
#include "frontend/NodeGeneratorCPP.hpp"
#include "frontend/NodeGeneratorDot.hpp"
#include "frontend/NodeSemanticAnalyzer.hpp"

namespace paracl
{

void Driver::setFilepath(const std::string& filepath) {
    lexer_->setFilepath(filepath);
    tree_->clear();
    reporter_->clear();
}

Parser::symbol_type Driver::getNextToken() {
    return lexer_->getNextToken();
}

AST::AST(const AST& rhs) {
    NodeCopier copier{*this};
    root_ = copier.copy(rhs.root_);
}

void AST::generateDot(std::string_view filepath) const {
    NodeGeneratorDot generator{filepath};
    generator.generate(root_);
}

void AST::generateCl(std::string_view filepath) const {
    NodeGeneratorCl generator{filepath};
    generator.generate(root_);
}

void AST::generateCpp(std::string_view filepath) const {
    NodeGeneratorCpp generator{filepath};
    generator.generate(root_);
}

void AST::semanticAnalyze(Driver& driver) {
    NodeSemanticAnalyzer analyzer{driver};
    analyzer.analyze(root_);
}

} // namespace paracl
