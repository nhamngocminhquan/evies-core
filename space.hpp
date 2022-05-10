#ifndef SPACE_HPP
#define SPACE_HPP

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
#define SPACE_FILE "magical.file"

// Bitwise helpers courtesy of:
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
        Seating() {}
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
        // .. 32 bits per each corresponding to 2.67 days
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
            originTime = p_originTime;
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
            // .. Hour is tracked (both start & end) from beginning o'clock -> floor is used here
            unsigned long startHours = (unsigned long)std::floor(std::difftime(p_startTime, originTime) / (60 * 60));
            unsigned long endHours = (unsigned long)std::floor(std::difftime(p_endTime, originTime) / (60 * 60));
            // Invalid reservation
            if (endHours < startHours || startHours < 0) return false;
            if (times.size() <= endHours / 32)
                while (times.size() <= endHours / 32) times.push_back(0);
            // Check if any hour in the reservation is booked
            if (startHours / 32 == endHours / 32) {
                for (unsigned long i = startHours % 32; i <= endHours % 32; i++) 
                    if GetBit(times[startHours / 32], i)
                        // Time is occupied
                        return false;
            } else {
                for (unsigned long i = startHours % 32; i < 32; i++) 
                    if GetBit(times[startHours / 32], i)
                        // Time is occupied
                        return false;
                unsigned long j = startHours / 32;
                while (j != endHours / 32) {
                    for (unsigned long i = 0; i < 32; i++) 
                        if GetBit(times[startHours / 32], i)
                            // Time is occupied
                            return false;
                    j++;
                }
                for (unsigned long i = 0; i <= endHours % 32; i++) 
                    if GetBit(times[endHours / 32], i)
                        // Time is occupied
                        return false;
            }
            // If not, proceed to select the hours
            if (startHours / 32 == endHours / 32) {
                for (unsigned long i = startHours % 32; i <= endHours % 32; i++) {
                    SetBit(times[startHours / 32], i); 
                }
            } else {
                for (unsigned long i = startHours % 32; i < 32; i++) 
                    SetBit(times[startHours / 32], i);
                for (unsigned long i = startHours / 32 + 1; i < endHours / 32; i++)
                    times[i] = ~0;
                for (unsigned long i = 0; i <= endHours % 32; i++) 
                    SetBit(times[endHours / 32], i);
            }
            price = dirhamsPerHour * (endHours - startHours + 1);
            return true;
        }
        // Function to remove reservations
        bool RemoveReservation(const time_t& p_startTime, const time_t& p_endTime) {
            // Get hours difference between startTime, endTime and originTime
            // .. Hour is tracked from beginning o'clock -> floor is used here
            unsigned long startHours = (unsigned long)std::floor(std::difftime(p_startTime, originTime) / (60 * 60));
            unsigned long endHours = (unsigned long)std::floor(std::difftime(p_endTime, originTime) / (60 * 60));
            // Invalid reservation
            if (endHours < startHours || startHours < 0) return false;
            if (times.size() < endHours / 32)
                while (times.size() <= endHours / 32) times.push_back(0);
            // Directly clear the hours
            if (startHours / 32 == endHours / 32) {
                for (unsigned long i = startHours % 32; i <= endHours % 32; i++) 
                    ClearBit(times[startHours / 32], i);
            } else {
                for (unsigned long i = startHours % 32; i < 32; i++) 
                    ClearBit(times[startHours / 32], i);
                for (unsigned long i = startHours / 32 + 1; i < endHours / 32; i++)
                    times[i] = 0;
                for (unsigned long i = 0; i <= endHours % 32; i++) 
                    ClearBit(times[endHours / 32], i);
            }
            return true;
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
            numberOfReviews = p_numberOfReviews;
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
        // Constructors don't copy times & reviews
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
            // Set current time as origin time
            timer = Time(p_dirhamsPerHour);
            outdoor = p_outdoor;
            catering = p_catering;
            naturalLight = p_naturalLight;
            artificialLight = p_artificialLight;
            projector = p_projector;
            sound = p_sound;
            cameras = p_cameras;

            review = Review();
        }
        // Copy constructor with optional ID
        Space(const Space& p_space, unsigned int p_ID = -1) {
            // Manual work
            if (p_ID != (unsigned int)(-1))
                ID = p_ID;
            else ID = p_space.GetID();
            name = p_space.GetName();
            // Use built-in copy constructor
            dims = Dimensions(p_space.dims.GetLength(), p_space.dims.GetWidth(), p_space.dims.GetHeight());
            seats = Seating(p_space.seats.GetNumberOfSeats(), p_space.seats.IsSlanted(), p_space.seats.IsSurround(), p_space.seats.IsComfy());
            numberOfPeople = p_space.GetNumberOfPeople();
            // Set current time as origin time
            timer = Time(p_space.timer.GetDirhamsPerHour(), p_space.timer.GetOriginTime());
            outdoor = p_space.IsOutdoor();
            catering = p_space.IsCatering();
            naturalLight = p_space.IsNaturalLight();
            artificialLight = p_space.IsArtificialLight();
            projector = p_space.IsProjector();
            sound = p_space.IsSound();
            cameras = p_space.IsCameras();

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

        // Utility
        // Print some details to cmd line
        inline void PrintSpace(bool withReviews = true, bool withTimes = true,
            bool withDetails = true) const {
            std::cout << "ID: " << ID
                      << "\nName: " << name
                      << "\nArea: " << dims.GetArea() << " m^2 -- "
                      << "Aspect ratio: " << dims.GetAspectRatio()
                      << std::endl;
            if (withDetails) {
                std::cout << "Supports " << numberOfPeople << " people\n";
                std::string tmp_string = "";
                std::vector<std::string> details;
                if (outdoor) details.push_back("outdoors atmosphere");
                if (naturalLight) details.push_back("natural daylight");
                if (artificialLight) details.push_back("lighting at night");
                if (catering) details.push_back("food & drinks");
                if (projector) details.push_back("projectors");
                if (sound) details.push_back("sound system");
                if (cameras) details.push_back("cameras available");
                if (details.size() == 1) std::cout << "Has " << details[0] << std::endl;
                else if (details.size() == 2) std::cout << "Has " << details[0] << " & " << details[1] << std::endl;
                else if (details.size() >= 3) {
                    std::cout << "Has ";
                    for (int i = 0; i < details.size() - 1; i++)
                        std::cout << details[i] << ", ";
                    std::cout << "and also " << details[details.size() - 1] << std::endl;
                } 
            }
            if (withReviews) {
                std::cout << "Reviews:";
                if (review.GetNumberOfReviews() != 0) {
                    std::cout << std::endl;
                    for (std::string i: review.GetReviews()) {
                        std::cout << "   -- " << i << std::endl;
                    }
                    std::cout << "Review score: " << review.GetReviewScore() << std::endl;
                } else std::cout << " None\n";
            }
            // Print time courtesy of:
            // https://stackoverflow.com/questions/997512/string-representation-of-time-t
            if (withTimes) {
                time_t tmp_time = timer.GetOriginTime();
                std::cout << "Space opened on: " << ctime(&tmp_time);
                std::cout << "Price per hour: " << timer.GetDirhamsPerHour() << " Dhs\n";
                std::cout << "Timetable:";
                if (timer.GetTimes().size() != 0) {
                    std::cout << std::endl;
                    tmp_time = timer.GetOriginTime();
                    tm* tmp_tm = localtime(&tmp_time);
                    std::string tmp_string;
                    int hourCounter = tmp_tm->tm_hour;
                    unsigned int timeCounter = 0;
                    unsigned int bitCounter = 0;

                    // Initial day
                    tmp_string = std::string(ctime(&tmp_time));
                    tmp_string.erase(11, 8);
                    tmp_string.erase(tmp_string.length() - 1);
                    std::cout << std::endl << "  -- " << tmp_string + " ";
                    for (int i = 0; i < hourCounter; i++)
                        std::cout << " ";

                    // Loop through the vector of times
                    while (timeCounter != timer.GetTimes().size()) {
                        while (bitCounter != 32) {
                            if (hourCounter == 24) {
                                hourCounter = 0;
                                tmp_tm = localtime(&tmp_time);
                                tmp_tm->tm_mday++;
                                tmp_time = mktime(tmp_tm);
                                tmp_string = std::string(ctime(&tmp_time));
                                tmp_string.erase(11, 8);
                                tmp_string.erase(tmp_string.length() - 1);
                                std::cout << std::endl << "  -- " << tmp_string + " ";
                            }
                            std::cout <<
                                (GetBit(timer.GetTimes()[timeCounter], bitCounter)? "/": ".");
                            bitCounter++;
                            hourCounter++;
                        }
                        bitCounter = 0;
                        timeCounter++;
                    }
                    std::cout << std::endl;
                } else std::cout << " Not booked yet\n";
            }
        }
        // Serialize function
        nljs::json Serialize() {
            nljs::json jdims = {
                {"length", dims.GetLength()},
                {"width", dims.GetWidth()},
                {"height", dims.GetHeight()}
            }, jseats = {
                {"numberOfSeats", seats.GetNumberOfSeats()},
                {"slanted", seats.IsSurround()},
                {"surround", seats.IsSlanted()},
                {"comfy", seats.IsComfy()}
            }, jtimer = {
                {"originTime", (unsigned long long)timer.GetOriginTime()},
                {"times", timer.GetTimes()},
                {"dirhamsPerHour", timer.GetDirhamsPerHour()}
            }, jreview = {
                {"reviewed", review.IsReviewed()},
                {"score", review.GetReviewScore()},
                {"numberOfReviews", review.GetNumberOfReviews()},
                {"reviews", review.GetReviews()}
            }, jspace = {
                {"name", name},
                {"ID", ID},
                {"numberOfPeople", numberOfPeople},
                {"outdoor", outdoor},
                {"catering", catering},
                {"naturalLight", naturalLight},
                {"artificialLight", artificialLight},
                {"projector", projector},
                {"sound", sound},
                {"cameras", cameras},
                {"dims", jdims},
                {"seats", jseats},
                {"timer", jtimer},
                {"review", jreview}
            };
            return jspace;
        }
        // Deserialize function
        void Deserialize(const nljs::json& p_jspace) {
            ID = p_jspace["ID"];
            name = p_jspace["name"];
            dims = Dimensions(
                p_jspace["dims"]["length"].get<nljs::json::number_float_t>(),
                p_jspace["dims"]["width"].get<nljs::json::number_float_t>(),
                p_jspace["dims"]["height"].get<nljs::json::number_float_t>()
            );
            numberOfPeople = p_jspace["numberOfPeople"];
            seats = Seating(
                p_jspace["seats"]["numberOfSeats"], p_jspace["seats"]["slanted"],
                p_jspace["seats"]["surround"], p_jspace["seats"]["comfy"]
            );
            outdoor = p_jspace["outdoor"];
            catering = p_jspace["catering"];
            naturalLight = p_jspace["naturalLight"];
            artificialLight = p_jspace["artificialLight"];
            projector = p_jspace["projector"];
            sound = p_jspace["sound"];
            cameras = p_jspace["cameras"];
            review = Review();
            review.SetBulkReviews(
                p_jspace["review"]["score"].get<nljs::json::number_float_t>(),
                p_jspace["review"]["numberOfReviews"],
                p_jspace["review"]["reviews"].get<std::vector<std::string>>()
            );
            timer = Time(p_jspace["timer"]["dirhamsPerHour"], p_jspace["timer"]["originTime"]);
            timer.SetBulkTimes(p_jspace["timer"]["times"].get<std::vector<unsigned long long>>());
        }
    };

    // Class to manage spaces
    // (running back of the application)
    class SpaceManager {
        std::vector<Space*> spaces;
        unsigned int emptyID = spaces.size();
    public:
        // Constructors & destructors
        SpaceManager() {}
        ~SpaceManager() {
            for (auto i = spaces.begin(); i != spaces.end(); i++)
                delete *i;
        }

        // Getters
        unsigned int GetEmptyID() const { return emptyID; }

        // Interface
        // Add space via reference (returns ID)
        unsigned int AddSpace(const Space& p_space) {
            // Check if full of running spaces
            bool isFull = false;
            if (emptyID == spaces.size()) {
                spaces.push_back(nullptr);
                isFull = true;
            }
            // Create new space at position
            int ID = emptyID;
            spaces[emptyID] = new Space(p_space, emptyID);

            // Find next empty space
            if (isFull) emptyID = spaces.size();
            else while (emptyID != spaces.size()) {
                if (spaces[emptyID] == nullptr) break;
                emptyID++;
            }
            return ID;
        }
        // Add space via pointer (returns ID)
        unsigned int AddSpace(Space* p_space_ptr) {
            // Check if full of running spaces
            bool isFull = false;
            if (emptyID == spaces.size()) {
                spaces.push_back(nullptr);
                isFull = true;
            }
            // Create new space at position
            int ID = emptyID;
            spaces[emptyID] = p_space_ptr;

            // Find next empty space
            if (isFull) emptyID = spaces.size();
            else while (emptyID != spaces.size()) {
                if (spaces[emptyID] == nullptr) break;
                emptyID++;
            }
            return ID;
        }
        // Delete space
        bool DeleteSpace(unsigned int ID) {
            if (ID >= spaces.size()) return false;
            if (spaces[ID] != nullptr) {
                delete spaces[ID];
                spaces[ID] = nullptr;
                if (emptyID > ID) emptyID = ID;
            } 
            return false;
        }
        // Get space
        Space* GetSpace(unsigned int ID) {
            if (ID >= spaces.size()) return nullptr;
            else return spaces[ID];
        }
        // Print some details to cmd line
        inline void PrintSpaces(bool withReviews = true, bool withTimes = true,
            bool withDetails = true) {
            for (auto space_ptr: spaces)
                if (space_ptr != nullptr) {
                    std::cout << std::endl;
                    space_ptr->PrintSpace(withReviews, withTimes, withDetails);
                }
            if (spaces.size() == 0) std::cout << "No spaces yet!\n";
        }

        // Data persistence
        // Storing & reading data
        bool StoreData(std::string p_fileName = SPACE_FILE) {
            std::ofstream outFile(p_fileName);
            if (!outFile.is_open())
                return false;
            
            // Wrap try-catch block
            try {
                nljs::json jspaces;
                for (auto space_ptr: spaces) {
                    if (space_ptr == nullptr) jspaces.push_back(nullptr);
                    else jspaces.push_back(space_ptr->Serialize());
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
        bool LoadData(std::string p_fileName = SPACE_FILE) {
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
                bool isFull = true;
                for (auto jspace: jspaces) {
                    if (jspace == nullptr) {
                        if (isFull) {
                            isFull = false;
                            emptyID = spaces.size();
                        }
                        spaces.push_back(nullptr);
                    } else {
                        spaces.push_back(new Space());
                        spaces.back()->Deserialize(jspace);
                    }
                }
                if (isFull) emptyID = spaces.size();
            } catch (std::exception e) {
                inFile.close();
                std::cout << e.what() << std::endl;
                return false;
            }
            // Load data success
            inFile.close();
            return true;
        }

        // Utility
        // Generate some random spaces
        void GetRandomizedSpaces(int n, std::string p_name = "") {
            // spaces = std::vector<Space*>{};
            std::srand(time(NULL));
            unsigned int returnID;
            for (int i = 0; i < n; i++) {
                // Check if name is supplied
                std::string tmpName = "";
                if (p_name != "") tmpName = p_name;
                // Randomize name
                else {
                    const std::string randLocs[] =
                        {"Building", "Park", "Hall", "Hotel", "Stadium", "Cafe", "Center", "Gallery", "Bar", "Arena"};
                    for (int j = 0; j < 3; j++)
                        tmpName.push_back((char)(rand() % 26 + 65));
                    tmpName += " " + randLocs[rand() % 10];
                }
                
                // Create space
                unsigned int newID = emptyID;
                AddSpace(
                    new Space(
                        newID,                                  // ID
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
                        (bool)(rand() % 2),                     // projector?
                        (bool)(rand() % 2),                     // sound?
                        (bool)(rand() % 2)                      // camera?
                    )
                );

                // Add bogus reviews
                const std::string randRevs[] = {
                    "Very bad, not good",
                    "Okay ish",
                    "Food is perfect! drinks are okay",
                    "Cozy atmosphere",
                    "Horrible reception n wifi",
                    "perfect for game night !!",
                    "Very fresh & spacious! Worth the price",
                    "Too far from the city",
                    "need more trashbins",
                    "Good sound system & cameras"
                };
                int numOfReviews = rand() % 3 + 1;
                for (int j = 0; j < numOfReviews; j++)
                    spaces[newID]->review.AddReview(randRevs[rand() % 10], rand() % 6);
                
            }
        }
    };
}

#endif