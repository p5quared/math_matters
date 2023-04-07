//
// Created by Peter Vaiciulis on 3/2/23.
//

#ifndef MATH_MATTERS_PROCESSOR_CPP
#define MATH_MATTERS_PROCESSOR_CPP

#include <vector>
#include <array>
#include <algorithm>
#include <map>
#include <cmath>
#include <regex>
#include "Stack.h"
#include "MathProcessor.h"

/*
 * This file contains functions that check the validity of an equation,
 * and also the functions that parse the equation into a stack of symbols.
 */


namespace psv
{
using equation = std::string;
static const std::array<char, 8> valid_ops = {'+', '-', '*', '/', '^', 'n'};
static const std::array<char, 20> all_valid = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.',
        '+', '-', '*', '/', '^', '(', ')', ' ', 'n'
};
static const std::map<char, int> precedence = {
        {'^', 5},
        {'n', 4},
        {'*', 3},
        {'/', 3},
        {'+', 2},
        {'-', 2},
        {'(', 1}
};


std::vector<const char*> invalid_characters;

// Error Messages
static const std::string invalid_characters_err = "Invalid Characters in Statement: Check your statement and ensure that"\
                                                  "it only contains numbers, operators, and parentheses. Valid operators"\
                                                  "include: +, -, *, /, ^";
static const std::string parentheses_err = "Unbalanced Parentheses: Check your statement and ensure that every '(' has"\
                                       "a corresponding ')' and vice versa.";
static const std::string operator_err = "Invalid Operator Placement: Check your statement and ensure that operators are"\
                                "not placed next to each other (exception for -), or next to a parenthesis.";
static const std::string zero_division_err = "Zero Division Error: Check your statement and ensure that you are not"\
                                             "dividing by zero.";

void checkValidCharacters(const equation& eq) {
    invalid_characters.clear();
    for (auto const& character : eq) {
        if (std::find(all_valid.begin(), all_valid.end(), character) == all_valid.end()) {
            invalid_characters.push_back(&character);
        }
    }
    if (!invalid_characters.empty()) {
        throw std::invalid_argument("Invalid characters in equation");
    }
}

void validScoping(const equation & eq) {
    psv::Stack<char> invalid_scope;
    for (auto c : eq) {
        if (c == '(') {
            invalid_scope.place(c);
        } else if (c == ')') {
            if (invalid_scope.isEmpty()) {
                throw std::invalid_argument(parentheses_err);
            } else {
                invalid_scope.pop();
            }
        }
    }
    if (!invalid_scope.isEmpty()) {
        throw std::invalid_argument(parentheses_err);
    }
}


bool validOperator(const char preceding, const char succeeding) {
    if (isOperator(preceding) || (isOperator(succeeding) && succeeding != 'n'))
        return false;

    return true;
}

std::vector<const char*> operatorLocations(const equation & eq) {
    std::vector<const char*> operator_locations;

    // gather pointers to all operators
    const char* ptr = (&eq[0]);
    for(int i = 0; i < eq.size(); i++) {
        if (std::find(valid_ops.begin(), valid_ops.end(), *ptr) != valid_ops.end()) {
            operator_locations.push_back(ptr);
        }
        ptr++;
    }

    return operator_locations;
}

bool isOperator(const char& c) {
    if (std::find(valid_ops.begin(), valid_ops.end(), c) != valid_ops.end())
        return true;
    else
        return false;
}

void eliminateWhiteSpace(equation& eq) {
    eq.erase(remove_if(eq.begin(), eq.end(), isspace), eq.end());
}

void validOperators(const equation& eq) {
    std::vector<const char*> operator_locations = operatorLocations(eq);
    for (auto const& op : operator_locations) {
        const char right = *(op + 1);
        if(*op == 'n'){
            if(!isdigit(right) && right != '(')
                throw std::invalid_argument(operator_err);
        } else {
            const char left = *(op - 1);
            if(!validOperator(left, right))
                throw std::invalid_argument(operator_err);
        }
    }
}

// Used to handle steps in the evaluation process
static std::string last_step;
std::vector<std::string> steps;

float nonRpnEvaluate(const equation& eq) {
    steps.clear();
    equation eq_copy = eq;

    // Use 'n' to represent unary minus
    for(auto & c : eq_copy) {
        if (c == '-' && (c == eq_copy[0] || isOperator(*(&c - 1)) && *(&c - 1) != ')'))
            c = 'n';
    }

    // Quick scan for most issues before trying to evaluate
    eliminateWhiteSpace(eq_copy);
    checkValidCharacters(eq_copy);
    validScoping(eq_copy);
    validOperators(eq_copy);
    last_step = eq_copy;
    steps.push_back(std::regex_replace(last_step, std::regex("n"), "-"));

    psv::Stack<float> output;
    psv::Stack<char> operators;
    std::string number_buffer;
    for(auto& symbol : eq_copy) {
        if(isdigit(symbol)){
            number_buffer += symbol;
            continue;
        } else {
            if(!number_buffer.empty()){
                output.place(std::stof(number_buffer));
                number_buffer.clear();
            }
        }
        // => symbol is operator or parenthesis
        if(operators.isEmpty() || symbol == '('){
            operators.place(symbol);
        } else if (symbol == ')') {
            while (operators.top() != '(') {
                cycleStack(output, operators);
            }
            operators.pop();
        } else { // is a binary operator
            while (!operators.isEmpty() && precedence.at(operators.top()) >= precedence.at(symbol)) {
                cycleStack(output, operators);
            }
            operators.place(symbol);
        }
    }
    if(!number_buffer.empty())
        output.place(std::stof(number_buffer));
    number_buffer.clear();
    // Utilize any remaining operators
    for (int i = 0; i < operators.size()+1; ++i) {
        cycleStack(output, operators);
    }
    return output.pop();
}

void parseLastStep(std::string step, const std::string& target_exp, const std::string& target_reduced) {
//    std::string target;
//    std::string replacement;
//    for (auto const& op : valid_ops) {
//        if (op == 'n') {
//            target = "n";
//            replacement = "-";
//        } else {
//            target = std::string(1, op);
//            replacement = std::string(1, op) + " ";
//        }
//        step.replace(step.find(target), target.length(), replacement);
//    }

    // Remove parentheses that do not contain operators
    std::regex re(R"(\(-?\d+\))");
    std::smatch match;
    while (std::regex_search(step, match, re)) {
        step.replace(step.find(match.str()), match.str().length(), match.str().substr(1, match.str().length()-2));
    }
    // Replace target expression with target reduced
    step.replace(step.find(target_exp), target_exp.length(), target_reduced);

    last_step = step;
    // Replace special unary minus character ('n') with regular minus ('-')
    step = std::regex_replace(step, std::regex("n"), "-");
    steps.push_back(step);
}

void cycleStack(Stack<float> &output, Stack<char> &operators) {
    // Every time we cycle the stack, we update the list of steps
    // Values are cast to int to avoid trailing zeros messing up find/replace
    std::string target;
    char op = operators.pop();
    float b = output.pop();
    if(isUnary(op)){
        output.place(operateUnary(b, op));
        target = op + std::to_string(static_cast<int>(b));
    }else{ // isBinary
        float a = output.pop();
        output.place(operateBinary(a, b, op));
        target = std::to_string(static_cast<int>(a)) + op + std::to_string(static_cast<int>(b));
    }
    // TODO: Clean this up: I think we can get rid of target and just use target_reduced/step_reduced
    parseLastStep(last_step, target, std::to_string(static_cast<int>(output.top())));
}

bool isUnary(char op) {
    return op == 'n';
}

float operateBinary(float a, float b, char op){
    switch (op) {
        case '+':
            return a + b;
        case '-':
            return a - b;
        case '*':
            return a * b;
        case '/':
            if (b == 0)
                throw std::invalid_argument(zero_division_err);
            return a / b;
        case '^':
            return pow(a, b);
        default:
            throw std::invalid_argument(invalid_characters_err);
    }
}
float operateUnary(float a, char op) {
    if(op != 'n'){
        throw std::invalid_argument(invalid_characters_err);
    } else { // only unary operator is negative for now
        return a * -1;
    }
}

} // namespace psv

#endif //MATH_MATTERS_PROCESSOR_CPP
