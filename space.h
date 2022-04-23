#ifndef SPACE_H
#define SPACE_H
#include <string>
#include <vector>

namespace Space {
    // Class for space dimensions
    // Currently assume box-like spaces with length-width-height
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

    // Class for seatings
    class Seating {
    private:
        unsigned int numberOfSeats = 0;
        // For slanted seating (conference halls, theater, cinema, ...)
        bool slanted = false;
        // For surround seating configs (theater, music events, ...)
        bool surround = false;
        // For if the chairs are not cheap plastic
        bool comfy = true;
    public:
        Seating();
        Seating(unsigned int p_numberOfSeats, bool p_slanted = false, bool p_surround = false, bool p_comfy = false);

        // Setters using overload
        void IsSlanted(bool p_slanted) { slanted = p_slanted; }
        void IsSurround(bool p_surround) { surround = p_surround; }
        void IsComfy(bool p_comfy) { comfy = p_comfy; }
        void SetNumberOfSeats(unsigned int p_numberOfSeats);

        // Getters
        bool IsSlanted() { return slanted; }
        bool IsSurround() { return surround; }
        bool IsComfy() {return comfy; }
        unsigned int GetNumberOfSeats() { return numberOfSeats; }
    };

    // Class for each discrete space
    class Space {
    private:
        unsigned int ID;
        std::string name;
        Dimensions dims;
        unsigned int numberOfAvailable = 1;

        // Event-specific characteristics
        unsigned int numberOfSupportedPeople = 0;
        Seating seats;
        bool isOutdoor = false;
        bool isNaturalLight = false;

        // Constrained to 0 to 5
        float review = 0;
        bool beenReviewed = false;

        std::vector<std::string> tags;
    };

    // Class for groups of spaces in the same location (superspace)
    class SuperSpace {
    private:
        unsigned long ID;
        std::string name;
        double latitude, longitude;
        
    };
}

#endif