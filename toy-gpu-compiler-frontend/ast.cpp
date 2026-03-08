#include <iostream>
// #include <cstdio>
#include <vector>
#include <string>
#include <memory>
#include <array>

class ASTNode {
public: 
    virtual ~ASTNode() = default; 
}; 

class Expression : public ASTNode {

}; 

class ArithmeticExpression : public Expression {
public: 
    std::unique_ptr<Expression> left; 
    std::unique_ptr<Expression> right; 
    char op; 
}; 

template<typename T>
class Vector : public Expression { // math definition of a vector
public: 
    size_t size = N; 
    T array[N]; 

    Vector() = default; 

    Vector(){}

};

class Term : public Expression {};

class Parens : public Term {
public: 
    char left_parens;
    std::unique_ptr<Expression> expr; 
    char right_parens; 
}; 

class IntegerLiteral : public Term {
public: 
    int value; 
};



