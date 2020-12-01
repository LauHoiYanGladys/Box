#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <math.h>
#include <chrono>
#include "Box.h"

class Rocket {
private:
	std::string label;
	std::vector<EngineBox*> theEngineBoxes;
	std::vector<PayloadBox*> thePayloadBoxes;
	double totalPropellantMass, totalPayloadMass, 
		totalStructuralMass, initialMass, propellantMassFraction;
	double baselineVelocity, velocity, comX, comY, comMassXSum, comMassYSum;
	double timeElapsedInStage;
	std::chrono::steady_clock::time_point begin;
	bool flightEnded;
	// reset all total mass to zero 
	void resetTotalMass();
	int stageNumber;
public:
	Rocket(std::string& theLabel, PayloadBox& aPayloadBox, EngineBox& anEngineBox) {
		label = theLabel;
		addEngineBox(anEngineBox);
		addPayloadBox(aPayloadBox);
		totalPropellantMass = theEngineBoxes.back()->getPropellantMass();
		totalPayloadMass = thePayloadBoxes.back()->getPayloadMass();
		totalStructuralMass = theEngineBoxes.back()->getStructuralMass();
		initialMass = totalPayloadMass + totalStructuralMass + totalPropellantMass;
		
		propellantMassFraction = totalPropellantMass/ initialMass;
		timeElapsedInStage = 0.;
		flightEnded = false;
		stageNumber = 1; // same as the number of engineBox present
		baselineVelocity = 0;
	}
	void addEngineBox(EngineBox& anEngineBox);
	void addPayloadBox(PayloadBox& aPayloadBox);

	// updates the instantaneous velocity
	void updateVelocity();

	// compute COM from scratch (weighted average of payloadBox and EngineBox Com)
	void computeCom();

	// update COM (used when a given box is changed or added/removed
	void updateCom(EngineBox& anEngineBox);
	void updateCom(PayloadBox& aPayloadBox);

	// updates position of rocket
	void fly(double deltaT);

	// get current COM
	double getComX() { return comX; };
	double getComY() { return comY; };
	double getVelocity() { return velocity; };

	// keeps track of time since stage starts

	void startStage();
};
