#include <iostream>
#include <fstream>
#include <filesystem>  // only helps when using C++17
#include <sstream>
#include "StringPlus.h"
#include "Manager.h"
#include "Box.h"
#include "DrawingUtilNG.h"
#include "fssimplewindow.h"
#include "ysglfontdata.h"
#include "GraphicFont.h"
#include "Camera3D.h"
#include "OrbitingViewer.h"

#include "Rocket.h"


using namespace std;

Manager::Manager()
{
	/*editModeIsOn = false;*/
	gravityIsOn = false;

	boxCounter = 0;

	//xOrigin = 0;
	//yOrigin = WIN_HEIGHT;
	xOrigin = 0.5 * WIN_WIDTH;
	yOrigin = 0.5 * WIN_HEIGHT;
	groundY = WIN_HEIGHT;

	maxX = 0;
	maxY = 0;
	minX = 0;
	minY = 0;

	currBox = nullptr;

	viewScale = 12.0;
	panChange = 10;
	zoomFactor = 1.2;

	getAvailableFiles(allModelfiles);

	modelComX = 0;
	modelComY = 0;
	modelComZ = 0;

	theMode = editMode;
	theRocketBoxType = engine; // defaults to setting engine box first
	currRocket = nullptr;

	
	

}
// Gladys
bool Manager::isIntersecting(Box& firstBox, Box& secondBox, overlappingDimension theDimension)
{
	double firstXInterval[] = { firstBox.getLeftUpperX(),  firstBox.getRightUpperX() };
	double firstYInterval[] = { firstBox.getLeftLowerY(),  firstBox.getLeftUpperY() };

	double secondXInterval[] = { secondBox.getLeftUpperX(),  secondBox.getRightUpperX() };
	double secondYInterval[] = { secondBox.getLeftLowerY(),  secondBox.getLeftUpperY() };
	cout << "x1: [" << firstXInterval[0] << "," << firstXInterval[1] << "]" << endl;
	cout << "y1: [" << firstYInterval[0] << "," << firstYInterval[1] << "]" << endl;
	cout << "x2: [" << secondXInterval[0] << "," << secondXInterval[1] << "]" << endl;
	cout << "y2: [" << secondYInterval[0] << "," << secondYInterval[1] << "]" << endl;
	if (theDimension == both) {
		// Compare x-intervals, y-intervals and z-intervals spanned by the boxes. 
		// If all three dimensions intersect, there is an intersection

		//compare x intervals
		if ((firstXInterval[0] >= secondXInterval[0] && firstXInterval[0] <= secondXInterval[1]) ||
			(firstXInterval[1] >= secondXInterval[0] && firstXInterval[1] <= secondXInterval[1]) ||
			(secondXInterval[0] >= firstXInterval[0] && secondXInterval[0] <= firstXInterval[1]) ||
			(secondXInterval[1] >= firstXInterval[0] && secondXInterval[1] <= firstXInterval[1]))
		{
			if ((firstYInterval[0] >= secondYInterval[0] && firstYInterval[0] <= secondYInterval[1]) ||
				(firstYInterval[1] >= secondYInterval[0] && firstYInterval[1] <= secondYInterval[1]))
			{
				cout << "intersecting" << endl;
				return true;
			}


		}
	}

	else if (theDimension == x) {
		if ((firstXInterval[0] >= secondXInterval[0] && firstXInterval[0] <= secondXInterval[1]) ||
			(firstXInterval[1] >= secondXInterval[0] && firstXInterval[1] <= secondXInterval[1]) ||
			(secondXInterval[0] >= firstXInterval[0] && secondXInterval[0] <= firstXInterval[1]) ||
			(secondXInterval[1] >= firstXInterval[0] && secondXInterval[1] <= firstXInterval[1])) {
			cout << "Intersecting " << endl;
			return true;

		}
	}
	cout << "Not Intersecting" << endl;
	return false;

}

void Manager::showMenu()
{
	cout << "Use these keys on the screen:" << endl;
	cout << "    L : load file (this does not clear all previous data)" << endl;
	cout << "    S : save file" << endl;
	cout << "    C : clear all data (with confirmation)" << endl;
	cout << endl;
	cout << "    E : toggle edit mode on/off (allows moving boxes)" << endl;
	cout << "         press DEL when box is highlighted to delete it" << endl;
	cout << "    U : undo last box move (if still in edit mode)" << endl;
	cout << endl;
	//cout << "    A : toggle add mode on/off (allows adding nodes along slide)" << endl;
	//cout << "         click on highlighted spline node to insert guide node" << endl;
	//cout << endl;
	//cout << "    N : toggle guide nodes on/off" << endl;
	//cout << "    P : cycle through 8 distinct slider colors" << endl;
	//cout << "    O : make slider thicker" << endl;
	//cout << "    I : make slide thinner" << endl;
	cout << endl;
	cout << "Simulation" << endl;
	cout << "    Q : add a box to the model" << endl;
	cout << "    W : remove a box from the model" << endl;
	cout << "    SPACE : turn gravity on/off" << endl;
	cout << endl;
	cout << "Panning and Zooming" << endl;
	cout << "    Use arrow keys on screen to pan model up/down/left/right" << endl;
	cout << "    Use +/- to zoom into (bigger) and out of (smaller), respectively" << endl;
	cout << "    Z : see all boxes face-on" << endl;
	cout << "        CTRL+mouse to pan, SHIFT+mouse to zoom or use mouse wheel" << endl;
	cout << endl;
}

bool Manager::manage(Camera3D& camera, OrbitingViewer& orbit)
{
	bool boxIsMoving = false;

	int key, mouseEvent, leftButton, middleButton, rightButton;
	int locX, locY, prevLocX, prevLocY;
	double modelX, modelY;

	int wid, hei;
	FsGetWindowSize(wid, hei);

	double vx, vy, vz;

	FsPollDevice();
	key = FsInkey();
	mouseEvent = FsGetMouseEvent(leftButton, middleButton,
		rightButton, locX, locY);

	getModelCoords(modelX, modelY, locX, locY);

	//cout << "mouse X model position: " << modelX << endl;
	//cout << "mouse Y model position: " << modelY << endl;

	/*ComicSansFont comicsans;
	ImpactFont impact;
	comicsans.setColorHSV(300, 1, 1);*/

	if (mouseEvent == FSMOUSEEVENT_LBUTTONDOWN || mouseEvent == FSMOUSEEVENT_MBUTTONDOWN || key == FSKEY_WHEELUP || key == FSKEY_WHEELDOWN) {
		prevLocX = locX; prevLocY = locY;  // capture location of first button press
	}

	// moving boxes
	if (leftButton && theMode == editMode && currBox != nullptr && FsGetKeyState(FSKEY_M)) {
		boxIsMoving = true;  // this will prevent searching for a new node

		// if current box is a selected box and more than 1 selected box, 
		// move all selected boxes by the same amount
		if (selectedBoxes.find(currBox->getLabel()) != selectedBoxes.end() && selectedBoxes.size() > 1) {
			getModelCoords(modelX, modelY, locX, locY);
			// record change in position
			double oldX = currBox->getComX();
			double oldY = currBox->getComY();
			// set current box position
			currBox->setComX(modelX);
			currBox->setComY(modelY);
			// set position of other selected boxes
			for (auto& aSelectedBox : selectedBoxes) {
				if (aSelectedBox.first.compare(currBox->getLabel()) != 0) {
					double aSelectedBoxOldX = aSelectedBox.second->getComX();
					double aSelectedBoxOldY = aSelectedBox.second->getComY();
					aSelectedBox.second->setComX(aSelectedBoxOldX + modelX - oldX);
					aSelectedBox.second->setComY(aSelectedBoxOldY + modelY - oldY);
				}
			}
		}
		// else just move the current Box 
		else {
			getModelCoords(modelX, modelY, locX, locY);
			// set current box position
			currBox->setComX(modelX);
			currBox->setComY(modelY);
		}


		// change it to update by the amount that the mouse moved instead
		//getModelCoords(modelX, modelY, locX, locY); not needed i think?
		/*currBox->setComX(currBox->getComX() + locX - prevLocX);
		currBox->setComY(currBox->getComY() + locY - prevLocY);*/
		//prevLocX = locX; prevLocY = locY; // reset previous values to continue move
	}

	//if (middleButton || (FsGetKeyState(FSKEY_CTRL) && leftButton)) { // pan in x and y axes
	//	prevLocX = locX; prevLocY = locY;
	//	xOrigin += (locX - prevLocX);
	//	yOrigin += (locY - prevLocY);
	//	prevLocX = locX; prevLocY = locY; // reset previous values to continue move
	//}

	//else if (((key == FSKEY_WHEELUP || key == FSKEY_WHEELDOWN) && FsGetKeyState(FSKEY_CTRL)) // these also are triggered by touchpad pinch and two finger scroll
	//	|| (FsGetKeyState(FSKEY_SHIFT) && leftButton)) { // zoom in and out
	//	double oldScale = viewScale;
	//	if (key == FSKEY_WHEELUP || locY < prevLocY)
	//		viewScale *= (zoomFactor - 1) * 0.4 + 1.0; // less jumpty than zooming with +/- keys
	//	else if (key == FSKEY_WHEELDOWN || locY > prevLocY)
	//		viewScale /= (zoomFactor - 1) * 0.4 + 1.0;


	//	xOrigin = (int)round((locX * (oldScale - viewScale)
	//		+ xOrigin * viewScale) / oldScale);
	//	yOrigin = (int)round((locY * (oldScale - viewScale)
	//		+ yOrigin * viewScale) / oldScale);

	//	prevLocX = locX; prevLocY = locY; // reset previous values to continue move
	//}
	if (key == FSKEY_U)
		restoreState();

	if (FsGetKeyState(FSKEY_LEFT)) {
		orbit.h += Camera3D::PI / 180.0;
	}
	if (FsGetKeyState(FSKEY_RIGHT))
		orbit.h -= Camera3D::PI / 180.0;
	if (FsGetKeyState(FSKEY_UP))
		orbit.p += Camera3D::PI / 180.0;
	if (FsGetKeyState(FSKEY_DOWN))
		orbit.p -= Camera3D::PI / 180.0;
	if (FsGetKeyState(FSKEY_F) && orbit.dist > 2) {
		orbit.dist /= 1.05;
		viewScale *= 1.05;

	}

	if (FsGetKeyState(FSKEY_B) && orbit.dist < camera.farZ * .8) {
		orbit.dist *= 1.05;
		viewScale /= 1.05;

	}

	if (FsGetKeyState(FSKEY_J))
		orbit.focusX += 2.;
	if (FsGetKeyState(FSKEY_L))
		orbit.focusX -= 2.;
	if (FsGetKeyState(FSKEY_I))
		orbit.focusY += 2.;
	if (FsGetKeyState(FSKEY_K))
		orbit.focusY -= 2.;

	if (key == FSKEY_Z)
		snapFaceOn(orbit, camera);

	orbit.setUpCamera(camera);

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glViewport(0, 0, wid, hei);

	// Set up 3D drawing
	camera.setUpCameraProjection();
	camera.setUpCameraTransformation();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1, 1);

	glColor3ub(93, 290, 112);


	switch (key) {

	case FSKEY_E: toggleMode();
		break;
	case FSKEY_SPACE: gravityIsOn = !gravityIsOn;
		break;
		//case FSKEY_UP: yOrigin += panChange;
		//	break;
		//case FSKEY_DOWN: yOrigin -= panChange;
		//	break;
		//case FSKEY_LEFT: xOrigin += panChange;
		//	break;
		//case FSKEY_RIGHT: xOrigin -= panChange;
		//	break;

		//case FSKEY_PLUS: viewScale *= zoomFactor;
		//	break;
		//case FSKEY_MINUS: viewScale /= zoomFactor;
		//	break;

	case FSKEY_S: save();
		break;
	case FSKEY_L: load();
		break;
	case FSKEY_Q: addBox(camera, orbit);
		break;
	case FSKEY_W: editBox();
		break;

	}
	// Gladys
	// if in edit mode
	if (theMode == editMode || theMode == rocketBuildMode) {
		double red, green, blue;
		double modelX, modelY;

		// draw a border around the window (commented out because not working)
		/*drawEditModeIndicator();*/


		// figure out if there's a current box
		getModelCoords(modelX, modelY, locX, locY);
		if (!boxIsMoving) {
			currBox = findBox(modelX, modelY, 3);
		}

		// for debugging selecting current box
		//if (currBox != nullptr)
		//	cout << "Current box is box " << currBox->getLabel() << endl;
		//else
		//	cout << "no current box" << endl;



	}

	// Gladys
	// selecting or deselecting boxes
	if (theMode == editMode && mouseEvent == FSMOUSEEVENT_LBUTTONDOWN && currBox != nullptr && !FsGetKeyState(FSKEY_M)) {

		// if not already selected, add to selected
		if (selectedBoxes.find(currBox->getLabel()) == selectedBoxes.end()) {
			selectedBoxes.insert({ currBox->getLabel(), currBox });
		}
		// otherwise, remove it from selected boxes (i.e. deselecting)
		else {
			selectedBoxes.erase(currBox->getLabel());
		}

		// old vector method
		//if (!selectedBoxes.empty()) {
		//	for (auto& aSelectedBox : selectedBoxes) {
		//		// add current box to selected boxes if not already there
		//		if (currBox->getLabel().compare(aSelectedBox->getLabel()) == 0) {
		//			selectedBoxes.push_back(currBox);
		//		}
		//		// otherwise, remove it from selected boxes (i.e. deselecting)
		//		else

		//	}
		//}
		//else
		//	selectedBoxes.push_back(currBox);

		// needed whether it is selecting or deselecting
		currBox->toggleIsHighlighted();
	}

	if (theMode == rocketBuildMode && FsGetKeyState(FSKEY_T)) {
		if (buildRocket())
			cout << "Rocket built!" << endl;
	}



	//// draw boxes
	//snapFaceOn(orbit, camera);
	draw();

	// draw axes
	drawAxes();

	for (auto& currBox : theBoxes) {
		// draw the box
		// update box position if gravity is on
		if (gravityIsOn)
		{
			/*currBox.second.fall(0.025);*/
			currBox.second.fall(0.1);
		}
	}

	FsPollDevice();
	key = FsInkey();
	mouseEvent = FsGetMouseEvent(leftButton, middleButton,
		rightButton, locX, locY);

	// An indicator showing where the mouse location is in the model x-y plane
	getModelCoords(modelX, modelY, locX, locY);

	glBegin(GL_QUADS);

	glVertex3d(modelX - 1, modelY - 1, 0);
	glVertex3d(modelX - 1, modelY + 1, 0);
	glVertex3d(modelX + 1, modelY + 1, 0);
	glVertex3d(modelX + 1, modelY - 1, 0);

	glEnd();
	// Set up 2D drawing (commented out because of lagging)
	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, (float)wid - 1, (float)hei - 1, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);

	comicsans.drawText("I'm Orbiting!", 10, 60, .25);

	std::string data;
	data = "X=" + std::to_string(camera.x) + " Y=" + std::to_string(camera.y) + " Z=" + std::to_string(camera.z);
	comicsans.setColorHSV(300, 1, .5);
	comicsans.drawText(data, 10, 80, .15);

	data = "Camera Orientation: h=" + std::to_string(camera.h * 45. / atan(1.))
		+ " deg, p=" + std::to_string(camera.p * 45. / atan(1.)) + " deg";
	comicsans.drawText(data, 10, 95, .15);*/

	FsSwapBuffers();

	//string inFileName;
	//ifstream inFile;
	//bool nodeIsMoving = false;

	//int mouseEvent, leftButton, middleButton, rightButton;
	//int locX, locY;


	// the following code seems to make it so Q has to be pressed multiple times to add a block
	//FsPollDevice();
	//key = FsInkey();
	//mouseEvent = FsGetMouseEvent(leftButton, middleButton,
	//	rightButton, locX, locY);

	return (key != FSKEY_ESC);
}

void Manager::drawAxes() {
	int length = 30;
	glLineWidth(6);

	glColor3ub(255, 0, 0);  // X (red)
	glBegin(GL_LINES);
	glVertex3i(0, 0, 0);
	glVertex3i(length, 0, 0);
	glEnd();

	glColor3ub(0, 255, 0);  // Y (green)
	glBegin(GL_LINES);
	glVertex3i(0, 0, 0);
	glVertex3i(0, length, 0);
	glEnd();

	glColor3ub(0, 0, 255);  // Z (blue)
	glBegin(GL_LINES);
	glVertex3i(0, 0, 0);
	glVertex3i(0, 0, length);
	glEnd();
}

// Gladys
void Manager::load()
{
	string inFileName;
	ifstream inFile;

	inFileName = getFileFromConsole();

	/*inFileName = getFileFromScreen(allModelfiles,
		"Enter file name of model to load.");*/

		// if user forgets extension, just add it in
	if (inFileName.find(".model") == string::npos)
		inFileName += ".model";

	inFile.open(inFileName);

	if (inFile.is_open()) {
		readFile(inFile);
		inFile.close();
	}
	else
		cout << "Was not able to open " << inFileName << " for input. " << endl;

	// set starting view params
	centerOnScreen();

}
// Gladys
void Manager::readFile(ifstream& inFile)
{
	string currLine, label;
	stringstream currStream;
	double comX, comY, xDim, yDim, hue;
	bool firstTime = true;


	while (!inFile.eof()) {
		getline(inFile, currLine);
		if (currLine != "") {
			currStream.str(currLine);
			currStream >> comX >> comY >> xDim >> yDim >> hue;  // don't care about  >> dist;
			currStream.clear();

			// initialize aggregates
			if (firstTime) {
				minX = maxX = comX;
				minY = maxY = comY;
				firstTime = false;
			}

			label = to_string(boxCounter);


			Box newBox(label, comX, comY, xDim, yDim, hue);

			if (isValidLoc(newBox)) {
				theBoxes.insert({ label, newBox }); // add copy of the node to model

			// take care of aggregate stuff
				if (comX < minX) minX = comX;
				if (comX > maxX) maxX = comX;
				if (comY < minY) minY = comY;
				if (comY > maxY) maxY = comY;

				boxCounter++;
			}

		}

		if (!inFile.eof())  // just to be 100% sure
			getline(inFile, currLine);

	}

}

void Manager::editBox()
{
	FsPollDevice();
	int key = FsInkey();
	int mouseEvent, leftButton, middleButton, rightButton;
	int locX, locY;
	double modelX = 0, modelY = 0;
	mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);
	getModelCoords(modelX, modelY, locX, locY);
	Box* toEdit = findBox(modelX, modelY, 10);
	while (toEdit == nullptr)
	{

		FsPollDevice();

		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);
		getModelCoords(modelX, modelY, locX, locY);
		if (leftButton)
		{
			toEdit = findBox(modelX, modelY, 10);
			if (toEdit == nullptr)
				cout << "nullptr" << endl;
		}


	}
	editBox(*toEdit);
	assignYDistanceFromBelow(*toEdit);

}

void Manager::editBox(Box& toEdit)
{
	FsPollDevice();
	int key = FsInkey();
	int mouseEvent, leftButton, middleButton, rightButton;
	int locX, locY;
	double modelX, modelY;

	double tX, tY, tH, tW, tHue;
	toEdit.getParams(tX, tY, tH, tW, tHue); //returns parameters from Box toEdit

	mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);
	//Set Width
	do
	{
		if (key == FSKEY_WHEELUP)
			toEdit.setWidth(min(toEdit.getWidth() + 1, double(100))); //add max
		else if (key == FSKEY_WHEELDOWN)
			toEdit.setWidth(max(toEdit.getWidth() - 1, double(1))); //add min

		FsPollDevice();
		key = FsInkey();
		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);

		draw();
		drawAxes();
		FsSwapBuffers();


	} while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN);

	if (isValidLoc(toEdit))
	{
		//Set Height
		FsPollDevice();
		key = FsInkey();
		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);
		while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN)
		{

			if (key == FSKEY_WHEELUP)
				toEdit.setHeight(min(toEdit.getHeight() + 1, double(100))); //add max
			else if (key == FSKEY_WHEELDOWN)
				toEdit.setHeight(max(toEdit.getHeight() - 1, double(1))); //add min

			FsPollDevice();
			key = FsInkey();
			mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);

			draw();
			drawAxes();
			FsSwapBuffers();

		}

		FsPollDevice();
		key = FsInkey();
		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);

		if (isValidLoc(toEdit))
		{
			//Set Hue
			while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN)
			{

				if (key == FSKEY_WHEELUP)
					toEdit.setHue(min((toEdit.getHue() + 3), double(360)));
				if (key == FSKEY_WHEELDOWN)
					toEdit.setHue(max((toEdit.getHue() - 3), double(0)));

				FsPollDevice();
				key = FsInkey();
				mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);

				draw();
				drawAxes();
				FsSwapBuffers();

			}

		}
		else
			toEdit.setParams(tX, tY, tH, tW, tHue);
	}
	else
		toEdit.setParams(tX, tY, tH, tW, tHue);

}

void Manager::addBox(Camera3D& camera, OrbitingViewer& orbit)
{
	// remember the current state before making changes
	boxStates.push_back(theBoxes);
	cout << "Adding Box" << endl;
	FsPollDevice();
	int key = FsInkey();
	int mouseEvent, leftButton, middleButton, rightButton;
	int locX, locY;
	double modelX, modelY;
	mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);

	getModelCoords(modelX, modelY, locX, locY);

	double tempDim = 10;
	double tempHue = 0;
	double tempThrust = 5000;
	double tempPropellantMassFlow = 10;
	//Box* currAdd; // pointer to currently adding box
	/*if (theMode == editMode) {*/
	boxCounter++;
	string label = to_string(boxCounter);
	Box toAdd(label, modelX, modelY, tempDim, tempDim, tempHue);
	theBoxes.insert({ toAdd.getLabel(), toAdd });
	auto currAdd = theBoxes.find(toAdd.getLabel());
	/*currAdd = &toAdd;*/
//}
/*else if (theMode == rocketBuildMode && theRocketBoxType == engine) {
	currRocket->incrementEngineBoxCounter();
	string label = to_string(currRocket->getEngineBoxCounter());
	EngineBox toAdd(label, modelX, modelY, tempDim, tempDim, tempHue,
		tempThrust, tempPropellantMassFlow);
	currRocket->addEngineBox(toAdd);
	currAdd = &toAdd;
}
else if (theMode == rocketBuildMode && theRocketBoxType == payload) {
	currRocket->incrementPayloadBoxCounter();
	string label = to_string(currRocket->getPayloadBoxCounter());
	PayloadBox toAdd(label, modelX, modelY, tempDim, tempDim, tempHue);
	currRocket->addPayloadBox(toAdd);
	currAdd = &toAdd;
}
else
	currAdd = nullptr;*/
	//Set Width
	cout << "Setting width" << endl;
	while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN)
	{
		//cout << "Setting width = " << toAdd.getWidth() << endl;
		//cout << "X = " << toAdd.getComX() << " Y = " << toAdd.getComY() << endl;
		getModelCoords(modelX, modelY, locX, locY);


		currAdd->second.setXY(modelX, modelY);
		if (key == FSKEY_WHEELUP)
			currAdd->second.setWidth(min(currAdd->second.getWidth() + 4, double(100))); //add max
		else if (key == FSKEY_WHEELDOWN)
			currAdd->second.setWidth(max(currAdd->second.getWidth() - 4, double(.05))); //add min

		draw();
		drawAxes();
		FsSwapBuffers();

		FsPollDevice();
		key = FsInkey();
		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);

	}


	//Set Height
	cout << "setting height" << endl;
	do
	{

		/*cout << "Setting height = " << currAdd->second.getHeight() << endl;*/
		getModelCoords(modelX, modelY, locX, locY);
		currAdd->second.setXY(modelX, modelY);
		if (key == FSKEY_WHEELUP)
			currAdd->second.setHeight(min(currAdd->second.getHeight() + 4, double(100))); //add max
		else if (key == FSKEY_WHEELDOWN)
			currAdd->second.setHeight(max(currAdd->second.getHeight() - 4, double(1))); //add min

		draw();
		drawAxes();
		FsSwapBuffers();

		FsPollDevice();
		key = FsInkey();
		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);


	} while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN);



	//Set Color (Hue)
	cout << "setting color" << endl;
	do
	{

		getModelCoords(modelX, modelY, locX, locY);
		currAdd->second.setXY(modelX, modelY);
		if (key == FSKEY_WHEELUP)
			currAdd->second.setHue(min((currAdd->second.getHue() + 3), double(360)));
		if (key == FSKEY_WHEELDOWN)
			currAdd->second.setHue(max((currAdd->second.getHue() - 3), double(0)));

		draw();
		drawAxes();
		FsSwapBuffers();

		FsPollDevice();
		key = FsInkey();
		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);


	} while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN);

	//do
	//{
	//	getModelCoords(modelX, modelY, locX, locY);
	//	currAdd->second.setXY(modelX, modelY);
	//} while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN);



	if (!isValidLoc(currAdd->second))
		deleteBox(currAdd->second);
	else {
		assignYDistanceFromBelow(currAdd->second);

	}


}


void Manager::deleteBox()
{
	FsPollDevice();
	int key = FsInkey();
	int mouseEvent, leftButton, middleButton, rightButton;
	int locX, locY;
	double modelX, modelY;
	mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);

	getModelCoords(modelX, modelY, locX, locY);
	Box* toDelete = findBox(modelX, modelY, 2);

	bool res = deleteBox(*toDelete);

	if (res)
		cout << "deleted" << endl;
	else
		cout << "delete failed" << endl;


}

bool Manager::deleteBox(Box& toDelete)
{
	int val = theBoxes.erase(toDelete.getLabel());
	if (val == 0)
		return false;
	else
		return true;
}

void Manager::move(Box aBox)
{
}

bool Manager::isValidLoc(Box& box1)
{
	for (auto& box2 : theBoxes)
	{
		if (box1.getLabel().compare(box2.first) == 0)
		{
			cout << "can't compare box against itself" << endl;
			continue;
		}
		else
		{
			cout << "comparing Box against " << box2.first << endl;
			if (isIntersecting(box1, box2.second, both))
				//if (isIntersecting(box1, box2.second, x))
				return false;
		}
	}
	return true;
}

// Gladys
void Manager::save()
{
	string outFileName;
	ofstream outFile;
	outFileName = getFileFromConsole();
	/*outFileName = getFileFromScreen(allModelfiles,
		"Enter file name to save the model.");*/

		// if user forgets extension, just add it in
	if (outFileName.find(".model") == string::npos)
		outFileName += ".model";

	// open the file
	outFile.open(outFileName);

	if (outFile.is_open()) {
		for (auto& currBox : theBoxes) {
			// call member function of Box that writes box properties into file
			currBox.second.print(outFile);
		}
		outFile.close();
		getAvailableFiles(allModelfiles); // reset list
	}
	else
		cout << "Was not able to open " << outFileName << " for output. " << endl;

}
// Gladys
void Manager::assignYDistanceFromBelow(Box& aBox)
{
	// if on ground or on box (onGround and onBox can be a boolean member variable of Box), return false
	/*if (aBox.isOnBox() || aBox.isOnGround())
		aBox.setYDistanceFromBelow(0.);*/

	double currYMax = 0;
	for (auto& currBox : theBoxes) {
		// loop through all other boxes with max y smaller than min y of the box 
		//not sure if != works properly with strings. I think .compare() is needed
		if (currBox.first.compare(aBox.getLabel()) != 0 && currBox.second.getMaxY() < aBox.getMinY()) {
			// and check for intersection in x- and -z interval with the box in question
			if (isIntersecting(currBox.second, aBox, x)) {
				if (currBox.second.getMaxY() > currYMax)

					currYMax = currBox.second.getMaxY();
			}

		}
	}
	aBox.setYDistanceFromBelow(aBox.getMinY() - currYMax);
}

// Gladys
Box* Manager::findBox(double x, double y, double distance)
{
	double minX = x - distance, maxX = x + distance;
	double minY = y - distance, maxY = y + distance;
	double currX, currY;
	for (unordered_map<string, Box>::iterator it = theBoxes.begin();
		it != theBoxes.end(); it++) {
		currX = it->second.getComX();
		currY = it->second.getComY();
		if (minX < currX && currX < maxX && minY < currY && currY < maxY)
			return &(it->second);
	}

	return nullptr;

	//// iterate through theBoxes
	//for (auto& currBox : theBoxes) {
	//	// compute distance between the mouse and the Box
	//	double mouseXDistance = currBox.second.getComX();
	//	double mouseYDistance = currBox.second.getComY();

	//	// if the Box's x- or y- distance from a node is less than given distance, return that Box
	//	if (abs(mouseXDistance) < distance && abs(mouseYDistance) < distance) {
	//		return &(currBox.second);
	//	}
	//}

	//return nullptr;
}
// Gladys
Box* Manager::findBox(const string& givenLabel)
{
	string searchFor = StringPlus::trim(givenLabel);

	// get pointer to a key-item pair
	auto result = theBoxes.find(searchFor);
	if (result != theBoxes.end())
		return &(result->second);

	return nullptr;
}
// Gladys
string Manager::getFileFromScreen(vector<string>& availableFiles, const string& prompt)
{
	int adjustLetter;
	int key;
	string fileName = "";

	FsPollDevice();
	key = FsInkey();
	while (key != FSKEY_ESC && key != FSKEY_ENTER) {
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		// ask for file name from the graphics window
		glColor3f(1, 0, 0);
		glRasterPos2d(140, 200);
		YsGlDrawFontBitmap16x20(prompt.c_str());
		glRasterPos2d(160, 225);
		YsGlDrawFontBitmap12x16("Press ENTER when done, ESC to cancel.");
		glColor3ub(255, 0, 255);
		DrawingUtilNG::drawRectangle(140, 235, 450, 50, false);
		cout << 'b';

		// show list of available files (need C++17 to work, set in project props)
		glRasterPos2d(440, 330);
		YsGlDrawFontBitmap12x16("Available Files:");
		for (int i = 0; i < availableFiles.size(); i++) {
			glRasterPos2d(460, 350 + i * 20);
			YsGlDrawFontBitmap12x16(availableFiles.at(i).c_str());
		}

		// build filename from keyboard entry, letter by letter
		DrawingUtilNG::buildStringFromFsInkey(key, fileName);

		fileName += "_"; // add an underscore as prompt
		glRasterPos2i(165, 275);  // sets position
		YsGlDrawFontBitmap16x24(fileName.c_str());
		fileName = fileName.substr(0, fileName.length() - 1); // remove underscore

		FsSwapBuffers();
		FsSleep(25);

		FsPollDevice();
		key = FsInkey();
	}

	if (key == FSKEY_ENTER) {
		glColor3f(1, 0, 0);
		glRasterPos2d(140, 400);
		YsGlDrawFontBitmap16x20("Loading . . .");

		FsSwapBuffers(); // this keeps the other stuff on because the previous buffer had it too
		return fileName;
	}
	else
		return "";

}

string Manager::getFileFromConsole()
{
	glColor3f(0, 0, 0);
	glRasterPos2d(150, 200);
	YsGlDrawFontBitmap20x28("Input required on console . . .");
	FsSwapBuffers();

	string longInput;
	cout << endl << "            Name of file to load (.model) >> ";
	getline(cin, longInput);

	showMenu(); // So that it is "fresh"

	return StringPlus::trim(longInput);
}

string Manager::getRocketLabelFromConsole()
{
	string longInput;
	cout << endl << "            Name of your rocket >> ";
	getline(cin, longInput);

	showMenu(); // So that it is "fresh"

	return StringPlus::trim(longInput);
}

// Gladys
void Manager::getAvailableFiles(vector<string>& availableFiles)
{
	// need C++17 to work (set in project properties)
	//     Configuration Properties -> General -> C++ Language Standard
	for (const auto& entry : std::filesystem::directory_iterator(".")) {

		wstring ws(entry.path().c_str());
		string currFileName(ws.begin(), ws.end());

		if (currFileName.find(".model") != string::npos)
			availableFiles.push_back(currFileName.substr(2));
	}
}
// Gladys
void Manager::centerOnScreen()
{
	double scaleX = WIN_WIDTH / (maxX - minX);
	double scaleY = WIN_HEIGHT / (maxY - minY);
	viewScale = min(scaleX, scaleY) * 0.95;   // leaves a little bit of white space all around
	xOrigin = WIN_WIDTH / 2 - viewScale * (maxX + minX) / 2;
	yOrigin = WIN_HEIGHT / 2 + viewScale * (maxY + minY) / 2;
}
// Gladys
void Manager::drawEditModeIndicator()
{
	glLineWidth(4);
	glColor3ub(10, 255, 10);
	DrawingUtilNG::drawRectangle(2, 2, WIN_WIDTH - 5, WIN_HEIGHT - 5, false);
	DrawingUtilNG::drawRectangle(0, WIN_HEIGHT, 100, -20, true);

	glLineWidth(1);
	glColor3ub(100, 100, 100);
	glRasterPos2i(10, WIN_HEIGHT - 5);
	YsGlDrawFontBitmap8x12("Edit Mode");

}
// Gladys
//void Manager::highlightBox(Box& aBox)
//{
//	glLineWidth(3);
//	glColor3ub(0, 255, 0);
//	//aBox.draw();
//	glLineWidth(1);
//
//}

void Manager::draw()
{
	double red, green, blue;
	/*
	for (int i = 0; i < theBoxes.size(); i++) {
		//Get color
		DrawingUtilNG::hsv2rgb(theBoxes[i].getHue(), 1, 1, red, green, blue);
		glColor3f(red, green, blue);
		//Draw boxes
		theBoxes[i].draw();
	}
	*/
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	// draw ordinary boxes
	for (unordered_map<string, Box>::iterator it = theBoxes.begin(); it != theBoxes.end(); it++) {
		//Get color
		/*DrawingUtilNG::hsv2rgb(it->second.getHue(), 1, 1, red, green, blue);
		glColor3f(red, green, blue);*/
		// Draw boxes
		//it->second.draw();
		//double screenX, screenY, screenW, screenH;
		//getScreenCoords(it->second.getComX(), it->second.getComY(), screenX, screenY);
		//screenW = it->second.getWidth() * viewScale;
		//screenH = it->second.getHeight() * viewScale;
		//DrawingUtilNG::drawRectangle3D(it->second.getLeftUpperX(), it->second.getLeftLowerY(), 
		//	it->second.getWidth(), it->second.getHeight(), it->second.getHue(), it->second.getIsHighlighted());
		DrawingUtilNG::drawCube(it->second.getLeftUpperX(), it->second.getLeftLowerY(), 0, it->second.getRightUpperX(), it->second.getRightUpperY(), -10, it->second.getHue(), it->second.getIsHighlighted());
	}

	// draw the boxes of each rocket
	// loop through rockets
	for (auto& aRocket : theRockets) {
		// loop through engine boxes of each rocket
		for (auto& currEngineBox : *(aRocket.second->getTheEngineBoxes())) {
			DrawingUtilNG::drawCube(currEngineBox.second->getLeftUpperX(), currEngineBox.second->getLeftLowerY(),
				0, currEngineBox.second->getRightUpperX(), currEngineBox.second->getRightUpperY(),
				-10, currEngineBox.second->getHue(), currEngineBox.second->getIsHighlighted());
		}

	}


}



void Manager::getModelCoords(double& modelX, double& modelY, double screenX, double screenY)
{
	modelX = (screenX - xOrigin) / viewScale;
	modelY = (screenY - yOrigin) / -viewScale;
}

void Manager::getScreenCoords(double modelX, double modelY, double& screenX, double& screenY)
{
	screenX = modelX * viewScale + xOrigin;
	screenY = modelY * -viewScale + yOrigin;

}

void Manager::snapFaceOn(OrbitingViewer& orbit, Camera3D& camera)
{
	orbit.initialize();
	orbit.setUpCamera(camera);
}

void Manager::updateModelCom(Box& newBox)
{
	int numBoxes = (int)theBoxes.size();
	modelComX = (modelComX * (numBoxes - 1) + newBox.getComX()) / numBoxes;
	modelComY = (modelComY * (numBoxes - 1) + newBox.getComY()) / numBoxes;
}

void Manager::storeState(std::unordered_map<std::string, Box> theBoxes) {
	// store a maximum of five states
	if (boxStates.size() == 5)
		boxStates.erase(boxStates.begin());
	boxStates.push_back(theBoxes);
}

void Manager::restoreState() {
	if (!boxStates.empty()) {
		theBoxes = boxStates.back();
		boxStates.pop_back();
	}


}

bool Manager::buildRocket()
{
	/*Rocket* currRocket;*/
	int key, mouseEvent, leftButton, middleButton, rightButton;
	int locX, locY, prevLocX, prevLocY;
	double modelX, modelY;
	char userChoice;
	// edit existing rocket or build new one?
	cout << "Do you want to edit existing rocket (E) or build a new one (N)?" << endl;
	cin >> userChoice;
	if (userChoice == 'e' || userChoice == 'E') {
		string rocketChoice;
		printAvailableRockets();
		cout << "Type the name of the rocket you want to edit" << endl;
		cin >> rocketChoice;
		// set the current rocket 
		if (theRockets.find(rocketChoice) != theRockets.end()) {
			currRocket = theRockets.find(rocketChoice)->second;
		}
		else {
			cout << "Your choice does not match any existing rocket" << endl;
			return false;
		}

	}
	else if (userChoice == 'n' || userChoice == 'N') {
		// get a rocket name from the user
		string newRocketLabel;
		newRocketLabel = getRocketLabelFromConsole();
		// initialize the rocket, set it to current and add to theRockets
		currRocket = new Rocket(newRocketLabel);

		theRockets.insert({ newRocketLabel, currRocket });
		currRocket = theRockets.find(newRocketLabel)->second;
	}
	else {
		cout << "Your input is not one of the choices. Returning to main menu" << endl;
		return false;
	}

	cout << "The rocket to be edited is " << currRocket->getLabel() << endl;

	return editRocketComponents(*currRocket);

	//FsPollDevice();
	//key = FsInkey();
	//mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);


//// if no rocket present, build the first one
//if (theRockets.size() == 0) {
//	// get a rocket name from the user
//	currRocketLabel = getRocketLabelFromConsole();
//	// initialize the rocket, set it to current and add to theRockets
//	Rocket newRocket(currRocketLabel);
//	currRocket = &newRocket;
//	theRockets.insert({ currRocketLabel, &newRocket });
//
//	// add an Engine box
//	theRocketBoxType = engine;
//	cout << "Creating an Engine Box" << endl;
//	addBox(camera, orbit);
//
//	// add a Payload box
//	theRocketBoxType = payload;
//	cout << "Creating an Payload Box" << endl;
//	addBox(camera, orbit);
//
//}
}

void Manager::printAvailableRockets()
{
	cout << "The available rockets are: " << endl;
	for (auto& currRocket : theRockets) {
		cout << currRocket.first << endl;
	}
}

bool Manager::editRocketComponents(Rocket& theRocket) {
	bool finishBuildRocket = false;
	char operationChoice;
	char boxChoice;
	while (!finishBuildRocket) {
		cout << "Do you want to add (A), delete (D) or edit (E) a box?" << endl;
		cin >> operationChoice;
		if (operationChoice == 'a' || operationChoice == 'A') {
			cout << "Do you want to add engineBox (E) or payloadBox (P)?" << endl;
			cin >> boxChoice;
			if (boxChoice == 'e' || boxChoice == 'E') {
				// returns true if successfully make engineBox
				if (EngineBox* newEngineBox = theRocket.makeEngineBox(*this))
					return true;
				else
					return false;
			}
			else if (boxChoice == 'p' || boxChoice == 'P') {
				// returns once successfully make payloadBox
				return makePayloadBox();
			}
		}
	}
}

//bool Manager::makeEngineBox() {
//	FsPollDevice();
//	int key = FsInkey();
//	int mouseEvent, leftButton, middleButton, rightButton;
//	int locX, locY;
//	double modelX, modelY;
//	mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);
//
//	getModelCoords(modelX, modelY, locX, locY);
//
//	double tempDim = 10;
//	double tempHue = 0;
//	double tempThrust = 5000;
//	double tempPropellantMassFlow = 10;
//	EngineBox* currAdd;
//
//	//create a label for the engine box
//	currRocket->incrementEngineBoxCounter();
//	string label = to_string(currRocket->getEngineBoxCounter());
//
//	// make the box and add to current rocket
//	EngineBox toAdd(label, modelX, modelY, tempDim, tempDim, tempHue,
//		tempThrust, tempPropellantMassFlow);
//	currRocket->addEngineBox(toAdd);
//	currAdd = currRocket->getEngineBox(label);
//
//	cout << "Setting width" << endl;
//	while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN)
//	{
//		//cout << "Setting width = " << toAdd.getWidth() << endl;
//		//cout << "X = " << toAdd.getComX() << " Y = " << toAdd.getComY() << endl;
//		getModelCoords(modelX, modelY, locX, locY);
//
//
//		currAdd->setXY(modelX, modelY);
//		if (key == FSKEY_WHEELUP)
//			currAdd->setWidth(min(currAdd->getWidth() + 4, double(100))); //add max
//		else if (key == FSKEY_WHEELDOWN)
//			currAdd->setWidth(max(currAdd->getWidth() - 4, double(.05))); //add min
//
//		draw();
//		drawAxes();
//		FsSwapBuffers();
//
//		FsPollDevice();
//		key = FsInkey();
//		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);
//
//	}
//
//
//	//Set Height
//	cout << "setting height" << endl;
//	do
//	{
//
//		/*cout << "Setting height = " << currAdd->second.getHeight() << endl;*/
//		getModelCoords(modelX, modelY, locX, locY);
//		currAdd->setXY(modelX, modelY);
//		if (key == FSKEY_WHEELUP)
//			currAdd->setHeight(min(currAdd->getHeight() + 4, double(100))); //add max
//		else if (key == FSKEY_WHEELDOWN)
//			currAdd->setHeight(max(currAdd->getHeight() - 4, double(1))); //add min
//
//		draw();
//		drawAxes();
//		FsSwapBuffers();
//
//		FsPollDevice();
//		key = FsInkey();
//		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);
//
//
//	} while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN);
//
//
//
//	//Set Color (Hue)
//	cout << "setting color" << endl;
//	do
//	{
//
//		getModelCoords(modelX, modelY, locX, locY);
//		currAdd->setXY(modelX, modelY);
//		if (key == FSKEY_WHEELUP)
//			currAdd->setHue(min((currAdd->getHue() + 3), double(360)));
//		if (key == FSKEY_WHEELDOWN)
//			currAdd->setHue(max((currAdd->getHue() - 3), double(0)));
//
//		draw();
//		drawAxes();
//		FsSwapBuffers();
//
//		FsPollDevice();
//		key = FsInkey();
//		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);
//
//
//	} while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN);
//
//	//sets thrust
//	cout << "setting thrust" << endl;
//	do
//	{
//		getModelCoords(modelX, modelY, locX, locY);
//		currAdd->setXY(modelX, modelY);
//
//		if (key == FSKEY_WHEELUP)
//			currAdd->setThrust(min((currAdd->getThrust() + 100), 10000.));
//		if (key == FSKEY_WHEELDOWN)
//			currAdd->setThrust(max((currAdd->getThrust() - 100), 100.));
//			
//		// could change to better visual feedback later 
//		cout << "current thrust = " << currAdd->getThrust() << endl;
//
//		draw();
//		drawAxes();
//		FsSwapBuffers();
//
//		FsPollDevice();
//		key = FsInkey();
//		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);
//
//
//	} while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN);
//
//
//	//sets propellant mass flow if EngineBox
//	
//	cout << "setting propellant mass flow" << endl;
//	do
//	{
//		getModelCoords(modelX, modelY, locX, locY);
//		currAdd->setXY(modelX, modelY);
//
//		if (key == FSKEY_WHEELUP)
//			currAdd->setPropellantMassFlow(min((currAdd->getPropellantMassFlow() + 10), 100.));
//		if (key == FSKEY_WHEELDOWN)
//			currAdd->setPropellantMassFlow(max((currAdd->getPropellantMassFlow() + 10), 10.));
//
//		// could change to better visual feedback later 
//		cout << "current propellant mass flow = " << currAdd->getPropellantMassFlow() << endl;
//
//		draw();
//		drawAxes();
//		FsSwapBuffers();
//
//		FsPollDevice();
//		key = FsInkey();
//		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);
//
//
//	} while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN);
//
//	return true;
//}

bool Manager::makePayloadBox() {
	FsPollDevice();
	int key = FsInkey();
	int mouseEvent, leftButton, middleButton, rightButton;
	int locX, locY;
	double modelX, modelY;
	mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);

	getModelCoords(modelX, modelY, locX, locY);

	double tempDim = 10;
	double tempHue = 0;

	PayloadBox* currAdd;

	//create a label for the engine box
	currRocket->incrementPayloadBoxCounter();
	string label = to_string(currRocket->getPayloadBoxCounter());

	// make the box and add to current rocket
	PayloadBox toAdd(label, modelX, modelY, tempDim, tempDim, tempHue);
	currRocket->addPayloadBox(toAdd);
	currAdd = currRocket->getPayloadBox(label);

	cout << "Setting width" << endl;
	while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN)
	{
		//cout << "Setting width = " << toAdd.getWidth() << endl;
		//cout << "X = " << toAdd.getComX() << " Y = " << toAdd.getComY() << endl;
		getModelCoords(modelX, modelY, locX, locY);


		currAdd->setXY(modelX, modelY);
		if (key == FSKEY_WHEELUP)
			currAdd->setWidth(min(currAdd->getWidth() + 4, double(100))); //add max
		else if (key == FSKEY_WHEELDOWN)
			currAdd->setWidth(max(currAdd->getWidth() - 4, double(.05))); //add min

		draw();
		drawAxes();
		FsSwapBuffers();

		FsPollDevice();
		key = FsInkey();
		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);

	}


	//Set Height
	cout << "setting height" << endl;
	do
	{

		/*cout << "Setting height = " << currAdd->second.getHeight() << endl;*/
		getModelCoords(modelX, modelY, locX, locY);
		currAdd->setXY(modelX, modelY);
		if (key == FSKEY_WHEELUP)
			currAdd->setHeight(min(currAdd->getHeight() + 4, double(100))); //add max
		else if (key == FSKEY_WHEELDOWN)
			currAdd->setHeight(max(currAdd->getHeight() - 4, double(1))); //add min

		draw();
		drawAxes();
		FsSwapBuffers();

		FsPollDevice();
		key = FsInkey();
		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);


	} while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN);



	//Set Color (Hue)
	cout << "setting color" << endl;
	do
	{

		getModelCoords(modelX, modelY, locX, locY);
		currAdd->setXY(modelX, modelY);
		if (key == FSKEY_WHEELUP)
			currAdd->setHue(min((currAdd->getHue() + 3), double(360)));
		if (key == FSKEY_WHEELDOWN)
			currAdd->setHue(max((currAdd->getHue() - 3), double(0)));

		draw();
		drawAxes();
		FsSwapBuffers();

		FsPollDevice();
		key = FsInkey();
		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);


	} while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN);
	return true;
}