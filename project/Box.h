#pragma once
#include <iostream>
#include <string>

//using namespace std;

class Box {
protected:
	std::string label;
	double comX, comY, xDim, yDim, hue, leftUpperX, leftUpperY,
		rightUpperX, rightUpperY, leftLowerX, leftLowerY,
		rightLowerX, rightLowerY, velocity, fallDuration,
		maxX, maxY, minX, minY, yDistanceFromBelow;
	/*double thrust, propellantMassFlow;*/ // these are only actually used in EngineBox (child class)
	bool isHighlighted;

	//bool onGround, onBox;
public:
	Box(std::string& theLabel, double& theComX, double& theComY, 
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
		//onGround = false;
		//onBox = false;
		yDistanceFromBelow = 0;
		isHighlighted = false;
	}

	void setBounds() {
		leftUpperX = comX - 0.5 * xDim;
		leftUpperY = comY + 0.5 * yDim;
		rightUpperX = comX + 0.5 * xDim;
		rightUpperY = comY + 0.5 * yDim;
		leftLowerX = comX - 0.5 * xDim;
		leftLowerY = comY - 0.5 * yDim;
		rightLowerX = comX + 0.5 * xDim;
		rightLowerY = comY - 0.5 * yDim;
		maxX = rightUpperX;
		maxY = rightUpperY;
		minX = leftLowerX;
		minY = leftLowerY;
	}
	
	double getRightUpperX() { return rightUpperX; }
	double getRightLowerX() { return rightLowerX; }
	double getRightUpperY() { return rightUpperY; }
	double getRightLowerY() { return rightLowerY; }
	double getLeftUpperX() { return leftUpperX; }
	double getLeftLowerX() { return leftLowerX; }
	double getLeftUpperY() { return leftUpperY; }
	double getLeftLowerY() { return leftLowerY; }
	double getMaxX() { return maxX; }
	double getMaxY() { return maxY; }
	double getMinX() { return minX; }
	double getMinY() { return minY; }
	double getComX() { return comX; }
	double getComY() { return comY; }
	//bool isOnGround() { return onGround; }
	//bool isOnBox() { return onBox; }
	std::string getLabel() { return label; }
	void setYDistanceFromBelow(double yDistance) { yDistanceFromBelow = yDistance; }
	double getYDistanceFromBelow() { return yDistanceFromBelow; }
	double getVelocity() { return velocity; }
	double getFallDuration() { return fallDuration; }

	void setComX(double X) { comX = X; setBounds();}
	void setComY(double Y) { comY = Y; setBounds(); }

	double getWidth() { return xDim; }
	double getHeight() { return yDim; }
	double getHue() { return hue; }
	

	void setWidth(double width) { xDim = width; setBounds(); }
	void setHeight(double height) { yDim = height; setBounds(); }
	void setXY(double xCoord, double yCoord) { comX = xCoord, comY = yCoord; setBounds(); }
	void setHue(double theHue) { hue = theHue;  }

	bool getIsHighlighted(){ return isHighlighted; };
	void toggleIsHighlighted() {
		isHighlighted = !isHighlighted;
		if (isHighlighted)
			std::cout << "box highlighted" << std::endl;
		else
			std::cout << "box not highlighted" << std::endl;
	}

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

	void print(std::ofstream& outFile);

	//void draw();


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

	//// these four are just for the engine box to inherit, not supposed to be used here
	//virtual void setThrust(double theThrust) {
	//	thrust = 0;
	//	std::cout << "thrust is not supposed to be set in ordinary boxes" << std::endl;
	//};
	//virtual void setPropellantMassFlow(double thePropellantMassFlow) { 
	//	propellantMassFlow = 0; 
	//	std::cout << "propellantMassFlow is not supposed to be set in ordinary boxes" << std::endl;
	//};
	//virtual double getThrust() { return 0; };
	//virtual double getPropellantMassFlow() { return 0; }

};

class EngineBox : public Box {
private:
	double propellantMass, structuralMass, maxPropellantMass, propellantFraction;
	double thrust, propellantMassFlow, effectiveExhaustVelocity;
	double burnTime;
	friend class Rocket;
public:
	EngineBox(std::string& theLabel, double& theComX, double& theComY,
		double& theXDim, double& theYDim, double& theHue, 
		double& theThrust, double& thePropellantMassFlow):
		Box(theLabel,theComX, theComY, theXDim, theYDim, theHue) {
		propellantMass = xDim * yDim; // initially set to be equal to box area, can change with adding/removing propellant
		structuralMass = 0.1 * xDim * yDim; // a function of dimensions
		effectiveExhaustVelocity = thrust / propellantMassFlow;
		burnTime = propellantMass/ propellantMassFlow;
		thrust = theThrust;
		propellantMassFlow = thePropellantMassFlow;
	};
	// change the amount of fuel
	void addPropellant();
	void removePropellant();

	// get fraction of fuel left
	void getPropellantFraction(double& propellantFraction) { propellantFraction = propellantMass / maxPropellantMass; };
	double getPropellantMass() { return propellantMass; };
	double getStructuralMass() { return structuralMass; };
	double getEffectiveExhaustVelocity() { return effectiveExhaustVelocity; };
	double getBurnTime() { return burnTime; };

	// initial settings
	void setThrust(double theThrust) { thrust = theThrust; /*std::cout << "setting thrust to " << thrust << std::endl;*/ updateDependentParams(); };
	void setPropellantMassFlow(double thePropellantMassFlow) { propellantMassFlow = thePropellantMassFlow; updateDependentParams();};

	double getThrust() { return thrust; };
	double getPropellantMassFlow() { return propellantMassFlow; }

	// update dependent params
	void updateDependentParams() { 
		effectiveExhaustVelocity = thrust / propellantMassFlow; 
		burnTime = propellantMass / propellantMassFlow;
	}

	// increment ComY
	void incrementComY(double incrementY) {
		comY += incrementY;
		setBounds();
	}

	// update propellant and structural mass after dimensions have changed
	void updateMass() {
		propellantMass = xDim * yDim; // initially set to be equal to box area, can change with adding/removing propellant
		structuralMass = 0.1 * xDim * yDim;
	};
	void fly(double deltaT, double rocketVelocity) {
		comY += (rocketVelocity * deltaT);
		setBounds();
	}

};

class PayloadBox : public Box {
private:
	double payloadMass;
	friend class Rocket;
public:
	PayloadBox(std::string& theLabel, double& theComX, double& theComY,
		double& theXDim, double& theYDim, double& theHue):
		Box(theLabel, theComX, theComY, theXDim, theYDim, theHue) {
		payloadMass = xDim * yDim;
	}
	double getPayloadMass() { return payloadMass; };
	// increment ComY
	void incrementComY(double incrementY) {
		comY += incrementY;
		setBounds();
	}
	// update payload mass after dimensions have changed
	void updateMass() {
		payloadMass = xDim * yDim;
	};
	void fly(double deltaT, double rocketVelocity) {
		comY += (rocketVelocity * deltaT);
		setBounds();
	}
};