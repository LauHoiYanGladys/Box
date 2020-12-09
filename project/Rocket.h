#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <math.h>
#include <chrono>
#include <string>
#include "Box.h"
#include "Manager.h"
#include "OrbitingViewer.h"
#include "Camera3D.h"

class Manager;
class Box;
class Camera3D;
class OrbitingViwer;

class Rocket {
private:
	std::string label;
	std::unordered_map<std::string, EngineBox*> theEngineBoxes;
	std::unordered_map<std::string, EngineBox*> theUsedEngineBoxes;
	std::unordered_map<std::string, PayloadBox*> thePayloadBoxes;
	std::vector<std::string> theEngineBoxesLabels;
	/*std::vector<PayloadBox*> thePayloadBoxes;*/
	double totalPropellantMass, totalPayloadMass, 
		totalStructuralMass, initialMass, propellantMassFraction;
	double baselineVelocity, velocity, comX, comY, comMassXSum, comMassYSum;
	double timeElapsedInStage;
	std::chrono::steady_clock::time_point begin;
	bool flightEnded;
	bool flightStarted;
	// reset all total mass to zero 
	void resetTotalMass();
	int stageNumber;
	int engineBoxCounter, payloadBoxCounter;
	int engineBoxFlightCounter; // keeps track of the "index" of the active engineBox during flight 
	bool isCurrent;
	enum rocketBoxType { engine, payload };
	friend class Manager;
	EngineBox* currEngineBox;
	double groundPositionY; // stores the comY at ground before flight so that rocket can be reused
public:
	Rocket(std::string& theLabel) {
		label = theLabel;

		totalPropellantMass = 0;
		totalPayloadMass = 0;
		totalStructuralMass = 0;
		initialMass = 0;
		/*propellantMassFraction = totalPropellantMass/ initialMass;*/
		timeElapsedInStage = 0.;
		flightEnded = false;
		flightStarted = false;
		stageNumber = 1; // increments during flight if more than 1 engine box
		baselineVelocity = 0;
		engineBoxCounter = 0;
		payloadBoxCounter = 0;
		engineBoxFlightCounter = 0;
		isCurrent = true;
		currEngineBox = nullptr;
		//totalPropellantMass = theEngineBoxes.back()->getPropellantMass();
		//totalPayloadMass = thePayloadBoxes.back()->getPayloadMass();
		//totalStructuralMass = theEngineBoxes.back()->getStructuralMass();
		//initialMass = totalPayloadMass + totalStructuralMass + totalPropellantMass;
	}
	~Rocket() {
		if (!theEngineBoxes.empty()) {
			for (auto& currEngineBox : theEngineBoxes)
				delete currEngineBox.second;
		}
		if (!thePayloadBoxes.empty()) {
			for (auto& currPayloadBox : thePayloadBoxes)
				delete currPayloadBox.second;
		}
		if (!theUsedEngineBoxes.empty()) {
			for (auto& currEngineBox : theUsedEngineBoxes)
				delete currEngineBox.second;
		}
	}

	void addEngineBox(EngineBox& anEngineBox);
	void addPayloadBox(PayloadBox& aPayloadBox);

	// updates the instantaneous velocity
	void updateVelocity();

	// compute COM from scratch (weighted average of payloadBox and EngineBox Com)
	void computeCom();

	// updates propellant mass fraction
	void updatePMF() {
		propellantMassFraction = totalPropellantMass / initialMass;
	};

	// update COM (used when a given box is changed or added/removed
	void updateCom(EngineBox& anEngineBox);
	void updateCom(PayloadBox& aPayloadBox);

	// updates position of rocket
	bool fly(double deltaT, Manager& theManager, Camera3D& camera, OrbitingViewer& orbit);

	// get current COM
	double getComX() { return comX; };
	double getComY() { return comY; };
	double getVelocity() { return velocity; };

	// do things required to start a new stage
	void startNextStage();

	// puts back the rocket to ground
	bool reuseRocket(OrbitingViewer& orbit);

	// these functions are called when new boxes are added
	void updateTotalPropellantMass(EngineBox& anEngineBox);
	void updateTotalStructuralMass(EngineBox& anEngineBox);
	void updateTotalPayloadMass(PayloadBox& aPayloadBox);
	void updateInitialMass(EngineBox& anEngineBox);
	void updateInitialMass(PayloadBox& aPayloadBox);
	void incrementEngineBoxCounter() { engineBoxCounter++; };
	void incrementPayloadBoxCounter() { payloadBoxCounter++; };

	// functions to access private members
	int getEngineBoxCounter() { return engineBoxCounter++; };
	int getPayloadBoxCounter() { return payloadBoxCounter++; };

	// access the component boxes by label
	EngineBox* getEngineBox(std::string& label);
	PayloadBox* getPayloadBox(std::string& label);

	// gives the rocket name
	std::string getLabel() { return label; };

	// create an engine box, add to rocket and returns a pointer to it
	EngineBox* makeEngineBox(Manager& theManager);
	PayloadBox* makePayloadBox(Manager& theManager);
	
	/*EngineBox* makeEngineBox();*/

	// access the collection of boxes
	std::unordered_map<std::string, EngineBox*>* getTheEngineBoxes() { return &theEngineBoxes; };
	std::unordered_map<std::string, PayloadBox*>* getThePayloadBoxes() { return &thePayloadBoxes; };

	// delete a box
	void deleteBox(Box* theBox);
	//void deleteBox(Manager& theManager, rocketBoxType theRocketBoxType);

	// toggle isCurrent
	void toggleIsCurrent() { isCurrent = !isCurrent; };

	// reset rocket settings to return it to ground from flight
	void resetRocket();
};
