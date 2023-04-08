//
// Created by Peter Vaiciulis on 3/2/23.
//

#ifndef MATH_MATTERS_PROCESSOR_CPP
#define MATH_MATTERS_PROCESSOR_CPP
#include "MathProcessor.h"

namespace psv
{
const std::regex paren_no_op(R"(\(-?\d+\))");
const boost::regex unary_minus(R"(((?<=[*\/\+\-^(])-(?=[\d\(])|^-))");

// If I could do so simply, I would remove this right now
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


// Only used for binary operators
bool validOperator(const char preceding, const char succeeding) {
    if(isOperator(preceding) || preceding == '(')
        return false;
    if(isOperator(succeeding) && succeeding != 'n' || succeeding == ')')
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

    eliminateWhiteSpace(eq_copy);
    // Use 'n' to represent unary minus
    // -3+ 4 * 2 / ( 1 - -5 ) ^ 2 ^ 3
    eq_copy = boost::regex_replace(eq_copy, unary_minus, "n");

    // Quick scan for most issues before trying to evaluate
    checkValidCharacters(eq_copy);
    validScoping(eq_copy);
    validOperators(eq_copy);
    last_step = eq_copy;

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

// Gather and parse the last step (for each step) in the evaluation process
void parseLastStep(const std::string& target_exp, const std::string& target_reduced) {
    // Remove parentheses that do not contain operators
    std::smatch match;
    while (std::regex_search(last_step, match, paren_no_op)) {
        last_step.replace(last_step.find(match.str()), match.str().length(), match.str().substr(1, match.str().length() - 2));
    }
    // Replace target expression with target reduced
    last_step.replace(last_step.find(target_exp), target_exp.length(), target_reduced);

    last_step = last_step;
    // Replace special unary minus character ('n') with regular minus ('-')
    steps.push_back(last_step);
}

void cycleStack(Stack<float> &output, Stack<char> &operators) {
    // Every time we cycle the stack, we update the list of steps
    // Values are cast to int to avoid trailing zeros messing up find/replace
    std::string target;
    char op = operators.pop();
    float b = output.pop();
    if(isUnary(op)){
        output.place(operateUnary(b, op));
        target = "n" + std::to_string(static_cast<int>(b));
    }else{ // isBinary
        float a = output.pop();
        output.place(operateBinary(a, b, op));
        target = std::to_string(static_cast<int>(a)) + op + std::to_string(static_cast<int>(b));
    }
    parseLastStep(target, std::to_string(static_cast<int>(output.top())));
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
