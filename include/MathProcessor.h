//
// Created by Peter Vaiciulis on 4/7/23.
//
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <map>
#include <cmath>
#include <regex>
#include <boost/regex.hpp>
#include "Stack.h"

#ifndef MATH_MATTERS_INPUT_H
#define MATH_MATTERS_INPUT_H

namespace psv {
    using equation = std::string;

// Primary Logic
    float nonRpnEvaluate(const equation &eq);

    void cycleStack(Stack<float> &output, Stack<char> &operators);

    float operateBinary(float a, float b, char op);

    float operateUnary(float a, char op);

// Parsing Functions
    void checkValidCharacters(const equation &eq);

    bool validOperator(char preceding, char succeeding);

    void validOperators(const equation &eq);

    void validScoping(const equation &eq);

    bool isOperator(const char &c);

    bool isUnary(char op);

    void eliminateWhiteSpace(equation &eq);

    void parseLastStep(const std::string& target_exp, const std::string& target_reduced);


// Accessible Variables
    extern std::vector<const char *> operatorLocations(const equation &eq);
    extern std::vector<std::string> steps;
    extern std::vector<const char*> invalid_characters;
    extern const std::regex paren_no_op;
    extern const boost::regex unary_minus;
}

#endif //MATH_MATTERS_INPUT_H
