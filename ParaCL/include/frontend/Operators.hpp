#pragma once

#include <string>

namespace paracl
{

enum class UnaryOperator
{
    UN_SUB,
    UN_ADD,
    UN_NOT,

    UN_PREFIX_INC,
    UN_PREFIX_DEC,
    UN_POSTFIX_INC,
    UN_POSTFIX_DEC,
};

inline std::string toString(UnaryOperator op) {
    switch (op) {
    case UnaryOperator::UN_SUB:
        return "-";
    case UnaryOperator::UN_ADD:
        return "+";
    case UnaryOperator::UN_NOT:
        return "!";
    case UnaryOperator::UN_PREFIX_INC:
        return "++";
    case UnaryOperator::UN_PREFIX_DEC:
        return "--";
    case UnaryOperator::UN_POSTFIX_INC:
        return "++";
    case UnaryOperator::UN_POSTFIX_DEC:
        return "--";
    default:
        return "";
    }
}

inline bool isPrefix(UnaryOperator op) {
    switch (op) {
        case UnaryOperator::UN_POSTFIX_DEC:
        case UnaryOperator::UN_POSTFIX_INC:
            return false;
        default:
            return true;
    }
}

inline bool isPostfix(UnaryOperator op) {
    switch (op) {
        case UnaryOperator::UN_POSTFIX_DEC:
        case UnaryOperator::UN_POSTFIX_INC:
            return true;
        default:
            return false;
    }
}

enum class BinaryOperator
{
    BIN_MUL,
    BIN_DIV,
    BIN_MOD,

    BIN_ADD,
    BIN_SUB,

    BIN_L,
    BIN_G,
    BIN_LE,
    BIN_GE,

    BIN_EQ,
    BIN_NE,
    BIN_AND,
    BIN_OR,

    BIN_ASSIGN,
    BIN_COMMA,
};

inline std::string toString(BinaryOperator op) {
    switch (op) {
    case BinaryOperator::BIN_MUL:
        return "*";
    case BinaryOperator::BIN_DIV:
        return "/";
    case BinaryOperator::BIN_MOD:
        return "%";
    case BinaryOperator::BIN_ADD:
        return "+";
    case BinaryOperator::BIN_SUB:
        return "-";
    case BinaryOperator::BIN_L:
        return "<";
    case BinaryOperator::BIN_G:
        return ">";
    case BinaryOperator::BIN_LE:
        return "<=";
    case BinaryOperator::BIN_GE:
        return ">=";
    case BinaryOperator::BIN_EQ:
        return "==";
    case BinaryOperator::BIN_NE:
        return "!=";
    case BinaryOperator::BIN_AND:
        return "&&";
    case BinaryOperator::BIN_OR:
        return "||";
    case BinaryOperator::BIN_ASSIGN:
        return "=";
    case BinaryOperator::BIN_COMMA:
        return ",";
    default:
        return "";
    }
}

} // namespace paracl
