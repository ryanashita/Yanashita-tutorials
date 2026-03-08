#ifndef AST_HPP
#define AST_HPP

#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <memory>
#include <array>

class ASTNode {
public: 
    virtual ~ASTNode() = default; 
}; 

class Expression : public ASTNode {
public: 
    virtual int eval() const = 0; // no impl, all derived classes override and impl
}; 

class ArithmeticExpression : public Expression {
public: 
    std::unique_ptr<Expression> left; 
    std::unique_ptr<Expression> right; 
    char op; 

    ArithmeticExpression(
        std::unique_ptr<Expression> l, 
        std::unique_ptr<Expression> r,
        char o) : left{std::move(l)}, right{std::move(r)}, op{o} {}; 

    int eval() const override {
        int lval = left->eval(); 
        int rval = right->eval(); 
        
        switch(op) {
            case '+': return lval + rval; 
            case '-': return lval - rval; 
            case '*': return lval * rval; 
            case '/': return lval / rval; 
            default: throw std::runtime_error("Unknown operator"); 
        }
    }
}; 

class ValueList : public Expression {

};

template<typename T>
class Vector : public Expression { // math definition of a vector
public: 
    // size_t size = N; 
    // T array[N]; 

    Vector() = default; 

    // Vector(ValueList vlist) {};

};

class Term : public Expression {};

class Parens : public Expression {
public: 
    char left_parens;
    std::unique_ptr<Expression> expr; 
    char right_parens; 
}; 

class IntegerLiteral : public Expression {
public: 
    int value; 

    IntegerLiteral(int v) : value{v} {}; 

    int eval() const override {
        return value; 
    }
};

#endif