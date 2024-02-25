#pragma once

#include <memory>
#include <utility>
#include <string_view>

#include "parser.tab.hh"
#include "frontend/Errors.hpp"
#include "frontend/Lexer.hpp"
#include "frontend/AST.hpp"

namespace paracl
{

class AST;
class Lexer;
class Parser;
class ErrorReposter;

class Driver final
{
private:
    std::unique_ptr<AST> tree_;
    std::unique_ptr<Lexer> lexer_;
    std::unique_ptr<Parser> parser_;
    std::unique_ptr<ErrorReporter> reporter_;

public:
    Driver() :
        tree_(std::make_unique<AST>()),
        lexer_(std::make_unique<Lexer>(*this)),
        parser_(std::make_unique<Parser>(*this)),
        reporter_(std::make_unique<ErrorReporter>()) {}

    void setFilepath(std::string_view filepath);

    template<typename NodeType, typename... NodeArgs>
    NodeType* createNode(NodeArgs&&... args) {
        return tree_->createNode<NodeType>(std::forward<NodeArgs>(args)...);
    }

    void setRoot(INode* root) {
        tree_->setRoot(root);
    }

    INode* getRoot() {
        return tree_->getRoot();
    }

    void generateDot(std::string_view filepath) const {
        tree_->generateDot(filepath);
    }

    void generateCl(std::string_view filepath) const {
        tree_->generateCl(filepath);
    }

    void generateCpp(std::string_view filepath) const {
        tree_->generateCpp(filepath);
    }

    void semanticAnalyze() {
        tree_->semanticAnalyze(*this);
    }

    Parser::symbol_type getNextToken();

    void parse() {
        parser_->parse();
    }

    bool hasErrors() const {
        return reporter_->hasErrors();
    }

    template<typename ErrorType, typename... ErrorArgs>
    void reportError(ErrorArgs&&... args) {
        reporter_->reportError<ErrorType>(std::forward<ErrorArgs>(args)...);
    }

    void reportAllErrors(std::ostream& os) const {
        reporter_->reportAllErrors(os);
    }
};

} // namespace paracl
