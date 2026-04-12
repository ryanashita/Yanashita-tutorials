#include <catch2/catch_test_macros.hpp>
#include "parser.hpp"
#include "ast.hpp"

#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>
namespace pegtl = tao::pegtl;

/*
    x = 1; [3,3,3]; 1  + (-2);
*/

TEST_CASE("Parse Integer", "[parser][numbers]") {
    auto root = parse_pegtl("44"); 
    REQUIRE(root);

    REQUIRE(!root->children.empty()); 
    my_ast_node* my_root = static_cast<my_ast_node*>(root->children[0].get()); 
    REQUIRE(my_root->ast != nullptr); 

    std::string print_res = my_root->ast->print_line(); 
    REQUIRE(print_res == "[44]");
    //"ExprList[Integer(44)]"
}

TEST_CASE("Parse Positive Integer", "[parser][numbers]") {
    auto root = parse_pegtl("+4"); 
    REQUIRE(root);

    REQUIRE(!root->children.empty()); 
    my_ast_node* my_root = static_cast<my_ast_node*>(root->children[0].get()); 
    REQUIRE(my_root->ast != nullptr); 

    std::string print_res = my_root->ast->print_line(); 
    REQUIRE(print_res == "[4]");
}

TEST_CASE("Parse Negative Integer", "[parser][numbers]") {
    auto root = parse_pegtl("-26"); 
    REQUIRE(root);

    REQUIRE(!root->children.empty()); 
    my_ast_node* my_root = static_cast<my_ast_node*>(root->children[0].get()); 
    REQUIRE(my_root->ast != nullptr); 

    std::string print_res = my_root->ast->print_line(); 
    REQUIRE(print_res == "[-26]");
}

TEST_CASE("Parse Addition", "[parser][arithmetic]") {
    auto root = parse_pegtl("1+1"); 
    REQUIRE(root);

    REQUIRE(!root->children.empty()); 
    my_ast_node* my_root = static_cast<my_ast_node*>(root->children[0].get()); 
    REQUIRE(my_root->ast != nullptr); 

    std::string print_res = my_root->ast->print_line(); 
    REQUIRE(print_res == "[(1 + 1)]");
}

TEST_CASE("Parse Subtraction", "[parser][arithmetic]") {
    auto root = parse_pegtl("17-5"); 
    REQUIRE(root);

    REQUIRE(!root->children.empty()); 
    my_ast_node* my_root = static_cast<my_ast_node*>(root->children[0].get()); 
    REQUIRE(my_root->ast != nullptr); 

    std::string print_res = my_root->ast->print_line(); 
    REQUIRE(print_res == "[(17 - 5)]");
}

TEST_CASE("Parse Multiplication", "[parser][arithmetic]") {
    auto root = parse_pegtl("2*9"); 
    REQUIRE(root);

    REQUIRE(!root->children.empty()); 
    my_ast_node* my_root = static_cast<my_ast_node*>(root->children[0].get()); 
    REQUIRE(my_root->ast != nullptr); 

    std::string print_res = my_root->ast->print_line(); 
    REQUIRE(print_res == "[(2 * 9)]");
}

TEST_CASE("Parse Division", "[parser][arithmetic]") {
    auto root = parse_pegtl("50/10"); 
    REQUIRE(root);

    REQUIRE(!root->children.empty()); 
    my_ast_node* my_root = static_cast<my_ast_node*>(root->children[0].get()); 
    REQUIRE(my_root->ast != nullptr); 

    std::string print_res = my_root->ast->print_line(); 
    REQUIRE(print_res == "[(50 / 10)]"); 
}

TEST_CASE("Parse Parentheses Addition ", "[parser][arithmetic][parentheses]") {
    auto root = parse_pegtl("(-1+5)+(2+(+3))"); 
    REQUIRE(root);

    REQUIRE(!root->children.empty()); 
    my_ast_node* my_root = static_cast<my_ast_node*>(root->children[0].get()); 
    REQUIRE(my_root->ast != nullptr); 

    std::string print_res = my_root->ast->print_line(); 
    REQUIRE(print_res == "[((-1 + 5) + (2 + 3))]");
}

TEST_CASE("Parse Parentheses Whitespace Addition ", "[parser][arithmetic][parentheses][whitespace]") {
    auto root = parse_pegtl("(-1  + 5) +  (2+ +3  )"); 
    REQUIRE(root);

    REQUIRE(!root->children.empty()); 
    my_ast_node* my_root = static_cast<my_ast_node*>(root->children[0].get()); 
    REQUIRE(my_root->ast != nullptr); 

    std::string print_res = my_root->ast->print_line(); 
    REQUIRE(print_res == "[((-1 + 5) + (2 + 3))]");
}

TEST_CASE("Parse Vector ", "[parser][vector]") {
    auto root = parse_pegtl("[1,2,3,4,5]"); 
    REQUIRE(root);

    REQUIRE(!root->children.empty()); 
    my_ast_node* my_root = static_cast<my_ast_node*>(root->children[0].get()); 
    REQUIRE(my_root->ast != nullptr); 

    std::string print_res = my_root->ast->print_line(); 
    REQUIRE(print_res == "[[1,2,3,4,5]]");
}

TEST_CASE("Parse Assignment ", "[parser][assignment][numbers]") {
    auto root = parse_pegtl("x = 1"); 
    REQUIRE(root);

    REQUIRE(!root->children.empty()); 
    my_ast_node* my_root = static_cast<my_ast_node*>(root->children[0].get()); 
    REQUIRE(my_root->ast != nullptr); 

    std::string print_res = my_root->ast->print_line(); 
    REQUIRE(print_res == "[x = 1]");
}

TEST_CASE("Parse Addition Assignment ", "[parser][assignment][numbers]") {
    auto root = parse_pegtl("x = (1+1+1-1)"); 
    REQUIRE(root);

    REQUIRE(!root->children.empty()); 
    my_ast_node* my_root = static_cast<my_ast_node*>(root->children[0].get()); 
    REQUIRE(my_root->ast != nullptr); 

    std::string print_res = my_root->ast->print_line(); 
    REQUIRE(print_res == "[x = (((1 + 1) + 1) - 1)]");
}

TEST_CASE("Parse Vector Assignment ", "[parser][assignment][vector]") {
    auto root = parse_pegtl("x = [1,2,3,4]"); 
    REQUIRE(root);

    REQUIRE(!root->children.empty()); 
    my_ast_node* my_root = static_cast<my_ast_node*>(root->children[0].get()); 
    REQUIRE(my_root->ast != nullptr); 

    std::string print_res = my_root->ast->print_line(); 
    REQUIRE(print_res == "[x = [1,2,3,4]]");
}

TEST_CASE("Parse Addition Whitespace Vector", "[parser][arithmetic][whitespace][vector]") {
    auto root = parse_pegtl("(-1  + 5) +  (2+ +3  ); [1,2];"); 
    REQUIRE(root);

    REQUIRE(!root->children.empty()); 
    my_ast_node* my_root = static_cast<my_ast_node*>(root->children[0].get()); 
    REQUIRE(my_root->ast != nullptr); 

    std::string print_res = my_root->ast->print_line(); 
    REQUIRE(print_res == "[((-1 + 5) + (2 + 3)),[1,2]]");
}

TEST_CASE("Parse Vector Vector", "[parser][vector]") {
    auto root = parse_pegtl("[1,2]; [3,4];"); 
    REQUIRE(root);

    REQUIRE(!root->children.empty()); 
    my_ast_node* my_root = static_cast<my_ast_node*>(root->children[0].get()); 
    REQUIRE(my_root->ast != nullptr); 

    std::string print_res = my_root->ast->print_line(); 
    REQUIRE(print_res == "[[1,2],[3,4]]");
}

TEST_CASE("Parse Addition Assignment Addition Integer", "[parser][arithmetic][number]") {
    auto root = parse_pegtl("x = (1+1+1-1); (5 + 5); 3;"); 
    REQUIRE(root);

    REQUIRE(!root->children.empty()); 
    my_ast_node* my_root = static_cast<my_ast_node*>(root->children[0].get()); 
    REQUIRE(my_root->ast != nullptr); 

    std::string print_res = my_root->ast->print_line(); 
    REQUIRE(print_res == "[x = (((1 + 1) + 1) - 1),(5 + 5),3]");
}

TEST_CASE("Parse Assignment Vector Addition", "[parser][vector][assignment][arithmetic]") {
    auto root = parse_pegtl("x = 1; [3,3,3]; 1  + (-2);"); 
    REQUIRE(root);

    REQUIRE(!root->children.empty()); 
    my_ast_node* my_root = static_cast<my_ast_node*>(root->children[0].get()); 
    REQUIRE(my_root->ast != nullptr); 

    std::string print_res = my_root->ast->print_line(); 
    REQUIRE(print_res == "[x = 1,[3,3,3],(1 + -2)]");
}

// fun test case with variables and addition
TEST_CASE("Parse Assignment Variable Addition Reassignment", "[parser][assignment][variable][arithmetic]") {
    auto root = parse_pegtl("x = 1; x = (x + x); x;"); 
    REQUIRE(root);

    REQUIRE(!root->children.empty()); 
    my_ast_node* my_root = static_cast<my_ast_node*>(root->children[0].get()); 
    REQUIRE(my_root->ast != nullptr); 

    std::string print_res = my_root->ast->print_line(); 
    REQUIRE(print_res == "[x = 1,x = (x + x),x]");
}