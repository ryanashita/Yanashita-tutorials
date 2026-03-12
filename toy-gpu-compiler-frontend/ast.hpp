#ifndef AST_HPP
#define AST_HPP

#include <iostream>
#include <cstdio>
#include <vector>

class ASTNode {
public: 
    virtual ~ASTNode() = default; 
}; 

class Expression : public ASTNode {
public: 
    virtual std::unique_ptr<Expression> eval() const = 0; // no impl, all derived classes override and impl
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
    
    std::unique_ptr<Expression> eval() const override {
        //dynamic type check
        auto lval = left->eval();
        auto rval = right->eval(); 

        //must typecheck that it is Integer
        Integer* lval_int = dynamic_cast<Integer*>(lval.get()); //dynamic_cast returns nullptr on failure. Failure means that the base class is not a derived class
        Integer* rval_int = dynamic_cast<Integer*>(rval.get());
        //The raw pointer becomes dangling after the unique_ptr deletes the memory
        if (lval_int && rval_int) {
            //after typecheck
            int ilval = lval_int->value;
            int irval = rval_int->value; 
            switch(op) {
                case '+': return std::make_unique<Integer>(ilval + irval); 
                case '-': return std::make_unique<Integer>(ilval - irval);
                case '*': return std::make_unique<Integer>(ilval * irval); 
                case '/': 
                    if (irval == 0) throw std::runtime_error("Division by zero"); 
                    return std::make_unique<Integer>(ilval / irval);
                default: throw std::runtime_error("Unknown operator"); 
            }
        } else {
            throw std::runtime_error("Cannot add nodes that are not Integer"); 
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

class Integer : public Expression {
public: 
    int value; 

    Integer(int v) : value{v} {}; 

    // std::unique_ptr<Expression> eval() const override {
    //     return Integer(); 
    // }
};

#endif