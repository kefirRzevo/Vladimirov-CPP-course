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
        return driver.getNextToken();
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
    expression

%type <paracl::Statement*>
    statement

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
        : IDENTIFIER                { $$ = driver.createNode<VariableExpression>(@1, $1); }
        | CONSTANT                  { $$ = driver.createNode<ConstantExpression>(@1, $1); }
        | QMARK                     { $$ = driver.createNode<InputExpression>(@$); }
        | LPAREN expression RPAREN  { $$ = $2; }

    postfix_expression
        : primary_expression        { $$ = $1; }
        | postfix_expression INC_OP { $$ = driver.createNode<UnaryExpression>(@$, UnaryOperator::UN_POSTFIX_INC, $1); }
        | postfix_expression DEC_OP { $$ = driver.createNode<UnaryExpression>(@$, UnaryOperator::UN_POSTFIX_DEC, $1); }

    unary_expression
        : postfix_expression        { $$ = $1; }
        | INC_OP unary_expression   { $$ = driver.createNode<UnaryExpression>(@$, UnaryOperator::UN_PREFIX_INC, $2); }
        | DEC_OP unary_expression   { $$ = driver.createNode<UnaryExpression>(@$, UnaryOperator::UN_PREFIX_DEC, $2); }
        | ADD_OP unary_expression   { $$ = driver.createNode<UnaryExpression>(@$, UnaryOperator::UN_ADD, $2); }
        | SUB_OP unary_expression   { $$ = driver.createNode<UnaryExpression>(@$, UnaryOperator::UN_SUB, $2); }
        | NOT_OP unary_expression   { $$ = driver.createNode<UnaryExpression>(@$, UnaryOperator::UN_NOT, $2); }

    multiplicative_expression
        : unary_expression                                  { $$ = $1; }
        | multiplicative_expression MUL_OP unary_expression { $$ = driver.createNode<BinaryExpression>(@$, BinaryOperator::BIN_MUL, $1, $3); }
        | multiplicative_expression DIV_OP unary_expression { $$ = driver.createNode<BinaryExpression>(@$, BinaryOperator::BIN_DIV, $1, $3); }
        | multiplicative_expression MOD_OP unary_expression { $$ = driver.createNode<BinaryExpression>(@$, BinaryOperator::BIN_MOD, $1, $3); }

    additive_expression
        : multiplicative_expression                             { $$ = $1; }
        | additive_expression ADD_OP multiplicative_expression  { $$ = driver.createNode<BinaryExpression>(@$, BinaryOperator::BIN_ADD, $1, $3); }
        | additive_expression SUB_OP multiplicative_expression  { $$ = driver.createNode<BinaryExpression>(@$, BinaryOperator::BIN_SUB, $1, $3); }

    relational_expression
        : additive_expression                               { $$ = $1; }
        | relational_expression L_OP additive_expression    { $$ = driver.createNode<BinaryExpression>(@$, BinaryOperator::BIN_L, $1, $3); }
        | relational_expression G_OP additive_expression    { $$ = driver.createNode<BinaryExpression>(@$, BinaryOperator::BIN_G, $1, $3); }
        | relational_expression LE_OP additive_expression   { $$ = driver.createNode<BinaryExpression>(@$, BinaryOperator::BIN_LE, $1, $3); }
        | relational_expression GE_OP additive_expression   { $$ = driver.createNode<BinaryExpression>(@$, BinaryOperator::BIN_GE, $1, $3); }

    equality_expression
        : relational_expression                             { $$ = $1; }
        | equality_expression EQ_OP relational_expression   { $$ = driver.createNode<BinaryExpression>(@$, BinaryOperator::BIN_EQ, $1, $3); }
        | equality_expression NE_OP relational_expression   { $$ = driver.createNode<BinaryExpression>(@$, BinaryOperator::BIN_NE, $1, $3); }

    logical_and_expression
        : equality_expression                               { $$ = $1; }
        | logical_and_expression AND_OP equality_expression { $$ = driver.createNode<BinaryExpression>(@$, BinaryOperator::BIN_AND, $1, $3); }

    logical_or_expression
        : logical_and_expression                                { $$ = $1; }
        | logical_or_expression OR_OP logical_and_expression    { $$ = driver.createNode<BinaryExpression>(@$, BinaryOperator::BIN_OR, $1, $3); }

    conditional_expression
        : logical_or_expression                                                 { $$ = $1; }
        | logical_or_expression QMARK expression COLON conditional_expression   { $$ = driver.createNode<TernaryExpression>(@$, $1, $3, $5); }

    assignment_expression
        : conditional_expression                        { $$ = $1; }
        | unary_expression ASSIGN assignment_expression { $$ = driver.createNode<BinaryExpression>(@$, BinaryOperator::BIN_ASSIGN, $1, $3); }

    expression
        : assignment_expression                     { $$ = $1; }
        | expression COMMA assignment_expression    { $$ = driver.createNode<BinaryExpression>(@$, BinaryOperator::BIN_COMMA, $1, $3); }

    statement
        : LCURLY statement_list RCURLY  { $$ = $2; }
        | expression_statement          { $$ = $1; }
        | selection_statement           { $$ = $1; }
        | iteration_statement           { $$ = $1; }
        | jump_statement                { $$ = $1; }
        | PRINT expression SEMICOL      { $$ = driver.createNode<OutputStatement>(@$, $2); }

    statement_list
        : %empty                        { $$ = driver.createNode<BlockStatement>(@$); }
        | statement_list SEMICOL        { $$ = $1; }
        | statement_list statement      { $1->addStatement($2); $$ = $1; }
        | statement_list error SEMICOL  { $$ = $1; yyerrok; }

    expression_statement
        : expression SEMICOL    { $$ = driver.createNode<ExpressionStatement>(@$, $1); }

    selection_statement
        : IF LPAREN expression RPAREN statement %prec IFX       { $$ = driver.createNode<IfStatement>(@$, $3, $5); }
        | IF LPAREN expression RPAREN statement ELSE statement  { $$ = driver.createNode<IfElseStatement>(@$, $3, $5, $7); }

    iteration_statement
        : WHILE LPAREN expression RPAREN statement  { $$ = driver.createNode<WhileStatement>(@$, $3, $5); }

    jump_statement
        : CONTINUE SEMICOL  { $$ = driver.createNode<ContinueStatement>(@$); }
        | BREAK SEMICOL     { $$ = driver.createNode<BreakStatement>(@$); }

    translation_unit
        : statement_list { $$ = $1; driver.setRoot($1); }
%%

void paracl::Parser::error(const location& loc, const std::string& msg) {
    driver.reportError<Syntax>(loc, msg);
}
