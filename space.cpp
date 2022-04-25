#include <string>
#include <vector>
#include <ctime>
#include <cmath>

// Bitwise helpers
// https://stackoverflow.com/questions/62689/bitwise-indexing-in-c
#define GetBit(var, bit) ((var & (1 << bit)) != 0) // Returns true / false if bit is set
#define SetBit(var, bit) (var |= (1 << bit))
#define ClearBit(var, bit) (var &= ~(1 << bit))
#define FlipBit(var, bit) (var ^= (1 << bit))

namespace Space {
    // Class for space dimensions
    // .. Currently assume box-like spaces with length-width-height
    class Dimensions {
    private:
        float length = 0, width = 0, height = 0;
        float area = 0, aspectRatio = 0;
        void UpdateAAR() {
            area = length * width;
            // Check for zero before division
            if (width * length != 0) {
                aspectRatio = (length > width) ? (length / width) : (width / length); 
            } else aspectRatio = 0;
        }
    public:
        Dimensions() {
            length = 0;
            width = 0;
            height = 0;
            area = 0;
            aspectRatio = 0;
        }
        Dimensions(float p_length, float p_width, float p_height) {
            length = p_length;
            width = p_width;
            height = p_height;
            UpdateAAR();
        }
        // Setters
        void SetLength(float p_length) {
            length = p_length;
            UpdateAAR();
        }
        void SetWidth(float p_width) {
            width = p_width;
            UpdateAAR();
        }
        void SetHeight(float p_height) {
            height = p_height;
        }
        void SetDimensions(float p_length, float p_width, float p_height) {
            length = p_length;
            width = p_width;
            height = p_height;
            UpdateAAR();
        }
        
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
        Seating(){}
        Seating(unsigned int p_numberOfSeats, bool p_slanted = false, bool p_surround = false, bool p_comfy = false) {
            numberOfSeats = p_numberOfSeats;
            slanted = p_slanted;
            surround = p_surround;
            comfy = p_comfy;
        }
        // Setters
        void SetNumberOfSeats(unsigned int p_numberOfSeats) { numberOfSeats = p_numberOfSeats; }
        // Setters using overload
        void IsSlanted(bool p_slanted) { slanted = p_slanted; }
        void IsSurround(bool p_surround) { surround = p_surround; }
        void IsComfy(bool p_comfy) { comfy = p_comfy; }

        // Getters
        bool IsSlanted() { return slanted; }
        bool IsSurround() { return surround; }
        bool IsComfy() {return comfy; }
        unsigned int GetNumberOfSeats() { return numberOfSeats; }
    };

    // Class for available times
    // .. Assume accomodative spaces: working all day
    class Time {
    private:
        time_t originTime;
        // Using a list of unsigned long long integers 
        // .. to keep track of allocated time
        // .. Each bit corresponds to 1 hour
        // .. More long longs added based on scheduler
        // .. 64 bits per each corresponding to 2.67 days
        std::vector<unsigned long long> times;
        // Price per hour
        double dirhamsPerHour = 0;
    public:
        Time() {
            originTime = time(NULL);
            dirhamsPerHour = 0;
        }
        Time(double p_dirhamsPerHour){
            originTime = time(NULL);
            // Round up to next hour
            originTime += (86400 - originTime % 86400);
            dirhamsPerHour = p_dirhamsPerHour;
        }
        Time(double p_dirhamsPerHour, const time_t& p_originTime) {
            originTime = p_originTime + (86400 - p_originTime % 86400);
            dirhamsPerHour = p_dirhamsPerHour;
        }
        // Setters
        void SetDirhamsPerHour(double p_dirhamsPerHour) { dirhamsPerHour = p_dirhamsPerHour; }
        // Getters
        double GetDirhamsPerHour() { return dirhamsPerHour; }
        time_t GetOriginTime() { return originTime; }
        std::vector<unsigned long long> GetTimes() { return times; }

        // Function to reserve
        // .. param price to return the price
        bool AddReservation(const time_t& p_startTime, const time_t& p_endTime, double& price) {
            // Initialize price
            price = 0;
            // Get hours difference between startTime, endTime and originTime
            // .. Hour is tracked from beginning o'clock -> floor is used here
            unsigned long startHours = (unsigned long)std::floor(std::difftime(p_startTime, originTime) / (60 * 60));
            unsigned long endHours = (unsigned long)std::floor(std::difftime(p_endTime, originTime) / (60 * 60));
            // Invalid reservation
            if (endHours < startHours or startHours < 0) return false;
            if (times.size() < endHours / 64)
                while (times.size() <= endHours / 64) times.push_back(0);
            // Check if any hour in the reservation is booked
            if (startHours / 64 == endHours / 64) {
                for (unsigned long i = startHours % 64; i <= endHours % 64; i++) 
                    if GetBit(times[startHours / 64], i)
                        // Time is occupied
                        return false;
            } else {
                for (unsigned long i = startHours % 64; i < 64; i++) 
                    if GetBit(times[startHours / 64], i)
                        // Time is occupied
                        return false;
                unsigned long j = startHours / 64;
                while (j != endHours / 64) {
                    for (unsigned long i = 0; i < 64; i++) 
                        if GetBit(times[startHours / 64], i)
                            // Time is occupied
                            return false;
                    j++;
                }
                for (unsigned long i = 0; i <= endHours % 64; i++) 
                    if GetBit(times[endHours / 64], i)
                        // Time is occupied
                        return false;
            }
            // If not, proceed to select the hours
            if (startHours / 64 == endHours / 64) {
                for (unsigned long i = startHours % 64; i <= endHours % 64; i++) 
                    SetBit(times[startHours / 64], i);
            } else {
                for (unsigned long i = startHours % 64; i < 64; i++) 
                    SetBit(times[startHours / 64], i);
                unsigned long j = startHours / 64;
                while (j != endHours / 64) {
                    times[startHours / 64] = ~0;
                    j++;
                }
                for (unsigned long i = 0; i <= endHours % 64; i++) 
                    SetBit(times[endHours / 64], i);
            }
            price = dirhamsPerHour * (endHours - startHours + 1);
        }
        // Function to remove reservations
        bool RemoveReservation(const time_t& p_startTime, const time_t& p_endTime) {
            // Get hours difference between startTime, endTime and originTime
            // .. Hour is tracked from beginning o'clock -> floor is used here
            unsigned long startHours = (unsigned long)std::floor(std::difftime(p_startTime, originTime) / (60 * 60));
            unsigned long endHours = (unsigned long)std::floor(std::difftime(p_endTime, originTime) / (60 * 60));
            // Invalid reservation
            if (endHours < startHours or startHours < 0) return false;
            if (times.size() < endHours / 64)
                while (times.size() <= endHours / 64) times.push_back(0);
            // Directly clear the hours
            if (startHours / 64 == endHours / 64) {
                for (unsigned long i = startHours % 64; i <= endHours % 64; i++) 
                    ClearBit(times[startHours / 64], i);
            } else {
                for (unsigned long i = startHours % 64; i < 64; i++) 
                    ClearBit(times[startHours / 64], i);
                unsigned long j = startHours / 64;
                while (j != endHours / 64) {
                    times[startHours / 64] = 0;
                    j++;
                }
                for (unsigned long i = 0; i <= endHours % 64; i++) 
                    ClearBit(times[endHours / 64], i);
            }
        }
    };

    // Class for each discrete space
    class Space {
    private:
        unsigned int ID;
        std::string name;
    public:
        Dimensions dims;
    private:

        // Event-specific characteristics
        // .. Accomodation
        unsigned int numberOfPeople = 0;
    public:
        Seating seats;
    private:
        bool outdoor = false;
        bool catering = false;

        // .. Design
        // .. .. Lighting
        bool naturalLight = false;
        bool artificialLight = true;
        bool projector = true;
        // .. .. Sound system
        bool sound = true;
        // .. .. Camera system
        bool cameras = true;

        // .. Available times
        double dirhamsPerHour;
    public:
        Time timer;
    private:

        // For reviews
        // .. Constrained to 0 to 5
        float score = 0;
        unsigned int numberOfReviews = 0;
        bool reviewed = false;
        std::vector<std::string> reviews;

        // Miscellaneous tags
        std::vector<std::string> tags;
    public:
        Space() {
            ID = 0;
            name = "";
            dims = Dimensions();
            seats = Seating();
            timer = Time();
        }
        Space(unsigned int p_ID,
            std::string p_name,
            const Dimensions& p_dims,
            const Seating& p_seats,
            unsigned int p_numberOfPeople,
            double p_dirhamsPerHour,

            // Optional
            bool p_outdoor = false,
            bool p_catering = false,
            bool p_naturalLight = false,
            bool p_artificialLight = true,
            bool p_projector = true,
            bool p_sound = true,
            bool p_cameras = true,
            float p_score = 0) {
            
            // Manual work
            ID = p_ID;
            name = p_name;
            // Use built-in copy constructor
            dims = p_dims;
            seats = p_seats;
            numberOfPeople = p_numberOfPeople;
            dirhamsPerHour = p_dirhamsPerHour;
            // Set current time as origin time
            timer = Time(p_dirhamsPerHour);
            outdoor = p_outdoor;
            catering = p_catering;
            naturalLight = p_naturalLight;
            artificialLight = p_artificialLight;
            projector = p_projector;
            sound = p_sound;
            cameras = p_cameras;
            score = p_score;
        }
        
        // Setters
        void Rename(const std::string& p_name) {
            name = p_name;
        }
        void SetID(unsigned int p_ID) {
            ID = p_ID;
        }
        void SetNumberOfPeople(int p_numberOfPeople) {
            numberOfPeople = p_numberOfPeople;
        }
        void AddReview(const std::string& p_review, float p_score) {
            reviewed = true;
            reviews.push_back(p_review);
            score = (score * numberOfReviews + p_score) / (++numberOfReviews);
        }
        // Setters using overload
        void IsOutdoor(bool p_outdoor) { outdoor = p_outdoor; }
        void IsCatering(bool p_catering) { catering = p_catering; }
        void IsNaturalLight(bool p_naturalLight) { naturalLight = p_naturalLight; }
        void IsArtificialLight(bool p_artificialLight) { artificialLight = p_artificialLight; }
        void IsProjector(bool p_projector) { projector = p_projector; }
        void IsSound(bool p_sound) { sound = p_sound; }
        void IsCameras(bool p_cameras) { cameras = p_cameras; }

        // Getters
        std::string GetName() { return name; }
        unsigned int GetID() { return ID; }
        int GetNumberOfPeople() { return numberOfPeople; }
        float GetReviewScore() { return score; }
        std::vector<std::string> GetReviews() { return reviews; }
        unsigned int GetNumberOfReviews() { return numberOfReviews; }
        bool IsOutdoor() { return outdoor; }
        bool IsCatering() { return catering; }
        bool IsNaturalLight() { return naturalLight; }
        bool IsArtificialLight() { return artificialLight; }
        bool IsProjector() { return projector; }
        bool IsSound() { return sound; }
        bool IsCameras() { return cameras; }
        bool IsReviewed() { return reviewed; }
    };

    // Class for groups of spaces in the same location (superspace)
    // .. Managed by space-managers
    class SuperSpace {
    private:
        unsigned long ID;
        std::string name;
        double latitude, longitude;
        
        // Spaces available in superspace
        std::vector<Space> spaces;
        
        // Internal spaces characteristics
        unsigned long maxNumberOfPeople = 0;
        // .. Portion of full capacity space can host parking
        float parkingCapacity = 1.0;
    public:
        SuperSpace(){}
        SuperSpace(unsigned long p_ID, const std::string& p_name, double p_longitude, double p_latitude) {
            ID = p_ID;
            name = p_name;
            longitude = p_longitude;
            latitude = p_latitude;
        }
    };
}