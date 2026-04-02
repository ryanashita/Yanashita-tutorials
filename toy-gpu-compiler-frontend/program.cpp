#include <iostream>
#include <cstdio>
#include <typeinfo>
#include "parser.hpp"
#include "ast.hpp"
#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>

namespace pegtl = tao::pegtl; 

// g++ parser.cpp program.cpp -std=c++17 -O2 -o program

/*
    (-1+5)+(2+(+3))
	[1,2,3,4,5]
	(-1  + 5) +  (2+ +3  )
	(-1  + 5) +  (2+ +3  ) [1,2]
	x = 1 [3,3,3] 1  + (-2)
	x = [1,2,3,4]
	x = 1+1+1-1 //doesn't work
*/

int main() {
    std::string input = "a = 1 + 1"; 
    auto root = parse_pegtl(input); 

    if (!root) {
		std::cout << "parse failed, root is nullptr" << "\n"; 
        return 1; 
    }
    
    if (!root->children.empty()) {
        auto* program_node = static_cast<my_ast_node*>(root->children[0].get()); 

        /*
        Tests for root type and children
            std::cout << "Root type: " << program_node->is_type<grammar::program>() << std::endl; 
            std::cout << "Root children count: " << program_node->children.size() << std::endl;
        */

        if (program_node->ast) {
            std::cout << "parse successful! original AST:\n"; 
            std::cout << program_node->ast->print_expr(2) << std::endl;

            std::cout << "evaluated AST:\n"; 
            auto eval_result = program_node->ast->eval(); 
            std::cout << eval_result->print_expr(2) << std::endl;

            std::cout << "env map pairs:\n"; // print out variables and the values they were initialized with
            for (const auto& [name,expr] : program_node->ast->env) {
                std::cout << name << " -> " << expr->print_expr() << std::endl;
            }
        } else {
            std::cout << "AST grammar::program root node is null" << std::endl; 
        }
    } else {
        std::cout << "parse failed, root has no children" << "\n"; 
        return 1; 
    }
}