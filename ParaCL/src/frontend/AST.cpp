#include "frontend/AST.hpp"
#include "frontend/NodeCopier.hpp"
#include "frontend/NodeDumper.hpp"
#include "frontend/NodeDecoder.hpp"
#include "frontend/NodeAnalyzer.hpp"

namespace paracl
{

Parser::symbol_type Driver::getNextToken() {
    return lexer_->getNextToken();
}

AST::AST(const AST& rhs) {
    NodeCopier copier{*this};
    root_ = copier.copy(rhs.root_);
}

void AST::dump(std::ostream& os) const {
    NodeDumper dumper{os};
    dumper.dump(root_);
}

void AST::decode(const std::string& filepath) const {
    NodeDecoder dumper{filepath};
    dumper.decode(root_);
}

void AST::semanticAnalyze(Driver& driver) {
    NodeSemanticAnalyzer analyzer{driver};
    analyzer.analyze(root_);
}

} // namespace paracl
