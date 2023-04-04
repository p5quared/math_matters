# Math Matters
A project dealing in _matters mathematical_.

## Build
This project uses [CMake](https://cmake.org/) to simplify the build process. To build the project, run the following 
commands:
```bash
    mkdir build
    cd build
    cmake ..
    make
```
### Targets
The following targets are available:
* `math_matters` - The main executable.
* `tests` - The test executable.

### Dependencies
This project depends on the following libraries:
* [Catch2](https://github.com/catchorg/Catch2) - A modern, C++-native, header-only, test framework for unit-tests, TDD and BDD.
* [FTXUI](https://github.com/ArthurSonzogni/FTXUI) - A C++ library to build terminal user interfaces.


## Usage
### Math Matters
Within the main executable, users can enter a mathematical expression in infix notation and have it evaluated.
At the moment, only integer values are supported.

Valid operators include:
* `+` - Addition
* `-` - Subtraction (AND unary negation)
* `*` - Multiplication
* `/` - Division
* `^` - Exponentiation

You may also use parentheses to group expressions.
For example, 3 + 4 * 2 / ( 1 - 5 ) ^ 2 ^ 3 would evaluate to -3.00.

### Tests
The test executable is used to run unit tests for the project.
At the moment, 93% of the code is covered by unit tests.
