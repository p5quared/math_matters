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
#include "math.h"

#include "Stack.h"

/*
 * This file contains functions that check the validity of an equation,
 * and also the functions that parse the equation into a stack of symbols.
 */

namespace psv
{

using equation = std::string;

// True if equation contains no invalid characters.
bool checkValidCharacters(const equation& eq);

// True if scoping is valid. (Use of parentheses)
void validScoping(const equation& eq);

// True if operators are used correctly.
bool validMinus(char preceding, char succeeding);
bool validOperator(char preceding, char succeeding);
std::vector<const char*> operatorLocations(const equation& eq);
bool isOperator(const char& c);

// Eliminates whitespace from a string.
void eliminateWhiteSpace(equation& eq);

std::vector<char> parseStatement(const equation& eq);
float evaluateParsed(const equation& eq);


float nonRpnEvaluate(const equation& eq);
float operate(float a, float b, char op);

static const std::array<char, 8> valid_opers = {'+', '-', '*', '/', '^', 'n', '(', ')'};
static const std::array<char, 14> valid_operands = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', ' ', '(', ')'};
static const std::array<char, 19> all_valid = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.',
        '+', '-', '*', '/', '^', '(', ')', ' '
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
bool checkValidCharacters(const equation& eq) {
    invalid_characters.clear();
    for (auto const& character : eq) {
        if (std::find(all_valid.begin(), all_valid.end(), character) == all_valid.end()) {
            invalid_characters.push_back(&character);
        }
    }
    return invalid_characters.empty();
}

void validScoping(const equation & eq) {
    psv::Stack<char> invalid_scope;
    for (auto c : eq) {
        if (c == '(') {
            invalid_scope.place(c);
        } else if (c == ')') {
            if (invalid_scope.isEmpty()) {
                throw std::runtime_error("Invalid scoping.");
            } else {
                invalid_scope.pop();
            }
        }
    }
    if (!invalid_scope.isEmpty()) {
        throw std::runtime_error("Invalid scoping.");
    }
}

bool validMinus(const char preceding, const char succeeding) {
    if (isOperator(succeeding))
        return false;
}

bool validOperator(const char preceding, const char succeeding) {
    if (isOperator(preceding) || (isOperator(succeeding) && succeeding != '-'))
        return false;

    return true;
}

std::vector<const char*> operatorLocations(const equation & eq) {
    std::vector<const char*> operator_locations;

    // gather pointers to all operators
    const char* ptr = (&eq[0]);
    for(int i = 0; i < eq.size(); i++) {
        if (std::find(valid_opers.begin(), valid_opers.end(), *ptr) != valid_opers.end()) {
            operator_locations.push_back(ptr);
        }
        ptr++;
    }

    return operator_locations;
}

bool isOperator(const char& c) {
    if (std::find(valid_opers.begin(), valid_opers.end(), c) != valid_opers.end())
        return true;
    else
        return false;
}

void eliminateWhiteSpace(equation& eq) {
    eq.erase(remove_if(eq.begin(), eq.end(), isspace), eq.end());
}

/*
 * Attempts to parse a math statement (string) into an output vector of operands and operators.
 *
 *
 * @param eq The statement to be parsed.
 * @return An output vector of operands and operators.
 * */
std::vector<char> parseStatement(const equation& eq) {

    // Eliminate whitespace
    equation temp = eq;

    eliminateWhiteSpace(temp);
    // Check for valid characters
    if (!checkValidCharacters(temp)) {
        throw std::invalid_argument("Invalid characters in equation.");
    }

    validScoping(temp);

    // Search and replace all unary minuses with 'n'
    for(auto & c : temp) {
        if (c == '-' && (c == temp[0] || isOperator(*(&c-1)) && *(&c-1) != ')'))
            c = 'n';
    }

    // Yes, not using else if here is intentional.
    // It is much more readable at the cost fo an extra 2 comparisons per character.
    psv::Stack<char> operators;
    std::vector<char> output;
    for(auto& c : temp) {
        if(!isOperator(c)){
            output.push_back(c);
        }else if (operators.isEmpty() || c == '('){
            operators.place(c);
        }else if (c == ')') {
            while (operators.top() != '(') {
                output.push_back(operators.pop());
            }
            operators.pop();
        } else { // is a non parenthesis operator
            while (!operators.isEmpty() && precedence.at(operators.top()) >= precedence.at(c)) {
                output.push_back(operators.pop());
            }
            operators.place(c);
        }
    }

    while (!operators.isEmpty())
        output.push_back(operators.pop());

    // just count that the number of operands is one less than the number of operators except for unary minuses
    return output;
}

float evaluateParsed(const equation& eq) {
    std::vector<float> operands;
    std::vector<char> parsed = parseStatement(eq);

    for(auto & c : parsed) {
        if (isOperator(c)) {
            if (c == 'n') {
                operands.back() *= -1;
                continue;
            }
            float b = operands.back();
            operands.pop_back();
            float a = operands.back();
            operands.pop_back();
            switch (c) {
                case '+':
                    operands.push_back(a + b);
                    break;
                case '-':
                    operands.push_back(a - b);
                    break;
                case '*':
                    operands.push_back(a * b);
                    break;
                case '/':
                    operands.push_back(a / b);
                    break;
                case '^':
                    operands.push_back(pow(a, b));
                    break;
                default:
                    throw std::invalid_argument("Invalid operator in parsed equation.");
            }
        } else {
            std::string temp(1, c);
            operands.push_back(std::stof(temp));
        }
    }
    return operands.back();
}

float nonRpnEvaluate(const equation& eq) {
    // Eliminate whitespace
    equation eq_copy = eq;

    eliminateWhiteSpace(eq_copy);
    // Check for valid characters
    if (!checkValidCharacters(eq_copy)) {
        throw std::invalid_argument("Invalid characters in equation.");
    }

    validScoping(eq_copy);

    // Search and replace all unary minuses with 'n'
    for(auto & c : eq_copy) {
        if (c == '-' && (c == eq_copy[0] || isOperator(*(&c - 1)) && *(&c - 1) != ')'))
            c = 'n';
    }




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
                float b = output.pop();
                float a = output.pop();
                char op = operators.pop();
                output.place(operate(a, b, op));
            }
            operators.pop();
        } else { // is a non parenthesis operator
            while (!operators.isEmpty() && precedence.at(operators.top()) >= precedence.at(symbol)) {
                float b = output.pop();
                float a = output.pop();
                char op = operators.pop();
                output.place(operate(a, b, op));
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
        float a = output.pop();
        output.place(operate(a, b, op));
    }
    // just count that the number of operands is one less than the number of operators except for unary minuses
    // Should always be true...?
    return output.pop();
}

float operate(float a, float b, char op){
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
} // namespace psv

#endif //MATH_MATTERS_INPUT_HPP
