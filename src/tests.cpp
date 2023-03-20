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


TEST_CASE("Valid Characters", "[validCharacters] [sanitization]")
{
    using namespace psv;
    // equations with one or more invalid characters
    equation invalidChars = "1 + 1 - 3 * 4 / 5 ^ 6 @";
    equation invalidChars2 = "1 + 1 - 3 * 4 / 5 ^ 6 #";
    equation invalidChars3 = "1 + 1 - 3 * 4 / 5 ^ 6 $";

    // valid equation
    equation validEq = "1 + 1 - 3 * 4 / 5 ^ 6";

    REQUIRE(validCharacters(invalidChars) == false);
    REQUIRE(validCharacters(invalidChars2) == false);
    REQUIRE(validCharacters(invalidChars3) == false);

    REQUIRE(validCharacters(validEq) == true);
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
        REQUIRE(validScoping(validEq) == true);
        REQUIRE(validScoping(validEq2) == true);
        REQUIRE(validScoping(validEq3) == true);
        REQUIRE(validScoping(validEq4) == true);
        REQUIRE(validScoping(validEq5) == true);
        REQUIRE(validScoping(validEq6) == true);
    }

    SECTION("Invalid Scoping") {
        REQUIRE(validScoping(invalidEq) == false);
        REQUIRE(validScoping(invalidEq2) == false);
        REQUIRE(validScoping(invalidEq3) == false);
        REQUIRE(validScoping(invalidEq4) == false);
        REQUIRE(validScoping(invalidEq5) == false);
        REQUIRE(validScoping(invalidEq6) == false);
    }
}

TEST_CASE("Operator Usage", "[operatorUsage] [sanitization]")
{
    using namespace psv;
    SECTION("Valid Operator Usage") {
        equation validEq = "1+1-3*4/5^6";
        equation validEq2 = "1+(3)";
        equation validEq3 = "1+(3+1)";

        REQUIRE(all_validOperatorUsage(validEq) == true);
        REQUIRE(all_validOperatorUsage(validEq2) == true);
        REQUIRE(all_validOperatorUsage(validEq3) == true);
    }

    SECTION("Invalid Operator Usage"){
        equation invalidEq4 = "1+1-3*4/5^6+";
        equation invalidEq5 = "+1+1-3*4/5^6";
        equation invalidEq6 = "1+*1-3*4/5^6";

        // invalid parentheses
        equation invalidEq = "1+1-3*4/5^6*)";
        equation invalidEq2 = "1+1-3*)4/5^6*(";

        REQUIRE(all_validOperatorUsage(invalidEq4) == false);
        REQUIRE(all_validOperatorUsage(invalidEq5) == false);
        REQUIRE(all_validOperatorUsage(invalidEq6) == false);
        REQUIRE(all_validOperatorUsage(invalidEq) == false);
        REQUIRE(all_validOperatorUsage(invalidEq2) == false);
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
    REQUIRE(isOperator('(') == false);
    REQUIRE(isOperator(')') == false);
    REQUIRE(isOperator('1') == false);
    REQUIRE(isOperator('a') == false);
    REQUIRE(isOperator(' ') == false);
}

TEST_CASE("Negative Numbers", "[operatorUsage] [sanitization]")
{
    using namespace psv;
    // negative numbers should be allowed
    // we may consider that a - may precede a number a (, or another -
    equation eq = "1+1-3*-4/5^6";
    equation eq2 = "1+1-3*-4/5^-6";
    equation eq3 = "-1+1-3*-4/5";


    REQUIRE(all_validOperatorUsage(eq) == true);
    REQUIRE(all_validOperatorUsage(eq2) == true);
    REQUIRE(all_validOperatorUsage(eq3) == true);
}


TEST_CASE("Sanitize Equation", "[sanitization] [integration]"){
    // sanitizeEquation should return True if the equation is valid, and False if it is not
    // it should also modify the equation to remove whitespace
    using namespace psv;

    // valid expression
    equation eq = "1 + 1 - 3 * 4 / 5 ^ 6";

    // valid expressions with parentheses usage
    equation eq2 = "(1 + 1 - 3 * 4 / 5 ^ 6)";
    equation eq3 = "((1 + 1 - 3 * 4 / 5 ^ 6))";
    equation eq4 = "((1 + 1 - 3 * 4 / 5 ^ 6) + 1)";
    equation eq5 = "((1 + 1 - 3 * 4 / 5 ^ 6) + 1) - 1";

    // improper parentheses pairs
    equation eq6 = "1 + 1 - 3 * 4 / 5 ^ 6) + 1";
    equation eq7 = "1 + 1 - 3 * 4 / 5 ^ 6 + 1)";
    equation eq8 = "(1 + 1 - 3 * 4 / 5 ^ 6 + 1";
    equation eq9 = "1 + 1 - 3 * 4 / 5 ^ 6 + 1(";

    // improper operator usage
    equation eq10 = "1 + 1 - 3 * 4 / 5 ^ 6 +";
    equation eq11 = "*1 + 1 - 3 * 4 / 5 ^ 6 + 1";

    // improper operator locations
    equation eq12 = "1 + 1 - 3 * 4 / 5 ^ 6 +";
    equation eq13 = "1 + 1 - 3 * 4 / 5 ^ 6 *+ 1";

    REQUIRE(sanitizeEquation(eq) == true);
    REQUIRE(eq == "1+1-3*4/5^6");

    REQUIRE(sanitizeEquation(eq2) == true);
    REQUIRE(eq2 == "(1+1-3*4/5^6)");

    REQUIRE(sanitizeEquation(eq3) == true);
    REQUIRE(eq3 == "((1+1-3*4/5^6))");

    REQUIRE(sanitizeEquation(eq4) == true);
    REQUIRE(eq4 == "((1+1-3*4/5^6)+1)");

    REQUIRE(sanitizeEquation(eq5) == true);
    REQUIRE(eq5 == "((1+1-3*4/5^6)+1)-1");

    REQUIRE(sanitizeEquation(eq6) == false);
    REQUIRE(eq6 == "1+1-3*4/5^6)+1");

    REQUIRE(sanitizeEquation(eq7) == false);
    REQUIRE(eq7 == "1+1-3*4/5^6+1)");

    REQUIRE(sanitizeEquation(eq8) == false);
    REQUIRE(eq8 == "(1+1-3*4/5^6+1");

    REQUIRE(sanitizeEquation(eq9) == false);
    REQUIRE(eq9 == "1+1-3*4/5^6+1(");

    REQUIRE(sanitizeEquation(eq10) == false);
    REQUIRE(eq10 == "1+1-3*4/5^6+");

    REQUIRE(sanitizeEquation(eq11) == false);
    REQUIRE(eq11 == "*1+1-3*4/5^6+1");

    REQUIRE(sanitizeEquation(eq12) == false);
    REQUIRE(eq12 == "1+1-3*4/5^6+");

    REQUIRE(sanitizeEquation(eq13) == false);
    REQUIRE(eq13 == "1+1-3*4/5^6*+1");
}

TEST_CASE("Stack Class"){
using namespace psv;
    // stack class should be able to push and pop values
    // it should also be able to return the top value
    Stack<int> s;
    s.push(1);
    s.push(2);
    s.push(3);
    s.push(4);
    s.push(5);

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

    SECTION("Emplace"){
        // stack should be able to emplace custom objects
        struct car {
            int year;
            std::string make;
            std::string model;
            car(int y=0, std::string m="", std::string mo="") : year(std::move(y)), make(std::move(m)), model
            (std::move(mo)) {}

            ~car() = default;
        };
        Stack<car> s2;
        s2.emplace(2019, "Ford", "Mustang");
        s2.emplace(2018, "Ford", "F-150");
        s2.emplace(2017, "Ford", "Fusion");
        s2.emplace(2016, "Ford", "Focus");
        s2.emplace(2015, "Ford", "Explorer");

        REQUIRE(s2.top().year == 2015);
        REQUIRE(s2.top().make == "Ford");
        REQUIRE(s2.top().model == "Explorer");
        s2.pop();
        REQUIRE(s2.top().year == 2016);
        REQUIRE(s2.top().make == "Ford");
        REQUIRE(s2.top().model == "Focus");
        s2.pop();
        REQUIRE(s2.top().year == 2017);
        REQUIRE(s2.top().make == "Ford");
        REQUIRE(s2.top().model == "Fusion");
        s2.pop();
        REQUIRE(s2.top().year == 2018);
        REQUIRE(s2.top().make == "Ford");
        REQUIRE(s2.top().model == "F-150");
        s2.pop();
        REQUIRE(s2.top().year == 2019);
        REQUIRE(s2.top().make == "Ford");
        REQUIRE(s2.top().model == "Mustang");
        s2.pop();
        REQUIRE(s2.isEmpty() == true);
    }
}