#include "space.hpp"
#include "user.hpp"

int main() {
	Space::SpaceManager spaceMgr;
	User::UserManager userMgr(&spaceMgr);
	userMgr.MainProgram();
}