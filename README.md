# Box stacking and multi-stage rocket flight simulation

## Introduction
Our program is designed to simulate simple box stacking and rocket launch. 

The simple box stacking allows users to design 3D structures with or without gravity. 

Our program simulates real rocket engine physics, taking into consideration both thrust and mass flow of the propellants. A rocket’s component boxes are either Engine or Payload. The dimensions of an Engine box is proportional to its propellant mass and structural mass, while the dimensions of a Payload box is proportional to its structural mass. Other important parameters of the Engine boxes, thrust and propellant mass flow, are set by the user during Engine box creation. If a rocket has multiple Engine Boxes, it supports multistage flight.

Our program has four modes, as elaborated below. Press “E” to switch between them.

## Modes
### View Mode
Allows the user to view the created boxes/rockets.
* Control camera orientation
  * Arrow Keys
* Pan camera
  * I,J,K,L
* Change zoom
  * +/-
* Toggle gravity
  * Spacebar  
  
### Edit Mode
Allows the user to create simple boxes with desired geometries and drop them on one another using simulated gravity.
* Create a new box
  * Q (then follow onscreen instructions)
* Select box(es)
  * CTRL + left mouse button
  * For dragging, editing and deleting
* Drag boxes around
  * Click on box and hold down left mouse button
  * All selected boxes move together if the dragged one is selected
* Delete Simple Box
  * Select then press ‘Del’ Key 
* Edit Box
  * Select then press “W” to initiate (then follow onscreen instructions)
* Undo change
  * ‘U’ key
* Toggle gravity
  * Spacebar  
 
 ![image](https://drive.google.com/uc?export=view&id=1wZ6tZL3ompykvIeNZPWymA7JwsirK17N)
 
 ### Rocket Build Mode
Allows users to build and edit rockets
* Press “T” to initiate rocket build or editing
* Follow instructions in the console to name rocket, add or delete Engine or Payload Box
* Each rocket must have at least one Payload Box and one Engine Box before launching
* Multiple rockets can be built
* The boxes of the current rocket will be highlighted in blue (Payload) and red (Engine)

![image](https://drive.google.com/uc?export=view&id=1J1jM8gUdqd84Lwcn2z6DL1gytXK_uIss)

 ### Rocket Launch Mode
Allows the user to launch rockets built
* Press “C” to toggle between rockets to select the current one for launch
* Press “G” to launch the rocket
* Note the on-screen instructions during flight. 
* If Rocket has multiple engine boxes, it has a multi-stage flight
* Press “N” to move on to the next stage when one stage has ended.
* When all stages has ended, press “N” to return rocket to ground (Reusable rockets is currently the trend - it lowers the cost of space exploration)
* You may select another rocket to launch

![image](https://drive.google.com/uc?export=view&id=184xFGDM7qU4BX3nrGz6c_mrrCNvnorBq)

### Known bugs for future fixes
* Gravity does not always work properly after box dragging
* Boxes sometimes do not stack up perfectly, especially after dragging
* On-screen position indicator only lines up with mouse position properly near the origin and before any panning/change in camera orientation
* Rocket boxes sometimes slip out of view at the beginning of flight, only to return to the correct positions afterwards

### Future feature improvements
* Add load and save options
* Make all user input work on the screen instead of the console
* Add the option to edit dimensions and properties of rocket Engine and Payload boxes
* Enable undo when building rocket
