#include "Box.h"
#include <iostream>
#include <fstream>
#include "fssimplewindow.h"
#include "DrawingUtilNG.h"

const double GRAVITY = 9.80665;  // this implies units are meters and seconds
// Gladys
void Box::fall(double deltaT)
{
	// Box class has Y-coordinate of COM (comY), velocity, fallDuration. There is also a constant gravity for all

	if (yDistanceFromBelow > 0.) {
		/*setComY(comY - (velocity * deltaT + 0.5 * GRAVITY * deltaT * deltaT));*/
		comY -= (velocity * deltaT + 0.5 * GRAVITY * deltaT * deltaT);
		setBounds();
		yDistanceFromBelow -= (velocity * deltaT + 0.5 * GRAVITY * deltaT * deltaT);

		// recalculates currVel by applying currAccel for deltaT
		velocity += GRAVITY * deltaT;
	}

	
}

//void Box::draw()
//{
//	// bool filled = true;
//
//	DrawingUtilNG::drawRectangle(comX, comY, xDim, yDim, true);
//	//cout << "Drawing Box (" << comX << "," << comY << ") " << endl;
//}

void Box::print(std::ofstream& outFile)
{
	outFile << "\t" << comX << "\t" << comY << "\t" 
		<<  xDim << "\t" << yDim << "\t" << hue << std::endl;

}

void EngineBox::addPropellant()
{
	if (propellantMass < maxPropellantMass) {
		if ((maxPropellantMass - propellantMass) < 2)
			propellantMass = maxPropellantMass;
		else {
			propellantMass += 2;
		}
	}
	else
		std::cout << "Fuel tank is full, no fuel can be added" << std::endl;
}

void EngineBox::removePropellant()
{
	if (propellantMass > 0) {
		if (propellantMass < 2)
			propellantMass = 0;
		else {
			propellantMass -= 2;
		}
	}
	else
		std::cout << "Fuel tank is empty, no fuel can be removed" << std::endl;
}


