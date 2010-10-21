///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
#include "REV.h"

N2D * MN;

void returnToLoader(u8 channel, BUTTON * self)
{
	REV_exit();
}

void homeMenuInit()
{
	MN = new N2D();//Menu node
	HBMCUSTOM * cm = (HBMCUSTOM*)malloc(sizeof(HBMCUSTOM));
	MN->custom = cm;
	cm->ldrBtn = new BUTTON(vector3(28, 200, 1),104,104,loadTexture("RevolutionEngine/returntoloader.png"));
	cm->wmnBtn = new BUTTON(vector3(188, 200, 1),104,104,loadTexture("RevolutionEngine/returntowiimenu.png"));
	cm->rstBtn = new BUTTON(vector3(348, 200, 1),104,104,loadTexture("RevolutionEngine/reset.png"));
	cm->sdnBtn = new BUTTON(vector3(508, 200, 1),104,104,loadTexture("RevolutionEngine/shutdown.png"));
	cm->ldrBtn->attach(MN);
	cm->ldrBtn->onClick = returnToLoader;
	//cm->wmnBtn->attach(MN);
	//cm->rstBtn->attach(MN);
	//cm->sdnBtn->attach(MN);
	//MN->flags = 0;
}

void REV_homeMenu(void)
{
	//HBMCUSTOM * cm = (HBMCUSTOM*)MN->custom;
	//MN->flags = F_Visible;
}