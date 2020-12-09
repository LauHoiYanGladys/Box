#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <math.h>
#include <chrono>
#include <string>
#include "Box.h"
#include "Manager.h"

class Manager;

class Rocket {
private:
	std::string label;
	std::unordered_map<std::string, EngineBox*> theEngineBoxes;
	std::unordered_map<std::string, PayloadBox*> thePayloadBoxes;
	/*std::vector<EngineBox*> theEngineBoxes;*/
	/*std::vector<PayloadBox*> thePayloadBoxes;*/
	double totalPropellantMass, totalPayloadMass, 
		totalStructuralMass, initialMass, propellantMassFraction;
	double baselineVelocity, velocity, comX, comY, comMassXSum, comMassYSum;
	double timeElapsedInStage;
	std::chrono::steady_clock::time_point begin;
	bool flightEnded;
	// reset all total mass to zero 
	void resetTotalMass();
	int stageNumber;
	int engineBoxCounter, payloadBoxCounter;
	bool isCurrent;
	enum rocketBoxType { engine, payload };
	friend class Manager;
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
		stageNumber = 1; // increments during flight if more than 1 engine box
		baselineVelocity = 0;
		engineBoxCounter = 0;
		payloadBoxCounter = 0;
		isCurrent = true;
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
	void fly(double deltaT, Manager& theManager);

	// get current COM
	double getComX() { return comX; };
	double getComY() { return comY; };
	double getVelocity() { return velocity; };

	// do things required to start a new stage
	void startStage();

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
};
