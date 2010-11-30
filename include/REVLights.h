///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Ligts header file

#ifndef REV_LIGHTS_H
#define REV_LIGHTS_H

#include "REVNodes.h"

typedef struct _shadowParams
{
	u16 rsx, rsy;//resolution
	f32 h, sx, sy;//distance from origin
	Vector up;
}SDW_PARAMS;

class LIGHT:public NODE
{
public:
	LIGHT(GXColor clr = SC_WHITE, u8 flags = F_Active);
	~LIGHT();
	GXColor clr;
	//virtual void setLight(GXLightObj* lObj, Mtx view)=0;
private:
	LIGHT * nextL;
protected:
	u8 lType;
};

class DLIGHT: public LIGHT//Directional Light
{
public:
	DLIGHT(Vector direction = vector3(0,0,-1), GXColor clr = SC_WHITE, u8 flags = F_Active);
	void setDir(Vector dir);
	//virtual void setLight(GXLightObj* lObj, Mtx view);
};

//Global data
extern GXColor ambientLight;

#endif
