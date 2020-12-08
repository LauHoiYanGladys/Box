#include <iostream>
#include <chrono>
#include <string>
#include <algorithm>
#include "Rocket.h"
#include "Manager.h"
#include "fssimplewindow.h"


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
	/*EngineBox* EngineBoxPtr = &anEngineBox;*/
	/*theEngineBoxes.push_back(&anEngineBox);*/
	theEngineBoxes.insert({ anEngineBox.getLabel(), &anEngineBox });
	updateTotalPropellantMass(anEngineBox);
	updateTotalStructuralMass(anEngineBox);
	updateInitialMass(anEngineBox);
}

void Rocket::addPayloadBox(PayloadBox& aPayloadBox)
{
	/*PayloadBox* PayloadBoxPtr = &aPayloadBox;*/
	/*thePayloadBoxes.push_back(&aPayloadBox);*/
	thePayloadBoxes.insert({ aPayloadBox.getLabel(), &aPayloadBox });
	updateTotalPayloadMass(aPayloadBox);
	updateInitialMass(aPayloadBox);
}

void Rocket::updateVelocity()
{
	if (!theEngineBoxes.empty()) {
		EngineBox* currEngineBox = theEngineBoxes.begin()->second;
		double currEffectiveExhaustVelocity = currEngineBox->getEffectiveExhaustVelocity();
		double currBurnTime = currEngineBox->getBurnTime();
		/*double timeElapsedInStage = std::chrono::duration_cast<std::chrono::nanoseconds>
			(std::chrono::high_resolution_clock::now() - begin).count() / 1e9;*/
		velocity = -currEffectiveExhaustVelocity * log(1 - propellantMassFraction * timeElapsedInStage / currBurnTime) + baselineVelocity;
	}
	else
		std::cout << "No active engine running, velocity cannot be changed" << std::endl;
	
}

void Rocket::computeCom()
{
	resetTotalMass();
	for (auto& currEngineBox : theEngineBoxes) {
		comMassXSum += currEngineBox.second->getComX() * (currEngineBox.second->getPropellantMass() + currEngineBox.second->getStructuralMass());
		comMassYSum += currEngineBox.second->getComY() * (currEngineBox.second->getPropellantMass() + currEngineBox.second->getStructuralMass());
		totalStructuralMass += currEngineBox.second->getStructuralMass();
		totalPropellantMass += currEngineBox.second->getPropellantMass();
	}
	for (auto& currPayloadBox : thePayloadBoxes) {
		comMassXSum += currPayloadBox.second->getComY() * (currPayloadBox.second->getPayloadMass());
		comMassYSum += currPayloadBox.second->getComY() * (currPayloadBox.second->getPayloadMass());
		totalPayloadMass += currPayloadBox.second->getPayloadMass();
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

void Rocket::fly(double deltaT)
{
	
	// update velocity
	updateVelocity();
	// update Com of rocket
	comY += velocity * deltaT;

	// update Com of all boxes in the rocket by the same amount
	for (auto& currEngineBox : theEngineBoxes) {
		currEngineBox.second->incrementComY(velocity * deltaT);
	}
	// update Com of all boxes in the rocket by the same amount
	for (auto& currPayloadBox : thePayloadBoxes) {
		currPayloadBox.second->incrementComY(velocity * deltaT);
	}
	
	timeElapsedInStage += deltaT;
}

void Rocket::startStage()
{
	theEngineBoxes.erase(theEngineBoxes.begin());
	baselineVelocity = velocity;
	/*begin = std::chrono::high_resolution_clock::now();*/
	timeElapsedInStage = 0;

}

void Rocket::updateTotalPropellantMass(EngineBox& anEngineBox)
{
	totalPropellantMass += anEngineBox.getPropellantMass();
}

void Rocket::updateTotalStructuralMass(EngineBox& anEngineBox)
{
	totalStructuralMass += anEngineBox.getStructuralMass();
}

void Rocket::updateTotalPayloadMass(PayloadBox& aPayloadBox)
{
	totalPayloadMass += aPayloadBox.getPayloadMass();
}

void Rocket::updateInitialMass(EngineBox& anEngineBox)
{
	initialMass += anEngineBox.getPropellantMass() + anEngineBox.getStructuralMass();
}

void Rocket::updateInitialMass(PayloadBox& aPayloadBox)
{
	initialMass += aPayloadBox.getPayloadMass();
}

EngineBox* Rocket::getEngineBox(std::string& label)
{
	if (theEngineBoxes.find(label) != theEngineBoxes.end()) {
		return theEngineBoxes.find(label)->second;
	}
	return nullptr;
}

PayloadBox* Rocket::getPayloadBox(std::string& label)
{
	if (thePayloadBoxes.find(label) != thePayloadBoxes.end()) {
		return thePayloadBoxes.find(label)->second;
	}
	return nullptr;
}

EngineBox* Rocket::makeEngineBox(Manager &theManager)
{
	double tempDim = 10;
	double tempHue = 0;
	double tempThrust = 5000;
	double tempPropellantMassFlow = 10;

	FsPollDevice();
	int key = FsInkey();
	int mouseEvent, leftButton, middleButton, rightButton;
	int locX, locY;
	double modelX, modelY;
	mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);

	theManager.getModelCoords(modelX, modelY, locX, locY);

	// make the box and add to current rocket
	incrementEngineBoxCounter();
	string label = to_string(getEngineBoxCounter());
	EngineBox* newEngineBox = new EngineBox(label, modelX, modelY, tempDim, tempDim, tempHue,
		tempThrust, tempPropellantMassFlow);
	addEngineBox(*newEngineBox);

	cout << "Setting width" << endl;
	while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN)
	{
		//cout << "Setting width = " << toAdd.getWidth() << endl;
		//cout << "X = " << toAdd.getComX() << " Y = " << toAdd.getComY() << endl;
		theManager.getModelCoords(modelX, modelY, locX, locY);


		newEngineBox->setXY(modelX, modelY);
		if (key == FSKEY_WHEELUP)
			newEngineBox->setWidth(min(newEngineBox->getWidth() + 4, double(100))); //add max
		else if (key == FSKEY_WHEELDOWN)
			newEngineBox->setWidth(max(newEngineBox->getWidth() - 4, double(.05))); //add min
		cout << "setting width to " << newEngineBox->getWidth() << endl;
		newEngineBox->updateMass(); // update structural and propellant mass according to dimension

		theManager.draw();
		theManager.drawAxes();
		FsSwapBuffers();

		FsPollDevice();
		key = FsInkey();
		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);

	}


	//Set Height
	cout << "setting height" << endl;
	do
	{

		/*cout << "Setting height = " << currAdd->second.getHeight() << endl;*/
		theManager.getModelCoords(modelX, modelY, locX, locY);
		newEngineBox->setXY(modelX, modelY);
		if (key == FSKEY_WHEELUP)
			newEngineBox->setHeight(min(newEngineBox->getHeight() + 4, double(100))); //add max
		else if (key == FSKEY_WHEELDOWN)
			newEngineBox->setHeight(max(newEngineBox->getHeight() - 4, double(1))); //add min
		cout << "setting height to " << newEngineBox->getHeight() << endl;
		newEngineBox->updateMass(); // update structural and propellant mass according to dimension

		theManager.draw();
		theManager.drawAxes();
		FsSwapBuffers();

		FsPollDevice();
		key = FsInkey();
		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);


	} while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN);



	//Set Color (Hue)
	cout << "setting color" << endl;
	do
	{

		theManager.getModelCoords(modelX, modelY, locX, locY);
		newEngineBox->setXY(modelX, modelY);
		if (key == FSKEY_WHEELUP)
			newEngineBox->setHue(min((newEngineBox->getHue() + 3), double(360)));
		if (key == FSKEY_WHEELDOWN)
			newEngineBox->setHue(max((newEngineBox->getHue() - 3), double(0)));

		theManager.draw();
		theManager.drawAxes();
		FsSwapBuffers();

		FsPollDevice();
		key = FsInkey();
		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);


	} while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN);

	//sets thrust
	cout << "setting thrust" << endl;
	do
	{
		theManager.getModelCoords(modelX, modelY, locX, locY);
		newEngineBox->setXY(modelX, modelY);
		/*newEngineBox->setThrust(200);*/

		if (key == FSKEY_WHEELUP)
			newEngineBox->setThrust(min((newEngineBox->getThrust() + 100), 10000.));
		if (key == FSKEY_WHEELDOWN)
			newEngineBox->setThrust(max((newEngineBox->getThrust() - 100), 100.));

		// could change to better visual feedback later 
		cout << "current thrust = " << newEngineBox->getThrust() << endl;

		theManager.draw();
		theManager.drawAxes();
		FsSwapBuffers();

		FsPollDevice();
		key = FsInkey();
		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);


	} while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN);


	//sets propellant mass flow 
	cout << "setting propellant mass flow" << endl;
	do
	{
		theManager.getModelCoords(modelX, modelY, locX, locY);
		newEngineBox->setXY(modelX, modelY);

		if (key == FSKEY_WHEELUP)
			newEngineBox->setPropellantMassFlow(min((newEngineBox->getPropellantMassFlow() + 10), 100.));
		if (key == FSKEY_WHEELDOWN)
			newEngineBox->setPropellantMassFlow(max((newEngineBox->getPropellantMassFlow() + 10), 10.));

		// could change to better visual feedback later 
		cout << "current propellant mass flow = " << newEngineBox->getPropellantMassFlow() << endl;

		theManager.draw();
		theManager.drawAxes();
		FsSwapBuffers();

		FsPollDevice();
		key = FsInkey();
		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);


	} while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN);

	cout << "The engine box " << newEngineBox->getLabel() << " has been added to rocket " << this->label << endl;
	return newEngineBox;
}

PayloadBox* Rocket::makePayloadBox(Manager& theManager)
{
	double tempDim = 10;
	double tempHue = 0;
	double tempThrust = 5000;
	double tempPropellantMassFlow = 10;

	FsPollDevice();
	int key = FsInkey();
	int mouseEvent, leftButton, middleButton, rightButton;
	int locX, locY;
	double modelX, modelY;
	mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);

	theManager.getModelCoords(modelX, modelY, locX, locY);

	// make the box and add to current rocket
	incrementPayloadBoxCounter();
	string label = to_string(getPayloadBoxCounter());
	PayloadBox* newPayloadBox = new PayloadBox(label, modelX, modelY, tempDim, tempDim, tempHue);
	addPayloadBox(*newPayloadBox);

	cout << "Setting width" << endl;
	while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN)
	{
		//cout << "Setting width = " << toAdd.getWidth() << endl;
		//cout << "X = " << toAdd.getComX() << " Y = " << toAdd.getComY() << endl;
		theManager.getModelCoords(modelX, modelY, locX, locY);


		newPayloadBox->setXY(modelX, modelY);
		if (key == FSKEY_WHEELUP)
			newPayloadBox->setWidth(min(newPayloadBox->getWidth() + 4, double(100))); //add max
		else if (key == FSKEY_WHEELDOWN)
			newPayloadBox->setWidth(max(newPayloadBox->getWidth() - 4, double(.05))); //add min
		cout << "setting width to " << newPayloadBox->getWidth() << endl;

		newPayloadBox->updateMass(); // update structural and propellant mass according to dimension

		theManager.draw();
		theManager.drawAxes();
		FsSwapBuffers();

		FsPollDevice();
		key = FsInkey();
		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);

	}


	//Set Height
	cout << "setting height" << endl;
	do
	{

		/*cout << "Setting height = " << currAdd->second.getHeight() << endl;*/
		theManager.getModelCoords(modelX, modelY, locX, locY);
		newPayloadBox->setXY(modelX, modelY);
		if (key == FSKEY_WHEELUP)
			newPayloadBox->setHeight(min(newPayloadBox->getHeight() + 4, double(100))); //add max
		else if (key == FSKEY_WHEELDOWN)
			newPayloadBox->setHeight(max(newPayloadBox->getHeight() - 4, double(1))); //add min
		cout << "setting height to " << newPayloadBox->getHeight() << endl;

		newPayloadBox->updateMass(); // update structural and propellant mass according to dimension

		theManager.draw();
		theManager.drawAxes();
		FsSwapBuffers();

		FsPollDevice();
		key = FsInkey();
		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);


	} while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN);



	//Set Color (Hue)
	cout << "setting color" << endl;
	do
	{

		theManager.getModelCoords(modelX, modelY, locX, locY);
		newPayloadBox->setXY(modelX, modelY);
		if (key == FSKEY_WHEELUP)
			newPayloadBox->setHue(min((newPayloadBox->getHue() + 3), double(360)));
		if (key == FSKEY_WHEELDOWN)
			newPayloadBox->setHue(max((newPayloadBox->getHue() - 3), double(0)));

		theManager.draw();
		theManager.drawAxes();
		FsSwapBuffers();

		FsPollDevice();
		key = FsInkey();
		mouseEvent = FsGetMouseEvent(leftButton, middleButton, rightButton, locX, locY);


	} while (mouseEvent != FSMOUSEEVENT_LBUTTONDOWN);

	
	cout << "The payload box " << newPayloadBox->getLabel() << " has been added to rocket " << this->label << endl;
	return newPayloadBox;
}

void Rocket::deleteBox(Box* theBox)
{
	delete theBox;
}



