#include <iostream>
#include "display.h"
#include "inputManager.h"

Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "OpenGL");	
Scene scn(glm::vec3(0.0f, 0.0f, -15.0f), CAM_ANGLE, relation, NEAR, FAR);
glm::vec3 YAxis = glm::vec3(0, 1, 0);
glm::vec3 TranslatePos = glm::vec3(3, 0, 0);

int main(int argc, char** argv)
{
	initCallbacks(display);
	//scn.addShape("./res/objs/bigbox.obj","./res/textures/box0.bmp");
	//scn.addShape("./res/objs/testboxNoUV.obj");
	//scn.addShape("./res/objs/testboxNoUV.obj", 6);

	scn.addShape("./res/objs/monkeyNoUV.obj", "./res/textures/grass.bmp", 400U);
	//scn.addShape("./res/objs/monkeyNoUV.obj", "./res/textures/grass.bmp", 1600U);
	//scn.addShape("./res/objs/monkeyNoUV.obj", "./res/textures/grass.bmp", 10000U);
	//scn.addShape("./res/objs/monkeyNoUV.obj", "./res/textures/grass.bmp");

	scn.addShader("./res/shaders/basicShader");
	scn.addShader("./res/shaders/pickingShader");

	// Each will rotate around its Y axis, and will be moved a predefined distance
	for (int i = 0; i < scn.shapes.size(); i++) {
		scn.shapes[i]->myTranslate((float)i * TranslatePos, 0);
	}
	

	while(!display.toClose())
	{
		display.Clear(0.7f, 0.7f, 0.7f, 1.0f);
		
		// Rotate shapes
		for (auto& shape : scn.shapes) {
			shape->myRotate(0.05f, YAxis);
		}
		
		scn.draw(0,0,true);
		display.SwapBuffers();
		display.pullEvent();
	}

	return 0;
}
