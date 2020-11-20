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


using namespace std;

Manager::Manager()
{
	editModeIsOn = false;
	gravityIsOn = true;

	boxCounter = 0;
	
	xOrigin = 0;
	yOrigin = WIN_HEIGHT;

	maxX = 0;
	maxY = 0;
	minX = 0;
	minY = 0;

	currBox = nullptr;

	viewScale = 1.0;
	panChange = 10;
	zoomFactor = 1.1;

	getAvailableFiles(allModelfiles);

	

}

bool Manager::isIntersecting(Box& firstBox, Box& secondBox, overlappingDimension theDimension)
{
	double firstXInterval[] = { firstBox.getLeftUpperX(),  firstBox.getRightUpperX() };
	double firstYInterval[] = { firstBox.getLeftUpperY(),  firstBox.getLeftLowerY() };

	double secondXInterval[] = { secondBox.getLeftUpperX(),  secondBox.getRightUpperX() };
	double secondYInterval[] = { secondBox.getLeftUpperY(),  secondBox.getLeftLowerY() };

	if (theDimension == both) {
		// Compare x-intervals, y-intervals and z-intervals spanned by the boxes. 
		// If all three dimensions intersect, there is an intersection
		if (((firstXInterval[0] > secondXInterval[0] && firstXInterval[0] < secondXInterval[1]) ||
			(firstXInterval[1] > secondXInterval[0] && firstXInterval[1] < secondXInterval[1])) &&
			((firstYInterval[0] > secondYInterval[0] && firstYInterval[0] < secondYInterval[1]) ||
				(firstYInterval[1] > secondYInterval[0] && firstYInterval[1] < secondYInterval[1]))) {
			return true;
		}
	}

	else if (theDimension == x) {
		if ((firstXInterval[0] > secondXInterval[0] && firstXInterval[0] < secondXInterval[1]) ||
			(firstXInterval[1] > secondXInterval[0] && firstXInterval[1] < secondXInterval[1])) {
			return true;
		}
	}

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
	cout << "    Z : zoom-all so that model is centered" << endl;
	cout << "        CTRL+mouse to pan, SHIFT+mouse to zoom or use mouse wheel" << endl;
	cout << endl;
}

bool Manager::manage()
{
	bool boxIsMoving = false;
	FsPollDevice();
	int key, mouseEvent, leftButton, middleButton, rightButton;
	int locX, locY, prevLocX, prevLocY;

	key = FsInkey();
	mouseEvent = FsGetMouseEvent(leftButton, middleButton,
			rightButton, locX, locY);

	if (middleButton || (FsGetKeyState(FSKEY_CTRL) && leftButton)) { // pan in x and y axes
		
		xOrigin += (locX - prevLocX);
		yOrigin += (locY - prevLocY);
		prevLocX = locX; prevLocY = locY; // reset previous values to continue move
	}

	else if (key == FSKEY_WHEELUP || key == FSKEY_WHEELDOWN // these also are triggered by touchpad pinch and two finger scroll
		|| (FsGetKeyState(FSKEY_SHIFT) && leftButton)) { // zoom in and out
		double oldScale = viewScale;
		if (key == FSKEY_WHEELUP || locY < prevLocY)
			viewScale *= (zoomFactor - 1) * 0.4 + 1.0; // less jumpty than zooming with +/- keys
		else if (key == FSKEY_WHEELDOWN || locY > prevLocY)
			viewScale /= (zoomFactor - 1) * 0.4 + 1.0;

	
		xOrigin = (int)round((locX * (oldScale - viewScale)
			+ xOrigin * viewScale) / oldScale);
		yOrigin = (int)round((locY * (oldScale - viewScale)
			+ yOrigin * viewScale) / oldScale);

		prevLocX = locX; prevLocY = locY; // reset previous values to continue move
	}


	switch (key) {

	case FSKEY_E: editModeIsOn = !editModeIsOn;
		break;

	case FSKEY_UP: yOrigin += panChange;
		break;
	case FSKEY_DOWN: yOrigin -= panChange;
		break;
	case FSKEY_LEFT: xOrigin += panChange;
		break;
	case FSKEY_RIGHT: xOrigin -= panChange;
		break;

	case FSKEY_PLUS: viewScale *= zoomFactor;
		break;
	case FSKEY_MINUS: viewScale /= zoomFactor;
		break;

	case FSKEY_Q: addBox();
		break;
	case FSKEY_W: deleteBox();
		break;

	}

	// if in edit mode
	if (editModeIsOn) {
		double red, green, blue;
		double modelX, modelY;

		// draw a border around the window
		drawEditModeIndicator();

		// figure out if there's a box to highlight
		if (!boxIsMoving) {
			double modelX, modelY;
			getModelCoords(modelX, modelY, locX, locY);
			currBox = findBox(modelX, modelY, 10 / viewScale);
		}

		if (currBox != nullptr) {
			highlightBox(*currBox);		
		}
	}


	// selecting boxes
	if (editModeIsOn && mouseEvent == FSMOUSEEVENT_LBUTTONDOWN && currBox != nullptr) {
		// add current box to selected boxes if not already there
		if (!selectedBoxes.empty()) {
			for (auto& aSelectedBox : selectedBoxes)
				if (currBox->getLabel().compare(aSelectedBox->getLabel()) == 0)
					selectedBoxes.push_back(currBox);
		}
		else
			selectedBoxes.push_back(currBox);
	}

	// moving boxes
	if (leftButton && editModeIsOn && currBox != nullptr) {
		boxIsMoving = true;  // this will prevent searching for a new node

		double modelX, modelY;
		// change it to update by the amount that the mouse moved instead
		//getModelCoords(modelX, modelY, locX, locY); not needed i think?
		currBox->setComX(currBox->getComX() + locX - prevLocX);
		currBox->setComY(currBox->getComY() + locY - prevLocY);
		prevLocX = locX; prevLocY = locY; // reset previous values to continue move
	}

	// draw boxes
	for (auto& currBox : theBoxes) {
		// draw the box
		// update box position if gravity is on
		if (gravityIsOn)
			currBox.second.fall(0.025);
	}

	FsSwapBuffers();

	//string inFileName;
	//ifstream inFile;
	//bool nodeIsMoving = false;

	//int mouseEvent, leftButton, middleButton, rightButton;
	//int locX, locY;

	

	FsPollDevice();
	key = FsInkey();
	mouseEvent = FsGetMouseEvent(leftButton, middleButton,
		rightButton, locX, locY);

	return (key != FSKEY_ESC);
}




void Manager::load()
{
	string inFileName;
	ifstream inFile;

	inFileName = getFileFromScreen(allModelfiles,
		"Enter file name of model to load.");

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
	double modelX, modelY;
	mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);

	getModelCoords(modelX, modelY, locX, locY);
	Box toEdit = *findBox(modelX, modelY, 2);

	editBox(toEdit);

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
	while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN)
	{
		if (key == FSKEY_WHEELUP)
			toEdit.setWidth(min(toEdit.getWidth() + 1, double(100))); //add max
		else if (key == FSKEY_WHEELDOWN)
			toEdit.setWidth(max(toEdit.getWidth() - 1, double(1))); //add min

		FsPollDevice();
		key = FsInkey();
		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);
	}

	if (isValidLoc(toEdit))
	{
		//Set Height
		FsPollDevice();
		key = FsInkey();
		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);
		while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN);
		{

			if (key == FSKEY_WHEELUP)
				toEdit.setHeight(min(toEdit.getHeight() + 1, double(100))); //add max
			else if (key == FSKEY_WHEELDOWN)
				toEdit.setHeight(max(toEdit.getHeight() - 1, double(1))); //add min

			FsPollDevice();
			key = FsInkey();
			mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);
		}

		FsPollDevice();
		key = FsInkey();
		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);

		if (isValidLoc(toEdit))
		{
			//Set Hue
			while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN);
			{

				if (key == FSKEY_WHEELUP)
					toEdit.setHue(min((toEdit.getHue() + 3), double(360)));
				if (key == FSKEY_WHEELDOWN)
					toEdit.setHue(max((toEdit.getHue() - 3), double(0)));

				FsPollDevice();
				key = FsInkey();
				mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);
			}

		}
		else
			toEdit.setParams(tX, tY, tH, tW, tHue);
	}
	else
		toEdit.setParams(tX, tY, tH, tW, tHue);

}

void Manager::addBox()
{
	
	FsPollDevice();
	int key = FsInkey();
	int mouseEvent, leftButton, middleButton, rightButton;
	int locX, locY;
	double modelX, modelY;
	mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);

	getModelCoords(modelX, modelY, locX, locY);
	string label = to_string(boxCounter);
	double tempDim = 10;
	double tempHue = 0;
	Box toAdd(label, modelX, modelY, tempDim, tempDim, tempHue);
	theBoxes.insert({ toAdd.getLabel(), toAdd });

	//Set Width
	while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN)
	{
		getModelCoords(modelX, modelY, locX, locY);
		toAdd.setXY(modelX, modelY);
		if (key == FSKEY_WHEELUP)
			toAdd.setWidth(min(toAdd.getWidth() + 1, double(100))); //add max
		else if (key == FSKEY_WHEELDOWN)
			toAdd.setWidth(max(toAdd.getWidth() - 1, double(1))); //add min

		FsPollDevice();
		key = FsInkey();
		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);
	}

	FsPollDevice();
	key = FsInkey();
	mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);

	//Set Height
	while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN);
	{
		getModelCoords(modelX, modelY, locX, locY);
		toAdd.setXY(modelX, modelY);
		if (key == FSKEY_WHEELUP)
			toAdd.setHeight(min(toAdd.getHeight() + 1, double(100))); //add max
		else if (key == FSKEY_WHEELDOWN)
			toAdd.setHeight(max(toAdd.getHeight() - 1, double(1))); //add min

		FsPollDevice();
		key = FsInkey();
		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);
	}

	FsPollDevice();
	key = FsInkey();
	mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);

	//Set Color (Hue)
	while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN);
	{
		getModelCoords(modelX, modelY, locX, locY);
		toAdd.setXY(modelX, modelY);
		if (key == FSKEY_WHEELUP)
			toAdd.setHue(min((toAdd.getHue() + 3), double(360)));
		if (key == FSKEY_WHEELDOWN)
			toAdd.setHue(max((toAdd.getHue() - 3), double(0)));

		FsPollDevice();
		key = FsInkey();
		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);
	}

	while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN)
		//add loop to select final location

		if (!isValidLoc(toAdd))
			deleteBox(toAdd);

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
			continue;
		else
		{
			if (isIntersecting(box1, box2.second, both))
				return false;
		}
	}
	return true;
}


void Manager::save()
{
	string outFileName;
	ofstream outFile;
	outFileName = getFileFromScreen(allModelfiles,
		"Enter file name to save the model.");

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

void Manager::assignYDistanceFromBelow(Box& aBox)
{
	// if on ground or on box (onGround and onBox can be a boolean member variable of Box), return false
	if (aBox.isOnBox() || aBox.isOnGround())
		aBox.setYDistanceFromBelow(0.);
	else {
		double currYMax = 0.;
		for (auto& currBox : theBoxes) {
			// loop through all other boxes with max y smaller than min y of the box 
			//not sure if != works properly with strings. I think .compare() is needed
			if (currBox.first != aBox.getLabel() && currBox.second.getMaxY() < aBox.getMinY()) {
				// and check for intersection in x- and -z interval with the box in question
				if (isIntersecting(currBox.second, aBox, x)) {
					if (currBox.second.getMaxY() > currYMax)
						currYMax = currBox.second.getMaxY();
				}

			}
		}
		aBox.setYDistanceFromBelow(currYMax);
	}

}

Box* Manager::findBox(double x, double y, double distance)
{
	// iterate through theBoxes
	for (auto& currBox : theBoxes) {
		// compute distance between the mouse and the Box
		double mouseXDistance = currBox.second.getComX();
		double mouseYDistance = currBox.second.getComY();

		// if the Box's x- or y- distance from a node is less than given distance, return that Box
		if (abs(mouseXDistance) < distance && abs(mouseYDistance) < distance) {
			return &(currBox.second);
		}
	}

	return nullptr;
}

Box* Manager::findBox(const string& givenLabel)
{
	string searchFor = StringPlus::trim(givenLabel);

	// get pointer to a key-item pair
	auto result = theBoxes.find(searchFor);
	if (result != theBoxes.end())
		return &(result->second);

	return nullptr;
}

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

void Manager::getAvailableFiles(vector<string>& availableFiles)
{
	availableFiles.clear();
	// need C++17 to work (set in project properties)
	//     Configuration Properties -> General -> C++ Language Standard
	for (const auto& entry : std::filesystem::directory_iterator(".")) {

		wstring ws(entry.path().c_str());
		string currFileName(ws.begin(), ws.end());

		if (currFileName.find(".slide") != string::npos)
			availableFiles.push_back(currFileName.substr(2));
	}
}

void Manager::centerOnScreen()
{
	double scaleX = WIN_WIDTH / (maxX - minX);
	double scaleY = WIN_HEIGHT / (maxY - minY);
	viewScale = min(scaleX, scaleY) * 0.95;   // leaves a little bit of white space all around
	xOrigin = WIN_WIDTH / 2 - viewScale * (maxX + minX) / 2;
	yOrigin = WIN_HEIGHT / 2 + viewScale * (maxY + minY) / 2;
}

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

void Manager::highlightBox(Box& aBox)
{
	glLineWidth(3);
	glColor3ub(0, 255, 0);
	aBox.draw();
	glLineWidth(1);

}

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
	for (unordered_map<string, Box>::iterator it = theBoxes.begin(); it != theBoxes.end(); it++) {
		//Get color
		DrawingUtilNG::hsv2rgb(it->second.getHue(), 1, 1, red, green, blue);
		glColor3f(red, green, blue);
		// Draw boxes
		it->second.draw();

	}

}


void Manager::getModelCoords(double& modelX, double& modelY, double screenX, double screenY)
{
	modelX = (screenX - xOrigin) / viewScale;
	modelY = (screenY - yOrigin) / -viewScale;
}
