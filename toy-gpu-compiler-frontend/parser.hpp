#ifndef PARSER_HPP
#define PARSER_HPP

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

void print_node(const std::unique_ptr<pegtl::parse_tree::node>& node, int indent);

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

std::unique_ptr<pegtl::parse_tree::node> parse_pegtl(std::string input);

#endif