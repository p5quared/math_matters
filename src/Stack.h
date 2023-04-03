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
    Stack(const Stack &other);
    ~Stack();

    // Modify
    void place(T element);

    template<typename... Args>
    void emplace(Args &&... args);

    T pop();
    void clear();

    // Query
    bool isEmpty();
    int size();
    T top();

private:
    int _size;
    int _topIndex;
    T *_stack;
};

template<typename T>
Stack<T>::Stack() : _size(0), _topIndex(-1){
    _stack = new T[1];
}

template<typename T>
Stack<T>::Stack(const Stack& other) {
    _size = other._size;
    _topIndex = other._topIndex;
    _stack = new T[_size];
    std::memcpy(_stack, other._stack, sizeof(T) * _size);
}


template<typename T>
Stack<T>::~Stack() {
    delete[] _stack;
}

template<typename T>
void Stack<T>::place(T element) {
    if (_topIndex == _size - 1) {
        T *temp = new T[_size * 2];
        std::memcpy(temp, _stack, sizeof(T) * _size);
        delete[] _stack;
        _stack = temp;
    }
    _stack[++_topIndex] = element;
    _size++;
}

// I was reading about perfect forwarding and I wanted to try it out.
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
    if (_topIndex <= -1) {
        throw std::out_of_range("Stack is empty");
    }
    T temp = _stack[_topIndex--];
    _size--;
    return temp;
}

template<typename T>
void Stack<T>::clear() {
    delete[] _stack;
    _stack = new T[1];
    _size = 0;
    _topIndex = -1;
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
    if (_size <= 0) {
        throw std::out_of_range("Stack is empty");
    }
    return _stack[_topIndex];
}

} // namespace psv

#endif //MATH_MATTERS_STACK_CPP
