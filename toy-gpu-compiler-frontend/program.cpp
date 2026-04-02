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
    std::string input = "4-16"; 
    auto root = parse_pegtl(input); 

    if (!root) {
		std::cout << "parse failed" << "\n"; 
        return 1; 
    }
    
    std::cout << "Parse successful! Original Ast:\n"; 
    if (!root->children.empty()) {
        auto* program_node = static_cast<my_ast_node*>(root->children[0].get()); 

        /*
        Tests for root type and children
            std::cout << "Root type: " << program_node->is_type<grammar::program>() << std::endl; 
            std::cout << "Root children count: " << program_node->children.size() << std::endl;
        */

        if (program_node->ast) {
            std::cout << program_node->ast->print_expr(2) << std::endl;

            std::cout << "Evaluated Ast:\n"; 
            auto eval_result = program_node->ast->eval(); 
            std::cout << eval_result->print_expr(2) << std::endl; 
        } else {
            std::cout << "AST grammar::program root node is null" << std::endl; 
        }
    }
}