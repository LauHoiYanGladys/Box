#include <iostream>
#include "fssimplewindow.h"
#include "Manager.h"
#include "Box.h"

using namespace std;

int main(void) {
	Manager theManager;

	// show menu on console
	theManager.showMenu();

	// set up graphics window
	FsOpenWindow(16, 16, WIN_WIDTH, WIN_HEIGHT, 1, "Box");

	// run user input loop
	while (theManager.manage()) {
		FsSleep(25);
	}

	// goodbye message and wait for user confirmation
	cout << endl << "Thank you for using our box simulation program." << endl;
	cout << "\t\t Press ENTER to close console" << endl;

	while (getchar() != '\n'); // this will clear the newline associated with last input
	getchar();  // this will wait for enter
}