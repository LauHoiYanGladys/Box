#include "Box.h"
#include <iostream>
#include <fstream>
#include "fssimplewindow.h"
#include "DrawingUtilNG.h"

const double GRAVITY = 9.80665;  // this implies units are meters and seconds

void Box::fall(double deltaT)
{
	// Box class has Y-coordinate of COM (comY), velocity, fallDuration. There is also a constant gravity for all

	if (yDistanceFromBelow > 0.) {
		comY -= velocity * deltaT + 0.5 * GRAVITY * deltaT * deltaT;
		yDistanceFromBelow -= velocity * deltaT + 0.5 * GRAVITY * deltaT * deltaT;

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

void Box::print(ofstream& outFile)
{
	outFile << "\t" << comX << "\t" << comY << "\t" 
		<<  xDim << "\t" << yDim << "\t" << hue << endl;

}