#include "space.h"
#include <vector>
#include <string>
using namespace Space;

void Dimensions::UpdateAAR() {
    area = length * width;
    // Check for zero before division
    if (width * length != 0) {
        aspectRatio = (length > width) ? (length / width) : (width / length); 
    } else aspectRatio = 0;
}
Dimensions::Dimensions() {
    length = 0;
    width = 0;
    height = 0;
    area = 0;
    aspectRatio = 0;
}
Dimensions::Dimensions(float p_length, float p_width, float p_height) {
    length = p_length;
    width = p_width;
    height = p_height;
    UpdateAAR();
}
void Dimensions::SetLength(float p_length) {
    length = p_length;
    UpdateAAR();
}
void Dimensions::SetWidth(float p_width) {
    width = p_width;
    UpdateAAR();
}
void Dimensions::SetHeight(float p_height) {
    height = p_height;
}
void Dimensions::SetDimensions(float p_length, float p_width, float p_height) {
    length = p_length;
    width = p_width;
    height = p_height;
    UpdateAAR();
}