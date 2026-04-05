#ifndef AST_HPP
#define AST_HPP

#include <iostream>
#include <cstdio>
#include <vector>
#include <unordered_map>

class ASTNode {
public: 
    virtual ~ASTNode() = default; 
}; 

class Expression : public ASTNode {
public: 
    static inline std::unordered_map<std::string, std::unique_ptr<Expression>> env; // use inline keyword, applied to static members. allows initialization directly in header file

    virtual ~Expression() = default; 

    virtual std::unique_ptr<Expression> eval() const = 0; // no impl, all derived classes override and impl

    virtual std::string print_expr(int indent = 0) const = 0; 

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

    std::string print_expr(int indent = 0) const override {
        // std::cout << "print int" << std::endl; 
        return std::string(indent * 2,' ') + "Integer(" + std::to_string(value) + ")"; 
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
                    if (irval == 0) {
                        throw std::runtime_error("Division by zero");
                    } 
                    return std::make_unique<Integer>(ilval / irval);
                default: throw std::runtime_error("Unknown operator"); 
            }
        } else {
            throw std::runtime_error("Cannot add nodes that are not Integer"); 
        }
    }

    std::string print_expr(int indent = 0) const override {
        // std::cout << "print arith" << std::endl; 
        return std::string(indent * 2,' ') + "Arithmetic(\n" + left->print_expr(indent + 1) + ",\n" + std::string(indent * 2 + 2,' ') + op + ",\n" + right->print_expr(indent + 1) + ")"; 
    }
}; 

class Variable : public Expression {
public:
    std::string varname; 

    Variable(std::string n) : varname{n} {}; 

    std::unique_ptr<Expression> eval() const override {
        auto expr = Expression::env.find(varname); 
        if (expr == env.end()) {
            throw std::runtime_error("undefined variable: " + varname);
        }
        return expr->second->eval(); 
    }

    std::string print_expr(int indent = 0) const override {
        return std::string(indent * 2,' ') + "Variable(" + varname + ")"; 
    }
};

class AssignmentExpression : public Expression {
public:
    std::string var; 
    std::unique_ptr<Expression> expr; 

    AssignmentExpression(
        std::string v, 
        std::unique_ptr<Expression> e) : var{std::move(v)}, expr{std::move(e)} {};
    
    std::unique_ptr<Expression> eval() const override {
        auto evaluated = expr->eval();
        Expression::env[var] = std::move(evaluated); 
        return Expression::env[var]->eval(); //TODO: probably keep but note
    }

    std::string print_expr(int indent = 0) const override {
        return std::string(indent * 2,' ') + "Assignment(\n" + std::string(indent * 2 + 2,' ') + '\'' + var + '\'' + ",\n" + expr->print_expr(indent + 1) + ")"; 
    }
};

class ExpressionList : public Expression {
public: 
    std::vector<std::unique_ptr<Expression>> expression_list; 
    
    // default constructor, add to the vector with the add method
    ExpressionList() = default; 

    std::unique_ptr<Expression> eval() const override {
        auto result_list = std::make_unique<ExpressionList>();
        for (const auto& expr : expression_list) {
            auto evaluated = expr->eval(); 
            result_list->add(std::move(evaluated)); 
        }
        return result_list; 
    }

    void add(std::unique_ptr<Expression> expr) {
        expression_list.push_back(std::move(expr)); 
    }

    std::string print_expr(int indent = 0) const override {
        // std::cout << "print exprlist" << std::endl; 
        std::string result = std::string(indent * 2,' ') + "ExprList[\n"; 
        for (int i = 0; i < expression_list.size(); ++i) {
            result += expression_list.at(i)->print_expr(indent + 1); 
            if (i + 1 < expression_list.size()) {
                result += ",\n"; // add comma between expressions
            }
        }
        return result += "]"; 
    }
};

class Vector : public Expression { // math definition of a vector
public: 
    size_t size; 
    std::vector<std::unique_ptr<Integer>> int_vector; 

    // default vector constructor. values will be added to vector instance with add function
    Vector() = default; 

    std::unique_ptr<Expression> eval() const override {
        auto result_vector = std::make_unique<Vector>(); 
        for (const auto& expr : int_vector) {
            auto evaluated = expr->eval(); 
            result_vector->add(std::move(evaluated)); 
        }
        return result_vector; 
    }

    void add(std::unique_ptr<Expression> expr) {
        // if the thing being added is a Integer, then allow the add. 
        auto int_expr = std::unique_ptr<Integer>(dynamic_cast<Integer*>(expr.get()));
        if (int_expr) {
            expr.release(); 
            int_vector.push_back(std::move(int_expr)); 
        } else {
            throw std::runtime_error("Can only add 'Integer' nodes to 'Vector' node"); 
        }
    } 

    std::string print_expr(int indent = 0) const override {
        std::string result = std::string(indent * 2, ' ') + "Vector[\n"; 
        for (int i = 0; i < int_vector.size(); ++i) {
            result += int_vector.at(i)->print_expr(indent + 1); 
            if (i + 1 < int_vector.size()) {
                result += ",\n"; // add comma between integers
            }
        }
        return result += ']'; 
    }
};

class Term : public Expression {};

class Parens : public Expression {
public: 
    char left_parens;
    std::unique_ptr<Expression> expr; 
    char right_parens; 
}; 

#endif