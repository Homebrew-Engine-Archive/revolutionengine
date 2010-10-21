///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Objects header file

#ifndef REV_OBJECTS_H
#define REV_OBJECTS_H

class OBJECT:public NODE
{
public:
	OBJECT(MODEL * model = NULL, Vector pos = nullVector, u8 flags = F_Visible);
	MODEL * model;
	MATERIAL * material;
	GXColor clr;
private:
	GXColor shadowClr;
friend class ROOT;
friend void render(NODE * node, Vector camPos);
};

#endif
