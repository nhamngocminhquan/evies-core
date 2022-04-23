#ifndef SPACE_H
#define SPACE_H
#include <string>
#include <vector>

namespace Space {
    // Currently assume box-like spaces with length-width-height
    // Can be extended with class to make better
    class Dimensions {
    private:
        float length, width, height;
        float area, aspectRatio;
        void UpdateAAR();
    public:
        Dimensions();
        Dimensions(float p_length, float p_width, float p_height);

        // Setters
        void SetLength(float p_length);
        void SetWidth(float p_width);
        void SetHeight(float p_height);
        void SetDimensions(float p_length, float p_width, float p_height);
        
        // Getters
        float GetLength() { return length; }
        float GetWidth() { return width; }
        float GetHeight() { return height; }
        float GetArea() { return area; }
        float GetAspectRatio() { return aspectRatio; }
    };
    class Space {
    private:
        __uint16_t ID;
        std::string name;

        unsigned int tmp;
    };
}

#endif