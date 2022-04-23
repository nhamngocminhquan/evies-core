#include "space.cpp"
#include <iostream>
#include <vector>
#include <string>
int main() {
    Space::Space space;
    space.AddReview("Very bad, not good", 1);
    space.AddReview("Okay ish", 3.5);
    for (std::string i: space.GetReviews()) {
        std::cout << i << std::endl;
    }
}

std::vector<Space::Space> GetRandomizedSpaces(int n){
    std::vector<Space::Space> returnObjects;
    std::srand(time(NULL));
    for (int i = 0; i < n; i++) {
        returnObjects.push_back(
            Space::Space(
                i,                                      // ID
                "Space" + std::to_string(i),           // name
                Space::Dimensions(
                    rand() % 90 + 10,                   // length
                    rand() % 45 + 5,                    // width
                    rand() % 10 + 2                     // height
                ),
                Space::Seating(
                    rand() % 490 + 10,                  // number of seats
                    (bool)(rand() % 2),                 // slanted?
                    (bool)(rand() % 2),                 // surround?
                    (bool)(rand() % 2)                  // comfy?
                ),
                rand() % 990 + 10,                      // number of people
                rand() % 9900 + 100,                    // price
                (bool)(rand() % 2),                     // outdoor?
                (bool)(rand() % 2),                     // catering?
                (bool)(rand() % 2),                     // naturalLight?
                (bool)(rand() % 2),                     // sound?
                (bool)(rand() % 2),                     // projector?
                (bool)(rand() % 2)                      // camera?
            )
        );
        returnObjects.back().AddReview("Very bad, not good", rand() % 5);
        returnObjects.back().AddReview("Okay ish", rand() % 5);
    }
    return returnObjects;
}