#include <iostream>
#include <cstdio>
#include "parser.hpp"
#include "ast.hpp"
#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>

namespace pegtl = tao::pegtl; 

// g++ parser.cpp eval.cpp -std=c++17 -O2 -o program

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
    //get the nodes
    //iterate through the nodes
    // create a arith_expr and then populate the member values
    // run eval
    std::string input = "1+1"; 
    auto root = parse_pegtl(input); 
    if (!root) {
		std::cout << "parse failed" << "\n"; 
        return 1; 
    }

    std::cout << "Parse successful! Ast\n"; 
	print_node(root, 0);

    

}