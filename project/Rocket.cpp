#include <iostream>
#include "Rocket.h"


void Rocket::addEngineBox(EngineBox& anEngineBox)
{
	EngineBox* EngineBoxPtr = &anEngineBox;
	theEngineBoxes.push_back(EngineBoxPtr);
}

void Rocket::addPayloadBox(PayloadBox& aPayloadBox)
{
	PayloadBox* PayloadBoxPtr = &aPayloadBox;
	thePayloadBoxes.push_back(PayloadBoxPtr);
}

void Rocket::updateVelocity()
{
	if (!theEngineBoxes.empty()) {
		EngineBox* currEngineBox = theEngineBoxes.back();
		double currEffectiveExhaustVelocity = currEngineBox->getEffectiveExhaustVelocity();
		double currBurnTime = currEngineBox->getBurnTime();
		velocity = -currEffectiveExhaustVelocity * log(1 - propellantMassFraction * timeElapsedSinceLaunch / currBurnTime) + baselineVelocity;
	}
	else
		std::cout << "No active engine running, velocity cannot be changed" << std::endl;
	
}
