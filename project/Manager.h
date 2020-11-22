#pragma once
#include <stack> 
#include <unordered_map>
#include <vector>
#include "Box.h"
#include "StringPlus.h"
#include "Camera3D.h"
#include "OrbitingViewer.h"

#define WIN_WIDTH 1200
#define WIN_HEIGHT 800

enum overlappingDimension {x, both};

using namespace std;

class Manager {

private:
	bool editModeIsOn;
	//bool simulatorIsOn;
	bool gravityIsOn;
	std::unordered_map<std::string, Box> theBoxes;
	
	// for determining the label of the next added box 
	// (will be changed to string when constructing the box)

	// to help with file names
	std::vector<std::string> allModelfiles;

	int xOrigin, yOrigin;  // screen coords of model coords 0,0
	double viewScale, zoomFactor, panChange;  // must be greater than zero

	// when reading in boxes, need to assign these
	double minX, maxX, minY, maxY, groundY;

	Box* currBox;

	vector<Box*> selectedBoxes;

	int boxCounter;

	double modelComX, modelComY, modelComZ;

	vector<std::unordered_map<std::string, Box>> boxStates;

public:
	Manager();

	bool isIntersecting(Box& firstBox, Box& secondBox, overlappingDimension theDimension);
	// returns true if two boxes are intersecting

	void toggleEditMode() {
		editModeIsOn = !editModeIsOn;
	}

	/*void toggleSimulatorIsOn() {
		simulatorIsOn = !simulatorIsOn;
	}*/

	void toggleGravityIsOn() {
		gravityIsOn = !gravityIsOn;
	}

	void showMenu();

	bool manage(Camera3D& camera, OrbitingViewer& orbit);

	void drawAxes();

	void addBox(Camera3D& camera, OrbitingViewer& orbit);

	void editBox();

	void editBox(Box& toEdit);

	void deleteBox();

	bool deleteBox(Box& toDelete);

	bool isValidLoc(Box& box1);

	void move(Box aBox);

	void load();

	void save();

	void assignYDistanceFromBelow(Box& aBox);

	Box* findBox(double x, double y, double distance);
	// given model coordinates x,y , function returns the first Box that is
	// within given distance
	// returns nullptr if no Box meets the criteria

	Box* findBox(const string& givenLabel);
	// given box label, return the box
	// potentially used when several boxes are selected to be edited together

	
	std::string getFileFromScreen(std::vector<std::string>& availableFiles,
		const std::string& prompt);
	// allows user to input file name on screen, with a list of available files
	// requires using C++17 compiler (set in project properties)

	void getAvailableFiles(std::vector<std::string>& availableFiles);

	void readFile(std::ifstream& inFile);

	void centerOnScreen();

	void drawEditModeIndicator();

	void highlightBox(Box& aBox);
	void draw();
	void getModelCoords(double& modelX, double& modelY, double screenX, double screenY);
	void getScreenCoords(double modelX, double modelY, double& screenX, double& screenY);
	void snapFaceOn(OrbitingViewer& orbit, Camera3D& camera);
	void updateModelCom(Box& newBox);
	void storeState(std::unordered_map<std::string, Box> theBoxes);
	void restoreState();
};

