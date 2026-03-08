#ifndef BUILDER_HPP
#define BUILDER_HPP

#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <memory>
#include <array>

#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>
#include "ast.hpp"
#include "parser.hpp"

namespace pegtl = tao::pegtl; 

class ASTBuilder {
    std::unique_ptr<Expression> build_node(const pegtl::parse_tree::node& node) {
        if (node.is_type<grammar::number>() && node.has_content()) {
            return std::make_unique<IntegerLiteral>(std::stoi(node.string())); 
        }
        // if (node.is_type<grammar::arith_expr>() && node.has_content()) {
        //     return std::make_unique<ArithmeticExpression>(
        //         auto left = build_node(node.);
        //         node.
        //     )

        // }
        //TODO: add the other conditional branches
    }
};

#endif

/*
    TODO: rework the arithmetic
    right now my AST nodes are left right op, but my 
    parse expression grammar is built so a arith_expr is 
    multiple arithmetic operations together. Can't have that happening
*/