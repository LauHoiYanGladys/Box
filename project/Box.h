#pragma once
#include <string>
using namespace std;

class Box {
private:
	string label;
	double comX, comY, xDim, yDim, hue, leftUpperX, leftUpperY, 
			rightUpperX, rightUpperY, leftLowerX, leftLowerY, 
			rightLowerX, rightLowerY, velocity, fallDuration,
			maxX, maxY, minX, minY, yDistanceFromBelow;
	
	bool onGround, onBox;
public:
	Box(string& theLabel, double& theComX, double& theComY, 
		double& theXDim, double& theYDim, double& theHue){
		label = theLabel;
		comX = theComX;
		comY = theComY;
		xDim = theXDim;
		yDim = theYDim;
		hue = theHue;
		leftUpperX = comX - 0.5 * xDim; 
		leftUpperY = comY + 0.5 * yDim;
		rightUpperX = comX + 0.5 * xDim;
		rightUpperY = comY + 0.5 * yDim;
		leftLowerX = comX - 0.5 * xDim;
		leftLowerY = comY - 0.5 * yDim;
		rightLowerX = comX + 0.5 * xDim;
		rightLowerY = comY - 0.5 * yDim;
		velocity = 0;
		fallDuration = 0;
		maxX = rightUpperX;
		maxY = rightUpperY;
		minX = leftLowerX;
		minY = leftLowerY;
		onGround = false;
		onBox = false;
		yDistanceFromBelow = 0;
	}

	double getLeftUpperX() { return leftUpperX; }
	double getRightUpperX() { return rightUpperX; }
	double getLeftUpperY() { return leftUpperY; }
	double getLeftLowerY() { return rightUpperX; }
	double getMaxX() { return maxX; }
	double getMaxY() { return maxY; }
	double getMinX() { return minX; }
	double getMinY() { return minY; }
	double getComX() { return comX; }
	double getComY() { return comY; }
	bool isOnGround() { return onGround; }
	bool isOnBox() { return onBox; }
	string getLabel() { return label; }
	void setYDistanceFromBelow(double yDistance) { yDistanceFromBelow = yDistance; }
	double getYDistanceFromBelow() { return yDistanceFromBelow; }
	double getVelocity() { return velocity; }
	double getFallDuration() { return fallDuration; }

	void setComX(double X) { comX = X; }
	void setComY(double Y) { comX = Y; }

	double getWidth() { return xDim; }
	double getHeight() { return yDim; }
	double getHue() { return hue; }

	double setWidth(double width) { xDim = width; }
	double setHeight(double height) { yDim = height; }
	double setXY(double xCoord, double yCoord) { comX = xCoord, comY = yCoord; }
	double setHue(double theHue) { hue = theHue; }


	void fall(double deltaT);
	// update box position and velocity when falling

	// returns true if box is falling
	bool isFalling()
	{
		if (yDistanceFromBelow > 0)
			return true;
		else
			return false;
	}

	void print(ofstream& outFile);

	void draw();

	void getParams(double& tX, double& tY, double& tH, double& tW, double& tHue) {
		tX = comX;
		tY = comY;
		tH = yDim;
		tW = xDim;
		tHue = hue;
	}
	void setParams(double x, double y, double h, double w, double theHue) {
		comX = x;
		comY = y;
		yDim = h;
		xDim = w;
		hue = theHue;
	}
};