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
Working: 
    (-1+5)+(2+(+3)); 
	[1,2,3,4,5]; 
	(-1  + 5) +  (2+ +3  );
    x = 1+1+1-1; 
    x = 1+1+1-1; (5 + 5); 3;
    [1,2];
    [1,2]; [3,4];
    (-1  + 5) +  (2+ +3  ); [1,2];
    x = 1; [3,3,3]; 1  + (-2);
    x = [1,2,3,4];
In progress:
*/

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " \"<expression>\"" << std::endl;
        return 1; 
    }
    std::string input = argv[1]; 
    auto root = parse_pegtl(input); 

    if (!root) {
		std::cout << "parse failed: result root is nullptr" << "\n"; 
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
            std::cout << program_node->ast->print_line() << std::endl;

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

        /*
            TODO:
            - change the arguments: ./program "filename" value1, value2, value3, ...
            - after parsing and creating an AST, generate PTX with the params being the 
        */
    } else {
        std::cout << "parse failed, root has no children" << "\n"; 
        return 1; 
    }
}