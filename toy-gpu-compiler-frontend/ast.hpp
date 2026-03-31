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
    virtual ~Expression() = default; 

    virtual std::unique_ptr<Expression> eval() const = 0; // no impl, all derived classes override and impl

    virtual std::string print_expr() const = 0; 

    virtual bool isInteger() const {
        return false; 
    } 
}; 

class Integer : public Expression {
public: 
    int value; 

    Integer(int v) : value{v} {}; 

    std::unique_ptr<Expression> eval() const override {
        return std::make_unique<Integer>(value); 
    }

    bool isInteger() const override {
        return true; 
    }

    std::string print_expr() const override {
        // std::cout << "print int" << std::endl; 
        return std::string("Integer(") + std::to_string(value) + ")"; 
    }
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

    std::string print_expr() const override {
        // std::cout << "print arith" << std::endl; 
        return std::string("Arithmetic(") + left->print_expr() + ", + , " + right->print_expr() + ")"; 
    }
}; 

class ExpressionList : public Expression {
public: 
    std::vector<std::unique_ptr<Expression>> expression_list; 
    
    // default constructor, add to the vector with the add method
    ExpressionList() = default; 

    std::unique_ptr<Expression> eval() const override {
        return nullptr;
    }

    void add(std::unique_ptr<Expression> expr) {
        expression_list.push_back(std::move(expr)); 
    }

    std::string print_expr() const override {
        // std::cout << "print exprlist" << std::endl; 
        std::string result = std::string("ExprList["); 
        for (int i = 0; i < expression_list.size(); ++i) {
            result += expression_list.at(i)->print_expr(); 
            if (i + 1 < expression_list.size()) {
                result += ", "; // add comma between expressions
            }
        }
        return result += "]"; 
    }
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

#endif