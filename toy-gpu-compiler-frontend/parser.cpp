#include "parser.hpp"
#include <iostream>

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

std::unique_ptr<pegtl::parse_tree::node> parse_pegtl(std::string input) {
	pegtl::memory_input in(input, "input"); 
	auto root = pegtl::parse_tree::parse< grammar::program, selector >( in );
	return root ? std::move(root) : nullptr;  
}