//
// Created by Peter Vaiciulis on 4/7/23.
//
#include <string>
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

// Accessible Variables
    std::vector<const char *> operatorLocations(const equation &eq);
    extern std::vector<std::string> steps;
    extern std::vector<const char*> invalid_characters;
}

#endif //MATH_MATTERS_INPUT_H
