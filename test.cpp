#include "space.hpp"
#include "user.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <ctime>

using namespace std;
// Display console message and get user input
string GetInput(const string& consoleMessage) {
	string userInput;
	cout << consoleMessage;
	getline(cin, userInput);
	return userInput;
}

int main() {
	// time_t tmp_t = time(NULL);
	// cout << "Current time: " << ctime(&tmp_t) << endl;
	// tmp_t += 3600 - (tmp_t % 3600);
	// cout << "Current time rounded: " << ctime(&tmp_t) << endl;
	for (long long i = 0; i < 1; i++) {
		Space::SpaceManager spaceMgr;
		spaceMgr.GetRandomizedSpaces(20);
		spaceMgr.DeleteSpace(10);
		spaceMgr.DeleteSpace(14);
		spaceMgr.StoreData();
		spaceMgr.LoadData();
		spaceMgr.PrintSpaces();

		try {
			for (int i = 0; i < 1; i++) {
				unsigned int ID  = stoi(GetInput("Space ID?: "));
				int choice = stoi(GetInput("Make reservation or remove reservation? (1/2): "));
				if (choice != 1 && choice != 2) return 0;

				time_t tmpStart = User::GetTime("Input begin time");
				time_t tmpEnd = User::GetTime("Input end time");

				if (choice == 1) {
					double price = 0;
					spaceMgr.GetSpace(ID)->timer.AddReservation(tmpStart, tmpEnd, price);
					cout << "Price: " << price << " Dhs" << endl;
				} else {
					spaceMgr.GetSpace(ID)->timer.RemoveReservation(tmpStart, tmpEnd);
				}
				spaceMgr.GetSpace(ID)->PrintSpace();
			}
		} catch (exception e) {
			cout << "Invalid input" << endl;
		}

		spaceMgr.StoreData();
		spaceMgr.LoadData();
		spaceMgr.PrintSpaces();
	}
}