#include <iostream>
#include "Rocket.h"


void Rocket::resetTotalMass()
{
	totalStructuralMass = 0;
	totalPropellantMass = 0;
	totalPayloadMass = 0;
	comMassXSum = 0;
	comMassYSum = 0;
}

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

void Rocket::computeCom()
{
	resetTotalMass();
	for (auto& currEngineBox : theEngineBoxes) {
		comMassXSum += currEngineBox->getComX() * (currEngineBox->getPropellantMass() + currEngineBox->getStructuralMass());
		comMassYSum += currEngineBox->getComY() * (currEngineBox->getPropellantMass() + currEngineBox->getStructuralMass());
		totalStructuralMass += currEngineBox->getStructuralMass();
		totalPropellantMass += currEngineBox->getPropellantMass();
	}
	for (auto& currPayloadBox : thePayloadBoxes) {
		comMassXSum += currPayloadBox->getComY() * (currPayloadBox->getPayloadMass());
		comMassYSum += currPayloadBox->getComY() * (currPayloadBox->getPayloadMass());
		totalPayloadMass += currPayloadBox->getPayloadMass();
	}
	comX = comMassXSum / (totalPayloadMass + totalStructuralMass + totalPropellantMass);
	comY = comMassYSum / (totalPayloadMass + totalStructuralMass + totalPropellantMass);
}

void Rocket::updateCom(EngineBox& anEngineBox)
{
	double currStructuralMass, currPropellantMass;
	currPropellantMass = anEngineBox.getPropellantMass();
	currStructuralMass = anEngineBox.getStructuralMass();
	comMassXSum += anEngineBox.getComX() * (currPropellantMass + currStructuralMass);
	comMassYSum += anEngineBox.getComY() * (currPropellantMass + currStructuralMass);
	totalStructuralMass += currStructuralMass;
	totalPropellantMass += currPropellantMass;
	comX = comMassXSum / (totalPayloadMass + totalStructuralMass + totalPropellantMass);
	comY = comMassYSum / (totalPayloadMass + totalStructuralMass + totalPropellantMass);

}

void Rocket::updateCom(PayloadBox& aPayloadBox)
{
	double currPayloadMass;
	
	currPayloadMass = aPayloadBox.getPayloadMass();
	comMassXSum += aPayloadBox.getComX() * currPayloadMass;
	comMassYSum += aPayloadBox.getComY() * currPayloadMass;
	totalPayloadMass += currPayloadMass;
	comX = comMassXSum / (totalPayloadMass + totalStructuralMass + totalPropellantMass);
	comY = comMassYSum / (totalPayloadMass + totalStructuralMass + totalPropellantMass);
}

void Rocket::fly()
{
	// update Com
}
