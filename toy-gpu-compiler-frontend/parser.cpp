#include <iostream>
#include <vector>
#include <string>
#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>
#include <type_traits>

// g++ parser.cpp -std=c++17 -O2 -o parser

namespace pegtl = tao::pegtl; //namespace alias, just a nickname

namespace grammar {

	struct arith_expr; 

	struct ws : pegtl::star<pegtl::space> {};

	// impl pos/neg numbers like so, or make them two separate entities
	struct number 
		: pegtl::seq<
			ws,
			pegtl::opt<pegtl::one<'+','-'>>,
			pegtl::plus<pegtl::digit>,
			ws
		> {}; 

	struct term 
		: pegtl::sor<
			pegtl::seq<
				pegtl::one<'('>,
				arith_expr,
				pegtl::one<')'>
			>,
			number
		> {}; 
	
	struct identifier : pegtl::identifier {}; 

	struct variable : identifier {}; 

	struct arith_op : pegtl::one<'+','-','*','/'> {};
	
	struct assign_op 
		: pegtl::seq<
			ws,
			pegtl::one<'='>,
			ws
		> {}; 

	struct arith_expr 
		: pegtl::seq<
			ws,
			term,
			pegtl::star<
				pegtl::seq<
					ws,
					arith_op,
					ws,
					term
				>
			>,
			ws
		> {}; 
	
	struct vector 
	: pegtl::seq<
		pegtl::one<'['>,
		ws,
		pegtl::sor<
			pegtl::seq<
				number,
				pegtl::plus<
					pegtl::seq<
						pegtl::one<','>,
						number
					>
				>
			>,
			number
		>,
		ws,
		pegtl::one<']'>
		> {}; 

	struct expr 
		: pegtl::sor<
			number,
			arith_expr,
			vector
		> {}; 
	
	struct assign_expr
		: pegtl::seq<
			variable,
			assign_op,
			expr,
			ws
		> {}; 

	struct program 
		: pegtl::seq<
			ws,
			pegtl::star<pegtl::sor<arith_expr,vector,assign_expr>>,
			pegtl::eof
		> {}; 
}; 

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

template< typename Rule >
using selector =
	pegtl::parse_tree::selector< 
		Rule, 
		pegtl::parse_tree::store_content::on<
			grammar::arith_expr,
			grammar::expr,
			grammar::arith_op,
			grammar::number,
			grammar::vector,
			grammar::variable,
			grammar::assign_expr
		>,
		pegtl::parse_tree::fold_one::on<
			grammar::program,
			grammar::term,
			grammar::identifier,
			grammar::assign_op
		>
>;

int main() {
	std::string input = "1+1"; 
	// (-1+5)+(2+(+3))
	// [1,2,3,4,5]
	// (-1  + 5) +  (2+ +3  )
	// (-1  + 5) +  (2+ +3  ) [1,2]
	// x = 1 [3,3,3] 1  + (-2)
	// x = [1,2,3,4]
	// DOESN'T WORK x = 1+1+1-1
	pegtl::memory_input in(input, "input"); 

	auto root = pegtl::parse_tree::parse< grammar::program, selector >( in );

	if (root) {
		std::cout << "Parse successful! Ast\n"; 
		print_node(root); 
	} else {
		std::cout << "parse failed" << "\n"; 
	}

	return 0; 
}; 
