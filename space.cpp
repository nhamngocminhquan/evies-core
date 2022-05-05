#include <string>
#include <vector>
#include <ctime>
#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>

// JSON library courtesy of:
// https://github.com/nlohmann/json
#include "json.hpp"
namespace nljs = nlohmann;
// Random hidden file
#define DATA_FILE ".magical.file"

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
        // Constructors & destructors
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
        float GetLength() const { return length; }
        float GetWidth() const { return width; }
        float GetHeight() const { return height; }
        float GetArea() const { return area; }
        float GetAspectRatio() const { return aspectRatio; }
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
        // Constructors & destructors
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
        bool IsSlanted() const { return slanted; }
        bool IsSurround() const { return surround; }
        bool IsComfy() const {return comfy; }
        unsigned int GetNumberOfSeats() const { return numberOfSeats; }
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
        // Constructors & destructors
        Time() {
            originTime = time(NULL);
            dirhamsPerHour = 0;
        }
        Time(double p_dirhamsPerHour){
            originTime = time(NULL);
            // Round up to next hour
            originTime += (3600 - originTime % 3600);
            dirhamsPerHour = p_dirhamsPerHour;
        }
        Time(double p_dirhamsPerHour, const time_t& p_originTime) {
            originTime = p_originTime + (3600 - p_originTime % 3600);
            dirhamsPerHour = p_dirhamsPerHour;
        }
        // Setters
        void SetDirhamsPerHour(double p_dirhamsPerHour) { dirhamsPerHour = p_dirhamsPerHour; }
        void SetBulkTimes(const std::vector<unsigned long long>& p_times) { times = p_times; }
        // Getters
        double GetDirhamsPerHour() const { return dirhamsPerHour; }
        time_t GetOriginTime() const { return originTime; }
        std::vector<unsigned long long> GetTimes() const { return times; }

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

    // Class for reviews
    class Review {
        // .. Constrained to 0 to 5
        bool reviewed = false;
        float score = 0;
        unsigned int numberOfReviews = 0;
        std::vector<std::string> reviews;
    public:
        // Constructors & destructors
        Review(float p_score = 0) {
            score = p_score;
        }
        // Setters
        void AddReview(const std::string& p_review, float p_score) {
            reviewed = true;
            reviews.push_back(p_review);
            score = (score * numberOfReviews + p_score) / (++numberOfReviews);
        }
        void SetBulkReviews(int p_score, int p_numberOfReviews, const std::vector<std::string>& p_reviews) {
            reviews = std::vector<std::string>{};
            if (p_numberOfReviews == 0) {
                reviewed = false;
                score = 0;
                return;
            }
            reviewed = true;
            score = p_score;
            reviews = p_reviews;
        }

        // Getters
        float GetReviewScore() const { return score; }
        std::vector<std::string> GetReviews() const { return reviews; }
        unsigned int GetNumberOfReviews() const { return numberOfReviews; }
        bool IsReviewed() const { return reviewed; }
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
    public:
        Review review;
    private:

        // Miscellaneous tags
        std::vector<std::string> tags;
    public:
        // Constructors & destructors
        Space() {
            ID = 0;
            name = "";
            dims = Dimensions();
            seats = Seating();
            timer = Time();
            review = Review();
        }
        Space(unsigned int p_ID,
            std::string p_name,

            // For dimensions
            float p_length, float p_width, float p_height, 
            unsigned int p_numberOfPeople,

            // For seats
            unsigned int p_numberOfSeats, bool p_slanted, bool p_surround, bool p_comfy,

            // For timer
            double p_dirhamsPerHour,

            // Optional
            bool p_outdoor = false,
            bool p_catering = false,
            bool p_naturalLight = false,
            bool p_artificialLight = true,
            bool p_projector = true,
            bool p_sound = true,
            bool p_cameras = true) {
            
            // Manual work
            ID = p_ID;
            name = p_name;
            // Use built-in copy constructor
            dims = Dimensions(p_length, p_width, p_height);
            seats = Seating(p_numberOfSeats, p_slanted, p_surround, p_comfy);
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

            timer = Time(dirhamsPerHour);
            review = Review();
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
        // Setters using overload
        void IsOutdoor(bool p_outdoor) { outdoor = p_outdoor; }
        void IsCatering(bool p_catering) { catering = p_catering; }
        void IsNaturalLight(bool p_naturalLight) { naturalLight = p_naturalLight; }
        void IsArtificialLight(bool p_artificialLight) { artificialLight = p_artificialLight; }
        void IsProjector(bool p_projector) { projector = p_projector; }
        void IsSound(bool p_sound) { sound = p_sound; }
        void IsCameras(bool p_cameras) { cameras = p_cameras; }

        // Getters
        std::string GetName() const { return name; }
        unsigned int GetID() const { return ID; }
        int GetNumberOfPeople() const { return numberOfPeople; }
        bool IsOutdoor() const { return outdoor; }
        bool IsCatering() const { return catering; }
        bool IsNaturalLight() const { return naturalLight; }
        bool IsArtificialLight() const { return artificialLight; }
        bool IsProjector() const { return projector; }
        bool IsSound() const { return sound; }
        bool IsCameras() const { return cameras; }
    };

    // Class to manage spaces
    // (running backbone of the application)
    class SpaceManager {
        std::vector<Space*> spaces;
    public:
        // Constructors & destructors
        SpaceManager() {}
        ~SpaceManager() {
            for (auto i = spaces.begin(); i != spaces.end(); i++)
                delete *i;
        }

        // Storing & reading data
        bool StoreData(std::string p_fileName = DATA_FILE) {
            std::ofstream outFile(p_fileName);
            if (!outFile.is_open())
                return false;
            
            // Wrap try-catch block
            try {
                nljs::json jspaces;
                int i = 0;
                for (auto space_ptr: spaces) {
                    nljs::json jdims = {
                        {"length", space_ptr->dims.GetLength()},
                        {"width", space_ptr->dims.GetWidth()},
                        {"height", space_ptr->dims.GetHeight()}
                    }, jseats = {
                        {"numberOfSeats", space_ptr->seats.GetNumberOfSeats()},
                        {"slanted", space_ptr->seats.IsSurround()},
                        {"surround", space_ptr->seats.IsSlanted()},
                        {"comfy", space_ptr->seats.IsComfy()}
                    }, jtimer = {
                        {"originTime", (unsigned long long)space_ptr->timer.GetOriginTime()},
                        {"times", nljs::json(space_ptr->timer.GetTimes())},
                        {"dirhamsPerHour", space_ptr->timer.GetDirhamsPerHour()}
                    }, jreview = {
                        {"reviewed", space_ptr->review.IsReviewed()},
                        {"score", space_ptr->review.GetReviewScore()},
                        {"numberOfReviews", space_ptr->review.GetNumberOfReviews()},
                        {"reviews", space_ptr->review.GetReviews()}
                    }, jspace = {
                        {"name", space_ptr->GetName()},
                        {"ID", space_ptr->GetID()},
                        {"numberOfPeople", space_ptr->GetNumberOfPeople()},
                        {"outdoor", space_ptr->IsOutdoor()},
                        {"catering", space_ptr->IsCatering()},
                        {"naturalLight", space_ptr->IsNaturalLight()},
                        {"artificialLight", space_ptr->IsArtificialLight()},
                        {"projector", space_ptr->IsProjector()},
                        {"sound", space_ptr->IsCameras()},
                        {"cameras", space_ptr->IsCameras()},
                        {"dims", jdims},
                        {"seats", jseats},
                        {"timer", jtimer},
                        {"review", jreview}
                    };
                    jspaces.push_back(jspace);
                }
                // Write to file
                outFile << std::setw(4) << jspaces << std::endl;
            } catch (std::exception e) {
                outFile.close();
                std::cout << e.what() << std::endl;
                return false;
            }
            // Save data success
            outFile.close();
            return true;
        }
        bool LoadData(std::string p_fileName = DATA_FILE) {
            std::ifstream inFile(p_fileName);
            if (!inFile.is_open())
                return false;
            
            // Wrap try-catch block
            try {
                nljs::json jspaces;
                inFile >> jspaces;
                // Deallocate
                for (auto i = spaces.begin(); i != spaces.end(); i++)
                    delete *i;
                spaces = std::vector<Space*>{};
                for (auto jspace: jspaces) {
                    spaces.push_back(
                        new Space(
                            jspace["ID"],
                            jspace["name"],
                            jspace["dims"]["length"], jspace["dims"]["width"], jspace["dims"]["height"],
                            jspace["numberOfPeople"],
                            jspace["seats"]["numberOfSeats"], jspace["seats"]["slanted"],
                            jspace["seats"]["surround"], jspace["seats"]["comfy"],
                            jspace["timer"]["dirhamsPerHour"],
                            jspace["outdoor"], jspace["catering"],
                            jspace["naturalLight"], jspace["artificialLight"],
                            jspace["projector"], jspace["sound"], jspace["cameras"]
                        )
                    );
                    spaces.back()->review.SetBulkReviews(
                        jspace["review"]["score"], jspace["review"]["numberOfReviews"],
                        jspace["review"]["reviews"].get<std::vector<std::string>>()
                    );
                    spaces.back()->timer = Time(jspace["timer"]["dirhamsPerHour"], jspace["timer"]["originTime"]);
                    spaces.back()->timer.SetBulkTimes(jspace["timer"]["times"].get<std::vector<unsigned long long>>());
                }
            } catch (std::exception e) {
                inFile.close();
                std::cout << e.what() << std::endl;
                return false;
            }
            // Load data success
            inFile.close();
            return true;
        }

        // Testing purposes
        // Generate some random spaces
        void GetRandomizedSpaces(int n) {
            spaces = std::vector<Space*>{};
            std::srand(time(NULL));
            for (int i = 0; i < n; i++) {
                const std::string randLocs[] =
                    {"Building", "Park", "Hall", "Hotel", "Stadium", "Cafe", "Center", "Gallery", "Bar", "Arena"};
                std::string tmpName = "";
                for (int j = 0; j < 3; j++)
                    tmpName.push_back((char)(rand() % 26 + 65));
                tmpName += " " + randLocs[rand() % 10];

                spaces.push_back(
                    new Space(
                        i,                                      // ID
                        tmpName,                                // name

                                                                // For dimensions
                        rand() % 90 + 10,                       // length
                        rand() % 45 + 5,                        // width
                        rand() % 10 + 2,                        // height

                        rand() % 990 + 10,                      // number of people

                                                                // For seating
                        rand() % 490 + 10,                      // number of seats
                        (bool)(rand() % 2),                     // slanted?
                        (bool)(rand() % 2),                     // surround?
                        (bool)(rand() % 2),                     // comfy?

                                                                // For timer
                        rand() % 9900 + 100,                    // price

                        (bool)(rand() % 2),                     // outdoor?
                        (bool)(rand() % 2),                     // catering?
                        (bool)(rand() % 2),                     // naturalLight?
                        (bool)(rand() % 2),                     // artificialLight?
                        (bool)(rand() % 2),                     // sound?
                        (bool)(rand() % 2),                     // projector?
                        (bool)(rand() % 2)                      // camera?
                    )
                );
                spaces.back()->review.AddReview("Very bad, not good", rand() % 5);
                spaces.back()->review.AddReview("Okay ish", rand() % 5);
            }
        }
        // // Print some details to cmd line
        void PrintSpaces() {
            for (auto space: spaces) {
                std::cout << "ID: " << space->GetID() << "\nName: " << space->GetName() << "\nArea: " << space->dims.GetArea() << " m^2" << std::endl;
                for (std::string i: space->review.GetReviews()) {
                    std::cout << i << std::endl;
                }
                std::cout << "Review score: " << space->review.GetReviewScore() << std::endl << std::endl;
            }
        }
    };
}