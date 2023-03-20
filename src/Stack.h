//
// Created by Peter Vaiciulis on 3/11/23.
//

#ifndef MATH_MATTERS_STACK_CPP
#define MATH_MATTERS_STACK_CPP
#include <cstddef>

namespace psv
{

template<typename T>
class Stack {
public:
    Stack();
    ~Stack();

    // Modify
    void push(T element);

    template<typename... Args>
    void emplace(Args &&... args);

    T pop();

    // Query
    bool isEmpty();
    int size();
    T top();

private:
    T *_stack;
    int _size = 0;
    int _topIndex;
};

template<typename T>
Stack<T>::Stack() {
    _stack = new T[1];
    _topIndex = -1;
}

template<typename T>
Stack<T>::~Stack() {
    delete[] _stack;
}

template<typename T>
void Stack<T>::push(T element) {
    if (_topIndex == _size - 1) {
        T *temp = new T[_size * 2];
        std::memcpy(temp, _stack, sizeof(T) * _size);
        delete[] _stack;
        _stack = temp;
    }
    _stack[++_topIndex] = element;
    _size++;
}

template<typename T>
template<typename... Args>
void Stack<T>::emplace(Args &&... args) {
    if (_topIndex == _size - 1) {
        T *temp = new T[_size * 2];
        std::memcpy(temp, _stack, sizeof(T) * _size);
        delete[] _stack;
        _stack = temp;
    }
    _stack[++_topIndex] = T(std::forward<Args>(args)...);
    _size++;
}

template<typename T>
T Stack<T>::pop() {
    if (_topIndex == -1) {
        throw std::out_of_range("Stack is empty");
    }
    T temp = _stack[_topIndex--];
    _size--;
    return temp;
}

// Query
template<typename T>
bool Stack<T>::isEmpty() {
    return _topIndex == -1;
}

template<typename T>
int Stack<T>::size() {
    return _size;
}

template<typename T>
T Stack<T>::top() {
    return _stack[_topIndex];
}

} // namespace psv

#endif //MATH_MATTERS_STACK_CPP
