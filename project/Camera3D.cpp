#include "Camera3D.h"

Camera3D::Camera3D()
{
	initialize();
}

void Camera3D::initialize(void)
{
	// camera located at the origin

	// FOV: 30 deg

	// set depth range
}

void Camera3D::setUpCameraProjection(void)
{
	// Sets up the view perspective
	// https://stackoverflow.com/questions/16571981/gluperspective-parameters-what-do-they-mean
}

void Camera3D::setUpCameraTransformation(void)
{
	// puts the camera in the correct orientation and position
}

void Camera3D::getForwardVector(double& vx, double& vy, double& vz)
{
	// assume there's a unit vector representing the direction of camera pointing
	// this function gets the components of the vector in the x, y and z directions
}
