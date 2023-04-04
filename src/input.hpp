//
// Created by Peter Vaiciulis on 3/2/23.
//

#ifndef MATH_MATTERS_INPUT_HPP
#define MATH_MATTERS_INPUT_HPP

#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <map>
#include <cmath>

#include "Stack.h"

/*
 * This file contains functions that check the validity of an equation,
 * and also the functions that parse the equation into a stack of symbols.
 */

namespace psv
{

using equation = std::string;

// Driving Functions
float nonRpnEvaluate(const equation& eq);
float operateBinary(float a, float b, char op);
float operateUnary(float a, char op);

// True if equation contains no invalid characters.
void checkValidCharacters(const equation& eq);

// True if scoping is valid. (Use of parentheses)

// True if operators are used correctly.
bool validOperator(char preceding, char succeeding);
void validOperators(const equation& eq);
void validScoping(const equation& eq);
bool isOperator(const char& c);
bool isUnary(char op);
// Eliminates whitespace from a string.
void eliminateWhiteSpace(equation& eq);

std::vector<char> parseStatement(const equation& eq);
float evaluateParsed(const equation& eq);

std::vector<const char*> operatorLocations(const equation& eq);



static const std::array<char, 8> valid_ops = {'+', '-', '*', '/', '^', 'n'};
static const std::array<char, 14> valid_operands = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', ' ', '(', ')'};
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
                throw std::invalid_argument("Unbalanced parentheses");
            } else {
                invalid_scope.pop();
            }
        }
    }
    if (!invalid_scope.isEmpty()) {
        throw std::invalid_argument("Unbalanced parentheses");
    }
}

bool validMinus(const char preceding, const char succeeding) {
    return isOperator(preceding) || isOperator(succeeding);
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
                throw std::invalid_argument("Invalid operator placement.");
        } else {
            const char left = *(op - 1);
            if(!validOperator(left, right))
                throw std::invalid_argument("Invalid operator placement.");
        }
    }
}

// TODO: Maybe add a way to save the states of the statement as it is evaluated...i.e. show steps
float nonRpnEvaluate(const equation& eq) {
    equation eq_copy = eq;

    // Use 'n' to represent unary minus
    for(auto & c : eq_copy) {
        if (c == '-' && (c == eq_copy[0] || isOperator(*(&c - 1)) && *(&c - 1) != ')'))
            c = 'n';
    }

    eliminateWhiteSpace(eq_copy);
    checkValidCharacters(eq_copy);
    validScoping(eq_copy);
    validOperators(eq_copy);

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
                char op = operators.pop();
                float b = output.pop();
                if(isUnary(op)){
                    output.place(operateUnary(b, op));
                    continue;
                }
                float a = output.pop();
                output.place(operateBinary(a, b, op));
            }
            operators.pop();
        } else { // is a binary operator
            while (!operators.isEmpty() && precedence.at(operators.top()) >= precedence.at(symbol)) {
                char op = operators.pop();
                float b = output.pop();
                if(isUnary(op)){
                    output.place(operateUnary(b, op));
                    continue;
                }
                float a = output.pop();
                output.place(operateBinary(a, b, op));
            }
            operators.place(symbol);
        }
    }
    if(!number_buffer.empty())
        output.place(std::stof(number_buffer));
    number_buffer.clear();
    // Utilize any remaining operators
    for (int i = 0; i < operators.size()+1; ++i) {
        char op = operators.pop();
        float b = output.pop();
        if(isUnary(op)){
            output.place(operateUnary(b, op));
            continue;
        }
        float a = output.pop();
        output.place(operateBinary(a, b, op));
    }
    // just count that the number of operands is one less than the number of operators except for unary minuses
    // Should always be true...?
    return output.pop();
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
                throw std::invalid_argument("Division by zero.");
            return a / b;
        case '^':
            return pow(a, b);
        default:
            throw std::invalid_argument("Invalid operator in parsed equation.");
    }
}
float operateUnary(float a, char op) {
    switch (op) {
        case 'n':
            return a * -1;
        default:
            throw std::invalid_argument("Invalid operator in parsed equation.");
    }
}

} // namespace psv

#endif //MATH_MATTERS_INPUT_HPP
