#include <stdio.h>
#include <algorithm>    // std::max
#include <string>
//#include "GL\glut.h"

#include "fssimplewindow.h"
//#include "GraphicFont.h"

#include "DrawingUtilNG.h"


using namespace DrawingUtilNG;

double DrawingUtilNG::getDistance(double x0, double y0, double x1, double y1)
{
	return sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
}

float DrawingUtilNG::vectorLength(vertexF v0)
{
	return sqrt(v0.x * v0.x + v0.y * v0.y + v0.z * v0.z);
}

vertexF DrawingUtilNG::getUnitVector(vertexF v0)
{
	return getUnitVector({ 0., 0., 0. }, v0);
}

vertexF DrawingUtilNG::getUnitVector(vertexF v1, vertexF v2)
{
	vertexF vector = { (v2.x - v1.x) , (v2.y - v1.y) , (v2.z - v1.z) };

	float length = vectorLength(vector);

	vector = { vector.x / length, vector.y / length, vector.z / length };

	return vector;
}

vertexF DrawingUtilNG::vectorSum(vertexF v1, vertexF v2)
{
	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

vertexF DrawingUtilNG::vectorSum(vertexF v1, vertexF v2, vertexF v3)
{
	return { v1.x + v2.x + v3.x, v1.y + v2.y + v3.y, v1.z + v2.z + v3.z };
}

vertexF DrawingUtilNG::scalarProduct(vertexF v0, float scalar)
{
	return { v0.x * scalar, v0.y * scalar, v0.z * scalar };
}

float DrawingUtilNG::dotProduct(vertexF v1, vertexF v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

vertexF DrawingUtilNG::crossProduct(vertexF v1, vertexF v2)
{
	return {
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	};
}

vertexF DrawingUtilNG::rotateVector(vertexF v, vertexF k, float theta)
{
	double cosTheta = cos(theta * atan(1.) / 45.);
	double sinTheta = sin(theta * atan(1.) / 45.);

	// make sure k is a unit vector
	k = getUnitVector(k);

	// using Rodrigues' Rotation Formula
	vertexF firstTerm = scalarProduct(v, cosTheta);
	vertexF secondTerm = scalarProduct(crossProduct(k, v), sinTheta);
	vertexF thirdTerm = scalarProduct(k, dotProduct(k, v) * (1 - cosTheta));

	return vectorSum(firstTerm, secondTerm, thirdTerm);
}

void DrawingUtilNG::drawCircle(double centerX, double centerY,
	double radius, bool filled)
{
	float x, y, angle;
	float radianConvert = atan(1.) / 45.;
	radius = fabs(radius);

	// adapt the number of segments based on radius size
	int stepSize = 1;
	if (radius < 10)
		stepSize = 3;
	else if (radius < 200)
		stepSize = round((3. - 1.) / (10. - 200.) * (radius - 200.) + 1.);

	stepSize *= 6;  // always want stepSize to be a factor of 360

	if (filled)
		glBegin(GL_POLYGON);
	else
		glBegin(GL_LINE_LOOP);

	for (int i = 0; i < 360; i += stepSize) {
		angle = i * radianConvert;
		x = cos(angle) * radius + centerX;
		y = sin(angle) * radius + centerY;
		glVertex2d(x, y);
	}
	glEnd();
}

void DrawingUtilNG::drawRectangle(double x, double y, int sizeX, int sizeY, bool filled)
{
	if (filled)
		glBegin(GL_POLYGON);
	else
		glBegin(GL_LINE_LOOP);

	glVertex2i(x, y);
	glVertex2i(x + sizeX, y);
	glVertex2i(x + sizeX, y + sizeY);
	glVertex2i(x, y + sizeY);

	glEnd();
}

void DrawingUtilNG::drawRectangle3D(double x, double y, int sizeX, int sizeY, double hue, bool isHighlighted)
{
	
	double red, green, blue;
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1, 1);

	glBegin(GL_QUADS);

	DrawingUtilNG::hsv2rgb(hue, 1, 1, red, green, blue);
	glColor3f(red, green, blue);

	//if (sixColors)
	//	glColor3ub(0, 0, 255);  // blue face
	glVertex3d(x, y, 0);
	glVertex3d(x + sizeX, y, 0);
	glVertex3d(x + sizeX, y + sizeY, 0);
	glVertex3d(x, y + sizeY, 0);

	//glVertex2i(x, y);
	//glVertex2i(x + sizeX, y);
	//glVertex2i(x + sizeX, y + sizeY);
	//glVertex2i(x, y + sizeY);


	//DrawingUtilNG::hsv2rgb(hue + 10, 1, 1, red, green, blue);
	//glColor3f(red, green, blue);
	////if (sixColors)
	////	glColor3ub(0, 255, 0);  // green
	//glVertex3d(x1, y1, z2);
	//glVertex3d(x2, y1, z2);
	//glVertex3d(x2, y2, z2);
	//glVertex3d(x1, y2, z2);

	//DrawingUtilNG::hsv2rgb(hue + 20, 1, 1, red, green, blue);
	//glColor3f(red, green, blue);
	////if (sixColors)
	////	glColor3ub(0, 255, 255); // cyan
	//glVertex3d(x1, y1, z1);
	//glVertex3d(x2, y1, z1);
	//glVertex3d(x2, y1, z2);
	//glVertex3d(x1, y1, z2);

	//DrawingUtilNG::hsv2rgb(hue + 30, 1, 1, red, green, blue);
	//glColor3f(red, green, blue);
	////if (sixColors)
	////	glColor3ub(255, 0, 0);  // red
	//glVertex3d(x1, y2, z1);
	//glVertex3d(x2, y2, z1);
	//glVertex3d(x2, y2, z2);
	//glVertex3d(x1, y2, z2);

	//DrawingUtilNG::hsv2rgb(hue + 40, 1, 1, red, green, blue);
	//glColor3f(red, green, blue);
	////if (sixColors)
	////	glColor3ub(255, 0, 255); // magenta
	//glVertex3d(x1, y1, z1);
	//glVertex3d(x1, y2, z1);
	//glVertex3d(x1, y2, z2);
	//glVertex3d(x1, y1, z2);

	//DrawingUtilNG::hsv2rgb(hue + 50, 1, 1, red, green, blue);
	//glColor3f(red, green, blue);
	////if (sixColors)
	////	glColor3ub(255, 255, 0);  // yellow
	//glVertex3d(x2, y1, z1);
	//glVertex3d(x2, y2, z1);
	//glVertex3d(x2, y2, z2);
	//glVertex3d(x2, y1, z2);

	glEnd();

	glDisable(GL_POLYGON_OFFSET_FILL);
	
	//if (filled)
	//	glBegin(GL_POLYGON);
	//else
	//	glBegin(GL_LINE_LOOP);

	if (isHighlighted) {
		glLineWidth(3);
		glBegin(GL_LINE_LOOP);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3d(x, y, 0);
		glVertex3d(x + sizeX, y, 0);
		glVertex3d(x + sizeX, y + sizeY, 0);
		glVertex3d(x, y + sizeY, 0);
		glEnd();
		glLineWidth(1);
	}

}

void DrawingUtilNG::drawStar(double centerX, double centerY,
	double outerSize, double innerSize, int numbPoints, double startAngle, bool filled)
{
	float radianConvert = atan(1.) / 45.;
	double stepSize = 360. / 2. / numbPoints;

	if (filled) {
		// GL_POLYGON cannot do concave shapes, so must draw series of quads (rhombuses)
		// BTW, quad strip don't work because they would share the "wrong edge"
		double currAngle, longX, longY;
		double currShortX, currShortY, prevShortX, prevShortY;

		currAngle = (startAngle - stepSize) * radianConvert;
		prevShortX = cos(currAngle) * innerSize + centerX;
		prevShortY = -sin(currAngle) * innerSize + centerY;

		for (int i = 0; i < numbPoints; i++) {
			currAngle += stepSize * radianConvert;
			longX = cos(currAngle) * outerSize + centerX;
			longY = -sin(currAngle) * outerSize + centerY;
			currAngle += stepSize * radianConvert;
			currShortX = cos(currAngle) * innerSize + centerX;
			currShortY = -sin(currAngle) * innerSize + centerY;

			glBegin(GL_QUADS);
			glVertex2d(centerX, centerY);
			glVertex2d(prevShortX, prevShortY);
			glVertex2d(longX, longY);
			glVertex2d(currShortX, currShortY);
			glEnd();

			prevShortX = currShortX;
			prevShortY = currShortY;
		}
	}
	else {
		double x, y, currRadius, currAngle;
		glBegin(GL_LINE_LOOP);
		for (int i = 0; i < (numbPoints * 2); i++) {
			currAngle = (startAngle + i * stepSize) * radianConvert;

			// if i is even , use outerSize 
			// (remainder of i div 2 is zero which means false, 
			// which means use second choice on trinary operator)
			currRadius = i % 2 ? innerSize : outerSize;

			x = cos(currAngle) * currRadius + centerX;
			y = -sin(currAngle) * currRadius + centerY;
			glVertex2d(x, y);
		}
		glEnd();
	}
}

void DrawingUtilNG::drawStarGram(double centerX, double centerY, double size,
	int numbPoints, int numbSkip, double startAngle, bool filled)
{
	int i;
	float radianConvert = atan(1.) / 45.;
	double stepSize = 360. / numbPoints;

	if (numbSkip >= numbPoints / 2)
		numbSkip = numbPoints / 2 - 1;

	if (filled) {
		double x0, y0, x1, y1;
		double currAngle = startAngle * radianConvert;

		x0 = cos(currAngle) * size + centerX;
		y0 = -sin(currAngle) * size + centerY;

		for (i = 0; i < numbPoints / 2; i++) {
			currAngle += stepSize * radianConvert * (numbSkip + 1);
			x1 = cos(currAngle) * size + centerX;
			y1 = -sin(currAngle) * size + centerY;
			glBegin(GL_TRIANGLES);
			glVertex2d(centerX, centerY);
			glVertex2d(x0, y0);
			glVertex2d(x1, y1);

			glEnd();
			x0 = x1; y0 = y1;
		}
		if (!(numbPoints - numbSkip + 1)% 2) {// if even number of points
			currAngle -= stepSize * radianConvert;
			x0 = cos(currAngle) * size + centerX;
			y0 = -sin(currAngle) * size + centerY;
		}

		for (; i < numbPoints; i++) {
			currAngle += stepSize * radianConvert * (numbSkip + 1);
			x1 = cos(currAngle) * size + centerX;
			y1 = -sin(currAngle) * size + centerY;
			glBegin(GL_TRIANGLES);
			glVertex2d(centerX, centerY);
			glVertex2d(x0, y0);
			glVertex2d(x1, y1);

			glEnd();
			x0 = x1; y0 = y1;
		}
	}
	else {
		int i;
		double x, y;
		double currAngle = startAngle * radianConvert;

		x = cos(currAngle) * size + centerX;
		y = -sin(currAngle) * size + centerY;
		glBegin(GL_LINE_LOOP);
		glVertex2d(x, y);

		for (i = 0; i < numbPoints / 2; i++) {
			currAngle += stepSize * radianConvert * (numbSkip + 1);

			x = cos(currAngle) * size + centerX;
			y = -sin(currAngle) * size + centerY;
			glVertex2d(x, y);

		}
		if (!(numbPoints - numbSkip + 1)% 2) {// if even number of points
			glEnd();
			currAngle -= stepSize * radianConvert;
			x = cos(currAngle) * size + centerX;
			y = -sin(currAngle) * size + centerY;
			glBegin(GL_LINE_LOOP);
			glVertex2d(x, y);

		}
		for (; i < numbPoints; i++) {
			currAngle += stepSize * radianConvert * (numbSkip + 1);

			x = cos(currAngle) * size + centerX;
			y = -sin(currAngle) * size + centerY;
			glVertex2d(x, y);

		}
		glEnd();

	}
}

//// drawStar test
//int currStarAngle = 0;
//DrawingUtilNG::drawStarGram(550, 180, 100, 9, 1, 90, true);
//DrawingUtilNG::drawStar(400, 400, 100, 100, 8, 90, true);
//glColor3ub(0, 255, 255);
//DrawingUtilNG::drawStarGram(550, 180, 70, 9, 1, 90, false);
//DrawingUtilNG::drawStar(400, 400, 95, 40, 8, 0, false);


bool DrawingUtilNG::hsv2rgb(double H, double S, double V, double& red, double& green, double& blue)
{
	// using formula from:
	// https://www.rapidtables.com/convert/color/hsv-to-rgb.html

	// adjust H to be 0 <= H <= 360
	H = fabs(fmod(H, 360.));  // note that H=360 is the same as H=0

	double C = V * S;
	double X = C * (1 - fabs(fmod(H / 60., 2) - 1));
	double m = V - C;
	double Rprime, Gprime, Bprime;

	if (H < 0.) {  // unlikely due to adjustment above
		return false;  //  <<< possible exit from function
	}
	else if (H < 60.) {
		Rprime = C;
		Gprime = X;
		Bprime = 0.;
	}
	else if (H < 120.) {
		Rprime = X;
		Gprime = C;
		Bprime = 0.;
	}
	else if (H < 180.) {
		Rprime = 0.;
		Gprime = C;
		Bprime = X;
	}
	else if (H < 240.) {
		Rprime = 0.;
		Gprime = X;
		Bprime = C;
	}
	else if (H < 300.) {
		Rprime = X;
		Gprime = 0.;
		Bprime = C;
	}
	else if (H < 360.) {
		Rprime = C;
		Gprime = 0.;
		Bprime = X;
	}
	else
		return false;  //  <<< possible exit from function

	red = Rprime + m;
	green = Gprime + m;
	blue = Bprime + m;

	return true;
}

/*
void DrawingUtilNG::writeHelvetica(std::string outString, int locX, int locY, int size)
{
	void *fontCode;
	switch (size) {
	case 10: fontCode = GLUT_BITMAP_HELVETICA_10;
		break;
	case 12: fontCode = GLUT_BITMAP_HELVETICA_12;
		break;
	case 18: fontCode = GLUT_BITMAP_HELVETICA_18;
		break;
	default: fontCode = GLUT_BITMAP_HELVETICA_10;
		break;
	}

	glRasterPos2i(locX, locY);
	for (int i = 0; i < outString.length(); i++) {
		glutBitmapCharacter(fontCode,
			outString.substr(i, 1).c_str()[0]);
	}
}
void DrawingUtilNG::writeTimesRoman(std::string outString, int locX, int locY, int size)
{
	void *fontCode;
	switch (size) {
	case 10: fontCode = GLUT_BITMAP_TIMES_ROMAN_10;
		break;
	case 24: fontCode = GLUT_BITMAP_TIMES_ROMAN_24;
		break;
	default: fontCode = GLUT_BITMAP_TIMES_ROMAN_10;
		break;
	}

	glutInitWindowPosition(45, 76);
	glRasterPos2i(locX, locY);
	for (int i = 0; i < outString.length(); i++) {
		glutBitmapCharacter(fontCode,
			outString.substr(i, 1).c_str()[0]);
	}
}
*/
void DrawingUtilNG::coverAndFade(int x1, int y1, int x2, int y2, int totalT, int currT,
	int r, int g, int b)
{
	if (currT > totalT)
		currT = totalT;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int temp = currT / totalT * 255;
	glColor4ub(r, g, b, currT / (double)totalT * 255);
	//glColor4ub(0, 0, 0, 180);
	drawRectangle(x1, y1, x2, y2, true);

	glDisable(GL_BLEND);

}

//void DrawingUtilNG::drawCube(vertexF v1, vertexF v2, bool sixColors)
//{
//	drawCube(v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, sixColors);
//}

void DrawingUtilNG::drawCube(double x1, double y1, double z1,
	double x2, double y2, double z2, double hue, bool isHighlighted, bool isCurrentEngine, bool isCurrentPayload/*, bool sixColors, GraphicFont* aFont*/)
{
	double red, green, blue;
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1, 1);

	glBegin(GL_QUADS);

	DrawingUtilNG::hsv2rgb(hue, 1, .6, red, green, blue);
	glColor3f(red, green, blue);

	//if (sixColors)
	//	glColor3ub(0, 0, 255);  // blue face
	glVertex3d(x1, y1, z1);
	glVertex3d(x2, y1, z1);
	glVertex3d(x2, y2, z1);
	glVertex3d(x1, y2, z1);


	DrawingUtilNG::hsv2rgb(hue, 1, 1, red, green, blue);
	glColor3f(red, green, blue);
	//if (sixColors)
	//	glColor3ub(0, 255, 0);  // green
	glVertex3d(x1, y1, z2);
	glVertex3d(x2, y1, z2);
	glVertex3d(x2, y2, z2);
	glVertex3d(x1, y2, z2);

	DrawingUtilNG::hsv2rgb(hue, 1, .8, red, green, blue);
	glColor3f(red, green, blue);
	//if (sixColors)
	//	glColor3ub(0, 255, 255); // cyan
	glVertex3d(x1, y1, z1);
	glVertex3d(x2, y1, z1);
	glVertex3d(x2, y1, z2);
	glVertex3d(x1, y1, z2);

	DrawingUtilNG::hsv2rgb(hue, 1, .8, red, green, blue);
	glColor3f(red, green, blue);
	//if (sixColors)
	//	glColor3ub(255, 0, 0);  // red
	glVertex3d(x1, y2, z1);
	glVertex3d(x2, y2, z1);
	glVertex3d(x2, y2, z2);
	glVertex3d(x1, y2, z2);

	DrawingUtilNG::hsv2rgb(hue, 1, .8, red, green, blue);
	glColor3f(red, green, blue);
	//if (sixColors)
	//	glColor3ub(255, 0, 255); // magenta
	glVertex3d(x1, y1, z1);
	glVertex3d(x1, y2, z1);
	glVertex3d(x1, y2, z2);
	glVertex3d(x1, y1, z2);

	DrawingUtilNG::hsv2rgb(hue, 1, 1, red, green, blue);
	glColor3f(red, green, blue);
	//if (sixColors)
	//	glColor3ub(255, 255, 0);  // yellow
	glVertex3d(x2, y1, z1);
	glVertex3d(x2, y2, z1);
	glVertex3d(x2, y2, z2);
	glVertex3d(x2, y1, z2);

	
	glEnd();

	

	if (isCurrentEngine) {
		glLineWidth(5);
		glBegin(GL_LINE_LOOP);
		glColor3f(1.0f, 0.0f, 0.0f); // red border
		glVertex3d(x1, y1, 0);
		glVertex3d(x2, y1, 0);
		glVertex3d(x2, y2, 0);
		glVertex3d(x1, y2, 0);
		glEnd();
		glLineWidth(1);
	}
	else if (isCurrentPayload) {
		glLineWidth(5);
		glBegin(GL_LINE_LOOP);
		glColor3f(0.0f, 0.0f, 1.0f); // blue border
		glVertex3d(x1, y1, 0);
		glVertex3d(x2, y1, 0);
		glVertex3d(x2, y2, 0);
		glVertex3d(x1, y2, 0);
		glEnd();
		glLineWidth(1);
	}

	if (isHighlighted) {
		glLineWidth(3);
		glBegin(GL_LINE_LOOP);
		glColor3f(1.0f, 1.0f, 1.0f); // white border
		glVertex3d(x1, y1, 0);
		glVertex3d(x2, y1, 0);
		glVertex3d(x2, y2, 0);
		glVertex3d(x1, y2, 0);
		glEnd();
		glLineWidth(1);
	}

	glDisable(GL_POLYGON_OFFSET_FILL);

	// font tester (needs GraphicFont.h)
	//if (aFont != NULL) {
	//	aFont->setColorHSV(30, 1, 1);
	//	//aFont->drawText3D("  ONE is the loneliest number",
	//	aFont->drawText3D(" 1ne",
	//		{ (float)x2, (float)y1, (float)z2 }, { (float)x2, (float)y2, (float)z2 }, { (float)x2, (float)y1, (float)z1 }, .15, 10.);

	//	aFont->drawText3D(" 2wo",
	//		{ (float)x2, (float)y1, (float)z1 }, { (float)x2, (float)y2, (float)z1 }, { (float)x1, (float)y1, (float)z1 }, .15, 10.);

	//	aFont->drawText3D(" 6ix",
	//		{ (float)x1, (float)y1, (float)z1 }, { (float)x1, (float)y2, (float)z1 }, { (float)x1, (float)y1, (float)z2 }, .15, 10.);

	//	aFont->drawText3D(" 5ive",
	//		{ (float)x1, (float)y1, (float)z2 }, { (float)x1, (float)y2, (float)z2 }, { (float)x2, (float)y1, (float)z2 }, .15, 10.);

	//	aFont->drawText3D(" 3ee",
	//		{ (float)x1, (float)y2, (float)z1 }, { (float)x2, (float)y2, (float)z1 }, { (float)x1, (float)y2, (float)z2 }, .15, 10.);

	//	aFont->drawText3D(" 4r",
	//		{ (float)x2, (float)y1, (float)z2 }, { (float)x2, (float)y1, (float)z1 }, { (float)x1, (float)y1, (float)z2 }, .15, 10.);


}

bool DrawingUtilNG::buildStringFromFsInkey(int key, std::string& currString)
{
	bool shiftIsOn = FsGetKeyState(FSKEY_SHIFT);;

	// get key input (may want to move this someplace else for re-use)

	if (FSKEY_A <= key && key <= FSKEY_Z) {
		int adjustLetter = shiftIsOn ? 0 : 32;
		currString += char(int('A') + key - FSKEY_A + adjustLetter);
	}
	else if (FSKEY_0 <= key && key <= FSKEY_9) {
		if (shiftIsOn) {
			std::string shiftOnNumerals = ")!@#$%^&*(";
			currString += shiftOnNumerals.substr(key - FSKEY_0, 1);
		}
		else
			currString += char(int('0') + key - FSKEY_0);
	}
	else {
		// note that since switch jumps to proper case, 
		// having large number of cases doesn't slow down program
		switch (key) {
		case FSKEY_BS: // remove last character on string
			currString = currString.substr(0, currString.length() - 1);
			break;
		case FSKEY_SPACE:currString += " ";
			break;
		case FSKEY_TAB:currString += "\t";
			break;
		case FSKEY_DOT:
			if (shiftIsOn)
				currString += ">";
			else
				currString += ".";
			break;
		case FSKEY_ENTER: // allows soft line break
			if (shiftIsOn)
				currString += "\n";
			break;
		case FSKEY_COMMA:
			if (shiftIsOn)
				currString += "<";
			else
				currString += ",";
			break;
		case FSKEY_MINUS:
			if (shiftIsOn)
				currString += "_";
			else
				currString += "-";
			break;
		case FSKEY_PLUS:
			if (shiftIsOn)
				currString += "+";
			else
				currString += "=";
			break;
		case FSKEY_LBRACKET:
			if (shiftIsOn)
				currString += "{";
			else
				currString += "[";
			break;
		case FSKEY_RBRACKET:
			if (shiftIsOn)
				currString += "}";
			else
				currString += "]";
			break;
		case FSKEY_BACKSLASH:
			if (shiftIsOn)
				currString += "|";
			else
				currString += "\\"; // backslash
			break;
		case FSKEY_SEMICOLON:
			if (shiftIsOn)
				currString += ":";
			else
				currString += ";";
			break;
		case FSKEY_SINGLEQUOTE:
			if (shiftIsOn)
				currString += "\""; // double quote
			else
				currString += "'";
			break;
		case FSKEY_SLASH:
			if (shiftIsOn)
				currString += "?";
			else
				currString += "/";
			break;
		case FSKEY_TILDA:
			if (shiftIsOn)
				currString += "~";
			else
				currString += "`";
			break;

		}
	}
	return true; // may use this later ???
}

