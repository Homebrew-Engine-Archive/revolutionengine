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
	REVConsole->open();
	TModel * bigBall = loadRMD("RevExamples/models/sphere.rmd");
	TModel * littleBall = loadRMD("RevExamples/models/sphere.rmd");
	bigBall->scale(10);
	OBJECT * little = new OBJECT(littleBall, vector3(-8, 0, 0));
	OBJECT * big = new OBJECT(bigBall, vector3(8, 0, 0));
	TTexture * rocksTexture = new TTexture("RevExamples/maps/plasma.png", false);
	little->setMaterial(new TDiffuseMaterial());
	big->setMaterial(new TDiffuseMaterial(rocksTexture));
	defaultCam->rotate(AXIS_Z, 180);
	defaultCam->move(vector3(0,50,5), RT_ABS);
	//Sky box
	/*mainRoot->skyBox.front = loadPng("RevExamples/maps/front-sunny.png");
	mainRoot->skyBox.back = loadPng("RevExamples/maps/back-sunny.png");
	mainRoot->skyBox.right = loadPng("RevExamples/maps/right-sunny.png");
	mainRoot->skyBox.left = loadPng("RevExamples/maps/left-sunny.png");*/
	
	//Lights
	DLIGHT(vector3(-1,-1,-1), SC_YELLOW);
	DLIGHT(vector3(0,0,1), rgb(0,30,10));
	DLIGHT(vector3(1,0,-1), SC_ORANGE);
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
		//ball->log();
		if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_B)
		{
			if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_RIGHT) defaultCam->move(vector3(50*gameTime,0,0),RT_SELF);
			if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_LEFT) defaultCam->move(vector3(-50*gameTime,0,0),RT_SELF);
			if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_UP) defaultCam->move(vector3(0,0,50*gameTime),RT_SELF);
			if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_DOWN) defaultCam->move(vector3(0,0,-50*gameTime),RT_SELF);
		}
		else
		{
			if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_RIGHT) defaultCam->rotate(AXIS_Z, -45*gameTime);
			if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_LEFT) defaultCam->rotate(AXIS_Z, 45*gameTime);
			if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_UP) defaultCam->move(vector3(0,50*gameTime,0),RT_SELF);
			if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_DOWN) defaultCam->move(vector3(0,-10*gameTime,0),RT_SELF);
		}
		//Updates
		if(WPAD_ButtonsDown(0) & WPAD_BUTTON_2) screenShot("REV-0.6_HW-Lighting.png");
		//After every frame
		REV_process(NULL);//
	}
}

