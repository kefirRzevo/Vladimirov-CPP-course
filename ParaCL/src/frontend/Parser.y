%skeleton "lalr1.cc"
%require "3.5"
%language "c++"

%defines

%define api.namespace  { paracl }
%define api.parser.class { Parser }
%define api.token.constructor
%define api.value.type variant
%define parse.error verbose
%define parse.lac full

%glr-parser
%expect-rr 0

%code requires
{
    #include <stdexcept>
    #include "frontend/AST.hpp"

    namespace paracl
    {

    class Driver;

    } // namespace paracl
} // %code requires

%code
{
    #include <string>
    #include <iostream>
    #include "frontend/Driver.hpp"
    #include "frontend/Lexer.hpp"

    namespace paracl
    {
    
    inline paracl::Parser::symbol_type yylex(paracl::Driver& driver) {
        return driver.getLexer()->getNextToken();
    }

    } // namespace paracl
} // %code

%lex-param { paracl::Driver& driver }
%parse-param { paracl::Driver& driver }

%define api.token.prefix {TOKEN_}
%locations
%define api.location.file "location.hpp"

%token
    BREAK       "break"
    CONTINUE    "continue"
    DO          "do"
    ELSE        "else"
    FOR         "for"
    IF          "if"
    INT         "int"
    PRINT       "print"
    WHILE       "while"

    ASSIGN      "="
//    ADD_ASSIGN  "+="
//    SUB_ASSIGN  "-="
//    MUL_ASSIGN  "*="
//    DIV_ASSIGN  "/="
//    MOD_ASSIGN  "%="
    INC_OP      "++"
    DEC_OP      "--"
    AND_OP      "&&"
    OR_OP       "||"
    LE_OP       "<="
    GE_OP       ">="
    G_OP        ">"
    L_OP        "<"
    EQ_OP       "=="
    NE_OP       "!="
    SEMICOL     ";"
    COMMA       ","
    COLON       ":"
    LSQUARE     "["
    RSQUARE     "]"
    LPAREN      "("
    RPAREN      ")"
    LCURLY      "{"
    RCURLY      "}"
    NOT_OP      "!"
    QMARK       "?"
    ADD_OP      "+"
    SUB_OP      "-"
    MUL_OP      "*"
    DIV_OP      "/"
    MOD_OP      "%"

    EOF 0       "end of file"

%token
    <int>         CONSTANT   "constant"
    <std::string> IDENTIFIER "identifier"

%type <paracl::Expression*>
    primary_expression
    postfix_expression
    unary_expression
    multiplicative_expression
    additive_expression
    relational_expression
    equality_expression
    logical_and_expression
    logical_or_expression
    conditional_expression
    assignment_expression
//    constant_expression
    expression

%type <paracl::Statement*>
    statement
    compound_statement

%type <paracl::BlockStatement*>
    statement_list

%type <paracl::Statement*>
    expression_statement
    selection_statement
    iteration_statement
    jump_statement

%type <paracl::Statement*>
    translation_unit

%nonassoc IFX
%nonassoc ELSE

%start translation_unit

%%

    primary_expression
        : IDENTIFIER                { $$ = driver.getAST()->createNode<VariableExpression>(@1, $1); }
        | CONSTANT                  { $$ = driver.getAST()->createNode<ConstantExpression>(@1, $1); }
        | QMARK                     { $$ = driver.getAST()->createNode<InputExpression>(@$); }
        | LPAREN expression RPAREN  { $$ = $2; }

    postfix_expression
        : primary_expression        { $$ = $1; }
        | postfix_expression INC_OP { $$ = driver.getAST()->createNode<UnaryExpression>(@$, UnaryOperation::UN_POSTFIX_INC, $1); }
        | postfix_expression DEC_OP { $$ = driver.getAST()->createNode<UnaryExpression>(@$, UnaryOperation::UN_POSTFIX_DEC, $1); }

    unary_expression
        : postfix_expression        { $$ = $1; }
        | INC_OP unary_expression   { $$ = driver.getAST()->createNode<UnaryExpression>(@$, UnaryOperation::UN_PREFIX_INC, $2); }
        | DEC_OP unary_expression   { $$ = driver.getAST()->createNode<UnaryExpression>(@$, UnaryOperation::UN_PREFIX_DEC, $2); }
        | ADD_OP unary_expression   { $$ = driver.getAST()->createNode<UnaryExpression>(@$, UnaryOperation::UN_ADD, $2); }
        | SUB_OP unary_expression   { $$ = driver.getAST()->createNode<UnaryExpression>(@$, UnaryOperation::UN_SUB, $2); }
        | NOT_OP unary_expression   { $$ = driver.getAST()->createNode<UnaryExpression>(@$, UnaryOperation::UN_NOT, $2); }

    multiplicative_expression
        : unary_expression                                  { $$ = $1; }
        | multiplicative_expression MUL_OP unary_expression { $$ = driver.getAST()->createNode<BinaryExpression>(@$, BinaryOperation::BIN_MUL, $1, $3); }
        | multiplicative_expression DIV_OP unary_expression { $$ = driver.getAST()->createNode<BinaryExpression>(@$, BinaryOperation::BIN_DIV, $1, $3); }
        | multiplicative_expression MOD_OP unary_expression { $$ = driver.getAST()->createNode<BinaryExpression>(@$, BinaryOperation::BIN_MOD, $1, $3); }

    additive_expression
        : multiplicative_expression                             { $$ = $1; }
        | additive_expression ADD_OP multiplicative_expression  { $$ = driver.getAST()->createNode<BinaryExpression>(@$, BinaryOperation::BIN_ADD, $1, $3); }
        | additive_expression SUB_OP multiplicative_expression  { $$ = driver.getAST()->createNode<BinaryExpression>(@$, BinaryOperation::BIN_SUB, $1, $3); }

    relational_expression
        : additive_expression                               { $$ = $1; }
        | relational_expression L_OP additive_expression    { $$ = driver.getAST()->createNode<BinaryExpression>(@$, BinaryOperation::BIN_L, $1, $3); }
        | relational_expression G_OP additive_expression    { $$ = driver.getAST()->createNode<BinaryExpression>(@$, BinaryOperation::BIN_G, $1, $3); }
        | relational_expression LE_OP additive_expression   { $$ = driver.getAST()->createNode<BinaryExpression>(@$, BinaryOperation::BIN_LE, $1, $3); }
        | relational_expression GE_OP additive_expression   { $$ = driver.getAST()->createNode<BinaryExpression>(@$, BinaryOperation::BIN_GE, $1, $3); }

    equality_expression
        : relational_expression                             { $$ = $1; }
        | equality_expression EQ_OP relational_expression   { $$ = driver.getAST()->createNode<BinaryExpression>(@$, BinaryOperation::BIN_EQ, $1, $3); }
        | equality_expression NE_OP relational_expression   { $$ = driver.getAST()->createNode<BinaryExpression>(@$, BinaryOperation::BIN_NE, $1, $3); }

    logical_and_expression
        : equality_expression                               { $$ = $1; }
        | logical_and_expression AND_OP equality_expression { $$ = driver.getAST()->createNode<BinaryExpression>(@$, BinaryOperation::BIN_AND, $1, $3); }

    logical_or_expression
        : logical_and_expression                                { $$ = $1; }
        | logical_or_expression OR_OP logical_and_expression    { $$ = driver.getAST()->createNode<BinaryExpression>(@$, BinaryOperation::BIN_OR, $1, $3); }

    conditional_expression
        : logical_or_expression                                                 { $$ = $1; }
        | logical_or_expression QMARK expression COLON conditional_expression   { $$ = driver.getAST()->createNode<TernaryExpression>(@$, $1, $3, $5); }

    assignment_expression
        : conditional_expression                        { $$ = $1; }
        | unary_expression ASSIGN assignment_expression { $$ = driver.getAST()->createNode<BinaryExpression>(@$, BinaryOperation::BIN_ASSIGN, $1, $3); }

    expression
        : assignment_expression                     { $$ = $1; }
        | expression COMMA assignment_expression    { $$ = driver.getAST()->createNode<BinaryExpression>(@$, BinaryOperation::BIN_COMMA, $1, $3); }

    //constant_expression
    //    : conditional_expression    { $$ = $1; }

    statement
        : compound_statement        { $$ = $1; }
        | expression_statement      { $$ = $1; }
        | selection_statement       { $$ = $1; }
        | iteration_statement       { $$ = $1; }
        | jump_statement            { $$ = $1; }
        | PRINT expression SEMICOL  { $$ = driver.getAST()->createNode<OutputStatement>(@$, $2); }

    compound_statement
        : LCURLY RCURLY                 { $$ = driver.getAST()->createNode<BlockStatement>(@$); }
        | LCURLY statement_list RCURLY  { $$ = $2; }

    statement_list
        : statement                     { auto block = driver.getAST()->createNode<BlockStatement>(@$); block->statements_.push_back($1); $$ = block; }
        | statement_list statement      { $1->statements_.push_back($2); $$ = $1; }
        | statement_list error SEMICOL  { $$ = $1; yyerrok; }

    expression_statement
        : SEMICOL               { }
        | expression SEMICOL    { $$ = driver.getAST()->createNode<ExpressionStatement>(@$, $1); }

    selection_statement
        : IF LPAREN expression RPAREN statement %prec IFX       { $$ = driver.getAST()->createNode<IfStatement>(@$, $3, $5); }
        | IF LPAREN expression RPAREN statement ELSE statement  { $$ = driver.getAST()->createNode<IfElseStatement>(@$, $3, $5, $7); }

    iteration_statement
        : WHILE LPAREN expression RPAREN statement  { $$ = driver.getAST()->createNode<WhileStatement>(@$, $3, $5); }

    jump_statement
        : CONTINUE SEMICOL  { $$ = driver.getAST()->createNode<ContinueStatement>(@$); }
        | BREAK SEMICOL     { $$ = driver.getAST()->createNode<BreakStatement>(@$); }

    translation_unit
        : statement_list { $$ = $1; driver.getAST()->setRoot($1); }
%%

void paracl::Parser::error(const location& loc, const std::string& msg) {
    driver.getReporter()->reportError<Syntax>(loc, msg);
}
