#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <stack>


// ALLOC : ID VAL
// JUMP : STR_NUM
// PRINT : ID
// LOAD : ID
// ADD, SUB, MUL, DIV : ID_res, ID_op1, ID_op2
// ASSIGN : ID, val/id

namespace Virtual_Machine
{
    enum class Command
    {
        ALLOC,
        JUMP,
        ASSIGN,
        PRINT,
        ADD,
        SUB,
        MUL,
        DIV
    };

    class Operand
    {   
        public:
        virtual int get_val(const std::unordered_map<std::string, int> &variables) const = 0;
        virtual std::string get_name() const = 0;
        virtual ~Operand() { };
    };

    class Const_operand : public Operand
    {
        int val;
        public:
        virtual int get_val(const std::unordered_map<std::string, int> &variables) const override { return val; };
        ~Const_operand() { }
    };

    class Variable_operand : public Operand
    {
        std::string var;
        public:
        virtual int get_val(const std::unordered_map<std::string, int> &variables) const override
        {   //add check for end of map
            return variables.find(var)->second;
        };
        virtual std::string get_name() const override { return var; };

        ~Variable_operand() { }
    };

    class Expression
    {
        public:
        Command action;
        std::vector<Operand> args;
    };

    class Stack_Frame final
    {   
        
        std::unordered_map<std::string, int> variables;
        
        // int push_stack_frame();

        void jump(const std::vector<Operand> &operand)
        {
            
        }

        void alloc(const std::vector<Operand> &operand)
        {
            std::string var = operand[0].get_name();    
            int val = operand[1].get_val(variables);
            variables[var] = val;
        }

        void print(const std::vector<Operand> &operand) const
        {
            int val = operand[0].get_val(variables);
            std::cout << val << std::endl;
        }
        void add(const std::vector<Operand> &operand)
        {
            std::string var = operand[0].get_name();
            int val1 = operand[1].get_val(variables);
            int val2 = operand[2].get_val(variables);
            variables[var] = val1 + val2;
        }
        void sub(const std::vector<Operand> &operand)
        {
            std::string var = operand[0].get_name();
            int val1 = operand[1].get_val(variables);
            int val2 = operand[2].get_val(variables);
            variables[var] = val1 - val2;
        }
        void mul(const std::vector<Operand> &operand)
        {
            std::string var = operand[0].get_name();
            int val1 = operand[1].get_val(variables);
            int val2 = operand[2].get_val(variables);
            variables[var] = val1 * val2;
        }
        void div(const std::vector<Operand> &operand)
        {
            std::string var = operand[0].get_name();
            int val1 = operand[1].get_val(variables);
            int val2 = operand[2].get_val(variables);
            // if(val2 == 0)
            // throw
            variables[var] = val1 / val2;
        }

        void assign(const std::vector<Operand> &operand)
        {
            std::string var = operand[0].get_name();
            int val = operand[1].get_val(variables);
            variables[var] = val;
        }

        public:
        Stack_Frame() { };

        void execute(Expression & expr)
        {
            switch(expr.action)
            {
                case Command::ASSIGN:
                    assign(expr.args);
                    break;
                case Command::ALLOC:
                    alloc(expr.args);
                    break;
                case Command::PRINT:
                    print(expr.args);
                    break;
                case Command::ADD:
                    add(expr.args);
                    break;
                case Command::SUB:
                    sub(expr.args);
                    break;
                case Command::MUL:
                    mul(expr.args);
                    break;
                case Command::DIV:
                    div(expr.args);
                    break;
                case Command::JUMP:
                    jump(expr.args);
            }
        }
    };

    class Loader
    {
        std::vector<Expression> instructions;
        void load_instructions_from_stream(std::istream & is)
        {

        }
    };

    class Executer
    {
        int stack_pointer = 0;

        
    };

}