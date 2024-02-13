#pragma once

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

    void setFilepath(const std::string& filepath);

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

    void clearAST() {
        return tree_->clear();
    }

    void dumpAST(const std::string& filepath) const {
        tree_->dump(filepath);
    }

    void decodeAST(const std::string& filepath) const {
        tree_->decode(filepath);
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

    void clearReporter() {
        reporter_->clear();
    }
};

} // namespace paracl
