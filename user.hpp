#ifndef USER_HPP
#define USER_HPP

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
#define USER_FILE ".file.magical"

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
        tmp_time.tm_min = 1;
        tmp_time.tm_sec = 1;
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

        // Check if spaceManager is running
        bool isSpace = false;
        Space::SpaceManager* spaceManager;
    public:
        // Constructors & destructors
        User(int p_ID, std::string p_name, Space::SpaceManager* p_spaceManager) {
            ID = p_ID;
            name = p_name;
            spaceManager = p_spaceManager;
            if (spaceManager != nullptr) isSpace = true;
        }

        // Utility
        // Actions function
        virtual void Actions() = 0;
    };

    // Class for space managers
    class SpaceUser : public User {
    public:
        // Constructors & destructors
        SpaceUser(int p_ID, std::string p_name, Space::SpaceManager* p_spaceManager)
            : User(p_ID, p_name, p_spaceManager) {}

        void Actions() {}
    };

    // Class for event managers
    class EventUser : public User {
    public:
        // Constructors & destructors
        EventUser(int p_ID, std::string p_name, Space::SpaceManager* p_spaceManager)
            : User(p_ID, p_name, p_spaceManager) {}

        void Actions() {}
    };

    // Class to manage users
    // (running front of the application)
    class UserManager {
        std::vector<User*> users;
        User* activeUser;
        unsigned int emptyID;

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

        // Login/register function (main)
        void LoginRegister() {
            bool isRunning = true;
            while (isRunning) {
                try {
                    std::cout << "--- +++ ------------------------------- +++ ---\n";
                    std::cout << "Welcome to Evies - the event scheduling system!\n";
                    std::string choice;

                    // Login/register
                    choice = GetInput("Would you like to login or register? (l/r): ");
                    if (choice[0] == 'l') {
                        std::string name = GetInput("Enter your name: ");
                        unsigned int ID = std::stoi(GetInput("Enter your ID: "));
                    } else if (choice[0] == 'r') {
                        // Role selection
                        choice = GetInput("Are you a space manager (1) or event manager (2)? (1/2): ");
                        if (choice[0] == '1' || choice[0] == '2') {
                            std::string name = GetInput("Enter your name: ");
                            if (choice[0] == '1') activeUser = new SpaceUser(1, name, spaceManager);
                            else activeUser = new EventUser(1, name, spaceManager);
                            
                        }
                    } else std::cout << "Invalid choice\n";

                    choice = GetInput("\nWould you like to continue? (y/n): ");
                    isRunning = (choice[0] == 'y');
                } catch (std::exception e) {
                    std::cout << "Invalid input" << std::endl;
                }
            }
        }
    };
}

#endif