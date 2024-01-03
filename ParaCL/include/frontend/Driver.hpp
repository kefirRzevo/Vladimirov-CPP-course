#pragma once

#include "parser.tab.hh"
#include "Errors.hpp"
#include "Lexer.hpp"
#include "AST.hpp"

namespace paracl
{

class AST;
class Lexer;
class Parser;
class ErrorReposter;

class Driver final
{
private:
    std::string filepath_ = "";

    std::unique_ptr<AST> tree_ = nullptr;
    std::unique_ptr<Lexer> lexer_ = nullptr;
    std::unique_ptr<Parser> parser_ = nullptr;
    std::unique_ptr<ErrorReporter> reporter_ = nullptr;

public:
    Driver(const std::string& filepath)
    : filepath_(filepath) {
        tree_ = std::make_unique<AST>();
        lexer_ = std::make_unique<Lexer>(*this, getFilepath());
        parser_ = std::make_unique<Parser>(*this);
        reporter_ = std::make_unique<ErrorReporter>();
    }

    void setFile(const std::string& filepath) {
        filepath_ = filepath;
        lexer_.reset();
        lexer_ = std::make_unique<Lexer>(*this, getFilepath());
        tree_->clear();
        reporter_->clear();
    }

    std::string* getFilepath() {
        return std::addressof(filepath_);
    }

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

    void dumpAST(std::ostream& os) const {
        tree_->dump(os);
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
