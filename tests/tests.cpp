//
// Created by Peter Vaiciulis on 3/2/23.
//
#include "catch2/catch_test_macros.hpp"
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
    const equation invalidChars = "1 + 1 - 3 * 4 / 5 ^ 6 @";
    const equation invalidChars2 = "1 + 1 - 3 * 4 / 5 ^ 6 #";
    const equation invalidChars3 = "1 + 1% - 3 * 4 / 5 ^ 6 $";

    // valid equation
    const equation validEq = "1 + 1 - 3 * 4 / 5 ^ 6";

//    REQUIRE_THROWS(checkValidCharacters(invalidChars));
//    REQUIRE(invalid_characters.size() == 1);
//    REQUIRE(*invalid_characters[0] == '@');
//
//    REQUIRE_THROWS(checkValidCharacters(invalidChars2));
//    REQUIRE(invalid_characters.size() == 1);
//    REQUIRE(*invalid_characters[0] == '#');

    REQUIRE_THROWS(checkValidCharacters(invalidChars3));
    REQUIRE(invalid_characters.size() == 2);
    REQUIRE(*invalid_characters[0] == '%');
    REQUIRE(*invalid_characters[1] == '$');

    REQUIRE_NOTHROW(checkValidCharacters(validEq));
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
    REQUIRE(isOperator('(') == false);
    REQUIRE(isOperator(')') == false);
    REQUIRE(isOperator('1') == false);
    REQUIRE(isOperator('a') == false);
    REQUIRE(isOperator(' ') == false);
}

/*
 * I have no idea why this test is failing...
 * I mean it likely has something to do with the emplace functionality, but debugging has been a pain.
 * It works when I run ONLY this test case, but when I run the entire test suite, it fails... UGH
 * */
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
        REQUIRE_THROWS(s.top(), "Stack is empty");
        REQUIRE_THROWS(s.pop(), "Stack is empty");
        REQUIRE_THROWS_AS(s.top(), std::out_of_range);
        REQUIRE_THROWS_AS(s.pop(), std::out_of_range);
    }

    SECTION("Custom Objects"){
        struct Car {
            int year;
            std::string make;
            std::string model;
            Car(int y, std::string m, std::string mo) : year(y), make(m), model(mo) {}
            Car() : year(0), make(""), model("") {}

        };
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
        struct Car {
            int year;
            std::string make;
            std::string model;
            Car(int y, std::string m, std::string mo) : year(y), make(m), model(mo) {}
            Car() : year(0), make(""), model("") {}

        };
        Stack<Car> car_stack2;
        car_stack2.emplace(2019, "Ford", "Mustang");
        car_stack2.emplace(2018, "Ford", "F-150");
        car_stack2.emplace(2017, "Ford", "Fusion");
        car_stack2.emplace(2016, "Ford", "Focus");
        car_stack2.emplace(2015, "Ford", "Explorer");

        REQUIRE(car_stack2.top().year == 2015);
        REQUIRE(car_stack2.top().make == "Ford");
        REQUIRE(car_stack2.top().model == "Explorer");
        car_stack2.pop();
        REQUIRE(car_stack2.top().year == 2016);
        REQUIRE(car_stack2.top().make == "Ford");
        REQUIRE(car_stack2.top().model == "Focus");
        car_stack2.pop();
        REQUIRE(car_stack2.top().year == 2017);
        REQUIRE(car_stack2.top().make == "Ford");
        REQUIRE(car_stack2.top().model == "Fusion");
        car_stack2.pop();
        REQUIRE(car_stack2.top().year == 2018);
        REQUIRE(car_stack2.top().make == "Ford");
        REQUIRE(car_stack2.top().model == "F-150");
        car_stack2.pop();
        REQUIRE(car_stack2.top().year == 2019);
        REQUIRE(car_stack2.top().make == "Ford");
        REQUIRE(car_stack2.top().model == "Mustang");
        car_stack2.pop();
        REQUIRE(car_stack2.isEmpty() == true);
    }
}

std::vector<char> as_vector(std::string s){
    std::vector<char> v;
    for(auto c : s){
        v.push_back(c);
    }
    return v;
}
// Totally unnecessary, but I want to get my code coverage up to 100% so here we are...
TEST_CASE("As Vector"){
    using namespace psv;
    SECTION("Basic"){
        REQUIRE(as_vector("12345") == std::vector<char>{'1', '2', '3', '4', '5'});
        REQUIRE(as_vector("abcde") == std::vector<char>{'a', 'b', 'c', 'd', 'e'});
        REQUIRE(as_vector("ABCDE") == std::vector<char>{'A', 'B', 'C', 'D', 'E'});
        REQUIRE(as_vector("12345abcdeABCDE") == std::vector<char>{'1', '2', '3', '4', '5', 'a', 'b', 'c', 'd', 'e', 'A', 'B', 'C', 'D', 'E'});
    }
}

// This tests the program's ability to evaluate statements
TEST_CASE("Non-RPN Evaluate"){
    using namespace psv;
    SECTION("Basic Operations"){
        SECTION("Addition") {
            REQUIRE(nonRpnEvaluate("1 + 2") == 3.0f);
        }
        SECTION("Subtraction") {
            REQUIRE(nonRpnEvaluate("5 - 3") == 2.0f);
        }
        SECTION("Multiplication") {
            REQUIRE(nonRpnEvaluate("2 * 3") == 6.0f);
        }
        SECTION("Division") {
            REQUIRE(nonRpnEvaluate("10 / 2") == 5.0f);
        }
        SECTION("Exponentiation") {
            REQUIRE(nonRpnEvaluate("2 ^ 3") == 8.0f);
            REQUIRE(nonRpnEvaluate("2 ^ 3 ^ 2") == 64.0f);
            REQUIRE(nonRpnEvaluate("2 ^ (3^2)") == 512.0f);
        }
        SECTION("Unary Minus") {
            REQUIRE(nonRpnEvaluate("-2+4") == 2.0f);
            REQUIRE(nonRpnEvaluate("-2+-4") == -6.0f);
            REQUIRE(nonRpnEvaluate("-2--4") == 2.0f);
        }
    }

    SECTION("Complex Expressions") {
        /* Let it be known that today was the day my own code went beyond
         * ChatGPT...
         * (Most all of these tests were generated by ChatGPT and/or GHCopilot)
         * /.../math_matters/src/tests.cpp:301: Failure:
                REQUIRE(nonRpnEvaluate("(1 + 3) * 2 + (1 - 32)") == -55.0f)
                    with expansion:
                -23.0f == -55.0f
         * */
//        REQUIRE(nonRpnEvaluate("(1 + 3) * 2 + (1 - 32)") == -55.0f);
        // Correct answer is in fact -23.0f, not -55.0f as ChatGPT would have you believe!!!
        REQUIRE(nonRpnEvaluate("(1 + 3) * 2 + (1 - 32)") == -23.0f);
        SECTION("Nested parentheses") {
            REQUIRE(nonRpnEvaluate("2 * (1 + (3 - 2) * (2 + (5 - 3)))") == 10.0f);
            REQUIRE(nonRpnEvaluate("((2 + 3) * 4 - 7) / (5 - 2) + 8 * (3 - 1)") == (20.0f + (1.0f / 3.0f)));
        }SECTION("Unbalanced parentheses"){
            REQUIRE_THROWS_AS(nonRpnEvaluate("(1 + 2)) * 3"), std::invalid_argument);
        }SECTION("Division by zero") {
            REQUIRE_THROWS_AS(nonRpnEvaluate("1 / 0"), std::invalid_argument);
        }

        SECTION("Invalid expression") {
            REQUIRE_THROWS_AS(nonRpnEvaluate("1 + "), std::invalid_argument);
            REQUIRE_THROWS_AS(nonRpnEvaluate("1 + 2 +"), std::invalid_argument);
        }

    }
}

