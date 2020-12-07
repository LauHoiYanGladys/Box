#include <iostream>
#include "fssimplewindow.h"
#include "Manager.h"
#include "Box.h"
#include "Camera3D.h"
#include "OrbitingViewer.h"
#include "ysglfontdata.h"
#include "GraphicFont.h"

using namespace std;

int main(void) {
	Manager theManager;

	// show menu on console
	theManager.showMenu();

	// set up graphics window
	//FsOpenWindow(16, 16, WIN_WIDTH, WIN_HEIGHT, 1, "Box");

	bool terminate = false;
	Camera3D camera;
	OrbitingViewer orbit;

	camera.z = 10.0; // look at point 0,0,10

	camera.y = 5.0;
	camera.farZ = 400.0;
	orbit.p -= 5*Camera3D::PI / 180.0;
	orbit.h += 2 * Camera3D::PI / 180.0;
	//initialize fonts (after FsOpenWindow)
	

	// run user input loop

	theManager.manageSetup(camera, orbit);

	/*while (theManager.manage(camera, orbit)) {
		FsSleep(5);
	}*/

	// goodbye message and wait for user confirmation
	cout << endl << "Thank you for using our box simulation program." << endl;
	cout << "\t\t Press ENTER to close console" << endl;

	while (getchar() != '\n'); // this will clear the newline associated with last input
	getchar();  // this will wait for enter
}