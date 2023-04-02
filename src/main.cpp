#include <iostream>
#include <iomanip>
#include <vector>
#include "input.hpp"


int main() {
    // more complicated statement
    std::string eq2 = "1+1-(3*4)/5";
    std::cout << "1+1-(3*4)/5= " << std::fixed << std::setprecision(2)
    << psv::nonRpnEvaluate(eq2) << std::endl;

    std::vector<char> parsed2 = psv::parseStatement(eq2);
    // convert vector to string
    std::string parsed2_str(parsed2.begin(), parsed2.end());


    return 0;
}
