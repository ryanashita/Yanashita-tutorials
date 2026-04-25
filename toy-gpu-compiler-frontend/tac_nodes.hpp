#ifndef TAC_NODES_HPP
#define TAC_NODES_HPP

#include <string>
#include <variant>
#include <format>
#include <iostream>

enum class TACOp {
    ADD, SUB, MUL, DIV,
    ASSIGN,
    LOAD,
    STORE
};

struct Temp {
    int identifer; 
    std::string name() const {
        return "t" + std::to_string(identifer); 
    }    
};

struct Variable {
    std::string varname; 
};

struct Constant {
    int value; 
};

using Operand = std::variant<Temp, Variable, Constant>; 

// base class 
class TACNode {
public: 
    virtual ~TACNode() = default; 
    virtual std::string to_string() const = 0; 
};

// just constant
class TACConstant : TACNode {
public: 
    Temp _dest; 
    Constant _constant; 

    TACConstant(Temp d, Constant c) : _dest{d}, _constant{c} {}; 

    std::string to_string() const override {
        return _dest.name() + " <- " + std::to_string(_constant.value); 
    }
}; 

// arithmetic 
class TACBinaryOp : TACNode {
public: 
    Temp _dest; 
    Operand _operand1; 
    Operand _operand2; 
    TACOp _op; 

    TACBinaryOp(Temp d, Operand o1, Operand o2, TACOp op) : _dest{d}, _operand1{o1}, _operand2{o2}, _op{op} {}; 

    std::string to_string() const override {
        auto operand_to_str = [](const Operand& ope) -> std::string {
            if (std::holds_alternative<Temp>(ope)) {
                return std::get<Temp>(ope).name(); 
            } else if (std::holds_alternative<Variable>(ope)) {
                return std::get<Variable>(ope).varname; 
            } else {
                return std::to_string(std::get<Constant>(ope).value);    
            }
        };
        std::string op_str = ""; 
        switch(_op) {
            case TACOp::ADD: op_str = " + "; break; 
            case TACOp::SUB: op_str = " - "; break; 
            case TACOp::MUL: op_str = " * "; break; 
            case TACOp::DIV: op_str = " / "; break; 
        }
        return _dest.name() + " <- " + operand_to_str(_operand1) + " " + op_str + " " + operand_to_str(_operand2); 
    }
};

// variable assignment

// integer, variable assignment, load, store

#endif