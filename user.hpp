#ifndef USER_HPP
#define USER_HPP

#include <algorithm>
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
#define USER_FILE "file.magical"

// Space library
#include "space.hpp"

namespace User {
    // Utility functions
    // Display console message and get user input
    std::string GetInput(const std::string& consoleMessage) {
        std::string userInput;
        std::cout << consoleMessage;
        getline(std::cin, userInput);
        return userInput;
    }
    // Get user input time rounded by hours (1 min 1 sec later to be safe)    
    time_t GetTime(const std::string message) {
        tm tmp_time{};
        int tmp_month, tmp_year;
        std::cout << std::endl << message;
        std::cout << "\nFormat: <Day> <Month> <Year> <O'clock>: ";
        std::cin >> tmp_time.tm_mday >> tmp_month >> tmp_year >> tmp_time.tm_hour;
        tmp_time.tm_year = tmp_year - 1900;
        tmp_time.tm_mon = tmp_month - 1;
        tmp_time.tm_min = 0;
        tmp_time.tm_sec = 0;
        std::cin.ignore(100, '\n');
        time_t returnTime = mktime(&tmp_time);
        std::cout << ctime(&returnTime) << std::endl;
        return returnTime;
    }

    // Abstract class for users
    class User {
    protected:
        unsigned int ID;
        std::string name;
        Space::SpaceManager* spaceManager;
    public:
        // Constructors & destructors
        User(Space::SpaceManager* p_spaceManager) {
            ID = 0;
            name = "";
            spaceManager = p_spaceManager;
        }
        User(int p_ID, std::string p_name, Space::SpaceManager* p_spaceManager) {
            ID = p_ID;
            name = p_name;
            spaceManager = p_spaceManager;
        }
        // Setters
        void SetName(const std::string& p_name) {
            name = p_name;
        }
        void SetID(unsigned int p_ID) {
            ID = p_ID;
        }

        // Getters
        std::string GetName() const { return name; }
        unsigned int GetID() const { return ID; }

        // Utility
        // Actions function
        virtual void Actions() = 0;
        // Print function
        virtual void PrintUser() = 0;
        // Serialize function
        virtual nljs::json Serialize() = 0;
        // Deserialize function
        virtual void Deserialize(const nljs::json& p_juser) = 0;
    };

    // Class for event managers
    class EventUser : public User {
        std::vector<std::pair<unsigned int, std::pair<time_t, time_t>>> RSVPs;
        double outstandingBalance = 0;
    public:
        // Constructors & destructors
        EventUser(Space::SpaceManager* p_spaceManager) : User(p_spaceManager) {}
        EventUser(int p_ID, std::string p_name, Space::SpaceManager* p_spaceManager,
            double p_outstandingBalance = 0) : User(p_ID, p_name, p_spaceManager) {
            outstandingBalance = p_outstandingBalance;
        }

        // Utility
        // Clean reservations function: remove reservations with invalid spaces
        inline void CleanReservations() {
            int i = RSVPs.size() - 1;
            while (i >= 0) {
                if (spaceManager->GetSpace(RSVPs[i].first) == nullptr)
                    RSVPs.erase(RSVPs.begin() + i);
                i--;
            }
        }
        // Print reservations function
        inline void PrintReservation(unsigned int ID) {
            std::cout << "\nReservation #" << ID << ":\n";
            spaceManager->GetSpace(RSVPs[ID].first)->PrintSpace(false, false);
            std::cout << "Reservation time:\n  -- from "
                      << ctime(&RSVPs[ID].second.first) << "  -- to "
                      << ctime(&RSVPs[ID].second.second);
        }
        inline void PrintReservations() {
            CleanReservations();
            if (RSVPs.size() > 0) {
                for (unsigned int i = 0; i < RSVPs.size(); i++) {
                    std::cout << std::endl;
                    PrintReservation(i);
                }
            } else {
                std::cout << " You have no reservations yet\n";
            }
        }
        // Print function
        inline void PrintUser() {
            std::cout << "ID: " << ID
                      << "\nName: " << name
                      << "\nRole: Event manager"
                      << "\nOutstanding balance: " << outstandingBalance << " Dhs"
                      << "\nReservations:";
            PrintReservations();
        }
        // Actions function
        void Actions() {
            std::string choice;
            bool isRunning = true;
            while (isRunning) {
                CleanReservations();
                std::cout << "\nWhat would you like to do?\n";
                std::cout << " 1. Browse spaces\n";
                std::cout << " 2. Add or remove reservations\n";
                std::cout << " 3. Browse my reservations\n";
                std::cout << " 4. Make payment\n";
                std::cout << " 5. Add review\n";
                std::cout << " 6. Log out\n";
                getline(std::cin, choice);
                switch (choice[0]) {
                    case '1': {
                        spaceManager->PrintSpaces(true, false, true);
                        choice = GetInput("View space timetables? (y/n): ");
                        while (choice[0] == 'y') {
                            try {
                                unsigned int ID  = std::stoi(GetInput("\nSpace ID: "));
                                if (spaceManager->GetSpace(ID) == nullptr) {
                                    std::cout << "Could not find space!\n";
                                } else spaceManager->GetSpace(ID)->PrintSpace(false, true, false);
                            } catch (std::exception e) {
                                std::cout << "Invalid input" << std::endl;
                            }
                            choice = GetInput("View other space timetables? (y/n): ");
                        }
                        break;
                    }
                    case '2': {
                        try {
                            choice = GetInput("\nAdd (1) or remove (2) reservation? (1/2): ");
                            if (choice[0] == '1') {
                                unsigned int ID  = std::stoi(GetInput("\nSpace ID to make/remove reservation: "));
                                if (spaceManager->GetSpace(ID) == nullptr) {
                                    std::cout << "Could not find space!\n";
                                    break;
                                }
                                double price = 0;
                                time_t tmpStart = GetTime("Input begin time");
                                time_t tmpEnd = GetTime("Input end time");
                                if (spaceManager->GetSpace(ID)->timer.AddReservation(tmpStart, tmpEnd - 3600, price)) {
                                    std::cout << "Reservation successful!\n";
                                    std::cout << "Price: " << price << " Dhs" << std::endl;
                                    outstandingBalance += price;
                                    RSVPs.push_back(std::make_pair(ID, std::make_pair(tmpStart, tmpEnd)));
                                } else {
                                    std::cout << "Reservation failed!\n";
                                    std::cout << "Possible time conflict or invalid time input\n";
                                }
                            } else if (choice[0] == '2') {
                                unsigned int RSVP_ID = std::stoi(GetInput("Enter your reservation #: "));
                                if (RSVP_ID >= RSVPs.size()) {
                                    std::cout << "Could not find reservation!\n";
                                    break;
                                }
                                if (spaceManager->GetSpace(RSVPs[RSVP_ID].first)->
                                    timer.RemoveReservation(RSVPs[RSVP_ID].second.first, RSVPs[RSVP_ID].second.second - 3600)) {
                                    RSVPs.erase(RSVPs.begin() + RSVP_ID);
                                    std::cout << "Reservation removed!\n";
                                    std::cout << "No refund :(\n";
                                }
                            } else {
                                std::cout << "Invalid input" << std::endl;
                            }
                        } catch (std::exception e) {
                            std::cout << "Invalid input" << std::endl;
                        }
                        break;
                    }
                    case '3': {
                        PrintReservations();
                        break;
                    }
                    case '4': {
                        if (outstandingBalance < 0.001) {
                            std::cout << "\nYou do not have any due payment at this time\n";
                        } else {
                            std::cout << "\nYour outstanding balance is " << outstandingBalance << " Dhs" << std::endl;
                            try {
                                double payment = stod(GetInput("How much do you want to pay? (Dhs): "));
                                if (payment > 0) {
                                    std::cout << "Payment received!\n";
                                    if (outstandingBalance < payment) {
                                        std::cout << "Returning " << payment - outstandingBalance << " Dhs in change\n";
                                        outstandingBalance = 0;
                                    } else {
                                        outstandingBalance -= payment;
                                        std::cout << "\nYour outstanding balance is " << outstandingBalance << " Dhs" << std::endl;
                                    }
                                }
                            } catch (std::exception e) {
                                std::cout << "Invalid input" << std::endl;
                            }
                        }
                        break;
                    }
                    case '5': {
                        try {
                            unsigned int ID  = std::stoi(GetInput("\nSpace ID to add review: "));
                            if (spaceManager->GetSpace(ID) == nullptr) {
                                std::cout << "Could not find space!\n";
                                break;
                            }
                            std::string review = GetInput("What would you like to say?\n  -- ");
                            unsigned int score = std::stoi(GetInput("Your review score (0-5): "));
                            if (score > 5) {
                                std::cout << "Invalid score!\n";
                                break;
                            }
                            spaceManager->GetSpace(ID)->review.AddReview(review, score);
                            std::cout << "Review successfully added!\n";
                        } catch (std::exception e) {
                            std::cout << "Invalid input" << std::endl;
                        }
                        break;
                    }
                    case '6': {
                        isRunning = false;
                        return;
                    }
                    default: {
                        std::cout << "Invalid input\n";
                    }
                }
                choice = GetInput("\nReturn to menu? ([y]/n): ");
                if (choice[0] == 'n') isRunning = false;
            }
        }
        // Serialize function
        nljs::json Serialize() {
            CleanReservations();
            nljs::json juser = {
                {"ID", ID},
                {"name", name},
                {"role", "eventUser"},
                {"RSVPs", nljs::json(RSVPs)},
                {"outstandingBalance", outstandingBalance}
            };
            return juser;
        }
        // Deserialize function
        void Deserialize(const nljs::json& p_juser) {
            ID = p_juser["ID"];
            name = p_juser["name"];
            RSVPs = p_juser["RSVPs"].get<std::vector<std::pair<unsigned int, std::pair<time_t, time_t>>>>();
            outstandingBalance = p_juser["outstandingBalance"];
        }
    };

    // Class for space managers
    class SpaceUser : public User {
        std::vector<unsigned int> spaceIDs;
    public:
        // Constructors & destructors
        SpaceUser(Space::SpaceManager* p_spaceManager) : User(p_spaceManager) {}
        SpaceUser(int p_ID, std::string p_name, Space::SpaceManager* p_spaceManager)
            : User(p_ID, p_name, p_spaceManager) {}

        // Utility
        // Print spaces function
        inline void PrintSpaces() {
            if (spaceIDs.size() > 0) {
                for (unsigned int i = 0; i < spaceIDs.size(); i++) {
                    std::cout << std::endl;
                    std::cout << "\nSpace #" << i << ":\n";
                    spaceManager->GetSpace(spaceIDs[i])->PrintSpace();
                }
            } else {
                std::cout << " You have no spaces yet\n";
            }
        }
        // Print function
        inline void PrintUser() {
            std::cout << "ID: " << ID
                      << "\nName: " << name
                      << "\nRole: Space manager"
                      << "\nManage space IDs: ";
            for (int i = 0; i < spaceIDs.size(); i++)
                std::cout << spaceIDs[i] << " ";
            if (spaceIDs.size() == 0) std::cout << "You have no spaces yet\n";
            std::cout << std::endl;
        }
        // Actions function
        void Actions() {
            std::string choice;
            bool isRunning = true;
            while (isRunning) {
                std::cout << "\nWhat would you like to do?\n";
                std::cout << " 1. Browse spaces\n";
                std::cout << " 2. Add or remove spaces\n";
                std::cout << " 3. Browse my spaces\n";
                std::cout << " 4. Log out\n";
                getline(std::cin, choice);
                switch (choice[0]) {
                    case '1': {
                        spaceManager->PrintSpaces();
                        break;
                    }
                    case '2': {
                        try {
                            choice = GetInput("\nAdd (1) or remove (2) space? (1/2): ");
                            if (choice[0] == '1') {
                                std::string name = GetInput("Enter the name of your space: ");
                                choice = GetInput("Randomize space or not (manual entry)? ([y]/n): ");
                                unsigned int ID = spaceManager->GetEmptyID();;
                                if (choice[0] == 'n') {
                                    float length, width, height;
                                    std::cout << "Enter length, width, height (m): ";
                                    std::cin >> length >> width >> height;

                                    unsigned int numberOfPeople;
                                    std::cout << "Enter number of people supported: ";
                                    std::cin >> numberOfPeople;

                                    unsigned int numberOfSeats;
                                    std::cout << "Enter number of seats available: ";
                                    std::cin >> numberOfSeats;
                                    std::cin.ignore(100, '\n');
                                    bool slanted = (GetInput("Are the seats slanted for audience? ([y]/n) ")[0] != 'n');
                                    bool surround = (GetInput("Are the seats in surround format? ([y]/n) ")[0] != 'n');
                                    bool comfy = (GetInput("Are they comfy? ([y]/n) ")[0] != 'n');

                                    double dirhamsPerHour;
                                    std::cout << "Enter hourly rate (Dhs): ";
                                    std::cin >> dirhamsPerHour;

                                    std::cin.ignore(100, '\n');
                                    bool outdoor = (GetInput("Is the space outdoor? ([y]/n) ")[0] != 'n');
                                    bool catering = (GetInput("Does your space provide catering? ([y]/n) ")[0] != 'n');
                                    bool naturalLight = (GetInput("Is there natural daylight? ([y]/n) ")[0] != 'n');
                                    bool artificialLight = (GetInput("Is there lighting at night? ([y]/n) ")[0] != 'n');
                                    bool projector = (GetInput("Are there projectors available? ([y]/n) ")[0] != 'n');
                                    bool sound = (GetInput("Are there any sound systems? ([y]/n) ")[0] != 'n');
                                    bool cameras = (GetInput("Are there cameras available? ([y]/n) ")[0] != 'n');

                                    spaceManager->AddSpace(
                                        new Space::Space(
                                            ID,
                                            name, length, width, height, numberOfPeople, numberOfSeats,
                                            slanted, surround, comfy, dirhamsPerHour, outdoor, catering,
                                            naturalLight, artificialLight, projector, sound, cameras
                                        )
                                    );
                                } else {
                                    spaceManager->GetRandomizedSpaces(1, name);
                                }
                                std::cout << "Space " << name << " successfully created with ID: " << ID << "!\n";
                                spaceIDs.push_back(ID);

                            } else if (choice[0] == '2') {
                                unsigned int ID  = std::stoi(GetInput("\nSpace ID to remove: "));
                                if (spaceManager->GetSpace(ID) == nullptr) {
                                    std::cout << "Could not find space!\n";
                                    break;
                                } 
                                auto pos = std::find(spaceIDs.begin(), spaceIDs.end(), ID);
                                if (pos == spaceIDs.end()) {
                                    std::cout << "Space is not under your management!\n";
                                    break;
                                }
                                spaceManager->DeleteSpace(ID);
                                spaceIDs.erase(pos);
                                std::cout << "Space successfully deleted!\n";
                            } else {
                                std::cout << "Invalid input" << std::endl;
                            }
                        } catch (std::exception e) {
                            std::cout << "Invalid input" << std::endl;
                        }
                        break;
                    }
                    case '3': {
                        PrintSpaces();
                        break;
                    }
                    case '4': {
                        isRunning = false;
                        return;
                    }
                    default: {
                        std::cout << "Invalid input\n";
                    }
                }
                choice = GetInput("\nReturn to menu? ([y]/n): ");
                if (choice[0] == 'n') isRunning = false;
            }
        }
        // Serialize function
        nljs::json Serialize() {
            nljs::json juser = {
                {"ID", ID},
                {"name", name},
                {"role", "spaceUser"},
                {"spaceIDs", nljs::json(spaceIDs)}
            };
            return juser;
        }
        // Deserialize function
        void Deserialize(const nljs::json& p_juser) {
            ID = p_juser["ID"];
            name = p_juser["name"];
            spaceIDs = p_juser["spaceIDs"].get<std::vector<unsigned int>>();
        }
    };

    // Class to manage users
    // (running front of the application)
    class UserManager {
        std::vector<User*> users;
        User* activeUser;

        // Check if spaceManager is running
        bool isSpace = false;
        Space::SpaceManager* spaceManager;
    public:
        // Constructors & destructors
        UserManager(Space::SpaceManager* p_spaceManager = nullptr) {
            spaceManager = p_spaceManager;
            if (p_spaceManager != nullptr) isSpace = true;
        }
        ~UserManager() {
            for (auto i = users.begin(); i != users.end(); i++)
                delete *i;
        }

        // Interface
        // Print some data to cmd line
        inline void PrintUsers() {
            for (auto user_ptr: users) {
                std::cout << std::endl;
                user_ptr->PrintUser();
            }
            if (users.size() == 0) std::cout << "No users yet!\n";
        }

        // Data persistence
        // Storing & reading data
        bool StoreData(std::string p_fileName = USER_FILE) {
            std::ofstream outFile(p_fileName);
            if (!outFile.is_open())
                return false;

            // Wrap try-catch block
            try {
                nljs::json jusers;
                for (auto user_ptr: users)
                    jusers.push_back(user_ptr->Serialize());
                // Write to file
                outFile << std::setw(4) << jusers << std::endl;
            } catch (std::exception e) {
                outFile.close();
                std::cout << e.what() << std::endl;
                return false;
            }
            // Save data success
            outFile.close();
            return true;
        }
        bool LoadData(std::string p_fileName = USER_FILE) {
            std::ifstream inFile(p_fileName);
            if (!inFile.is_open())
                return false;
            
            // Wrap try-catch block
            try {
                nljs::json jusers;
                inFile >> jusers;
                // Deallocate
                for (auto i = users.begin(); i != users.end(); i++)
                    delete *i;
                users = std::vector<User*>{};
                for (auto juser: jusers) {
                    if (juser["role"] == "eventUser") {
                        users.push_back(new EventUser(spaceManager));
                        users.back()->Deserialize(juser);
                    } else if (juser["role"] == "spaceUser") {
                        users.push_back(new SpaceUser(spaceManager));
                        users.back()->Deserialize(juser);
                    }
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

        // Utility
        // Main program function
        void MainProgram() {
            std::string choice;
            bool isRunning = true;
            while (isRunning) {
                try {
                    std::cout << std::endl;
                    std::cout << "--- +-+ ------------------------------- +-+ ---\n";
                    std::cout << "Welcome to Evies - the event scheduling system!\n";
                    std::cout << "--- +-+ ------------------------------- +-+ ---\n";

                    std::cout << "\nWhat would you like to do?\n";
                    std::cout << " 1. Login/register\n";
                    std::cout << "/// Developer area ///\n";
                    std::cout << " 2. Browse spaces\n";
                    std::cout << " 3. Browse users (gray legality)\n";
                    std::cout << " 4. Store/load data\n";
                    std::cout << " 5. Generate random spaces\n";
                    std::cout << " 6. Exit\n";

                    getline(std::cin, choice);
                    switch (choice[0]) {
                        case '1': {
                            // Login/register
                            choice = GetInput("\nWould you like to login or register? (l/r): ");
                            if (choice[0] == 'l') {
                                bool isLoggedIn = false;
                                while (!isLoggedIn) {
                                    std::string name = GetInput("Enter your name: ");
                                    unsigned int ID = std::stoi(GetInput("Enter your ID: "));
                                    if (ID >= users.size() || users[ID]->GetName() != name) {
                                        std::cout << "Invalid credentials\n";
                                        choice = GetInput("Retry login? ([y]/n): ");
                                        if (choice[0] == 'n') break;
                                    } else {
                                        isLoggedIn = true;
                                        std::cout << "\nLogged in successfully as:\n";
                                        users[ID]->PrintUser();
                                        users[ID]->Actions();
                                    }
                                }
                            } else if (choice[0] == 'r') {
                                // Role selection
                                choice = GetInput("Are you a event manager (1) or space manager (2)? (1/2): ");
                                if (choice[0] == '1' || choice[0] == '2') {
                                    std::string name = GetInput("Enter your name: ");
                                    unsigned int ID = users.size();
                                    std::cout << "\nYour ID is: " << ID << std::endl;
                                    std::cout << "Please remember for next login.\n";
                                    if (choice[0] == '1') activeUser = new EventUser(ID, name, spaceManager);
                                    else activeUser = new SpaceUser(ID, name, spaceManager);
                                    users.push_back(activeUser);
                                    users[ID]->Actions();
                                } else std::cout << "Invalid input" << std::endl;
                            } else std::cout << "Invalid input" << std::endl;
                            break;
                        }
                        case '2': {
                            spaceManager->PrintSpaces();
                            break;
                        }
                        case '3': {
                            PrintUsers();
                            break;
                        }
                        case '4': {
                            choice = GetInput("\nWould you like to store (1) or load (2) data? (1/2): ");
                            if (choice[0] == '1') {
                                if (spaceManager->StoreData())
                                    if (StoreData()) {
                                        std::cout << "Data stored successfully!\n";
                                        break;
                                    }
                                std::cout << "Store data failed!\nRecommend program restart\n";
                            } else if (choice[0] == '2') {
                                if (spaceManager->LoadData())
                                    if  (LoadData())
                                        std::cout << "Data loaded successfully!\n";
                                        break;
                                std::cout << "Load data failed!\nRecommend program restart\n";
                            } else std::cout << "Invalid input" << std::endl;
                            break;
                        }
                        case '5': {
                            try {
                                int count = std::stoi(GetInput("\nNumber of generated spaces: "));
                                if (count <= 0) {
                                    std::cout << "Invalid input" << std::endl;
                                    break;
                                }
                                spaceManager->GetRandomizedSpaces(count);
                                std::cout << "Space" << (count > 1? "s": "") << " generated successfully!\n";
                            } catch (std::exception e) {
                                std::cout << "Invalid input" << std::endl;
                            }
                            break;
                        }
                        case '6': {
                            isRunning = false;
                            return;
                        }
                        default: {
                            std::cout << "Invalid input\n";
                        }
                    }

                } catch (std::exception e) {
                    std::cout << "Invalid input" << std::endl;
                }
                choice = GetInput("\nReturn to main menu? ([y]/n): ");
                isRunning = !(choice[0] == 'n');
            }
        }
    };
}

#endif