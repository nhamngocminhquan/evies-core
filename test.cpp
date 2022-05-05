#include "space.cpp"
#include <iostream>
#include <vector>
#include <string>

int main() {
	for (long long i = 0; i < 200000; i++) {
		Space::SpaceManager spaceMgr;
		spaceMgr.GetRandomizedSpaces(100);
		spaceMgr.StoreData();
		spaceMgr.LoadData();
		spaceMgr.PrintSpaces();
	}
}