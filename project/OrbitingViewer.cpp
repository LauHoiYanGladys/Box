#include "OrbitingViewer.h"

OrbitingViewer::OrbitingViewer()
{	
	// sets up the initial values
	initialize();
}

void OrbitingViewer::initialize(void)
{
	// setups the initial viewing angles, distance from object and focus coordinates
	h = 0;
	p = 0;
	dist = 120.0;
	focusX = 0.0;
	focusY = 0.0;
	focusZ = 0.0;
}

void OrbitingViewer::setUpCamera(Camera3D& camera)
{
	// make camera point towards the focus point at the determined angle
	camera.h = h;
	camera.p = p;
	camera.b = 0.0;

	double vx, vy, vz;
	camera.getForwardVector(vx, vy, vz);
	camera.x = focusX - vx * dist;
	camera.y = focusY - vy * dist;
	camera.z = focusZ - vz * dist;
}
