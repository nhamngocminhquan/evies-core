#include "space.cpp"
#include <iostream>
#include <vector>
#include <string>
using namespace Space;
int main() {
    Space::Space space;
    space.AddReview("Very bad, not good", 1);
    space.AddReview("Okay ish", 3.5);
    for (std::string i: space.GetReviews()) {
        std::cout << i << std::endl;
    }
}