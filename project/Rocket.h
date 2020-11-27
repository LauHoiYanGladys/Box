#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <math.h>
#include "Box.h"

class Rocket {
private:
	std::string label;
	std::vector<EngineBox*> theEngineBoxes;
	std::vector<PayloadBox*> thePayloadBoxes;
	double totalPropellantMass, totalPayloadMass, 
		totalStructuralMass, initialMass, propellantMassFraction;
	double baselineVelocity, velocity;
	double timeElapsedSinceLaunch;
	bool flightEnded;
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
		timeElapsedSinceLaunch = 0.;
		flightEnded = false;
	}
	void addEngineBox(EngineBox& anEngineBox);
	void addPayloadBox(PayloadBox& aPayloadBox);

	// updates the instantaneous velocity
	void updateVelocity();

	// updates position of rocket
	void fly();

};
