///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Lights source file

#include "REV.h"

//Global data
GXColor ambientLight = DEFAULT_AMBIENT_LIGHT;

LIGHT::LIGHT(GXColor clr, u8 flags)
{
	mainRoot->getLights().push_back(this);
	this->flags = flags;
	this->clr = clr;
	char msg[16];
}

DLIGHT::DLIGHT(Vector v, GXColor clr, u8 flags)
{
	lType = LT_DIR;
	this->flags = flags;
	this->clr = clr;
	this->setDir(vector3(v.x, v.y, v.z));
}

LIGHT::~LIGHT()
{
	std::vector<LIGHT*>::iterator iter = mainRoot->m_vLights.begin();
	for(;iter != mainRoot->getLights().end(); ++iter)
	{
		if((*iter) == this)
		{
			mainRoot->m_vLights.erase(iter);
			break;
		}
	}
}

void DLIGHT::setDir(Vector dir)
{
	Vector v = vector3(-dir.x, -dir.y, -dir.z);
	setPos(v);
}
