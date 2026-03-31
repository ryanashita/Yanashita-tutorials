#include "parser.hpp"
#include <iostream>

// TODO: change into a prettyprint that prints the Expression print() output in a nicer format
void print_node(const std::unique_ptr<pegtl::parse_tree::node>& node, int indent = 0)
{
	if (!node) return; 
	std::cout << std::string(indent, ' '); 
	std::cout << node->type; //this is the type of each AST node

	if (node->has_content()) {
		std::cout << " : '" << node->string_view() << "'"; 
	}
    std::cout << "\n";

    for (const auto& child : node->children) {
        print_node(child, indent + 2);
    }
};

std::unique_ptr<my_ast_node> parse_pegtl(std::string input) {
	std::cout << "=== START PARSE PEGTL ===" << std::endl; 

	pegtl::memory_input in(input, "input"); 
	auto root = pegtl::parse_tree::parse< grammar::program,my_ast_node,selector>( in );

	std::cout << "\n=== END PARSE PEGTL ===\n" << std::endl; 
	return root ? std::move(root) : nullptr;
	/*
		if (root) return std::move(root)
		else return nullptr 
	*/
}