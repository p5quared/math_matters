//
// Created by Peter Vaiciulis on 3/9/23.
//

#ifndef MATH_MATTERS_SYMBOL_H
#define MATH_MATTERS_SYMBOL_H

#include <string>
#include <vector>
#include "Stack.h"

namespace psv {

class MathCore {
public:
    virtual ~MathCore() = default;
    virtual float evaluate() = 0;
};

class Operand : public MathCore {
public:
    Operand(std::string operand);
    float evaluate() override;
    ~Operand() override;
private:
    float _operand;
};

class Operation : public MathCore {
public:
    virtual float evaluate() = 0;
    int _precedence;

    float _operand;
};


class BinaryOperation : public Operation {
public:
    std::pair<float, float> _operands;
};


// For now, the only unary operation is negation.
class UnaryMinus : public Operation {
public:
    UnaryMinus(std::string operand);
    float _operand;
};

class Minus : public BinaryOperation {
public:
    Minus(std::string operand);
    float evaluate() override;
    ~Minus() override;
private:
    int _precedence = 3;
};

// Scope is a special case of Operation. It is a container for other Operations.
// It is used to group and evaluate operations in parentheses, in order.
class Scope : public Operation {
public:
    Scope(std::string eq);
    float evaluate() override;
    ~Scope() override;
private:
    std::vector<Operation*> _children;
    int _precedence = 0;
};


}
#endif //MATH_MATTERS_SYMBOL_H
