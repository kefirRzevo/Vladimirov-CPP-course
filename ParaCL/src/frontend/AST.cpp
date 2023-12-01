#include "frontend/AST.hpp"
#include "frontend/NodeVisitor.hpp"
#include "frontend/NodeCopier.hpp"
#include "frontend/NodeDumper.hpp"
#include "frontend/NodeSemanticAnalyzer.hpp"

namespace paracl
{

void INode::accept(NodeVisitor& visitor) {
    visitor.visit(this);
}

AST::AST(const AST& rhs) {
    NodeCopier copier{*this};
    root_ = copier.copy(rhs.root_);
}

void AST::dump(std::ostream& os) const {
    NodeDumper dumper{os};
    dumper.dump(root_);
}

void AST::semanticAnalyze(Driver& driver) {
    NodeSemanticAnalyzer analyzer{driver};
    analyzer.analyze(root_);
}

} // namespace paracl
