///////////////////////////////////
//Technik 2009 - 2010
//Revolution Engine 0.5
///////////////////////////////////

//Header files
#include "REV.h"//Just one header file to include
#include <asndlib.h>

int main()
{
	REV_init();//One function inits everything (WPAD and PAD included)
	SMODEL * floorMd = new SMODEL("RevExamples/models/plane.rms");
	floorMd->scale(100000);
	SMODEL * ballMd = new SMODEL("RevExamples/models/sphere.rms");
	SMODEL * pointMd = new SMODEL("RevExamples/models/sphere.rms");
	
	ballMd->scale(50);
	//Objects
	OBJECT * floor = new OBJECT(floorMd);
	floor->clr = SC_LBLUE;
	OBJECT * ball01 = new OBJECT(ballMd, vector3(0, 0, 50));
	OBJECT * ball02 = new OBJECT(ballMd, vector3(150, 0, 50));
	OBJECT * ball03 = new OBJECT(pointMd, vector3(-150, 0, 50));
	ball02->body = new SPHEREBD(50);
	ball01->body = new SPHEREBD(50);
	ball02->setSpeed(vector3(0, 100, -100), nullVector);
	defaultCam->body = new SPHEREBD(50);
	floor->body = new INFPLANEBD();
	ball01->setSpeed(vector3(10,0,0), nullVector);
	defaultCam->rotate(AXIS_Z, 180);
	defaultCam->move(vector3(0,500,50), RT_ABS);
	//Sky box
	mainRoot->skyBox.front = loadPng("RevExamples/maps/front-sunny.png");
	mainRoot->skyBox.back = loadPng("RevExamples/maps/back-sunny.png");
	mainRoot->skyBox.right = loadPng("RevExamples/maps/right-sunny.png");
	mainRoot->skyBox.left = loadPng("RevExamples/maps/left-sunny.png");
	
	//Lights
	DLIGHT(vector3(-1,-1,-1), SC_YELLOW);
	DLIGHT(vector3(0,0,1), rgb(0,30,10));
	DLIGHT(vector3(1,0,-1), SC_ORANGE);
	f32 sx, sy, sz;
	REVConsole->open();
	REVConsole->write("program running");
	REVConsole->write("program running");
	REVConsole->write("program running");
	//Main Loop
	while(1)
	{
		//Before every frame
		REV_preProcess();
		//If someone pressed home, exit
		if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)
		{
			REV_exit();
		}
		if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_B)
		{
			if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_RIGHT) defaultCam->move(vector3(500*gameTime,0,0),RT_SELF);
			if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_LEFT) defaultCam->move(vector3(-500*gameTime,0,0),RT_SELF);
			if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_UP) defaultCam->move(vector3(0,0,500*gameTime),RT_SELF);
			if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_DOWN) defaultCam->move(vector3(0,0,-500*gameTime),RT_SELF);
		}
		else
		{
			if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_RIGHT) defaultCam->rotate(AXIS_Z, -45*gameTime);
			if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_LEFT) defaultCam->rotate(AXIS_Z, 45*gameTime);
			if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_UP) defaultCam->move(vector3(0,500*gameTime,0),RT_SELF);
			if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_DOWN) defaultCam->move(vector3(5,-100*gameTime,0),RT_SELF);
		}
		//Updates
		if(keyState[KEY_H]) screenShot("SunnyDay.png");
		sx = sy = sz = 0;
		if(keyState[KEY_W]) sz+=100;
		if(keyState[KEY_S]) sz-=100;
		if(keyState[KEY_A]) sx+=100;
		if(keyState[KEY_D]) sx-=100;
		ball01->setSpeed(vector3(sx,sy,sz),nullVector);
		if((ball01->collide(floor) >= 0)||(ball01->collide(ball02) >= 0)||(ball01->collide(ball03) >= 0))
			ball01->clr = SC_RED;
		else ball01->clr = SC_WHITE;
		//After every frame
		REV_process(NULL);//
	}
}

