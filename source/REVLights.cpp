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
	nextL = mainRoot->fstLight;
	mainRoot->fstLight = this;
	this->flags = flags;
	this->clr = clr;
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
	LIGHT * aux = mainRoot->fstLight;
	if(aux == this)
	{
		aux = nextL;
		return;
	}
	while(aux)
	{
		if(aux->nextL == this)
		{
			aux->nextL = nextL;
		}
	}
}

void DLIGHT::setDir(Vector dir)
{
	Vector v = vector3(-dir.x, -dir.y, -dir.z);
	setPos(v);
}
