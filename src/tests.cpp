//
// Created by Peter Vaiciulis on 3/2/23.
//
#include <catch2/catch_test_macros.hpp>
#include <string>

#include "input.hpp"
#include "Stack.h"

TEST_CASE("Pre-Flight")
{
    REQUIRE(1 == 1);
}


TEST_CASE("Valid Characters", "[checkValidCharacters] [sanitization]")
{
    using namespace psv;
    // equations with one or more invalid characters
    equation invalidChars = "1 + 1 - 3 * 4 / 5 ^ 6 @";
    equation invalidChars2 = "1 + 1 - 3 * 4 / 5 ^ 6 #";
    equation invalidChars3 = "1 + 1% - 3 * 4 / 5 ^ 6 $";

    // valid equation
    equation validEq = "1 + 1 - 3 * 4 / 5 ^ 6";

    REQUIRE(checkValidCharacters(invalidChars) == false);
    REQUIRE(invalid_characters.size() == 1);
    REQUIRE(*invalid_characters[0] == '@');

    REQUIRE(checkValidCharacters(invalidChars2) == false);
    REQUIRE(invalid_characters.size() == 1);
    REQUIRE(*invalid_characters[0] == '#');

    REQUIRE(checkValidCharacters(invalidChars3) == false);
    REQUIRE(invalid_characters.size() == 2);
    REQUIRE(*invalid_characters[0] == '%');
    REQUIRE(*invalid_characters[1] == '$');

    REQUIRE(checkValidCharacters(validEq) == true);
    REQUIRE(invalid_characters.empty());
}

TEST_CASE("Eliminate Whitespace", "[eliminateWhitespace] [sanitization]")
{
    using namespace psv;

    // equations with whitespace
    equation with_whitespace = "1 + 1 - 3 * 4 / 5 ^ 6";
    equation without_whitespace = "1+1-3*4/5^6";

    // by definition of equation
    equation example = "The quick brown fox jumps over the lazy dog.";
    equation example_shrunk = "Thequickbrownfoxjumpsoverthelazydog.";

    eliminateWhiteSpace(with_whitespace);
    REQUIRE(with_whitespace == without_whitespace);

    eliminateWhiteSpace(example);
    REQUIRE(example == example_shrunk);
}

TEST_CASE("Scoping", "[scoping] [sanitization]")
{
    using namespace psv;

    // valid equations
    equation validEq = "1 + 1 - 3 * 4 / 5 ^ 6";
    equation validEq2 = "(1 + 1 - 3 * 4 / 5 ^ 6)";
    equation validEq3 = "((1 + 1 - 3 * 4 / 5 ^ 6))";
    equation validEq4 = "((1 + 1 - 3 * 4 / 5 ^ 6) + 1)";
    equation validEq5 = "((1 + 1 - 3 * 4 / 5 ^ 6) + 1) - 1";
    equation validEq6 = "(1 + (1 - 3 * 4 / 5 ^ 6) + 1 - (10))";

    // invalid equations
    equation invalidEq = "1 + 1 - 3 * 4 / 5 ^ 6)";
    equation invalidEq2 = "(1 + 1 - 3 * 4 / 5 ^ 6";
    equation invalidEq3 = "((1 + 1 - 3 * 4 / 5 ^ 6) + 1))";
    equation invalidEq4 = "((1 + 1 - 3 * 4 / 5 ^ 6) + 1) - 1)";
    equation invalidEq5 = "((1 + 1 - 3 * 4 / 5 ^ 6) + 1) - 1(";
    equation invalidEq6 = "(1 + (1 - 3 * 4 / 5 ^ 6) + 1 - 1";


    SECTION("Valid Scoping") {
        REQUIRE_NOTHROW(validScoping(validEq));
        REQUIRE_NOTHROW(validScoping(validEq2));
        REQUIRE_NOTHROW(validScoping(validEq3));
        REQUIRE_NOTHROW(validScoping(validEq4));
        REQUIRE_NOTHROW(validScoping(validEq5));
        REQUIRE_NOTHROW(validScoping(validEq6));
    }

    SECTION("Invalid Scoping") {
        REQUIRE_THROWS(validScoping(invalidEq), "Invalid Scoping");

        REQUIRE_THROWS(validScoping(invalidEq2), "Invalid Scoping");

        REQUIRE_THROWS(validScoping(invalidEq3), "Invalid Scoping");

        REQUIRE_THROWS(validScoping(invalidEq4), "Invalid Scoping");

        REQUIRE_THROWS(validScoping(invalidEq5), "Invalid Scoping");

        REQUIRE_THROWS(validScoping(invalidEq6), "Invalid Scoping");
    }
}

TEST_CASE("Operator Locations", "[operatorUsage] [sanitization]")
{
    using namespace psv;
    // operatorLocations should return a vector of references to the operators in the equation
    equation eq = "1+1-3*4/5^6";
    std::vector<const char*> operators = operatorLocations(eq);

    REQUIRE(operators.size() == 5);
    REQUIRE(operators[0] == eq.data() + 1);
    REQUIRE(operators[1] == eq.data() + 3);
    REQUIRE(operators[2] == eq.data() + 5);
    REQUIRE(operators[3] == eq.data() + 7);
    REQUIRE(operators[4] == eq.data() + 9);

}

TEST_CASE("isOperator", "[operatorUsage] [sanitization]")
{
    using namespace psv;
    REQUIRE(isOperator('+') == true);
    REQUIRE(isOperator('-') == true);
    REQUIRE(isOperator('*') == true);
    REQUIRE(isOperator('/') == true);
    REQUIRE(isOperator('^') == true);
    REQUIRE(isOperator('n') == true);
    REQUIRE(isOperator('(') == true);
    REQUIRE(isOperator(')') == true);
    REQUIRE(isOperator('1') == false);
    REQUIRE(isOperator('a') == false);
    REQUIRE(isOperator(' ') == false);
}

TEST_CASE("Stack Class"){
using namespace psv;
    // stack class should be able to place and pop values
    // it should also be able to return the top value
    SECTION("Standard Functions") {
        Stack<int> s;
        s.place(1);
        s.place(2);
        s.place(3);
        s.place(4);
        s.place(5);

        REQUIRE(s.top() == 5);
        s.pop();
        REQUIRE(s.top() == 4);
        s.pop();
        REQUIRE(s.top() == 3);
        s.pop();
        REQUIRE(s.top() == 2);
        s.pop();
        REQUIRE(s.top() == 1);
        s.pop();
        REQUIRE(s.isEmpty() == true);
    }

    struct Car {
        int year;
        std::string make;
        std::string model;
        Car(int y, std::string m, std::string mo) : year(y), make(m), model(mo) {}
        Car() : year(0), make(""), model("") {}

    };
    SECTION("Custom Objects"){
        Car car1 = Car(2019, "Ford", "Mustang");
        Car car2 = Car(2018, "Ford", "F-150");
        Car car3 = Car(2017, "Ford", "Fusion");
        Car car4 = Car(2016, "Ford", "Focus");
        Car car5 = Car(2015, "Ford", "Explorer");

        Stack<Car> car_stack;
        car_stack.place(Car(2019, "Ford", "Mustang"));
        REQUIRE(car_stack.top().year == 2019);
        REQUIRE(car_stack.top().make == "Ford");

        car_stack.place(car2);
        REQUIRE(car_stack.top().year == 2018);
        REQUIRE(car_stack.top().make == "Ford");

        car_stack.pop();
        REQUIRE(car_stack.top().year == 2019);

        car_stack.place(car3);
        car_stack.place(car4);
        car_stack.place(car5);
        REQUIRE(car_stack.top().year == 2015);
        REQUIRE(car_stack.size() == 4);

    }

    // Emplace is way cooler than place... |-D-D| (cool-sunglasses)
    SECTION("Emplace"){
        Stack<Car> car_stack;
        car_stack.emplace(2019, "Ford", "Mustang");
        car_stack.emplace(2018, "Ford", "F-150");
        car_stack.emplace(2017, "Ford", "Fusion");
        car_stack.emplace(2016, "Ford", "Focus");
        car_stack.emplace(2015, "Ford", "Explorer");

        REQUIRE(car_stack.top().year == 2015);
        REQUIRE(car_stack.top().make == "Ford");
        REQUIRE(car_stack.top().model == "Explorer");
        car_stack.pop();
        REQUIRE(car_stack.top().year == 2016);
        REQUIRE(car_stack.top().make == "Ford");
        REQUIRE(car_stack.top().model == "Focus");
        car_stack.pop();
        REQUIRE(car_stack.top().year == 2017);
        REQUIRE(car_stack.top().make == "Ford");
        REQUIRE(car_stack.top().model == "Fusion");
        car_stack.pop();
        REQUIRE(car_stack.top().year == 2018);
        REQUIRE(car_stack.top().make == "Ford");
        REQUIRE(car_stack.top().model == "F-150");
        car_stack.pop();
        REQUIRE(car_stack.top().year == 2019);
        REQUIRE(car_stack.top().make == "Ford");
        REQUIRE(car_stack.top().model == "Mustang");
        car_stack.pop();
        REQUIRE(car_stack.isEmpty() == true);
    }
}

std::vector<char> as_vector(std::string s){
    std::vector<char> v;
    for(auto c : s){
        v.push_back(c);
    }
    return v;
}
TEST_CASE("Parse Equation"){
    using namespace psv;
    SECTION("Valid Statements"){
        // Standard statement in order
       std::string valid1 = "1+1-3*4/5^6";
       std::vector<char> valid1_parsed;
       REQUIRE_NOTHROW(valid1_parsed = parseStatement(valid1));
       REQUIRE(valid1_parsed == as_vector("11+34*56^/-"));

       // Statement with operators out of order
       std::string valid2 = "1+1-3*4/5^6+1";
       std::vector<char> valid2_parsed;
       REQUIRE_NOTHROW(valid2_parsed = parseStatement(valid2));
       REQUIRE(valid2_parsed == as_vector("11+34*56^/-1+"));

       // Statement with parenthesis affecting order of operations
       std::string valid3 = "(1+1)*3";
       std::vector<char> valid3_parsed;
       REQUIRE_NOTHROW(valid3_parsed = parseStatement(valid3));
       REQUIRE(valid3_parsed == as_vector("11+3*"));


       // Statement with two parenthesis
         std::string valid4 = "(1+1)*(3+3)";
            std::vector<char> valid4_parsed;
            REQUIRE_NOTHROW(valid4_parsed = parseStatement(valid4));
            REQUIRE(valid4_parsed == as_vector("11+33+*"));

            // Statement with nested parenthesis
            std::string valid5 = "((1+1)*3)";
            std::vector<char> valid5_parsed;
            REQUIRE_NOTHROW(valid5_parsed = parseStatement(valid5));
            REQUIRE(valid5_parsed == as_vector("11+3*"));
    }
    SECTION("Invalid Statements"){
        std::string invalid1 = "1+1-3*4/5^6+";
        REQUIRE_THROWS(parseStatement(invalid1));

    }

}
// This tests the program's ability to evaluate statements
TEST_CASE("Non-RPN Evaluate"){
    using namespace psv;
    SECTION("Test basic addition") {
        REQUIRE(nonRpnEvaluate("1 + 2") == 3.0f);
    }

    SECTION("Test basic subtraction") {
        REQUIRE(nonRpnEvaluate("5 - 3") == 2.0f);
    }

    SECTION("Test basic multiplication") {
        REQUIRE(nonRpnEvaluate("2 * 3") == 6.0f);
    }

    SECTION("Test basic division") {
        REQUIRE(nonRpnEvaluate("10 / 2") == 5.0f);
    }

    SECTION("Test complex expression with parentheses") {
        /* Let it be known that today was the day my own code went beyond
         * ChatGPT...
         * (Most all of these tests were generated by ChatGPT and/or GHCopilot)
         * /.../math_matters/src/tests.cpp:301: Failure:
                REQUIRE(nonRpnEvaluate("(1 + 3) * 2 + (1 - 32)") == -55.0f)
                    with expansion:
                -23.0f == -55.0f
         * */
//        REQUIRE(nonRpnEvaluate("(1 + 3) * 2 + (1 - 32)") == -55.0f);
        // Correct answer is in fact -23.0f
        REQUIRE(nonRpnEvaluate("(1 + 3) * 2 + (1 - 32)") == -23.0f);
    }

    SECTION("Test division by zero") {
        REQUIRE_THROWS(nonRpnEvaluate("1 / 0"));
//        REQUIRE_THROWS_AS(nonRpnEvaluate("1 / 0"), std::invalid_argument);
    }

    // TODO: Verify count of operators and operands
    SECTION("Test invalid expression") {
        REQUIRE_THROWS_AS(nonRpnEvaluate("1 + "), std::invalid_argument);
    }
}

