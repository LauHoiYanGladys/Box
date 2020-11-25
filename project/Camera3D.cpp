#include <math.h>
#include "Camera3D.h"
#include "fssimplewindow.h"

const double Camera3D::PI = 3.1415927;

Camera3D::Camera3D()
{
	initialize();
}

void Camera3D::initialize(void)
{
	// camera located at the origin
	x = y = z = 0;
	h = p = b = 0;

	// FOV: 30 deg
	fov = PI / 6.0;
	
	// set depth range
	nearZ = 0.1;
	farZ = 400.0;
}

void Camera3D::setUpCameraProjection(void)
{
	// Sets up the view perspective
	// https://stackoverflow.com/questions/16571981/gluperspective-parameters-what-do-they-mean
	int wid, hei;
	double aspect;

	FsGetWindowSize(wid, hei);
	aspect = (double)wid / (double)hei;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov * 180.0 / PI, aspect, nearZ, farZ);
}

void Camera3D::setUpCameraTransformation(void)
{
	// puts the camera in the correct orientation and position
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotated(-b * 180.0 / PI, 0.0, 0.0, 1.0);
	glRotated(-p * 180.0 / PI, 1.0, 0.0, 0.0);
	glRotated(-h * 180.0 / PI, 0.0, 1.0, 0.0);
	glTranslated(-x, -y, -z);
}

void Camera3D::getForwardVector(double& vx, double& vy, double& vz)
{
	// assume there's a unit vector representing the direction of camera pointing
	// this function gets the components of the vector in the x, y and z directions
	vx = -cos(p) * sin(h);
	vy = sin(p);
	vz = -cos(p) * cos(h);
}
